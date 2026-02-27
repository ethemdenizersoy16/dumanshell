# DumanShell 

A custom Unix shell implementation in C featuring low-level terminal manipulation, process pipelines, and efficient data structures.

##  Key Features

* **Multi-Stage Pipelines (|):** Supports chaining multiple commands using the pipe operator. Implements a sophisticated "FD-handoff" logic where each child process redirects its input/output to the pipe using `dup2`.
* **Non-Destructive History Scrolling:** Features Up/Down arrow navigation. If you modify a command while viewing it in history, your changes are tracked locally but do not overwrite the original history entry, preserving your command logs.
* **Raw Mode Input Engine:** Uses `termios.h` to handle input character-by-character. This supports manual cursor tracking, backspaces, and escape sequences for arrow keys.
* **Circular History Buffer:** Implements a ring buffer for command history to ensure O(1) performance.

##  Technical Implementation

### Pipeline Logic
The shell iterates through a list of piped commands, creating a `pipe()` for each stage except the last.
* **Redirection:** The child process uses `dup2` to wire `stdin` to the "previous read end" and `stdout` to the current pipe's "write end".
* **Resource Management:** Both parent and child processes carefully close unused file descriptors to prevent hanging processes and memory leaks.

### Terminal Control
To achieve Raw Mode, I implemented bitwise filtering to disable `ECHO` and `ICANON`:
`raw_termios.c_lflag &= ~(ECHO | ICANON);`

### Process Lifecycle
1. **Read:** Manual character collection in Raw Mode.
2. **Interpret:** Tokenization via `strtok`; `|` is replaced with null characters to split the arguments list.
3. **Execute:** Recursive/Loop-based `fork()` and `execvp()` calls for each pipeline stage.

##  Roadmap
- [x] Basic Command Execution & Built-ins
- [x] Raw Mode Terminal Configuration
- [x] Circular Array History List
- [x] Multi-directional Cursor Movement
- [x] Process Pipelines (|)
- [ ] Output Redirection (>, >>, <)

##  Build & Run
1. Build: `make`
2. Run: `./dumanshell`

##  Credits
The initial process management logic (fork/exec loop) was inspired by [Manoj Singh Negi's Shell Tutorial](https://www.youtube.com/watch?v=vgxWYYdwKLc). The terminal raw mode handling (in part), the history buffer logic, pipe handling, and the line editing features were researched and implemented independently.
