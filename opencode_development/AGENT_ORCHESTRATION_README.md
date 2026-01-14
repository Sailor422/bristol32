# OpenCode Agent Orchestration System

This system automatically routes tasks to specialized AI agents based on predefined directives.

## Active Directives

Every time you start OpenCode, these agent assignments are automatically loaded:

### Routing Rules
- **Web & YouTube Searches** → Gemini Agent
  - Handles all internet searches, web browsing, and video content
  - Keywords: web, search, google, internet, online, browse, youtube, video, watch, tutorial

- **Research Tasks** → Research Agent
  - Information gathering, analysis, and documentation
  - Keywords: research, investigate, study, analyze, explore

- **Coding Tasks** → OpenCode Agent
  - Software development, debugging, programming
  - Keywords: code, program, script, function, debug, develop

- **Prompt Enhancement** → Claude Agent
  - Prompt optimization, creative writing, content improvement
  - Keywords: optimize, enhance, improve, prompt, rewrite

- **Local AI Tasks** → Ollama Agent
  - Offline processing, privacy-focused tasks
  - Keywords: local, offline, private, ollama

## How to Use

### Quick Start
```bash
# Start interactive session
python3 opencode_agent_startup.py

# Run specific task directly
python3 opencode_agent_startup.py "Search for marine electrical standards"
```

### Manual Orchestration
```bash
# Use the orchestrator directly
python3 agent_orchestrator.py "your task description"
```

## System Architecture

- **`routing_config.json`** - Defines routing rules and agent capabilities
- **`agent_orchestrator.py`** - Main orchestration engine
- **`opencode_agent_startup.py`** - Startup script with interactive mode
- **`agents/`** directory - Individual agent implementations

## Adding New Routing Rules

Edit `agents/routing_config.json` to add new routing rules:

```json
"new_rule": {
  "agent": "target_agent_id",
  "description": "What this rule does",
  "keywords": ["keyword1", "keyword2"],
  "priority": 1
}
```

## Agent Capabilities

- **Gemini Agent**: Web search, YouTube, multimodal content, current events
- **Research Agent**: Information gathering, analysis, documentation
- **OpenCode Agent**: Coding, debugging, software engineering
- **Claude Agent**: Prompt optimization, creative writing
- **Ollama Agent**: Local processing, privacy, offline work
- **Codex Agent**: ChatGPT integration, prompt enhancement

## Automatic Operation

The system automatically:
1. Analyzes your task description
2. Matches keywords to routing rules
3. Selects appropriate agents by priority
4. Optimizes prompts for each agent
5. Runs agents (parallel when possible)
6. Combines and presents results

## Examples

```
Task: "Find YouTube tutorials for boat wiring"
→ Routes to Gemini agent for YouTube search

Task: "Debug this Python function"
→ Routes to OpenCode agent for coding tasks

Task: "Research lithium battery technology"
→ Routes to Research agent, with web search support from Gemini
```