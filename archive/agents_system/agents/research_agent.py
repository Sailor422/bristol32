#!/usr/bin/env python3
"""
Research Agent - Specialized agent for research and information gathering
Uses only Ollama for local AI responses
"""

import subprocess
import sys

class ResearchAgent:
    def __init__(self):
        print("🤖 Research Agent initialized")
        print("Specialized agent for research and information gathering")
        print("-" * 50)

    def process_request(self, user_input):
        """Process user request with Ollama"""
        try:
            result = subprocess.run(
                ["ollama", "run", "llama3.2:1b", user_input],
                capture_output=True,
                text=True,
                timeout=30
            )

            if result.returncode == 0:
                print("🤖 Response:")
                print(result.stdout.strip())
            else:
                print("❌ Error processing request")

        except Exception as e:
            print("❌ Error: " + str(e))

def main():
    agent = ResearchAgent()

    if len(sys.argv) > 1:
        user_input = " ".join(sys.argv[1:])
        agent.process_request(user_input)
    else:
        print("Usage: python3 research_agent.py 'your question'")

if __name__ == "__main__":
    main()
