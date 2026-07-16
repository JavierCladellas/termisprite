  # Termisprite

Termisprite is a terminal-based pixel art sprite editor written in C++. It allows you to create, edit, and manipulate pixel art directly from your command-line interface.

## Features

* **Terminal-Based Interface:** No graphical desktop environment required; runs directly in your terminal window.
* **Pixel Art Editing:** Built-in tools for drafting and drawing sprites using standard keyboard inputs.

## Installation

### Package Managers (TODO)

*Note: Official package repository distribution is currently planned for a future release.*

**Debian/Ubuntu (APT):**
```bash
# TODO: sudo apt install termisprite
```

**Arch Linux (Pacman):**
```bash
# TODO: sudo pacman -S termisprite
```

### Building from Source

To build Termisprite locally from source, ensure your system has the following prerequisites installed:

* A modern C++ compiler (GCC, Clang, or MSVC)
* CMake (version 3.10 or higher)
* A build system (Make, Ninja, etc.)

**1. Clone the repository:**
```bash
git clone https://github.com/JavierCladellas/termisprite.git
cd termisprite
```

**2. Configure the build environment:**
This project uses standard CMake and includes a `CMakePresets.json` to simplify the build process.
```bash
cmake --preset default
```

**3. Build the executable:**
```bash
cmake --build --preset default
```

**4. Install the package:**
To run the command globally, install the built files to your system path. You may need elevated privileges (e.g., `sudo`) depending on your operating system.
```bash
cmake --install --preset default
```

## Usage

Once installed, you can launch the editor from any directory in your terminal:

```bash
termisprite
```

## Contributing

Contributions to Termisprite are highly appreciated. To ensure a smooth collaboration process, please review the following guidelines:

### Reporting Bugs and Requesting Features
* Use the GitHub Issues tracker to report bugs or suggest new features.
* Prior to creating a new issue, please search existing open issues to avoid duplicates.
* When reporting bugs, include detailed steps to reproduce the issue, your operating system, and the terminal emulator you are using.

### Development Workflow
1. Fork the repository and create your feature branch from `main` (`git checkout -b feature/my-new-feature`).
2. Write clear and descriptive commit messages.
3. Ensure your code aligns with the existing project structure and style.
4. Test your changes locally to ensure they do not introduce regressions.
5. Push your branch (`git push origin feature/my-new-feature`) and open a Pull Request.
6. Provide a comprehensive description of your changes within the Pull Request.

## License

This project is licensed under the GNU General Public License v3.0 - see the [LICENSE](LICENSE) file for details.
