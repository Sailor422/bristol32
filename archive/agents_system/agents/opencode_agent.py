#!/usr/bin/env python3
"""
OpenCode Agent - Agent that uses opencode CLI capabilities for software engineering tasks
Integrates with opencode for coding assistance and automation
"""

import subprocess
import sys
import json
from datetime import datetime
import os

class OpenCodeAgent:
    def __init__(self):
        self.session_history = []
        print("⚡ OpenCode Agent initialized")
        print("I can help with software engineering tasks using opencode")
        print("Commands: 'quit' to exit, 'history' to see past commands")
        print("-" * 50)

    def analyze_request(self, user_input):
        """Analyze the request and determine appropriate actions"""
        user_input_lower = user_input.lower()

        # Simple keyword-based analysis
        if any(word in user_input_lower for word in ['find', 'search', 'locate', 'list']):
            if 'python' in user_input_lower or '.py' in user_input_lower:
                return {
                    "task_type": "file_search",
                    "description": "Find Python files",
                    "commands": ["find . -name '*.py' -type f"],
                    "explanation": "Use find command to locate Python files"
                }
            elif 'file' in user_input_lower:
                return {
                    "task_type": "file_search",
                    "description": "List files in directory",
                    "commands": ["ls -la"],
                    "explanation": "List all files with details"
                }

        elif any(word in user_input_lower for word in ['read', 'show', 'display', 'content']):
            return {
                "task_type": "file_operation",
                "description": "Read file contents",
                "commands": ["echo 'Please specify which file to read'"],
                "explanation": "Need specific file path to read"
            }

        elif any(word in user_input_lower for word in ['git', 'commit', 'status', 'branch']):
            return {
                "task_type": "git_operation",
                "description": "Git repository operations",
                "commands": ["git status"],
                "explanation": "Check git repository status"
            }

        elif any(word in user_input_lower for word in ['test', 'run', 'execute']):
            return {
                "task_type": "build_test",
                "description": "Run tests or build",
                "commands": ["echo 'No test framework detected'"],
                "explanation": "Check for test files or build scripts"
            }

        else:
            return {
                "task_type": "general",
                "description": "General assistance",
                "commands": [f"echo 'Request: {user_input}'"],
                "explanation": "General request processing"
            }

    def fallback_analysis(self, user_input):
        """Fallback analysis when Ollama fails"""
        return {
            "task_type": "general",
            "description": "General assistance request",
            "commands": [f"echo 'User request: {user_input}'"],
            "explanation": "Basic request processing"
        }

    def execute_opencode_commands(self, analysis):
        """Execute the determined opencode commands"""
        commands = analysis.get("commands", [])
        task_type = analysis.get("task_type", "unknown")

        print(f"🔧 Executing {task_type} commands...")
        print("-" * 50)

        for i, cmd in enumerate(commands, 1):
            print(f"📝 Command {i}: {cmd}")

            try:
                # For demo purposes, we'll simulate opencode commands
                # In reality, you'd call actual opencode CLI here
                if "read" in cmd.lower():
                    result = subprocess.run(["ls", "-la"], capture_output=True, text=True)
                elif "grep" in cmd.lower():
                    result = subprocess.run(["echo", "Simulated grep results"], capture_output=True, text=True)
                elif "git" in cmd.lower():
                    result = subprocess.run(["git", "status"], capture_output=True, text=True)
                else:
                    result = subprocess.run([cmd], shell=True, capture_output=True, text=True)

                if result.returncode == 0:
                    print("✅ Output:")
                    print(result.stdout.strip() or "Command completed successfully")
                else:
                    print("⚠️  Command had issues:")
                    print(result.stderr.strip())

            except Exception as e:
                print(f"❌ Error: {e}")

            print("-" * 30)

        # Record in history
        self.session_history.append({
            "timestamp": datetime.now().isoformat(),
            "original_input": analysis.get("description", "Unknown"),
            "task_type": task_type,
            "commands_executed": commands,
            "explanation": analysis.get("explanation", "")
        })

        return True

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

        # Analyze the request
        analysis = self.analyze_request(user_input)

        # Execute opencode commands
        success = self.execute_opencode_commands(analysis)

        return True

    def show_history(self):
        """Display session history"""
        if not self.session_history:
            print("📝 No history yet")
            return

        print("📚 Session History:")
        for i, entry in enumerate(self.session_history[-5:], 1):  # Show last 5
            print(f"{i}. {entry['timestamp'][:19]}: {entry['task_type']} - {entry['original_input'][:40]}...")
        print("-" * 50)

def main():
    print("🚀 Starting OpenCode Agent...")

    # Check if Ollama is running
    try:
        subprocess.run(["ollama", "list"], capture_output=True, check=True)
    except (subprocess.CalledProcessError, FileNotFoundError):
        print("⚠️  Ollama not available - analysis may be limited")

    agent = OpenCodeAgent()

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
