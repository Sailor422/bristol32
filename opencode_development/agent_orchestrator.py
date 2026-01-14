#!/usr/bin/env python3
"""
Agent Orchestrator - Automatically routes tasks to appropriate agents
Implements persistent directives for agent assignment and routing
"""

import json
import sys
import subprocess
import threading
import time
from pathlib import Path
from concurrent.futures import ThreadPoolExecutor, as_completed

class AgentOrchestrator:
    def __init__(self):
        self.agents_dir = Path("agents")
        self.routing_config = self.agents_dir / "routing_config.json"
        self.agents_config = self.agents_dir / "agents.json"

        self.load_configs()
        print("🎯 Agent Orchestrator initialized")
        print("📋 Loaded routing directives:")
        for rule_name, rule in self.routing_rules.items():
            print(f"  • {rule_name}: {rule['description']}")
        print("-" * 50)

    def load_configs(self):
        """Load routing and agent configurations"""
        # Load routing config
        if self.routing_config.exists():
            with open(self.routing_config, 'r') as f:
                config = json.load(f)
                self.routing_rules = config.get('routing_rules', {})
                self.orchestration_settings = config.get('orchestration_settings', {})
                self.agent_capabilities = config.get('agent_capabilities', {})
        else:
            print("❌ Routing config not found!")
            sys.exit(1)

        # Load agents config
        if self.agents_config.exists():
            with open(self.agents_config, 'r') as f:
                self.agents = json.load(f)
        else:
            print("❌ Agents config not found!")
            sys.exit(1)

    def analyze_task(self, user_input):
        """Analyze user input and determine which agents to use"""
        user_input_lower = user_input.lower()
        matched_agents = {}

        # Check each routing rule
        for rule_name, rule in self.routing_rules.items():
            score = 0
            keywords = rule.get('keywords', [])

            # Count keyword matches
            for keyword in keywords:
                if keyword in user_input_lower:
                    score += 1

            if score > 0:
                agent_id = rule['agent']
                priority = rule.get('priority', 5)

                if agent_id not in matched_agents:
                    matched_agents[agent_id] = {
                        'score': score,
                        'priority': priority,
                        'rules': [rule_name]
                    }
                else:
                    matched_agents[agent_id]['score'] += score
                    matched_agents[agent_id]['rules'].append(rule_name)
                    # Use highest priority
                    matched_agents[agent_id]['priority'] = min(
                        matched_agents[agent_id]['priority'], priority
                    )

        # Sort by priority (lower number = higher priority), then by score
        sorted_agents = sorted(
            matched_agents.items(),
            key=lambda x: (x[1]['priority'], -x[1]['score'])
        )

        return [(agent_id, data) for agent_id, data in sorted_agents]

    def optimize_prompt_for_agent(self, user_input, agent_id):
        """Return original prompt directly (no Ollama optimization)"""
        print(f"📝 Using original prompt for {agent_id} (no optimization)")
        return user_input

    def run_agent(self, agent_id, optimized_prompt):
        """Run a specific agent with optimized prompt"""
        if agent_id not in self.agents:
            return f"❌ Agent '{agent_id}' not found"

        agent_config = self.agents[agent_id]
        if not agent_config.get('active', False):
            return f"⏸️  Agent '{agent_id}' is inactive"

        script_path = self.agents_dir / agent_config['script']
        if not script_path.exists():
            return f"❌ Agent script not found: {script_path}"

        print(f"🚀 Running {agent_id} with optimized prompt...")

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
                return f"✅ {agent_id} completed:\n{result.stdout}"
            else:
                return f"❌ {agent_id} failed:\n{result.stderr}"

        except subprocess.TimeoutExpired:
            return f"⏰ {agent_id} timed out after {timeout} seconds"
        except Exception as e:
            return f"❌ {agent_id} error: {str(e)}"

    def orchestrate_task(self, user_input):
        """Main orchestration method"""
        print(f"🎯 Analyzing task: {user_input[:100]}{'...' if len(user_input) > 100 else ''}")

        # Analyze which agents to use
        agent_matches = self.analyze_task(user_input)

        if not agent_matches:
            fallback_agent = self.orchestration_settings.get('fallback_agent', 'ollama_agent')
            print(f"🤔 No specific routing rules matched, using fallback: {fallback_agent}")
            agent_matches = [(fallback_agent, {'score': 1, 'priority': 5, 'rules': ['fallback']})]

        print(f"📋 Selected agents: {', '.join([agent_id for agent_id, _ in agent_matches])}")

        results = {}

        # Run agents (parallel if multiple)
        max_parallel = self.orchestration_settings.get('max_parallel_agents', 3)
        use_parallel = len(agent_matches) > 1 and self.orchestration_settings.get('multi_agent_support', True)

        if use_parallel and len(agent_matches) <= max_parallel:
            print(f"🔄 Running {len(agent_matches)} agents in parallel...")
            with ThreadPoolExecutor(max_workers=max_parallel) as executor:
                future_to_agent = {}
                for agent_id, match_data in agent_matches:
                    optimized_prompt = self.optimize_prompt_for_agent(user_input, agent_id)
                    future = executor.submit(self.run_agent, agent_id, optimized_prompt)
                    future_to_agent[future] = agent_id

                for future in as_completed(future_to_agent):
                    agent_id = future_to_agent[future]
                    try:
                        results[agent_id] = future.result()
                    except Exception as e:
                        results[agent_id] = f"❌ {agent_id} execution error: {str(e)}"
        else:
            # Run sequentially
            for agent_id, match_data in agent_matches:
                print(f"🔄 Running {agent_id}...")
                optimized_prompt = self.optimize_prompt_for_agent(user_input, agent_id)
                results[agent_id] = self.run_agent(agent_id, optimized_prompt)

        # Display results
        print("\n" + "="*60)
        print("📊 ORCHESTRATION RESULTS")
        print("="*60)

        for agent_id, result in results.items():
            print(f"\n🤖 {agent_id.upper()} RESULTS:")
            print("-" * 40)
            print(result)

        return results

def main():
    orchestrator = AgentOrchestrator()

    if len(sys.argv) < 2:
        print("🎯 Agent Orchestrator")
        print("Automatically routes tasks to appropriate agents based on directives")
        print()
        print("Usage:")
        print("  python3 agent_orchestrator.py 'your task description'")
        print()
        print("Active routing rules:")
        for rule_name, rule in orchestrator.routing_rules.items():
            agent = rule.get('agent', 'unknown')
            keywords = ', '.join(rule.get('keywords', []))
            print(f"  • {rule_name} → {agent} (keywords: {keywords})")
        return

    user_input = " ".join(sys.argv[1:])
    orchestrator.orchestrate_task(user_input)

if __name__ == "__main__":
    main()