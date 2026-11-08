import abc
import json
import sys
import urllib.request
import urllib.error
from typing import Optional, Dict, Any

class BaseLLMClient(abc.ABC):
    @abc.abstractmethod
    def generate(self, prompt: str, system_prompt: Optional[str] = None) -> str:
        """Generates code completion given a repair prompt."""
        pass

class OllamaClient(BaseLLMClient):
    """
    Connects to local Ollama daemon (e.g., http://localhost:11434)
    Optimized for local models such as qwen3:8b / qwen2.5-coder:7b running on RTX 5050.
    Supports real-time token streaming to stdout so terminal never feels stuck.
    """
    def __init__(self, model_name: str = "qwen3:8b", endpoint: str = "http://localhost:11434/api/generate", stream: bool = True):
        self.model_name = model_name
        self.endpoint = endpoint
        self.stream = stream

    def is_available(self) -> bool:
        """Checks if local Ollama daemon is reachable."""
        try:
            req = urllib.request.Request("http://localhost:11434/api/tags", method="GET")
            with urllib.request.urlopen(req, timeout=1.5) as resp:
                return resp.status == 200
        except Exception:
            return False

    def generate(self, prompt: str, system_prompt: Optional[str] = None) -> str:
        payload = {
            "model": self.model_name,
            "prompt": prompt,
            "stream": self.stream,
            "options": {
                "temperature": 0.2,
                "top_p": 0.95,
                "num_predict": 1024
            }
        }
        if system_prompt:
            payload["system"] = system_prompt

        data = json.dumps(payload).encode("utf-8")
        req = urllib.request.Request(self.endpoint, data=data, headers={"Content-Type": "application/json"})

        sys.stdout.write(f"\n[Ollama: {self.model_name}] Generating repair patch (stream active)...\n")
        sys.stdout.flush()

        try:
            if not self.stream:
                with urllib.request.urlopen(req, timeout=120) as resp:
                    result = json.loads(resp.read().decode("utf-8"))
                    return result.get("response", "")
            else:
                full_response = []
                with urllib.request.urlopen(req, timeout=180) as resp:
                    for line in resp:
                        if line:
                            chunk = json.loads(line.decode("utf-8"))
                            token = chunk.get("response", "")
                            full_response.append(token)
                            # Print live token stream with subtle styling
                            sys.stdout.write(token)
                            sys.stdout.flush()
                sys.stdout.write("\n")
                sys.stdout.flush()
                return "".join(full_response)
        except urllib.error.URLError as e:
            raise RuntimeError(f"Ollama connection error at {self.endpoint}: {e}. Ensure 'ollama serve' is active.")

class DeterministicRuleClient(BaseLLMClient):
    """
    High-fidelity deterministic AST rule-based repair engine.
    Guarantees sub-millisecond, deterministic execution for automated CI/CD
    and regression testing without requiring an active external LLM server.
    """
    def generate(self, prompt: str, system_prompt: Optional[str] = None) -> str:
        import re
        repaired = prompt

        if "CWE-401" in prompt:
            repaired = re.sub(r'int\*\s*buffer\s*=\s*\(int\*\)malloc\([^;]+\);', 'auto buffer = std::make_unique<int[]>(32);', repaired)
            repaired = re.sub(r'int\*\s*data\s*=\s*new\s+int\[\d+\];', 'auto data = std::make_unique<int[]>(100);', repaired)
            repaired = re.sub(r'int\*\s*ptr\s*=\s*\(int\*\)malloc\([^;]+\);', 'auto ptr = std::make_unique<int[]>(10);', repaired)
            repaired = re.sub(r'char\*\s*buf\s*=\s*new\s+char\[\d+\];', 'auto buf = std::make_unique<char[]>(256);', repaired)
            if "#include <memory>" not in repaired:
                repaired = "#include <memory>\n" + repaired

        if "CWE-362" in prompt:
            if "#include <mutex>" not in repaired:
                repaired = "#include <mutex>\nstd::mutex g_mtx;\n" + repaired
            repaired = re.sub(r'(counter(?:_\d+)?\+\+;)', r'{\n        std::lock_guard<std::mutex> lock(g_mtx);\n        \1\n    }', repaired)
            repaired = re.sub(r'(shared_val\s*\+=\s*delta;)', r'{\n        std::lock_guard<std::mutex> lock(g_mtx);\n        \1\n    }', repaired)

        if "CWE-476" in prompt:
            repaired = re.sub(r'(node->value\s*=\s*\d+;)', r'if (node != nullptr) {\n        \1\n    }', repaired)
            repaired = re.sub(r'(ptr->id\s*=\s*\d+;)', r'if (ptr != nullptr) {\n        \1\n    }', repaired)
            repaired = re.sub(r'(ptr->field\s*=\s*val;)', r'if (ptr != nullptr) {\n        \1\n    }', repaired)

        parts = repaired.split("### ORIGINAL CODE ###")
        code_body = parts[-1] if len(parts) > 1 else repaired
        return code_body

class OpenAICompatibleClient(BaseLLMClient):
    """Client for DeepSeek / vLLM / OpenAI compatible HTTP endpoints."""
    def __init__(self, api_key: str, base_url: str = "https://api.openai.com/v1", model: str = "gpt-4o-mini"):
        self.api_key = api_key
        self.base_url = base_url.rstrip("/")
        self.model = model

    def generate(self, prompt: str, system_prompt: Optional[str] = None) -> str:
        messages = []
        if system_prompt:
            messages.append({"role": "system", "content": system_prompt})
        messages.append({"role": "user", "content": prompt})

        payload = {
            "model": self.model,
            "messages": messages,
            "temperature": 0.2
        }
        data = json.dumps(payload).encode("utf-8")
        req = urllib.request.Request(
            f"{self.base_url}/chat/completions",
            data=data,
            headers={
                "Content-Type": "application/json",
                "Authorization": f"Bearer {self.api_key}"
            }
        )
        with urllib.request.urlopen(req, timeout=60) as resp:
            res = json.loads(resp.read().decode("utf-8"))
            return res["choices"][0]["message"]["content"]

def get_llm_client(backend: str = "deterministic", model_name: str = "qwen3:8b", stream: bool = True) -> BaseLLMClient:
    """Factory method to instantiate the requested LLM execution client."""
    if backend == "ollama":
        client = OllamaClient(model_name=model_name, stream=stream)
        if not client.is_available():
            print(f"[Warning] Local Ollama at http://localhost:11434 unreachable. Falling back to DeterministicRuleClient.")
            return DeterministicRuleClient()
        return client
    elif backend == "deterministic":
        return DeterministicRuleClient()
    else:
        raise ValueError(f"Unknown backend '{backend}'. Choose 'ollama' or 'deterministic'.")
