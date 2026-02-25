# DumanShell 

A custom Unix shell implementation in C that moves beyond basic command execution into low-level terminal manipulation and efficient data structures. 

## Key Features

* **Raw Mode Input Engine:** Built using `termios.h`, the shell bypasses the standard terminal "canonical" mode. This allows for:
    * **Real-time character processing:** Handling input character-by-character rather than line-by-line.
    * **Custom Key Bindings:** Manual implementation of backspace, Enter, and arrow key signals.
    * **Non-ASCII Handling:** Logic to manage terminal escape sequences.
* **Live History Scrolling:** Features Up/Down arrow navigation through previous commands.
    * **Buffer Persistence:** Saves your current "in-progress" command to a backup buffer, allowing you to scroll through history and return to your original draft without losing it.
* **Circular History Buffer:** Implemented a ring buffer for command history to ensure O(1) performance and fixed memory footprint.
* **Advanced Line Editing:** Supports Left/Right arrow movement with manual cursor tracking and real-time screen re-rendering during mid-line insertions/deletions.

##  Technical Implementation

### Terminal Bit-masking
To achieve Raw Mode, I implemented bitwise filtering to disable `ECHO` and `ICANON`:

`raw_termios.c_lflag &= ~(ECHO | ICANON);`

This flips the bits for these specific flags while preserving all other terminal attributes, which are then loaded via `tcsetattr()`.

### Process Lifecycle
The shell follows the classic **Read-Interpret-Execute** loop:
1.  **Read:** Manual character collection in Raw Mode.
2.  **Interpret:** Tokenization via `strtok` and identifying built-ins vs. external commands.
3.  **Execute:** Creation of child processes using `fork()`, executing binaries via `execvp()`, and parent synchronization with `wait()`.

##  Roadmap
- [x] Basic Command Execution & Built-ins (`cd`, `exit`, `history`)
- [x] Raw Mode Terminal Configuration
- [x] Circular Array History List
- [x] Multi-directional Cursor Movement & Line Editing
- [ ] **Next Milestone:** Pipe Operator (|) for Inter-process Communication
- [ ] Output Redirection (>, <)

##  Build & Run
Ensure you have `gcc` and `make` installed.

1. Build: `make`
2. Run: `./dumanshell`

##  Credits & Acknowledgments
The initial process management logic (fork/exec loop) was inspired by [Manoj Singh Negi's Shell Tutorial](https://www.youtube.com/watch?v=vgxWYYdwKLc).The terminal raw mode handling (in part), the history buffer logic, and the line editing features were researched and implemented independently.
