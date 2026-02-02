# Agent Skill: MoWatch Development

A [Claude Code](https://github.com/anthropics/claude-code) agent skill for developing watch faces and apps for MoWatch e-ink smartwatch.

## Installation

```bash
# Clone to your Claude skills directory
cd ~/.claude/skills
git clone https://github.com/yetone/agent-skill-mowatch-dev.git
```

## Usage

Once installed, Claude Code will automatically use this skill when you ask about MoWatch development. You can also explicitly invoke it:

```
/mowatch-dev
```

## What's Included

- **Hardware Specifications**: 200x200 e-ink display, ARM Cortex-M3
- **Bitmap Format**: Horizontal 8-point left-high-bit encoding
- **Project Structure**: Code organization and required SDK files
- **Docker Build Process**: Complete compilation workflow
- **API Reference**: Drawing, time, and system functions
- **Entry Points**: app_init, onDraw, onKey, onUpdate
- **Image Processing**: Python scripts for converting images to 1-bit bitmaps
- **Avatar Processing**: Skin color detection for clean face rendering
- **Deployment**: .mwa packaging and installation

## Example Projects

- [mowatch-scifi-avatar](https://github.com/yetone/mowatch-scifi-avatar) - Sci-Fi HUD watch face with avatar

## License

MIT
