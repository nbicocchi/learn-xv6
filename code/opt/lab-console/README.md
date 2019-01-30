## Operating Systems Course - DIEF UNIMORE ##

In this exercise we will improve the console, making it more like Unix. In particular, we are talking about a very simple implementation of the clear function and some changes of the keys on our keyboard.

# clear.c
Here we have to create a new command: to do that we need to create a new file .c and to add its name in the "UPROGS" list within the makefile.
Our console has 80 characters per line and 24 lines in total, so it is sufficient to print '\n' 24 times to clear the screen.

# console.c
In console.c the system handles user input. Every key on the keyboard is associated to a symbol in the ASCII map (function consoleintr(), line 318), we want to change some particular keys like TAB and the four arrows.

# TAB
In Unix, TAB is used to complete the command the user is writing, but to do that we would need to handle files (which is not possible within console.c due to import conflicts), so if the key is TAB (c == 9) we are simply writing 4 spaces on the console.

To write a character on the console we have to understand how the console is done: there is a struct buffer named "input" which contains a buffer (128 characters long) where every character is stored; when we reach its end we have to rewrite our characters from the beginning.
Input also has three unsigned integers: 'w', 'e' and 'r'. Input.e is the edit index, which, used along with '%' operator and the buffer dimension (INPUT_BUF), indicates the position of last character of the current line in the buffer: we have to update it whenever we write a character. Input.w is the writing index, it is updated automatically when the user presses ENTER (basically the beginning of the current line); input.r is the read index, but we are not going to use it.
* e.g.	Insertion of the character ' ' (space):
	input.buf[input.e++ % INPUT_BUF] = ' ';
        consputc(' ');

# Left and right arrows
We are going to use the left arrow the exact same way of BACKSPACE, and the right one to restore the characters we have just deleted.
* e.g.	Delete a character:
	if(input.e != input.w) {
            input.e--;
       	    consputc(BACKSPACE);
      	}
To restore deleted characters it is sufficient to follow the character insertion method, incrementing input.e but without writing on input.buf (the character is already written on the buffer). Be aware that you still have to use consputc() function and it is not possible to restore unwritten characters, so we have to be sure that the character we are restoring is not 0 (the end of the buffer, if we haven't reached 128 characters yet) or '\n', because if we have written more than 128 characters then our buffer is already full, so we may restore previous code lines. We still do it, because it is not possible to know where the current line finishes, but at least we stop at the first '\n'.

# Up and down arrows
The up arrow in Unix restores the previous command the user has entered. Since we don't have a main() function in console.c we cannot create variables or arrays where to store our commands, so the best we can do is to create a function and call it whenever we press the up or down arrow.
In this function we will check if no command has been submitted yet (exit from the function without doing anything in this case), then we have to delete the whole current line, check if there are some '\n' before the last command (going backwards) and remember the position we have reached. Eventually, starting from the beginning of the buffer, we will check every '\n' until the position we have remembered and we will print the string starting from the last one.

# BONUS: second-last command
Here you have to change the function you just made to let it know if it has already been used, in which case it has to print the second-last command entered. You can use whatever algorithm you want, here it follows the one we used.

Check if no command have been submitted yet, copy the current line in a temporary buffer, skip all the '\n' and remember the position, check if the current line is equal to the last command; if it is true, skip all the '\n' between the two commands. Now delete the current line, check for the previous command using the last '\n' before the remembered position and print it on the screen.
