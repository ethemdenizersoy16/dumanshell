Hello, this is my devlog for the custom shell I am building.

I will be writing about the new functions I learn and implement, as well as the logic behind my code as I write it.

The shell is essentially an infinite while loop which repeats the same 3 steps:

Read

Interpret

Execute

Implementing the "read" part was easy enough. All you need is fgets and a string to store your input.

Interpretation is also quite simple. First, I needed to turn the command line input into tokens. Although I thought this would be quite difficult, it turns out string.h has a function called strtok, which made tokenization trivial. After this point, I needed my shell to cover two cases:

Built-in commands which the shell does itself (cd, exit)

Commands which use external, precompiled files (ls, cat)

For implementing built-in commands, I needed to rewrite them from scratch, which can be a bit complex depending on the command. However, C has a lot of built-in functions which help implement these. For example, the function int chdir(const char* path) allows you to change the current directory. This provides a simple solution to implementing cd. However, not all built-in functions were so simple. The implementation of history proved to be quite a challenge. I will delve into it later on.

For external commands, I needed a built-in C function, fork(). And in order to understand it, I first need to mention execvp(). int execvp(const char *file, char* const argv[]) takes two parameters: the name of the file to be executed and the list of arguments for that executable. Since the non-built-in commands exist as separate files, they can be used with the execvp function. However, once execvp is called and an external file is successfully run, the calling program is replaced by the called program. So using execvp to use a command would indeed work, but it would also shut down the shell. In order to prevent this, we use another function, fork().

fork() creates a child process identical to the calling program. The child process starts execution from the line in which fork was called (i.e., if fork was called on line 70, the child process will only execute the lines after 70). The child process uses an exact copy of the parent's memory, including the variable names and pointers. So with this, we can use external functions without closing our shell. The logic is the following:

Main Shell script calls fork() and starts to wait for the child to finish —> Child Script uses execvp() to use an external command —> The Child Script is replaced by the command executable —> Command is executed, child process finishes —> Main Shell continues.

As for the implementation of history, I used a circular array. By simply keeping track of the start index, end index, and the element count, I was able to successfully implement the history list. There are two history functions: history_add, which adds a new entry to the list, and history_print, which prints the current history list. The only problem (which I don't even know if it would be considered a problem) is that invalid commands are also stored in history. This is easy to overcome by calling history_add after the command was recognized. The history functions were implemented in a separate file and linked during compilation for modularity and code scalability.

In order to expand my shell, I decided to add the ability to scroll between previously used commands using the up and down arrow keys. This required a complete revamp of the input system. Since the up and down arrows aren't characters, but escape sequences, I changed my terminal from canonical mode to raw mode using the termios.h library. This meant turning off ECHO and ICANON through some bitwise operations. This meant two things:

The written characters won't show up on screen.

The input isn't processed after a newline is pressed, but after each character.

Before I move on to how I used this to implement this feature, I want to talk about the exact bitwise operation I used. The line of code looks like this:
raw_termios.c_lflag &= ~(ECHO | ICANON);

raw_termios is a variable of type struct termios. This is a struct defined in termios.h which can store terminal attributes in its fields. This essentially allows you to change some settings on the variable and load them onto your terminal using tcsetattr().

ECHO and ICANON are flags and they are 1 bit. This means their binary representations look similar to 00001. By using the bitwise OR operator, we create a mask, essentially combining the two bits. So if ICANON was 10000, the resulting mask would be 10001. ~ is the bitwise NOT operator; applying it to the mask yields a filter, flipping every bit. Our filter would look like this: 01110. And finally, we have &= (which is the same as x = x & y), which is the bitwise AND operator. This takes the bitwise conjunction of all flags with our filter; since all flags are 1 bit, the flags other than ECHO and ICANON are unaffected. In the end, ECHO and ICANON are both set to 0.

Now that the terminal is set to raw mode, we can start taking in input one by one. A while loop runs, reading one character on the terminal at a time. We handle 4 cases:

The user hit Enter: stop the loop and terminate.

The user hit Backspace: delete one character and move the cursor back.

The user hit an Up or Down arrow: display a command from history.

The user hit a letter: add it to the buffer and print it on screen.

These processes create the illusion of a canonical terminal, as the user cannot tell the difference. However, we do a lot of the things canonical mode does manually.

The up and down arrow handling uses a variable called step. step is set to 0 on each loop unless the input was one of the arrow keys. step keeps track of how deep into the history list we are. step 0 means we are not viewing any of the old commands. In that case, hitting the down arrow would do nothing. When the up arrow is pressed when step is 0, or down is pressed when step is 1, a special thing happens. For the former, the current state of the buffer is saved in a different variable named buffer_backup. As for the latter, the buffer_backup is loaded into the buffer and printed. This allows the user to come back to what they were writing after they scroll through history.

The right and left scrolling proved to be quite difficult, as it required an overhaul of the write and delete operations as well. The scrolling itself is quite simple as it only requires two lines of code. In order to handle the writing and deletion, I needed to track the position of the cursor at all times, which I did with the use of a variable called cursor. This integer takes values between 0 and len, with each number representing the position of the cursor starting from the far left.

Insertion and deletion both use a similar logic. First, in a for loop, the contents of the buffer are shifted left/right (depending on the operation) starting from the cursor position. As the buffer is being updated, the new characters are also being printed. This allows me to update both the screen and the buffer in a single for loop, without wasting time on the unchanged parts of the buffer. After this loop, the cursor ends up on the far right of the output, which I then push back to its original position.

I reworked the history list scrolling implementation so that you can make a change while viewing a command from history and can still return to your original input. However, I made it so that the original entries in history aren't changed. This way, if you make a mistake, you can still access the original command in the history list by simply scrolling to that index again.

I made it so that | is replaced with a null character in the args list. The indices of the pipes are also recorded in an integer array. The args list, the array containing the pipe indices, and the number of commands are passed on as arguments to a pipe loop function. The logic of the pipe is simple. At each command, a pipe is created in the kernel with pipe(). Then, dup2() is used to connect stdin and stdout to the read and write ends of this pipe. This is repeated until the last command is reached, in which case no new pipe is created.

The PIDs of each child process are saved in an array in order to check their exit status one by one. After the loop is over, waitpid() is called for each command so that the main process catches up. The exit status of the last command is then printed to the screen if it isn't 0. I also check for an exit signal from any of the commands in the pipe loop.

My next step will be to make my pipe handle signals. After that, I will try to solve input/output redirection.
 
