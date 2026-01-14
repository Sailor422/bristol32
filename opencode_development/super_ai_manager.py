#!/usr/bin/env python3
"""
Super AI Manager - Intelligent Agent Orchestration System
Analyzes user prompts and optimally distributes tasks across multiple AI agents
"""

import subprocess
import json
import sys
import threading
import time
from concurrent.futures import ThreadPoolExecutor, as_completed
from pathlib import Path

class SuperAIManager:
    def __init__(self):
        self.agents_dir = Path("agents")
        self.agents_config = self.agents_dir / "agents.json"
        self.routing_config = self.agents_dir / "routing_config.json"
        self.super_config = Path("super_ai_config.json")

        self.load_configs()
        print("🎯 Super AI Manager initialized")
        print("🤖 Available agents:", len(self.agents))
        print("📋 Routing rules loaded")
        print("⚙️  Configuration loaded")

    def load_configs(self):
        """Load agent and routing configurations"""
        # Load agents
        if self.agents_config.exists():
            with open(self.agents_config, 'r') as f:
                self.agents = json.load(f)
        else:
            print("❌ Agents config not found")
            sys.exit(1)

        # Load routing
        if self.routing_config.exists():
            with open(self.routing_config, 'r') as f:
                routing_data = json.load(f)
                self.routing_rules = routing_data.get('routing_rules', {})
                self.orchestration_settings = routing_data.get('orchestration_settings', {})
                self.agent_capabilities = routing_data.get('agent_capabilities', {})
        else:
            print("❌ Routing config not found")
            sys.exit(1)

        # Load super AI config
        if self.super_config.exists():
            with open(self.super_config, 'r') as f:
                super_config = json.load(f)
                self.super_settings = super_config.get('super_ai_manager', {})
                self.user_preferences = super_config.get('user_preferences', {})
        else:
            # Default settings
            self.super_settings = {
                "enabled": True,
                "auto_analyze": True,
                "parallel_processing": True,
                "max_parallel_agents": 3
            }
            self.user_preferences = {
                "show_agent_selection": True,
                "verbose_output": True
            }

    def analyze_prompt(self, user_prompt):
        """Analyze user prompt and determine optimal agent strategy"""
        prompt_lower = user_prompt.lower()

        # Define agent expertise areas
        agent_expertise = {
            'gemini_agent': {
                'strengths': ['web_search', 'youtube', 'current_events', 'multimodal', 'research', 'information_gathering'],
                'keywords': ['search', 'find', 'youtube', 'video', 'web', 'online', 'research', 'current', 'news']
            },
            'research_agent': {
                'strengths': ['deep_analysis', 'academic_research', 'data_analysis', 'systematic_review'],
                'keywords': ['analyze', 'research', 'study', 'investigate', 'deep', 'comprehensive']
            },
            'claude_agent': {
                'strengths': ['complex_reasoning', 'creative_writing', 'optimization', 'strategy', 'planning'],
                'keywords': ['plan', 'strategy', 'optimize', 'creative', 'design', 'complex', 'reasoning']
            },
            'opencode_agent': {
                'strengths': ['coding', 'debugging', 'software_engineering', 'technical_implementation'],
                'keywords': ['code', 'program', 'debug', 'function', 'script', 'develop', 'implement']
            },
            'codex_agent': {
                'strengths': ['chatgpt_tasks', 'conversational', 'general_assistance', 'explanation'],
                'keywords': ['explain', 'help', 'general', 'chat', 'conversation']
            },
            'ollama_agent': {
                'strengths': ['local_processing', 'privacy', 'offline', 'fast_response'],
                'keywords': ['local', 'private', 'offline', 'quick']
            }
        }

        # Score each agent based on prompt analysis
        agent_scores = {}
        for agent_id, expertise in agent_expertise.items():
            score = 0
            keywords = expertise['keywords']

            # Keyword matching
            for keyword in keywords:
                if keyword in prompt_lower:
                    score += 2

            # Context analysis
            if 'web' in prompt_lower or 'internet' in prompt_lower:
                if agent_id == 'gemini_agent':
                    score += 3
            if 'code' in prompt_lower or 'program' in prompt_lower:
                if agent_id == 'opencode_agent':
                    score += 3
            if 'research' in prompt_lower or 'analyze' in prompt_lower:
                if agent_id in ['research_agent', 'claude_agent']:
                    score += 2
            if len(user_prompt.split()) > 50:  # Complex prompts
                if agent_id in ['claude_agent', 'research_agent']:
                    score += 1

            if score > 0:
                agent_scores[agent_id] = score

        # Determine strategy
        if not agent_scores:
            # Default to Ollama for general tasks
            return [('ollama_agent', 'General task handling')]

        # Sort by score
        sorted_agents = sorted(agent_scores.items(), key=lambda x: x[1], reverse=True)

        # For complex tasks, use multiple agents
        if len(user_prompt.split()) > 30 or 'and' in prompt_lower or ',' in prompt_lower:
            # Return top 2-3 agents for complex tasks
            return [(agent_id, f"Specialized handling (score: {score})")
                   for agent_id, score in sorted_agents[:3] if score > 1]

        # For simple tasks, use best single agent
        best_agent = sorted_agents[0][0]
        return [(best_agent, f"Primary handling (score: {sorted_agents[0][1]})")]

    def optimize_prompt_for_agent(self, user_prompt, agent_id):
        """Create an optimized prompt for the specific agent"""
        agent_info = self.agents.get(agent_id, {})
        capabilities = self.agent_capabilities.get(agent_id, {})

        agent_type = agent_info.get('type', 'custom')
        strengths = capabilities.get('strengths', [])

        # Agent-specific prompt optimization
        if agent_id == 'gemini_agent':
            optimized = f"""Please help with this request: {user_prompt}

Leverage your web search and multimodal capabilities to provide comprehensive, current information.
Focus on practical, actionable results with specific details and sources where relevant."""
        elif agent_id == 'research_agent':
            optimized = f"""Conduct thorough research on: {user_prompt}

Provide systematic analysis with evidence-based findings.
Structure your response with clear sections, supporting data, and practical implications."""
        elif agent_id == 'claude_agent':
            optimized = f"""Analyze and provide expert assistance for: {user_prompt}

Use your reasoning capabilities to break down complex problems, provide strategic insights, and offer well-structured solutions."""
        elif agent_id == 'opencode_agent':
            optimized = f"""Technical implementation task: {user_prompt}

Provide code solutions, debugging assistance, or software engineering guidance with clear, working examples."""
        elif agent_id == 'codex_agent':
            optimized = f"""Help with this request: {user_prompt}

Provide clear, helpful responses with practical guidance and explanations."""
        else:  # ollama_agent or default
            optimized = f"""Please assist with: {user_prompt}

Provide helpful, accurate information with clear reasoning."""

        return optimized

    def execute_agent_task(self, agent_id, optimized_prompt):
        """Execute a task with a specific agent"""
        if agent_id not in self.agents:
            return f"❌ Agent '{agent_id}' not found"

        agent_config = self.agents[agent_id]
        if not agent_config.get('active', False):
            return f"⏸️  Agent '{agent_id}' is inactive"

        script_path = self.agents_dir / agent_config['script']
        if not script_path.exists():
            return f"❌ Agent script not found: {script_path}"

        print(f"🚀 Executing with {agent_id}...")
        print(f"   Prompt: {optimized_prompt[:100]}{'...' if len(optimized_prompt) > 100 else ''}")

        # Get timeout for this agent
        capabilities = self.agent_capabilities.get(agent_id, {})
        timeout = capabilities.get('timeout', 60)

        try:
            cmd = [sys.executable, str(script_path), optimized_prompt]
            result = subprocess.run(
                cmd,
                capture_output=True,
                text=True,
                timeout=timeout
            )

            if result.returncode == 0:
                return f"✅ {agent_id} completed:\n{result.stdout.strip()}"
            else:
                return f"❌ {agent_id} failed:\n{result.stderr.strip()}"

        except subprocess.TimeoutExpired:
            return f"⏰ {agent_id} timed out after {timeout} seconds"
        except Exception as e:
            return f"❌ {agent_id} error: {str(e)}"

    def process_super_task(self, user_prompt):
        """Main super AI manager processing"""
        print(f"🎯 SUPER AI MANAGER analyzing: {user_prompt[:100]}{'...' if len(user_prompt) > 100 else ''}")
        print("=" * 60)

        # Analyze and select agents
        selected_agents = self.analyze_prompt(user_prompt)

        if not selected_agents:
            print("🤔 No suitable agents found, using default")
            selected_agents = [('ollama_agent', 'Default handling')]

        print(f"📋 SELECTED AGENTS: {len(selected_agents)}")
        for agent_id, reason in selected_agents:
            print(f"   • {agent_id}: {reason}")
        print()

        # Execute tasks
        results = {}

        if len(selected_agents) == 1:
            # Single agent task
            agent_id, reason = selected_agents[0]
            print(f"🔄 Processing with single agent: {agent_id}")
            optimized_prompt = self.optimize_prompt_for_agent(user_prompt, agent_id)
            results[agent_id] = self.execute_agent_task(agent_id, optimized_prompt)

        else:
            # Multi-agent parallel processing
            print(f"🔄 Processing with {len(selected_agents)} agents in parallel...")

            with ThreadPoolExecutor(max_workers=len(selected_agents)) as executor:
                future_to_agent = {}
                for agent_id, reason in selected_agents:
                    optimized_prompt = self.optimize_prompt_for_agent(user_prompt, agent_id)
                    future = executor.submit(self.execute_agent_task, agent_id, optimized_prompt)
                    future_to_agent[future] = agent_id

                for future in as_completed(future_to_agent):
                    agent_id = future_to_agent[future]
                    try:
                        results[agent_id] = future.result()
                    except Exception as e:
                        results[agent_id] = f"❌ {agent_id} execution error: {str(e)}"

        # Display results
        print("\n" + "="*60)
        print("📊 SUPER AI MANAGER RESULTS")
        print("="*60)

        for agent_id, result in results.items():
            print(f"\n🤖 {agent_id.upper()} RESULTS:")
            print("-" * 40)
            print(result)

        # Provide synthesis if multiple agents
        if len(results) > 1:
            print(f"\n🎯 SYNTHESIS ({len(results)} agents contributed):")
            print("-" * 40)
            successful_results = [r for r in results.values() if r.startswith("✅")]
            if successful_results:
                print(f"✅ {len(successful_results)} agents completed successfully")
                print("📋 Review individual results above for comprehensive insights")
            else:
                print("❌ All agents encountered issues - check individual results")

        return results

def main():
    if len(sys.argv) < 2:
        print("🎯 SUPER AI MANAGER")
        print("Intelligent multi-agent task orchestration")
        print()
        print("Usage:")
        print("  python3 super_ai_manager.py 'your task description'")
        print()
        print("Examples:")
        print("  python3 super_ai_manager.py 'research solar panel maintenance'")
        print("  python3 super_ai_manager.py 'create a Python script for data analysis'")
        print("  python3 super_ai_manager.py 'find YouTube tutorials for boat repair'")
        return

    user_prompt = " ".join(sys.argv[1:])
    manager = SuperAIManager()
    manager.process_super_task(user_prompt)

if __name__ == "__main__":
    main()