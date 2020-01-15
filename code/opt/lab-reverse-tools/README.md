# xv6 Reverse Tools
This repository provides some basic tools for Reverse Engineering for the xv6
operating system, such as an 8086 disassembler, an ELF32 analyzer and an ELF32
disassembler.

The software is provided as a patch for xv6, as such you must copy the `end`
folder into your xv6-public folder and recompile the whole project.

Versioning is provided from the original version of xv6 files.

You can find the intial source code of xv6 under
[begin/xv6-public](begin/xv6-public).

The first idea was to port [btbd's
disassembler](https://github.com/btbd/disassembler).
Turns out it really didn't work that well under xv6, moreover it used GOTOs and
labels, thus I decided to abandon that idea and to port [kesrut's
disassembler](https://github.com/kesrut/disasm8086)

The ELF32 analyzer  is written from scratch thanks to many online resources
\[1\]\[2\]\[3\].

## Directory structure
```
xv6-reverse-tools/
|-begin
This directory has the intial code of external sources.
|-end
This directory contains the actual patches for xv6.
```

# Patches
## 1. dasm
The `dasm` utility has been ported to xv6. If launched without parameters it
reads stdin as **binary data** and dissassembles it. If launched with a file
path as a parameters it opens the file, reads it and disassembles it.

IT DOES NOT search for the code, it just "stupidly" disassembles.

You can test this program by running `dasm DISASMME`, the output should be this:
```
0  55              push bp
1  31D2            xor dx,dx
3  89E5            mov bp,sp
5  8B458          mov ax,[di+0x8]
8  56              push si
9  8B75C          mov si,[di+0xC]
C  53              push bx
D  8D58FF          lea bx,[bx+si-0x1]
10  F              db 0xF
11  B6C            mov dh,0xC
13  16              push ss
14  884C13          mov [si+0x13],cl
17  183C21        add [bp+di+0x1C2],ax
1B  84C9            test cl,cl
1D  75F1            jnz 0x10
1F  5B              pop bx
20  5E              pop si
21  5D              pop bp
22  C3              ret
```
As explained before, you could also call the command by invoking `cat DISASMME |
dasm`.
## 2. standard library expansion
Various functions have been added to the standard library, such as `sprintf`,
`strcat`, `strncat` and `memcpy`. Those were needed to port `dasm`. These
changes can be found in `printf.c`, `user.h` and `ulib.c`.
## 3. filesystem double indirection
The inital `dasm` binary (the one from btbd's disassembler)
couldn't fit the maximum file size, which is 140 blocks.
By default each block is made out of 512 bytes, and each file can have 12 direct
blocks and 128 indirect blocks, for a total of 140 blocks \[4\].
I edited the filesystem (`fs.c`, `fs.h`) to use double indirection instead of
direct one, now the filesystems max file size is _12 + (128\*128)_ = **16396**
blocks.

The `big` utility is present to check the correctness of this functionality.

To support the "embedding" of largefiles in the initial filesystem (`fs.img`) I
had to further edit `mkfs.c` to support double indirection and `param.h` to have
a larger filesystem.
## 4. sectdump tool
To enable disassemblying of real binary executable files, I've implemented a
little program, which is able to parse ELF files and to dump sections based on
name.
```
USAGE: sectdump BINARY_NAME [SECTION_NAME]
```
If no section name is provided, `.text` will be used as default.

An example of usage, is either to dump the string table (E.G. `sectdump cat
.strtab`) or in pipeline with `dasm` (E.G.`sectdump sectdump | dasm`).


# Known bugs
Only the `bmap` function has been implemented, and as a consequence of this,
deletion of "large files" (files exceeding the direct block size, which is
12) will not free the double indirected blocks correctly.

# Author

antipatico

# License

Read the file [LICENSE](LICENSE) in this directory.

# References

1. [The 101 of ELF files on Linux: Understanding and
Analysis](https://linux-audit.com/elf-binaries-on-linux-understanding-and-analysis/)- Linux Audit
2. [Executable and Linkable Format (ELF)](https://stevens.netmeister.org/631/elf.html) - charngda
3. Executable and Linkable Format (ELF) - Tool Interface Standards (TIS)
4. [Homework: bigger files for xv6](https://pdos.csail.mit.edu/6.828/2018/homework/xv6-big-files.html) - MIT
