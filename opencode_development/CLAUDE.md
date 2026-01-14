# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Repository Overview

This repository contains documentation for the Bristol 32 sailboat "Liberty" (1974 model) and includes an AI agent orchestration system for automated task routing.

**Dual Purpose:**
1. **Documentation**: Research and information related to liveaboard sailing, equipment specifications, and boat details
2. **AI Agent System**: Automated task routing system that intelligently assigns tasks to specialized AI agents

The repository includes both documentation files and a functional AI agent orchestration system with persistent routing directives.

## Repository Contents

### Main Documentation Files

1. **liberty-info.md** - Comprehensive information about the Bristol 32 sailboat
   - Design heritage (Ted Hood & Dieter Empacher)
   - Hull construction and specifications
   - Interior layout and liveaboard suitability
   - Sailing characteristics
   - Owner sentiment and legacy information

2. **12v-marine-fans-report.md** - Detailed research on 12-volt marine fans for liveaboard use
   - 12 specific fan models with technical specifications
   - Power consumption analysis for limited battery capacity
   - Recommendations tiered by budget and performance
   - Installation and placement best practices
   - Accessibility considerations for physical limitations

3. **AI Agent System** - Automated task routing and orchestration
   - `agent_orchestrator.py` - Main orchestration engine
   - `opencode_agent_startup.py` - Interactive startup script
   - `agents/routing_config.json` - Persistent routing directives
   - Multiple specialized agents (Gemini, Claude, Ollama, etc.)
   - Automatic task analysis and agent assignment

## Context

The documentation is specifically tailored for:
- A 1974 Bristol 32 sailboat named "Liberty"
- Liveaboard cruising lifestyle
- Limited electrical capacity (200-400Ah battery bank)
- Physical considerations (shoulder, spine, and mobility challenges mentioned in fan report)

## Working with This Repository

### Documentation Guidelines
When editing or adding to this repository:
- Maintain the markdown format and clear section structure
- Keep technical specifications accurate and detailed
- Focus on practical, liveaboard-specific information
- Consider power efficiency and physical accessibility in recommendations
- Use clear headings and subsections for easy navigation

### AI Agent System

The repository includes an automated agent orchestration system with persistent directives:

**Active Routing Rules:**
- Web/YouTube searches → Gemini agent
- Research tasks → Research agent
- Coding tasks → OpenCode agent
- Prompt optimization → Claude agent
- Local AI tasks → Ollama agent

**Usage:**
```bash
# Start interactive agent session
python3 opencode_agent_startup.py

# Run specific task
python3 opencode_agent_startup.py "Search for marine electrical standards"
```

**System automatically:**
- Analyzes task requirements
- Routes to appropriate agents
- Optimizes prompts for each agent
- Runs agents in parallel when beneficial
- Combines and presents results

See `AGENT_ORCHESTRATION_README.md` for detailed documentation.
