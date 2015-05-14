//============================================================================
// Name        : FileSystem.cpp
// Author      : Jordan Villanueva, Omar Santiago, Matt Mishler
// Version     :
// Copyright   : Your copyright notice
// Description : First Project Lab
//============================================================================

#include "FileSystem.hpp"

/* Constructor of this File system.
 *   1. Initialize IO system.
 *   2. Format it if not done.
 */
FileSystem::FileSystem()
{
	ldisk = new unsigned char*[L];
	for(int i = 0; i < L; ++i)
	{
		ldisk[i] = new unsigned char[B];
	}
	desc_table = new unsigned char*[K];
	for(int i = 0; i < K; ++i)
	{
		desc_table[i] = new unsigned char[B];
	}
	OpenFileTable();
	format();
}

FileSystem::FileSystem(int l, int b)
{
	ldisk = new unsigned char*[L];
	for(int i = 0; i < L; ++i)
	{
		ldisk[i] = new unsigned char[B];
	}

	desc_table = new unsigned char*[K];
	for(int i = 0; i < K; ++i)
	{
		desc_table[i] = new unsigned char[B];
	}
	OpenFileTable();
	format();
}

FileSystem::~FileSystem()
{
	for(int i = 0; i < L; ++i)
		delete[] ldisk[i];
	delete[] ldisk;

	for(int i = 0; i < K; ++i)
		delete[] desc_table[i];
	delete[] desc_table;

	closeFileTable();
}

// Open File Table(OFT).
//constructor for oft
void FileSystem::OpenFileTable()
{
	oft = new unsigned char*[ARRAY_SIZE];
	for(int i = 0; i < ARRAY_SIZE; ++i)
		oft[i] = new unsigned char[B + FILE_SIZE_FIELD + FILE_SIZE_FIELD];
}

//destructor for oft
void FileSystem::closeFileTable()
{
	for(int i = 0; i < ARRAY_SIZE; ++i)
		delete[] oft[i];
	delete[] oft;
}

// Allocate open file table
int FileSystem::find_oft()
{
	for(int i = 0; i<3;i++)
		if(oft[i][B+FILE_SIZE_FIELD] == '0')
			return i;
	return -1;
}

//Deallocate
void FileSystem::deallocate_oft(int index)
{
	for(int i = 0; i< B + FILE_SIZE_FIELD + FILE_SIZE_FIELD;i++ )
		oft[index][i] = '0';
}

/* Format file system.
 *   1. Initialize the first K blocks with zeros.
 *   2. Create root directory descriptor for directory file.
 * Parameter(s):
 *   none
 * Return:
 *   none
 */
void FileSystem::format()
{
	for(int i = 0; i < L; ++i)
	{
		for(int j = 0; j < B; ++j)
		{
			if(i < K)
				desc_table[i][j] = '0';
		}
		write_block(i,desc_table[0]);
	}
	desc_table[0][K] = '1';
	desc_table[0][K+1] = '1';
	desc_table[0][K+2] = '1';
	write_block(0,desc_table[0]);
	for(int i = 0; i<3;++i)
		for(int j = 0; j < B+FILE_SIZE_FIELD+FILE_SIZE_FIELD;j++)
			oft[i][j] = '0';
}


/* Read descriptor
 * Parameter(s):
 *    no: Descriptor number to read
 * Return:
 *    Return char[4] of descriptor
 */
unsigned char* FileSystem::read_descriptor(int no)
{
	unsigned char* desc = new unsigned char[4];
	if (no > 3)
		for(int i = 0; i<4;i++)
			desc[i] = desc_table[(no)/2][((no%2)*4)+i];
	else
		for(int i = 0; i<4;i++){
			desc[i] = desc_table[1][((no-1)*4)+i];
		}
	return desc;
}


/* Clear descriptor
 *   1. Clear descriptor entry
 *   2. Clear bitmap
 *   3. Write back to disk
 * Parameter(s):
 *    no: Descriptor number to clear
 * Return:
 *    none
 */
void FileSystem::clear_descriptor(int no)
{
	unsigned char* desc = read_descriptor(no);
	for(int i = 1; i < 4;i++){
		if(desc[i] != '0')
			desc_table[0][static_cast<int>(desc[i])] = '0';
	}
	write_block(0,desc_table[0]);
	for(int i = 0; i< 4;i++)
	{
		desc[i] = '0';
	}
	write_descriptor(no,desc);
	delete [] desc;
}


/* Write descriptor
 *   1. Update descriptor entry
 *   2. Mark bitmap
 *   3. Write back to disk
 * Parameter(s):
 *    no: Descriptor number to write
 *    desc: descriptor to write
 * Return:
 *    none
 */
void FileSystem::write_descriptor(int no, unsigned char* desc)
{
	int block = 1;
	int index = 0;
	if(no  > 3){
		block =  (no)/2;
		index =  ((no)%2)*4;
	}
	else
		index = (no-1) * 4;
	for(int i = 0;i<4;i++,desc++ ){
		desc_table[block][index+i] = *(desc);
	}
	write_block(block,desc_table[block]);
}


/* Search for an unoccupied descriptor.
 * If ARRAY[0] is 0, this descriptor is not occupied.
 * Then it returns descriptor number.
 * Return: -1 if no available descriptor found
 */
int FileSystem::find_empty_descriptor()
{
	int max;
	int index = 0;
	for(int i = 1; i < K; ++i)
	{
		if(i == 1)
			max = 12;
		else
			max = 8;
		for(int j = 0; j < max; j+=4){
			index++;
			if(desc_table[i][j] == '0')
				return index;
		}
	}
	return -1;
}


/* Search for an unoccupied block.
 *   This returns the first unoccupied block in bitmap field.
 *   Return value -1 means all blocks are occupied.
 * Parameter(s):
 *    none
 * Return:
 *    Returns the block number
 *    -1 if not found
 */
int FileSystem::find_empty_block()
{ // was returnning i even if  i > 14*3+k maxfiles * 3blocks + desc_table actual maximum capacity of LDISK
	for(int i = K; i < MAX_FILE_NO * ARRAY_SIZE +K ; i++)
		if(desc_table[0][i] == '0')
			return i;
	return -1;
}


/* Get one character.
 *    Returns the character currently pointed by the internal file position
 *    indicator of the specified stream. The internal file position indicator
 *    is then advanced to the next character.
 * Parameter(s):
 *    index (descriptor number of the file to be added.)
 * Return:
 *    On success, the character is returned.
 *    If a writing error occurs, EOF is returned.
 */
unsigned char FileSystem::fgetc(int index)
{
	int cp = static_cast<int>(oft[index][B]);
	unsigned char* desc = new unsigned char[4];
	desc = read_descriptor(static_cast<int>(oft[index][B+FILE_SIZE_FIELD]));
	if(feof(index))
		return _EOF;
	unsigned to_return = oft[index][cp%64];
	cp++;
	if(cp% 64 == 0 )
	{
		int block = (cp == 64 ? 2: 3);
		write_block(desc[block-1],oft[index]);
		if(oft[index][cp%64] != static_cast<char>(_EOF))
			read_block(desc[block],oft[index]);
		else
		{
			delete [] desc;
			return _EOF;
		}
	}
	oft[index][B] = cp;
	delete [] desc;
	return to_return;
}


/* Put one character.
 *    Writes a character to the stream and advances the position indicator.
 *    The character is written at the position indicated by the internal position
 *    indicator of the file, which is then automatically advanced by one.
 * Parameter(s):
 *    c: character to write
 *    index (descriptor number of the file to be added.)
 * Return:
 *    On success, the character written is returned.
 *    If a writing error occurs, -2 is returned.
 */
int FileSystem::fputc(unsigned char c, int index)
{
	unsigned char* desc = new unsigned char[4];
	int no = oft[index][B+FILE_SIZE_FIELD];
	desc = read_descriptor(no);
	int cp = static_cast<int>(oft[index][B]);
	if(cp == 191)
		return _EOF;
	if(cp < 64)
		oft[index][cp] = c;
	else
		oft[index][cp%64] = c;
	cp++;
	if (cp > desc[0])
	{
		desc[0]  = static_cast<int>(desc[0])+1;
		write_descriptor(no,desc);
	}
	if (cp% 64 == 0){
		int current_block = (cp == 64 ? 2: 3);
		if(desc[current_block] == '0')
		{
			desc[current_block] = find_empty_block();
			desc_table[0][desc[current_block]] = '1';
			write_descriptor(no,desc);
			write_block(0,desc_table[0]);
		}
		write_block(static_cast<int>(desc[current_block-1]),oft[index]);
		read_block(static_cast<int>(desc[current_block]),oft[index]);
	}
	oft[index][B] = cp;
	delete [] desc;
	return c;
}


/* Check for the end of file.
 * Parameter(s):
 *    index (descriptor number of the file to be added.)
 * Return:
 *    Return true if end-of-file reached.
 */
bool FileSystem::feof(int index)
{
	return oft[index][oft[index][B]%64] == _EOF;
}


/* Search for a file
 * Parameter(s):
 *    index: index to start looking at in directory
 *    st: The name of file to search.
 * Return:
 *    index: An integer number position of found file entry.
 *    Return -1 if not found.
 */
int FileSystem::search_dir(int index, std::string symbolic_file_name)
{
	//convert this to use ldisk directly and so it only looks at the first 55 chars for the first 2 directory blocks and first 33 for the third
	std::string directory;
	int block = K;
	int j = index;
	unsigned char *to_read = new unsigned char[64];
	read_block(block,to_read);
	for(int i = 1; i < MAX_FILE_NO; i++){
		for(int k =0; k< 10; k++){
			directory += to_read[j+k];
		}
		directory = Remove_trailing(directory);
		if(directory == symbolic_file_name)
			return to_read[j+10];
		j+=11;
		directory = "";
		if(j == 55){
			block +=1;
			read_block(block,to_read);
			j = 0;
		}
	}
	return -1;
}


/* Clear a file entry from directory file
 *
 * Parameter(s):
 *    index: open file table index
 *    start_pos:
 *    length:
 * Return:
 *    none
 */
void FileSystem::delete_dir(int index, int start_pos, int len)
{
	unsigned char* to_read = new unsigned char[B];
	read_block(index, to_read);
	for(int i = 0; i< 11;i++)
	{
		to_read[start_pos + i] = '0';
	}
	write_block(index,to_read);
	delete [] to_read;
}


/* File creation function:
 *    1. creates empty file with file size one.
 *    2. makes/allocates descriptor.
 *    3. updates directory file.
 * Parameter(s):
 *    symbolic_file_name: The name of file to create.
 * Return:
 *    Return 0 for successful creation.
 *    Return -1 for error (no space in disk)
 *    Return -2 for error (for duplication)
 */
int FileSystem::create(std::string symbolic_file_name)
{
	int directoryIndex = search_dir(0, symbolic_file_name);//index in the directory file for the new file
	if(directoryIndex != -1)
		return -2;

	int descIndex = find_empty_descriptor();//index in the descriptor table
	int blockIndex = find_empty_block();//index of the first available block in bytemap
	if(blockIndex == -1 || descIndex == -1)
		return -1;
	unsigned char* to_read = new unsigned char[B];
	unsigned char* desc = new unsigned char[4];
	int block = 1;
	if(descIndex  > 3){
		block =  (descIndex/2);
	}
	desc_table[0][blockIndex] = '1';
	write_block(0,desc_table[0]);
	read_block(blockIndex,to_read);
	desc[0] = 0;
	desc[1] = blockIndex;
	desc[2] = '0';
	desc[3] = '0';
	to_read[0] = _EOF;
	write_descriptor(descIndex,desc);
	write_block(blockIndex,to_read);
	write_block(block,desc_table[block]);
	delete [] to_read;
	delete [] desc;
	return createFileInDirectory(descIndex, symbolic_file_name);
}

//used in create function to insert new file name and position into directory file
int FileSystem::createFileInDirectory(int descIndex, std::string symbolic_file_name)
{
	unsigned char *to_read = new unsigned char[B];
	int block = K;
	read_block(block,to_read);
	int j = 0;
	for(int i = 1; i < MAX_FILE_NO; i++){
		if(to_read[j] == '0'){
			for(unsigned int k = 0; k < symbolic_file_name.size();k++)
				to_read[j+k] = symbolic_file_name[k];
			to_read[j+10] = descIndex;
			write_block(block,to_read);
			break;
		}
		j+=11;
		if(j >= 55){
			block +=1;
			read_block(block,to_read);
			j = 0;
		}
	}
	delete [] to_read;
	return 1;
}
/* Open file with descriptor number function:
 * Parameter(s):
 *    desc_no: descriptor number
 * Return:
 *    index: index if open file table if successfully allocated.
 *    Return -1 for error.
 *
 */
int FileSystem::open_desc(int desc_no)
{
	int block = K;
	int index = 0;
	unsigned char* to_read = new unsigned char[K];
	read_block(block,to_read);
	for(int i = 0;i<MAX_FILE_NO;i++){
		if( desc_no == static_cast<int>(to_read[index+10]))
			break;
		index +=11;
		if( index == 55){
			block++;
			read_block(block,to_read);
			index = 0;
		}
	}
	std::string file;
	for(int i = 0;i < 11; i++){
		if(to_read[index+i] == '0')
			break;
		file += desc_table[block][index+i];
	}
	delete [] to_read;
	return open(file);
}


/* Open file with file name function:
 * Parameter(s):
 *    symbolic_file_name: The name of file to open.
 * Return:
 *    index: An integer number, which is a index number of open file table.
 *    Return -1 or -2 if it cannot be open.
 */
// TODOs:
// 1. Open directory file
// 2. Search for a file with given name
//    Return -1 if not found.
// 3. Get descriptor number of the found file
// 4. Looking for unoccupied entry in open file table.
//    Return -2 if all entry are occupied.
// 5. Initialize the entry (descriptor number, current position, etc.)
// 6. Return entry number
int FileSystem::open(std::string symbolic_file_name)
{
	int found = search_dir(0,symbolic_file_name);
	if(found == -1)
		return -1;
	for(int i = 0 ; i < 3; i++)
	{
		if(oft[i][B+FILE_SIZE_FIELD] == found)
			return -1;
	}
	int index = find_oft();
	if(index == -1)
		return -1;
	oft[index][B + FILE_SIZE_FIELD] = found;
	oft[index][B] = 0;
	unsigned char*desc = read_descriptor(found);
	read_block(desc[1],oft[index]);
	delete [] desc;
	return index;


}


/* File Read function:
 *    This reads a number of bytes from the the file indicated by index.
 *    Reading should start from the point pointed by current position of the file.
 *    Current position should be updated accordingly after read.
 * Parameter(s):
 *    index: File index which indicates the file to be read.
 *    mem_area: buffer to be returned
 *    count: number of byte(s) to read
 * Return:
 *    Actual number of bytes returned in mem_area[].
 *    -1 value for error case "File hasn't been open"
 *    -2 value for error case "End-of-file"
     TODOs:
        1. Read the open file table using index.
           1.1 Get the file descriptor number and the current position.
           1.2 Can't get proper file descriptor, return -1.
        2. Read the file descriptor
           2.1 Get file size and block array.
        3. Read 'count' byte(s) from file and store in mem_area[].
           3.1 If current position crosses block boundary, call read_block() to read the next block.
           3.2 If current position==file size, stop reading and return.
           3.3 If this is called when current position==file size, return -2.
           3.4 If count > mem_area size, only size of mem_area should be read.
           3.5 Returns actual number of bytes read from file.
           3.6 Update current position so that next read() can be done from the first byte haven't-been-read.
 */
int FileSystem::read(int index, unsigned char* mem_area, int count)
{
	int no = static_cast<int>(oft[index][B+FILE_SIZE_FIELD]);
	if(no < 1 && no > 14)
		return -1;
	unsigned char* desc = new unsigned char[4];
	desc = read_descriptor(no);
	int cp = static_cast<int>(oft[index][B]);
	if(cp == static_cast<int>(desc[0]))
	{
		delete []desc;
		return -2;
	}
	int char_read = 0;
	for(int i = 0;i < count;i++,char_read++)
	{
		unsigned char read = fgetc(index);
		if(read != static_cast<unsigned char>(EOF))
			mem_area[i] = read;
		else
		{
			delete []desc;
			return char_read;
		}
	}
	delete []desc;
	return char_read;
}


/* File Write function:
 *    This writes 'count' number of 'value'(s) to the file indicated by index.
 *    Writing should start from the point pointed by current position of the file.
 *    Current position should be updated accordingly.
 * Parameter(s):
 *    index: File index which indicates the file to be read.
 *    value: a character to be written.
 *    count: Number of repetition.
 * Return:
 *    >0 for successful write
 *    -1 value for error case "File hasn't been open"
 *    -2 for error case "Maximum file size reached" (not implemented.)
 */
int FileSystem::write(int index, unsigned char value, int count)
{
	int no = static_cast<int> (oft[index][B+FILE_SIZE_FIELD]);
	if(no< 1 or no > 14)
		return -1;
	int cp = static_cast<int>(oft[index][B]);
	if(cp + count >= B*3)
		return -2;;
	int char_write = 0;
	for(int i = count;i > 0;i--,cp++,char_write ++)
	{

		if (fputc(value,index) == _EOF)
			return -2;
	}
	unsigned char* desc = read_descriptor(no);
	if(oft[index][B] == desc[0]){
		oft[index][cp%64] = _EOF;
	}
	delete [] desc;
	return (char_write != count ? -2 : char_write);
}


/* Setting new read/write position function:
 * Parameter(s):
 *    index: File index which indicates the file to be read.
 *    pos: New position in the file. If pos is bigger than file size, set pos to file size.
 * Return:
 *    0 for successful write
 *    -1 value for error case "File hasn't been open"
 */
int FileSystem::lseek(int index, int pos)
{
	int no = static_cast<int>(oft[index][B+FILE_SIZE_FIELD]);
	if(no == 48)
		return -1;
	int cp = oft[index][B];
	unsigned char* desc = new unsigned char[4];
	desc = read_descriptor(no);
	int block = (cp < 64 ? desc[1]:( cp < 127? desc[2]: desc[3]));
	write_block(block,oft[index]);
	if(static_cast<int>(desc[0]) > pos)
		oft[index][B] = pos;
	else
		oft[index][B] = desc[0];
	cp = oft[index][B];
	read_block((cp < 64 ? desc[1]:( cp < 127? desc[2]: desc[3])),oft[index]);
	delete [] desc;
	return static_cast<int>(oft[index][B]);
}


/* Close file function:
 * Parameter(s):
 *    index: The index of open file table
 * Return:
 *    none (write up states must return status change from void can change back if needed)
 */
int FileSystem::close(int index)
{
	if(oft[index][B+FILE_SIZE_FIELD] == '0')
		return -1;
	unsigned char* desc = new unsigned char[4];
	int no = static_cast<int>(oft[index][B+FILE_SIZE_FIELD]);
	desc = read_descriptor(no);
	int cp = static_cast<int>(oft[index][B]);
	int block = 1;
	if(cp > 127)
		block = 3;
	else if(cp > 63 && cp <128)
		block = 2;
	write_block(static_cast<int>(desc[block]),oft[index]);
	for(int j =0;j<B + FILE_SIZE_FIELD + FILE_SIZE_FIELD;j++)
		oft[index][j] = '0';
	delete [] desc;
	return index;
}


/* Delete file function:
 *    Delete a file
 * Parameter(s):
 *    symbolic_file_name: a file name to be deleted.
 * Return:
 *    Return 0 with success
 *    Return -1 with error (ie. No such file).
 */
int FileSystem::deleteFile(std::string fileName)
{
	int index = search_dir(0, fileName);
	if(index == -1)
		return -1;
	//	//should close or return error if file is open
	for(int i  = 0; i < 3;i++)
		if(oft[i][B + FILE_SIZE_FIELD] == index)
			close(i);
	//			return -1;

	unsigned char* desc = new unsigned char[4];
	desc = read_descriptor(index);
	unsigned char* to_write = new unsigned char[B];
	for(int j = 0; j<B;j++)
		to_write[j] = '0';
	for(int i = 1; i < 4 ;i++){
		if(desc[i] != '0' ){
			write_block(static_cast<int>(desc[i]),to_write);
		}
	}
	int dir_index = (index < 6 ? K: (index < 11 ? K+1: K+2 ));
	int start_pos =(index < 6 ? (index- 1)*11 : (index < 11 ? (index-1)* 11 *55: (index-1)*11 - 110));
	clear_descriptor(index);
	delete_dir(dir_index,start_pos,11);
	delete []to_write;
	delete []desc;
	return 0;
}

/* Directory listing function:
 *    List the name and size of files in the directory. (We have only one directory in this project.)
 *    Example of format:
 *       abc 66 bytes, xyz 22 bytes
 * Parameter(s):
 *    None
 * Return:
 *    None
 */
void FileSystem::directory()
{
	std::string directory;
	std::string names[14];
	int block = K;
	int j = 0;
	unsigned char* to_write = new unsigned char[B];
	read_block(block,to_write);
	for(int i = 0; i < MAX_FILE_NO; i++){
		for(int k = 0; k < 10; k++){
			directory += to_write[j+k];
		}
		directory = Remove_trailing(directory);
		names[i] = directory;
		directory = "";
		j+=11;
		if(j >= 55){
			block +=1;
			read_block(block,to_write);
			j = 0;
		}
	}
	unsigned char* desc= new unsigned char[4];
	for(int i = 0; i < MAX_FILE_NO; i++)
	{
		if(names[i].size() != 0)
		{
			desc = read_descriptor(search_dir(0,names[i]));
			std::cout << names[i]<<" "<<static_cast<int>(desc[0]);
			std::cout<<(i == MAX_FILE_NO-1 ? ",":"\n");
		}
	}
	delete [] desc;
	delete [] to_write;
}

// Disk dump, from block 'start' to 'start+size-1'.
void FileSystem::diskdump(int start, int size)
{
	unsigned char* to_write = new unsigned char[B];
	for(int i = start; i < start+size-1;i++)
	{
		read_block(i,to_write);
		std::string str(reinterpret_cast<char*>(to_write));
		std::cout<< str <<std::endl;
	}
	delete []to_write;
}


/*------------------------------------------------------------------
      Disk management functions.
      These functions are not really a part of file system.
      They are provided for convenience in this emulated file system.
      ------------------------------------------------------------------
     Restores the saved disk image in a file to the array.
 */
void FileSystem::restore()
{
	//closes all files as restore should have no file open written data will be deleted
	for( int i = 0; i < 3;i++)
		close(i);
	std::ifstream file("filesystem.txt");
	char new_line;
	for(int i = 0; i < L; i++)
	{
		for(int j = 0; j < B;j++)
		{
			ldisk[i][j] = file.get();
		}
		new_line = file.get();//newline char don't need it
	}
	file.close();
}

// Saves the array to a file as a disk image.
void FileSystem::save()
{
	// files dont close but data on buffer is written on to disk
	unsigned char* desc = new unsigned char[4];
	int cp = 0;
	for(int i  = 0; i < 3;i++)
	{
		if(oft[i][B + FILE_SIZE_FIELD] != '0')
		{
			desc = read_descriptor(oft[i][B + FILE_SIZE_FIELD]);
			cp = oft[i][B];
			write_block(desc[(cp  < 64 ? 1 :(cp < 128 ? 2: 3))],oft[i]);
		}
	}
	delete [] desc;
	std::ofstream saveFile("filesystem.txt");
	std::string to_write = "";
	for(int i = 0; i < MAX_BLOCK_NO; ++i){
		for(int j = 0; j < B;j++){
			to_write += ldisk[i][j];
		}
		saveFile <<to_write<<std::endl;
		to_write = "";
	}
	saveFile.close();
}

void FileSystem::write_block(int i, unsigned char* p)
{
	for(int j = 0; j < B; j++)
		ldisk[i][j] = p[j];
}

void FileSystem::read_block(int i, unsigned char* p)
{
	for(int j = 0; j < B; j++)
		p[j] = ldisk[i][j];
}
std::string FileSystem::Remove_trailing(std::string name)
{
	for(int i = name.size()-1; 0 <= i; i--){
		if(name[i] != '0')
			return name.substr(0,i+1);
	}
	return "";
}
