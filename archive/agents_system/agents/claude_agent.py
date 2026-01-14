#!/usr/bin/env python3
"""
Claude Agent - Intelligent Claude prompt enhancer using Ollama + Claude API
Accepts user input and creates prompts for Claude AI
"""

import subprocess
import sys
import json
from datetime import datetime
import os

class ClaudeAgent:
    def __init__(self, dry_run=False, timeout=600):
        self.session_history = []
        self.dry_run = dry_run
        self.timeout = timeout  # Can be int or None for unlimited
        print("🧠 Claude Agent initialized (Real Claude CLI)")
        print(f"   Default Timeout: {timeout} seconds (10 minutes for complex analysis)")
        if dry_run:
            print("🔍 DRY RUN MODE - No actual Claude CLI calls will be made")
        print("Type your requests and I'll send them to the real Claude CLI")
        print("Commands: 'quit' to exit, 'history' to see past prompts")
        print("Options: --timeout N (custom), --long-query (20 min), --ultra-long (45 min), --unlimited (2 hrs)")
        print("-" * 50)

    def enhance_prompt(self, user_input):
        """Return user input directly without Ollama enhancement"""
        print("📝 Using original prompt (no Ollama enhancement)")
        return user_input

    def call_claude_api(self, prompt):
        """Call Claude API with the formatted prompt"""
        try:
            print(f"🚀 Sending to Claude: {prompt[:80]}...")
            print("-" * 50)

            # Use the real Claude CLI
            if self.dry_run:
                print("🔍 DRY RUN: Would call Claude CLI")
                print("-" * 50)
                # Record in history
                self.session_history.append({
                    "timestamp": datetime.now().isoformat(),
                    "original_input": prompt,
                    "command": f"claude '{prompt}' [DRY RUN]",
                    "exit_code": 0,
                    "dry_run": True
                })
                return True

            # Call the actual Claude CLI
            print(f"🤖 Sending to Claude (timeout: {self.timeout}s)...")
            try:
                # Use stdin input method (most reliable for Claude CLI)
                result = subprocess.run(
                    ["claude"],
                    input=prompt,
                    capture_output=True,
                    text=True,
                    timeout=self.timeout
                )
            except subprocess.TimeoutExpired:
                print(f"❌ Claude request timed out after {self.timeout} seconds")
                print("   Complex queries may need more time for processing")
                print("   Try: --timeout 1200 (20 minutes) for very complex analysis")
                print("   Or simplify the question for faster response")
                return False

            if result.returncode == 0:
                response = result.stdout.strip()
                print("🧠 Claude Response:")
                print(response)
                print("-" * 50)

                # Record in history
                self.session_history.append({
                    "timestamp": datetime.now().isoformat(),
                    "original_input": prompt,
                    "command": f"claude '{prompt}'",
                    "exit_code": 0,
                    "response": response
                })
                return True
            else:
                print("❌ Claude CLI call failed")
                print(f"Error: {result.stderr}")
                return False

        except Exception as e:
            print(f"❌ Error calling Claude: {e}")
            return False

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

        # Enhance the prompt using Ollama
        enhanced_prompt = self.enhance_prompt(user_input)

        # Call Claude API
        success = self.call_claude_api(enhanced_prompt)

        return True

    def show_history(self):
        """Display session history"""
        if not self.session_history:
            print("📝 No history yet")
            return

        print("📚 Session History:")
        for i, entry in enumerate(self.session_history[-5:], 1):  # Show last 5
            print(f"{i}. {entry['timestamp'][:19]}: {entry['original_input'][:50]}...")
        print("-" * 50)

def main():
    print("🚀 Starting Claude Agent...")

    # Parse command line arguments
    dry_run = "--dry-run" in sys.argv
    if dry_run:
        sys.argv.remove("--dry-run")

    # Check for unlimited mode (very long timeout)
    unlimited = "--unlimited" in sys.argv
    if unlimited:
        sys.argv.remove("--unlimited")
        timeout = 7200  # 2 hours for extremely long queries
        print("♾️ UNLIMITED MODE: 2-hour timeout enabled")
    else:
        # Check for ultra-long-query mode (automatic 45-minute timeout)
        ultra_long = "--ultra-long" in sys.argv
        if ultra_long:
            sys.argv.remove("--ultra-long")
            timeout = 2700  # 45 minutes for extremely complex research
            print("🐌🐌 ULTRA LONG QUERY MODE: 45-minute timeout enabled")
        else:
            # Check for long-query mode (automatic 20-minute timeout)
            long_query = "--long-query" in sys.argv
            if long_query:
                sys.argv.remove("--long-query")
                timeout = 1200  # 20 minutes for very complex queries
                print("🐌 LONG QUERY MODE: 20-minute timeout enabled")
            else:
                # Check for timeout argument
                timeout = 600  # default 10 minutes for complex queries

    if "--timeout" in sys.argv:
        timeout_idx = sys.argv.index("--timeout")
        if timeout_idx + 1 < len(sys.argv):
            try:
                timeout = int(sys.argv[timeout_idx + 1])
                sys.argv.pop(timeout_idx)  # remove --timeout
                sys.argv.pop(timeout_idx)  # remove value
            except ValueError:
                print("⚠️  Invalid timeout value, using default 600 seconds")

    # Check if Claude CLI is available
    try:
        subprocess.run(["claude", "--version"], capture_output=True, check=True)
    except (subprocess.CalledProcessError, FileNotFoundError):
        print("⚠️  Claude CLI not found. Please ensure it's installed and in your PATH")
        print("Install from: https://github.com/anthropics/claude-cli")

    agent = ClaudeAgent(dry_run=dry_run, timeout=timeout)

    if len(sys.argv) > 1:
        # Command line mode
        user_input = " ".join(sys.argv[1:])
        agent.process_request(user_input)
    else:
        # Interactive mode
        while True:
            try:
                user_input = input("You: ").strip()
                if not agent.process_request(user_input):
                    break
            except KeyboardInterrupt:
                print("\n👋 Goodbye!")
                break
            except EOFError:
                break

if __name__ == "__main__":
    main()
