# VERBATRON

<div align="center">
  <img src="assets/verbatron-banner.svg" alt="VERBATRON Banner" width="900"/>
</div>

<p align="center">
  <strong>A lightweight, terminal-based text editor inspired by vim, built in C with a focus on simplicity and performan
</p>

<p align="center">
  <a href="#features">Features</a>
  <a href="#installation">Installation</a>
  <a href="#usage">Usage</a>
  <a href="#commands">Commands</a>
  <a href="#license">License</a>
</p>

---

## Table of Contents

- [Features](#features)
- [Installation](#installation)
- [Usage](#usage)
  - [Basic Usage](#basic-usage)
  - [Opening Files](#opening-files)
  - [Navigation](#navigation)
  - [Editing](#editing)
  - [Command Mode](#command-mode)
- [Commands](#commands)
- [Key Bindings](#key-bindings)
- [Technical Details](#technical-details)
- [Building from Source](#building-from-source)
- [Contributing](#contributing)
- [License](#license)

## Features

- **Dual Mode Interface**: Input mode for editing and command mode for file operations
- **File Operations**: Open, save, and create files with vim-like commands
- **Dynamic Window Sizing**: Automatically adapts to terminal size
- **Line Numbers**: Grey-colored line numbers for easy navigation
- **Large File Support**: Handles files up to 1000 lines with 120 columns
- **Scrolling**: Both horizontal and vertical scrolling for large documents
- **Syntax-Free**: Clean, distraction-free editing environment
- **Fast Startup**: Minimal dependencies and quick load times
- **Cross-Platform**: Works on Linux and Unix-like systems

## Installation

### Prerequisites

- GCC compiler
- Make
- Linux/Unix terminal

### Quick Install

```bash
git clone https://github.com/yourusername/VERBATRON.git
cd VERBATRON
make
```

## Usage

### Basic Usage

Start VERBATRON:

```bash
./VERBATRON(or whatever you may call it)
```

Open a specific file:

```bash
./VERBATRON(or whatever you may call it) filename.txt
```

### Opening Files

VERBATRON starts in **Input Mode** by default. You can immediately start typing to create content.

### Navigation

**In Input Mode:**

- Arrow keys: Move cursor
- Enter: New line
- Backspace: Delete previous character

**In Command Mode:**

- Arrow keys: Move cursor without editing
- Type commands after the `:` prompt

### Editing

1. **Input Mode** (default): Type normally to insert text
2. **Command Mode**: Press `ESC` to enter, type commands

### Command Mode

Press `ESC` to enter command mode. You'll see a `:` prompt at the bottom of the screen.

## Commands

| Command          | Description               |
| ---------------- | ------------------------- |
| `:i` or `:input` | Switch to input mode      |
| `:w`             | Save current file         |
| `:w filename`    | Save as specific filename |
| `:o filename`    | Open file                 |
| `:q`             | Quit                      |
| `:wq`            | Save and quit             |

## Key Bindings

### Input Mode

| Key               | Action             |
| ----------------- | ------------------ |
| `ESC`             | Enter command mode |
| `Arrow Keys`      | Navigate cursor    |
| `Backspace`       | Delete character   |
| `Enter`           | New line           |
| `Ctrl+D`          | Exit program       |
| `Printable chars` | Insert character   |

### Command Mode

| Key          | Action                   |
| ------------ | ------------------------ |
| `ESC`        | Return to input mode     |
| `Arrow Keys` | Move cursor position     |
| `Enter`      | Execute command          |
| `Backspace`  | Delete command character |

## Technical Details

### Architecture

- **Buffer Size**: 1000 rows  120 columns
- **Display**: Dynamic window sizing (adapts to terminal)
- **Memory**: Static allocation for predictable performance
- **I/O**: Raw terminal mode for immediate key response

### File Format Support

- Plain text files
- Handles tabs (converted to 4 spaces)
- Unix line endings
- Printable ASCII characters (32-126)

### Performance

- **Startup**: < 1 second (includes splash screen)
- **File Loading**: Efficient character-by-character reading
- **Memory Usage**: ~120KB for text buffer
- **Scrolling**: Smooth horizontal and vertical scrolling

## Building from Source

### Build Commands

```bash
# Clean build
make re

# Regular build
make

# Clean only
make clean

# Remove all generated files
make fclean
```

### Project Structure

```
VERBATRON/
 includes/
    editor.h        # Function declarations
    includes.h      # System includes
    typedefs.h      # Type definitions and constants
 srcs/
    editor.c        # Core editor functions
    input.c         # Input handling and display
    main.c          # Program entry point
    term.c          # Terminal management
 obj/                # Object files (generated)
 Makefile           # Build configuration
 README.md          # This file
 verbatron.txt      # Splash screen content
```

### Compilation Flags

- `-Wall -Wextra -Werror`: Strict error checking
- `-Iincludes`: Include directory
- `C99 standard`

## Contributing

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

### Code Style

- Follow the existing code style
- Use tabs for indentation
- Keep functions under 25 lines when possible
- Comment complex logic

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- Inspired by vim and other terminal-based editors
- Built as a learning project to understand terminal programming
- Thanks to the open-source community for inspiration

---

**VERBATRON** - Simple, fast, distraction-free editing.

- this readme was written with VERBATRON v1.0
