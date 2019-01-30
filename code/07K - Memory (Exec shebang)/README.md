## Operating Systems Course - DIEF UNIMORE ##

Before we get into the implementation of this, we need a script with a shebang line (#!/interp) so that it will be interpreted and we will be able to debug easily as we implement the code. In Linux, edit a file called script1.sh as below:

```text
  #!/sh
  
  echo hello;
  ls;
```

Then, we need to modify the MAKEFILE so that this script is included in xv6’s filesystem. Find the following line and modify it as shown below:

```text
fs.img: mkfs README script1.sh $(UPROGS) 
./mkfs fs.img README script1.sh $(UPROGS) 
```

Now when we compile xv6, we will see a file named “script1.sh” in xv6’s filesystem.

Implementing a shebang consists of two parts. First, we need to have this functionality present in our kernel. Only after that, user level programs can benefit from it and interpret the contents of the script. Since xv6 is a small system, only our beloved “sh” has the capability to interpret the script. To implement the functionality in the kernel, “exec.c” is our first stop.

```text
  // Check Shebang
  char shebang[3];
  char interp_path[16]; // 16 is for historical reasons. 
    
  if(readi(ip, (char*)&shebang, 0, sizeof(shebang)) != sizeof(shebang)) 
    goto bad;
	
  if (shebang[0] == '#' && shebang[1] == '!') {
    // shebang[2] = '\0';
    // cprintf("shebang: %s\n", shebang);
    readi(ip, (char*)&interp_path, 2, sizeof(interp_path));
    
    // replaces new line (0xa) with end of buffer (\0) 
    for(i = 0; i < sizeof(interp_path); i++) {
      if(interp_path[i] == 0xa) {
        interp_path[i] = '\0';
        // cprintf("interp_path: %s\n", interp_path);
	break;
      }
    }
    
    // release fs resources
    iunlockput(ip);
    end_op();

    // execute the interpreter passing the script as the first argument
    // this implies a change in sh.c
    char *new_argv[] = {interp_path, path, 0};
    return exec(interp_path, new_argv);
  }
```

The code is pretty much self-explanatory. We read the first 3 bytes of the file into a buffer and check whether the file starts with “#!”.

You might say “Hey, how do you know the file actually has 3 bytes? Won’t this cause some problems?” The answer is no because this functionality is implemented in “exec.c”, so we expect that the files that come here are either ELF files, whose header size is already bigger than 3 bytes, or script files, which start with “#!” bytes anyway.

Later, we read the first 16 bytes into interp_path variable, starting from the second byte into the file. Again, you might ask “Why 16 bytes? The interpreter path might be longer!” That is correct. However, xv6 is a remake of the legendary UNIX v6, and traditionally, interpreter path was assumed as 16 bytes in the first implementations. You can check the history of it from here:

https://www.in-ulm.de/~mascheck/various/shebang/

One last thing, “0xa” is the newline in xv6, so it is used to separate commands from the shebang line. After that, we are done in “exec.c”, so we call “exec.c” itself, with interpreter path as the file to be executed, and the script as the argument. Since interpreter path is the first argument, which is what ip points to, first two bytes will not be “#!”, instead it will be the interpreter’s ELF binary.

Now that we are done, let’s see the code we need to write in “sh.c” in order to interpret the actual script content.

```text
  if ((argc >= 2)) {
    printf(1, "script execution...\n");

    fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
      printf(1, "sh could not open: %s\n", argv[1]);
      exit();
    }

    read_bytes = read(fd, buf, sizeof(buf));
    if(buf[read_bytes - 1] == '\n')
      buf[read_bytes - 1] = '\0';
    script_start = strchr(buf, '\n');
	
    if(fork1() == 0)
      runcmd(parsecmd(script_start));
    wait();
    exit();
  }
```

At first line, we are checking whether the program has only one argument. If it is, we don’t have any arguments, so this condition will not be met and sh will continue executing normally. Otherwise, we have an argument which needs to be interpreted. We just read the file, set a pointer (script_start) to the first newline (immediately after shebang) and treat the rest as a command to be executed. Semicolons are supported by sh.
