#!/usr/bin/env python3
"""
Ollama Agent - Local AI agent using Ollama for responses and analysis
Advanced local AI agent with model selection and session management
"""

import subprocess
import sys
import json
from datetime import datetime
import os

class OllamaAgent:
    def __init__(self, model="llama3.2:1b"):
        self.session_history = []
        self.current_model = model
        self.available_models = self.get_available_models()

        print("🦙 Ollama Agent initialized")
        print(f"Current model: {self.current_model}")
        print(f"Available models: {', '.join(self.available_models)}")
        print("Commands: 'quit' to exit, 'history' to see past prompts, 'models' to list models, 'switch <model>' to change model")
        print("-" * 50)

    def get_available_models(self):
        """Get list of available Ollama models"""
        try:
            result = subprocess.run(
                ["ollama", "list"],
                capture_output=True,
                text=True,
                timeout=10
            )
            if result.returncode == 0:
                lines = result.stdout.strip().split('\n')
                models = []
                for line in lines[1:]:  # Skip header
                    if line.strip():
                        parts = line.split()
                        if parts:
                            models.append(parts[0])
                return models
            else:
                return ["llama3.2:1b"]  # fallback
        except Exception:
            return ["llama3.2:1b"]  # fallback

    def switch_model(self, model_name):
        """Switch to a different Ollama model"""
        if model_name in self.available_models:
            self.current_model = model_name
            print(f"🔄 Switched to model: {model_name}")
            return True
        else:
            print(f"❌ Model '{model_name}' not available")
            print(f"Available models: {', '.join(self.available_models)}")
            return False

    def enhance_prompt(self, user_input):
        """Return user input directly without enhancement"""
        print("📝 Using original prompt (no enhancement)")
        return user_input

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

        if user_input.lower() == 'models':
            self.list_models()
            return True

        if user_input.lower().startswith('switch '):
            model_name = user_input[7:].strip()
            self.switch_model(model_name)
            return True

        # Enhance the prompt
        enhanced_prompt = self.enhance_prompt(user_input)

        # Process with Ollama
        try:
            print(f"🦙 Processing with {self.current_model}: {enhanced_prompt[:80]}...")
            print("-" * 50)

            result = subprocess.run(
                ["ollama", "run", self.current_model, enhanced_prompt],
                capture_output=True,
                text=True,
                timeout=60  # Ollama can take longer
            )

            if result.returncode == 0:
                response = result.stdout.strip()
                print("🦙 Ollama Response:")
                print(response)
                print("-" * 50)

                # Record in history
                self.session_history.append({
                    "timestamp": datetime.now().isoformat(),
                    "model": self.current_model,
                    "original_input": user_input,
                    "enhanced_input": enhanced_prompt,
                    "response": response
                })
                return True
            else:
                print("❌ Ollama processing failed")
                print(f"Error: {result.stderr}")
                return False

        except subprocess.TimeoutExpired:
            print("⏰ Response timed out - model may be busy")
            return False
        except Exception as e:
            print(f"❌ Error: {e}")
            return False

    def show_history(self):
        """Display session history"""
        if not self.session_history:
            print("📝 No history yet")
            return

        print("📚 Session History:")
        for i, entry in enumerate(self.session_history[-5:], 1):  # Show last 5
            print(f"{i}. {entry['timestamp'][:19]} - {entry['model']}: {entry['original_input'][:50]}...")
        print("-" * 50)

    def list_models(self):
        """List available models"""
        print("🦙 Available Ollama Models:")
        for model in self.available_models:
            marker = " ← CURRENT" if model == self.current_model else ""
            print(f"  • {model}{marker}")
        print("-" * 50)

def main():
    print("🚀 Starting Ollama Agent...")

    # Check if Ollama is available
    try:
        subprocess.run(["ollama", "list"], capture_output=True, check=True)
    except (subprocess.CalledProcessError, FileNotFoundError):
        print("❌ Ollama not found. Please install Ollama first:")
        print("https://ollama.ai/download")
        return

    # Parse command line arguments
    model = "llama3.2:1b"  # default
    if len(sys.argv) > 1 and sys.argv[1].startswith("--model="):
        model = sys.argv[1][8:]
        sys.argv.pop(1)

    agent = OllamaAgent(model)

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
