//============================================================================
// Name        : FileSystem.hpp
// Author      : Jordan Villanueva, Omar Santiago, Matt Mishler
// Version     : 1.0
// Copyright   : This stuff is mine
// Description : First Project Lab
//============================================================================

#ifndef FILESYSTEM_HPP
#define FILESYSTEM_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>

class FileSystem {
    const static int B = 64; //Block length
    const static int K = 7; //Number of blocks for descriptor table
    const static int L = 64; //number of logical blocks for ldisk

    // This is aka cache. It's contents should be
    // maintained to be same as first K blocks in disk.
    // Descriptor table format:
    // +-------------------------------------+
    // | bitmap | dsc_0 | dsc_1 | .. | dsc_N |
    // +-------------------------------------+
    //   bitmap: Each bit represent a block in a disk. MAX_BLOCK_NO/8 bytes
    //   dsc_0 : Root directory descriptor
    //   dsc_i : i'th descriptor. Each descriptor is FILE_SIZE_FIELD+ARRAY_SIZE bytes long.
    unsigned char** desc_table; // Descriptor Table (in memory).
    unsigned char** ldisk; //represents hard disk
    unsigned char** oft; //open file table //has space for 3 files and 66 bytes for each file (64 for one block buffer + current position + file descriptor index)
    const static int DIRECTORY_MAX_SIZE = 154; //max size the directory file can take (14 files max * 11 bytes each per file)
    const static int FILE_SIZE_FIELD = 1; // Size of file size field in bytes. Maximum file size allowed in this file system is 192.
    const static int ARRAY_SIZE = 3; // The length of array of disk block numbers that hold the file contents.
    const static int DESCR_SIZE = FILE_SIZE_FIELD + ARRAY_SIZE;
    const static int MAX_FILE_NO = 14; // Maximum number of files which can be stored by this file system.
    const static int MAX_BLOCK_NO = 64; // Maximum number of blocks which can be supported by this file system.
    const static int MAX_FILE_NAME_LEN = 10; // Maximum size of file name in byte.
    const static int MAX_OPEN_FILE = 3; // Maximum number of files to open at the same time.
    const static int FILEIO_BUFFER_SIZE = 64; // Size of file io bufer
    const static int _EOF = -1; // End-of-File


public:
    FileSystem();
    FileSystem(int l, int b);
    ~FileSystem();

    void OpenFileTable();
    void closeFileTable();
    int find_oft();
    void deallocate_oft(int index);
    void format();
    unsigned char* read_descriptor(int no);
    void clear_descriptor(int no);
    void write_descriptor(int no, unsigned char* desc);
    int find_empty_descriptor();
    int find_empty_block();
    unsigned char fgetc(int index);
    int fputc(unsigned char c, int index);
    bool feof(int index);
    int search_dir(int index, std::string symbolic_file_name);
    void delete_dir(int index, int start_pos, int len);
    int create(std::string symbolic_file_name);
    int createFileInDirectory(int, std::string);
    int open_desc(int desc_no);
    int open(std::string symbolic_file_name);
    int read(int index, unsigned char* mem_area, int count);
    int write(int index, unsigned char value, int count);
    int lseek(int index, int pos);
    int close(int index);
    int deleteFile(std::string fileName);
    void directory();
    void diskdump(int start, int size);
    void restore();
    void save();
    void write_block(int i, unsigned char* p);
    void read_block(int i, unsigned char* p);
    std::string Remove_trailing(std::string name);
};

#endif /* FILESYSTEM_HPP */
