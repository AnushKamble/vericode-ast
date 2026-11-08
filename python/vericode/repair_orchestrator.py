import re
import time
from typing import Dict, Any, Union, Callable, Optional
from vericode.bridge import VerifierBridge
from vericode.prompt_synthesizer import PromptSynthesizer
from vericode.llm_client import BaseLLMClient, DeterministicRuleClient

class RepairOrchestrator:
    """
    Executes iterative neuro-symbolic self-repair loops between
    the C++ AST Verifier and Code Generation LLMs.
    """
    def __init__(self, verifier_bridge: Optional[VerifierBridge] = None, max_iterations: int = 3):
        self.bridge = verifier_bridge or VerifierBridge()
        self.max_iterations = max_iterations

    def run_repair_loop(self, 
                        initial_code: str, 
                        llm_target: Union[BaseLLMClient, Callable[[str], str]]) -> Dict[str, Any]:
        """
        Executes the constraint-satisfaction self-repair loop until the C++ AST
        verifier reports zero invariant violations or max_iterations is reached.
        """
        current_code = initial_code
        iteration_history = []
        start_time = time.perf_counter()

        # Wrap callable into BaseLLMClient if needed
        if isinstance(llm_target, BaseLLMClient):
            generate_fn = llm_target.generate
        else:
            generate_fn = llm_target

        for step in range(1, self.max_iterations + 1):
            t_verify_start = time.perf_counter()
            verification_result = self.bridge.verify_source(current_code, f"iteration_{step}")
            verify_latency_us = (time.perf_counter() - t_verify_start) * 1_000_000

            iteration_record = {
                "iteration": step,
                "passed": verification_result["passed"],
                "diagnostic_count": verification_result["diagnostic_count"],
                "diagnostics": verification_result["diagnostics"],
                "verify_latency_us": verify_latency_us,
                "code_snapshot": current_code
            }
            iteration_history.append(iteration_record)

            # If all invariants pass, return success immediately
            if verification_result["passed"]:
                total_duration_ms = (time.perf_counter() - start_time) * 1000
                return {
                    "status": "SUCCESS",
                    "total_iterations": step,
                    "total_duration_ms": total_duration_ms,
                    "final_code": current_code,
                    "history": iteration_history
                }

            # Synthesize formal AST diagnostic prompt
            repair_prompt = PromptSynthesizer.construct_repair_prompt(
                current_code,
                verification_result["diagnostics"]
            )

            # Query LLM (Ollama / Local / Deterministic)
            try:
                raw_response = generate_fn(repair_prompt)
                current_code = self._extract_code(raw_response, current_code)
            except Exception as e:
                # Fallback to deterministic rule if external client times out
                fallback = DeterministicRuleClient().generate(repair_prompt)
                current_code = self._extract_code(fallback, current_code)

        # Final invariant check after loop concludes
        final_check = self.bridge.verify_source(current_code, "final_iteration")
        total_duration_ms = (time.perf_counter() - start_time) * 1000

        return {
            "status": "CONVERGED_CLEAN" if final_check["passed"] else "MAX_ITERATIONS_EXCEEDED",
            "total_iterations": self.max_iterations,
            "total_duration_ms": total_duration_ms,
            "final_code": current_code,
            "history": iteration_history
        }

    def _extract_code(self, response: str, fallback: str) -> str:
        # Match ```cpp ... ``` or ```c++ ... ```
        match = re.search(r"```(?:cpp|c\+\+)?\s*(.*?)\s*```", response, re.DOTALL | re.IGNORECASE)
        if match:
            return match.group(1).strip()
        if "#include" in response or "int main" in response:
            return response.strip()
        return fallback
