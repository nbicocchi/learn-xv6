## Operating Systems Course - DIEF UNIMORE ##

In this exercise we will implement a kind of PATH variable. Our aim is to improve user experience, letting them the possibility to insert commands by their local name on the console, even if they are not in the same directory.

We need the user to be able to "export" a new path and to remove an already exported one. We will save every path in a file (.sh_profile) created in the "/" directory.


# exportpath.c
To create a new command, we need to create a new file .c and to add its name in the "UPROGS" list within the Makefile.
First we will check the number of parameters, which has to be 2. Once we know that, we can create a temporary buffer, read data from .sh_profile (if there are some), check if our path is not inserted already and add it at the end of the buffer. Eventually, we will write our buffer on the file.

# removepath.c
Here we have to create a new command as well.
After have checked the number of parameters, which has to be 2 even here, we will open .sh_profile and save its content on a temporary buffer. Then we can unlink the file to delete its data and eventually create it again, filling it with the content of our buffer.
Optional: insert an option -a to delete all the paths.

# sh.c
We have to modify sh.c because we need to call exec once for every path in our file plus once for the current directory (to be sure that every location is properly checked when the user use a command). It is not possible to do it within exec.c because we cannot handle files (it would cause conflicts between libraries).
In sh.c, about at line 80, exec is called for the current command. We have to open our file and write it on a temporary buffer, then, for every path, we have to call exec with path+command as first argument (in the current implementation, paths are separated by a '\n').

#exec.c
Eventually we have to delete the 26th line of exec.c (which prints an error for every exec failed), otherwise we would have an error for every path in .sh_profile which does not match an existing executable file in our system.
