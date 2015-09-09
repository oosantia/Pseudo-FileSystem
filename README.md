# Pseudo-FileSystem
File System using char arrays.
PROJECT OVERVIEW
This project develops a simple file system using an emulated I/O system. The following diagram shows the basic organization:
 
The user interacts with the file system using commands, such as create, open, or read file. The file system views the disk as a linear sequence of logical blocks numbered from 0 to L – 1. The I/O system uses a memory array to emulate the disk and presents the logical blocks abstraction to the file system as its interface. 
This project will be decomposed into 3 different layers, 1) The I/O System, 2) The File System, and 3) The Presentation Shell. 

1	THE INPUT/OUTPUT SYSTEM
The physical disk is a two-dimensional structure consisting of cylinders, tracks within cylinders, sectors within tracks, and bytes within sectors. The task of the I/O system is to hide the two-dimensional organization by presenting the disk as a linear sequence of logical blocks, numbered 0 through L – 1, where L is the total number of blocks on the physical disk.
We will model the disk as a character array ldisk[L][B], where L is the number of logical blocks and B is the block length, i.e., the number of bytes per block. The task of the I/O system is to accept a logical block number from the file system and to read or write the corresponding block into a memory area specified by the command.


2	THE FILE SYSTEM
The file system is built on top of the emulated I/O system described above.

2.1.1	The Directory File
There is only one directory file to keep track of all files. This is just like an ordinary file, except it is never explicitly created or destroyed. It corresponds to the very first file descriptor on the disk (see diagram). Initially, when there are no files, it contains length 0 and has no disk blocks allocated. As files are created, it expands.
The directory file is organized as an array of entries. Each entry contains the following information:
•	symbolic file name;
•	index of file descriptor.

2.2	Interface Between User and File System
The file system must support the following functions: create, destroy, open, close, read, write, lseek, and directory.
•	create(symbolic_file_name): create a new file with the specified name.
•	destroy(symbolic_file_name): destroy the named file.
•	open(symbolic_file_name): open the named file for reading and writing; return an index value which is used by subsequent read, write, lseek, or close operations.
•	close(index): close the specified file.
•	read(index, mem_area, count): sequentially read a number of bytes from the specified file into main memory. The number of bytes to be read is specified in count and the starting memory address in mem_area. The reading starts with the current position in the file.
•	write(index, char, count): write 'count' number of 'char'(s) to the file indicated by index. Writing should start from the point pointed by current position of the file. Current position should be updated accordingly.
•	lseek(index, pos): move the current position of the file to pos, where pos is an integer specifying the number of bytes from the beginning of the file. When a file is initially opened, the current position is automatically set to zero. After each read or write operation, it points to the byte immediately following the one that was accessed last. lseek permits the position to be explicitly changed without reading or writing the data. Seeking to position 0 implements a reset command, so that the entire file can be reread or rewritten from the beginning.
•	directory: list the name of all files and their lengths.



3	THE PRESENTATION SHELL
The functionality of the file system can be tested by developing a set of programs to exercise various functions provided by the file system. To demonstrate your project interactively, develop a presentation shell (similar to the one of the process and resource manager) that accepts commands from your terminal, invokes the corresponding functions of the file system, and displays the results on your terminal. For example, cr <name> creates a new file with the specified name; op <name> opens the named file for reading and writing and displays an index value on the screen; and rd <index> <count> reads the number of bytes specified as <count> from the open file <index> and displays them on the screen.
