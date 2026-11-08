from typing import List, Dict, Any

class PromptSynthesizer:
    """
    Synthesizes formal AST-diagnostic feedback prompts to guide code LLMs
    in resolving static verification defects in an iterative repair loop.
    """

    @staticmethod
    def construct_repair_prompt(original_code: str, diagnostics: List[Dict[str, Any]]) -> str:
        prompt_parts = [
            "### CODE VERIFICATION DEFECT REPORT ###",
            "The code you generated failed static invariant verification with the following formal defects:\n"
        ]

        for idx, d in enumerate(diagnostics, 1):
            prompt_parts.append(
                f"[{idx}] Defect: {d['rule_id']} - {d['rule_name']} (Severity: {d['severity']})\n"
                f"    Location : Line {d['line_start']}:{d['col_start']}\n"
                f"    Violating Snippet: `{d['context_snippet']}`\n"
                f"    Explanation: {d['message']}\n"
                f"    Required Invariant: {d['repair_hint']}\n"
            )

        prompt_parts.extend([
            "### REPAIR INSTRUCTIONS ###",
            "1. Rewrite the code to strictly satisfy all specified invariants.",
            "2. Ensure all raw pointers and manual memory management are refactored to modern C++ RAII (e.g., std::unique_ptr, std::make_unique).",
            "3. Ensure all shared concurrent access is guarded with std::mutex and std::lock_guard or std::atomic.",
            "4. Return ONLY the complete, compilable, corrected C++ code enclosed in ```cpp and ``` codeblocks without extraneous conversational text.\n",
            "### ORIGINAL CODE ###",
            "```cpp",
            original_code,
            "```"
        ])

        return "\n".join(prompt_parts)
