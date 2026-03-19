[![Contributors](https://img.shields.io/badge/CONTRIBUTORS-01-blue?style=plastic)](https://github.com/zouari-oss/crun/graphs/contributors)
[![Forks](https://img.shields.io/badge/FORKS-00-blue?style=plastic)](https://github.com/zouari-oss/crun/network/members)
[![Stargazers](https://img.shields.io/badge/STARS-01-blue?style=plastic)](https://github.com/zouari-oss/crun/stargazers)
[![Issues](https://img.shields.io/badge/ISSUES-00-blue?style=plastic)](https://github.com/zouari-oss/crun/issues)
[![GPL3.0 License](https://img.shields.io/badge/LICENSE-GPL3.0-blue?style=plastic)](LICENSE)
[![Linkedin](https://img.shields.io/badge/Linkedin-6.9k-blue?style=plastic)](https://www.linkedin.com/in/zouari-omar)

<div align="center">
  <a href="https://github.com/zouari-oss/crun">
    <img src="res/img/logo.ico" alt="crun-logo" width="300">
  </a>
  <h1>Crun CLI</h1>
  <h6>C-based project scaffolding CLI</h6>
  <br />
</div>

<p align="center">
  <a href="#overview">Overview</a> •
  <a href="#key-features">Key Features</a> •
  <a href="#requirements">Requirements</a> •
  <a href="#usage">Usage</a> •
  <a href="#runtime-flow">Runtime Flow</a> •
  <a href="#template-sources">Template Sources</a> •
  <a href="#download">Download</a> •
  <a href="#emailware">Emailware</a> •
  <a href="#contributing">Contributing</a> •
  <a href="#license">License</a> •
  <a href="#contact">Contact</a> •
  <a href="#acknowledgments">Acknowledgments</a>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/c-%2300599C.svg?style=for-the-badge&logo=c&logoColor=white"/>
  <img src="https://img.shields.io/badge/bash_script-%23121011.svg?style=for-the-badge&logo=gnu-bash&logoColor=white"/>
  <img src="https://img.shields.io/badge/CMake-064F8C?style=for-the-badge&logo=cmake&logoColor=white"/>
  <img src="https://img.shields.io/badge/CLI-Command%20Line-informational?style=for-the-badge&logo=gnubash&logoColor=white"/>
  <img src="https://img.shields.io/badge/Cross--Platform-Yes-success?style=for-the-badge"/>
  <img src="https://img.shields.io/badge/Open%20Source-3DA639?style=for-the-badge&logo=opensourceinitiative&logoColor=white"/>
</p>

![screenshot](imgs/crun.png)

## Overview

**crun-cli** fetches template metadata, lets you choose a language/template, downloads or reuses cached archives, executes optional init scripts, and copies the extracted scaffold into your current directory.

## Key Features

- Interactive language/template menus
- Template cache in `~/.local/share/crun/`
- Hidden extraction workspace in `~/.local/share/crun/tmp/`
- Optional init execution:
  - `__init__.sh` (Unix)
  - `__init__.bat` (Windows)
- Init script cleanup after execution
- Copy extracted output only (zip artifacts are excluded)
- Timestamped audit logs in CLI and `~/.local/share/crun/audit.log`

## Requirements

- [CMake](https://cmake.org)
- [Ninja](https://github.com/ninja-build/ninja)
- [libcurl](https://github.com/curl/curl)
- [cJSON](https://github.com/DaveGamble/cJSON)
- [minizip](https://github.com/domoticz/minizip)

## Usage

```bash
git clone https://github.com/zouari-oss/crun-cli
cd crun-cli/project

./run rb   # configure
./run i    # build
./run r    # run crun
```

### CLI Flags

```bash
./bin/build/crun --help
./bin/build/crun --update
```

- `--help` prints CLI usage and notes.
- `--update` refreshes local `crun_stacks.json` from the remote source.

### Install Binary (optional)

```bash
sudo cp bin/build/crun /usr/local/bin/crun
```

## Runtime Flow

1. Load `crun_stacks.json` (download if missing)
2. Select language and template
3. Resolve template archive (cache hit or download)
4. Extract into hidden temp workspace
5. Run init script if present
6. Remove `__init__.sh` / `__init__.bat`
7. Copy output to current directory (excluding `*.zip`)
8. Clean temporary workspace

## Template Sources

- [cpkg templates](https://github.com/zouari-oss/cpkg/tree/main/templates)

## Download

You can [download](https://github.com/zouari-oss/crun-cli/releases) the latest installable version of crun-cli for Windows, macOS and Linux.

## Emailware

crun-cli is an emailware. Meaning, if you liked using this app or it has helped you in any way,
would like you send as an email at <zouariomar20@gmail.com> about anything you'd want to say about
this software. I'd really appreciate it!

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## License

This repository is licensed under the **GPL-3.0 License**. You are free to use, modify, and distribute the content. See the [LICENSE](LICENSE) file for details.

## Contact

For questions or suggestions, feel free to reach out:

- **GitHub**: [ZouariOmar](https://github.com/ZouariOmar)
- **Email**: [zouariomar20@gmail.com](mailto:zouariomar20@gmail.com)
- **LinkedIn**: [Zouari Omar](https://www.linkedin.com/in/zouari-omar)

## Acknowledgments

Built with ❤️ for the open-source community.
