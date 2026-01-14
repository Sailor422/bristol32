#!/usr/bin/env python3
"""
OpenCode Agent Startup - Initializes agent orchestration system
Run this to start opencode with automatic agent routing
"""

import os
import sys
import subprocess
from pathlib import Path

def initialize_agent_system():
    """Initialize the agent orchestration system"""
    print("🚀 Initializing OpenCode Agent System...")
    print("=" * 50)

    # Check if required files exist
    required_files = [
        "agents/routing_config.json",
        "agents/agents.json",
        "agent_orchestrator.py"
    ]

    missing_files = []
    for file_path in required_files:
        if not Path(file_path).exists():
            missing_files.append(file_path)

    if missing_files:
        print("❌ Missing required files:")
        for file in missing_files:
            print(f"  • {file}")
        print("\nPlease ensure all agent files are present.")
        return False

    print("✅ All required files found")
    print("✅ Agent routing directives loaded:")
    print("  • Web/YouTube searches → Gemini agent")
    print("  • Research tasks → Research agent")
    print("  • Coding tasks → OpenCode agent")
    print("  • Prompt optimization → Claude agent")
    print("  • Local AI → Ollama agent")
    print()

    return True

def start_interactive_session():
    """Start interactive agent orchestration session"""
    print("🎯 OpenCode Agent Orchestrator Ready")
    print("Type your tasks and they'll be automatically routed to appropriate agents")
    print("Commands: 'quit' to exit, 'help' for commands")
    print("-" * 50)

    while True:
        try:
            user_input = input("🎯 Task: ").strip()

            if not user_input:
                continue

            if user_input.lower() in ['quit', 'exit', 'q']:
                print("👋 Goodbye!")
                break

            if user_input.lower() in ['help', 'h', '?']:
                print_help()
                continue

            # Run the orchestrator
            cmd = [sys.executable, "agent_orchestrator.py", user_input]
            subprocess.run(cmd)

        except KeyboardInterrupt:
            print("\n👋 Goodbye!")
            break
        except Exception as e:
            print(f"❌ Error: {e}")

def print_help():
    """Print help information"""
    print("\n🎯 OpenCode Agent Orchestrator Help")
    print("=" * 40)
    print("This system automatically routes tasks to specialized agents:")
    print()
    print("Routing Rules:")
    print("• Web/YouTube searches → Gemini agent")
    print("• Research tasks → Research agent")
    print("• Coding tasks → OpenCode agent")
    print("• Prompt enhancement → Claude agent")
    print("• Local/offline tasks → Ollama agent")
    print()
    print("Commands:")
    print("• 'help' - Show this help")
    print("• 'quit' - Exit the system")
    print()
    print("Example tasks:")
    print("• 'Search for marine electrical standards'")
    print("• 'Debug this Python function'")
    print("• 'Research lithium battery technology'")
    print("• 'Find YouTube tutorials for boat wiring'")
    print()

def main():
    if not initialize_agent_system():
        return

    if len(sys.argv) > 1:
        # Run specific task
        task = " ".join(sys.argv[1:])
        print(f"🎯 Executing task: {task}")
        cmd = [sys.executable, "agent_orchestrator.py", task]
        subprocess.run(cmd)
    else:
        # Start interactive session
        start_interactive_session()

if __name__ == "__main__":
    main()