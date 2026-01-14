#!/usr/bin/env python3
"""
Codex Agent - ChatGPT Prompt Enhancer
Uses Ollama + Codex API for ChatGPT prompt optimization
"""

import subprocess
import sys
import json
from datetime import datetime
import os

class CodexAgent:
    def __init__(self, dry_run=False):
        self.session_history = []
        self.dry_run = dry_run
        print("🤖 Codex Agent initialized")
        if dry_run:
            print("🔍 DRY RUN MODE - No actual Codex calls will be made")
        print("Type your requests and I'll format them for ChatGPT")
        print("Commands: 'quit' to exit, 'history' to see past prompts")
        print("-" * 50)

    def enhance_prompt(self, user_input):
        """Return user input directly without Ollama enhancement"""
        print("📝 Using original prompt (no Ollama enhancement)")
        return user_input

    def call_codex_api(self, prompt):
        """Call Codex API with the formatted prompt"""
        try:
            print(f"🚀 Sending to ChatGPT: {prompt[:80]}...")
            print("-" * 50)

            # Use the real Codex CLI (placeholder - would need actual implementation)
            if not self.dry_run:
                # This would call the actual Codex/ChatGPT API
                # For now, simulate with a placeholder
                print("💭 ChatGPT Response:")
                print("(Codex API integration would go here)")
                print("This is where the actual ChatGPT response would appear.")
            else:
                print("🔍 DRY RUN - Would send to ChatGPT:")
                print(f"'{prompt}'")

        except Exception as e:
            print(f"❌ Error calling Codex API: {e}")

    def show_history(self):
        """Show session history"""
        if not self.session_history:
            print("📝 No history yet")
            return

        print("📚 Session History:")
        print("-" * 30)
        for i, entry in enumerate(self.session_history[-10:], 1):  # Show last 10
            print(f"{i}. {entry['timestamp']}: {entry['input'][:50]}...")
        print()

    def process_request(self, user_input):
        """Main processing logic"""
        user_input = user_input.strip()

        if not user_input:
            return True

        if user_input.lower() in ['quit', 'exit', 'q']:
            print("👋 Goodbye!")
            return False

        if user_input.lower() == 'history':
            self.show_history()
            return True

        # Add to history
        self.session_history.append({
            'timestamp': datetime.now().strftime('%H:%M:%S'),
            'input': user_input
        })

        # Enhance prompt (now just returns original)
        enhanced_prompt = self.enhance_prompt(user_input)

        # Call Codex API
        self.call_codex_api(enhanced_prompt)

        return True

def main():
    dry_run = '--dry-run' in sys.argv
    agent = CodexAgent(dry_run=dry_run)

    if len(sys.argv) > 1 and not dry_run:
        # Command line usage
        user_input = " ".join(arg for arg in sys.argv[1:] if arg != '--dry-run')
        agent.process_request(user_input)
    else:
        # Interactive mode
        while True:
            try:
                user_input = input("🎯 Request: ").strip()
                if not agent.process_request(user_input):
                    break
            except KeyboardInterrupt:
                print("\n👋 Goodbye!")
                break
            except Exception as e:
                print(f"❌ Error: {e}")

if __name__ == "__main__":
    main()