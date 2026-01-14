#!/usr/bin/env python3
"""
AI Assistant Integration - Super AI Manager Mode
Automatically routes all user requests through the Super AI Manager
"""

import sys
import os
from pathlib import Path

# Add current directory to path for imports
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

def handle_user_request(user_input):
    """Handle any user request through the Super AI Manager"""
    try:
        from super_ai_manager import SuperAIManager

        print("🎯 SUPER AI MANAGER MODE ACTIVATED")
        print("=" * 50)

        manager = SuperAIManager()
        results = manager.process_super_task(user_input)

        return results

    except ImportError as e:
        print(f"❌ Super AI Manager not found: {e}")
        print("Please ensure super_ai_manager.py is in the current directory")
        return None
    except Exception as e:
        print(f"❌ Error initializing Super AI Manager: {e}")
        return None

def interactive_mode():
    """Run in interactive mode"""
    print("🤖 AI Assistant - Super AI Manager Mode")
    print("Type your requests and they'll be intelligently routed to the best agents")
    print("Commands: 'quit' to exit, 'help' for commands")
    print("=" * 60)

    while True:
        try:
            user_input = input("\n🎯 Your request: ").strip()

            if not user_input:
                continue

            if user_input.lower() in ['quit', 'exit', 'q']:
                print("👋 Goodbye!")
                break

            if user_input.lower() in ['help', 'h', '?']:
                print_help()
                continue

            # Process through Super AI Manager
            handle_user_request(user_input)

        except KeyboardInterrupt:
            print("\n👋 Goodbye!")
            break
        except Exception as e:
            print(f"❌ Error: {e}")

def print_help():
    """Print help information"""
    print("\n🎯 Super AI Manager Help")
    print("=" * 40)
    print("This AI assistant automatically routes your requests to specialized agents:")
    print()
    print("Available Agents:")
    print("• Gemini Agent: Web search, YouTube, current events")
    print("• Research Agent: Deep analysis, systematic research")
    print("• Claude Agent: Complex reasoning, creative tasks")
    print("• OpenCode Agent: Coding, debugging, development")
    print("• Codex Agent: ChatGPT tasks, general assistance")
    print("• Ollama Agent: Local processing, privacy-focused")
    print()
    print("The system automatically:")
    print("• Analyzes your request")
    print("• Selects the best agent(s)")
    print("• Optimizes prompts for each agent")
    print("• Runs tasks in parallel when beneficial")
    print("• Combines results for comprehensive answers")
    print()
    print("Examples:")
    print("• 'research solar panel maintenance'")
    print("• 'create a Python script for data analysis'")
    print("• 'find YouTube tutorials for boat repair'")
    print("• 'analyze maintenance cost trends'")
    print()

def main():
    if len(sys.argv) > 1:
        # Command line mode - process the request
        user_input = " ".join(sys.argv[1:])
        handle_user_request(user_input)
    else:
        # Interactive mode
        interactive_mode()

if __name__ == "__main__":
    main()