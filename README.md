# MyShell

A simple Unix-style command-line shell written in C, as a lab exercise.  
Features:

- **External command execution** via `fork()` + `execvp()`
- **Built-in commands**:  
  - `cd <dir>`: change working directory  
  - `quit`: exit the shell  
  - **Signal control**:  
    - `halt <pid>` → send `SIGTSTP` (suspend)  
    - `wakeup <pid>` → send `SIGCONT` (resume)  
    - `ice <pid>` → send `SIGINT` (interrupt)  
- **I/O redirection**: `< infile`, `> outfile`  
- **Blocking / non-blocking** (`&` suffix)  
- **Debug mode** (`-d`): prints child PID & command before execution  
- **Custom parser** with support for pipelines (via `LineParser.h` / `LineParser.c`)
