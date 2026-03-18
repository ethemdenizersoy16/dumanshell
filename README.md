# DumanShell (Custom Unix Shell)
[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
## Project Summary
DumanShell is a functional Unix shell built from the ground up in C to explore low-level systems programming. It bypasses high-level abstractions to interact directly with the kernel for process management and I/O control. The latest **V1.0** release introduces a robust I/O redirection system, completing the shell's core functionality.

**For a detailed walkthrough of the "why" and "how" behind the implementation logic and engineering challenges, please see the [DEVLOG.md](./DEVLOG.md).**

---

## Core Features (V1.0)
The V1.0 release marks the completion of the fundamental Unix shell features:

* **Process Management:** Full support for external command execution using the `fork-exec-wait` lifecycle.
* **Multi-Stage Pipelines (|):** Recursive implementation of pipelines using `pipe()` and `dup2()` for seamless inter-process communication.
* **I/O Redirection:** New support for input redirection (`<`), output overwrite (`>`), and output append (`>>`) with proper file descriptor restoration.
* **Raw Mode Input Engine:** Custom terminal handling via `termios.h` to support manual cursor tracking, backspaces, and real-time history scrolling.
* **Non-Destructive History Scrolling:** Uses a **Circular Array** for O(1) performance, allowing you to modify history entries locally without overwriting the original logs.



## Build & Run
**Build:** `make`  
**Run:** `./dumanshell`

## Credits
The initial process management logic (fork/exec loop) was inspired by [Manoj Singh Negi's Shell Tutorial](https://www.youtube.com/watch?v=vgxWYYdwKLc). The terminal raw mode handling (in part), the history buffer logic, pipe handling, I/O redirection, and the line editing features were researched and implemented independently.
