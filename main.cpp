#include <vector>
#include "FileSystem.hpp"

void getcommand();
void read(int index,int count,FileSystem &k);
void write(int index,char value, int count,FileSystem &k);
void open(std::string file_name, FileSystem &k);
void create(std::string file_name, FileSystem &k);
void seek(int index,int pos, FileSystem &k);
std::vector<std::string> splits(const std::string &s, char delim, std::vector<std::string> &elems);
std::vector<std::string> split(const std::string &s);
void close_file(int index, FileSystem &k);
void delete_file(std::string file_name, FileSystem &k);


int main(){
	getcommand();
	return 0;
}
std::vector<std::string> split(const std::string &s) {
	std::vector<std::string> elems;
	splits(s, ' ', elems);
	return elems;
}

std::vector<std::string> splits(const std::string &s, char delim, std::vector<std::string> &elems) {
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}

void getcommand()
{
	FileSystem k;
	std::string command = "";
	std::vector<std::string> input;
	bool init = false;
	while(true){
		std::getline(std::cin,command);
		if (command == "q")
			break;
		else if(command == "")
			continue;
		else
			input = split(command);
		std::string command = input[0];
		if(!init){
			if (command == "in")
			{
				init = true;
				std::cout << "disk initialized" << std::endl;
			}
			else
				std::cout<<"Invalid command"<<std::endl;
		}
		else if (init)
		{
			if (command == "in")
			{
				k.restore();
				std::cout << "disk restored" << std::endl;
			}
			else if(command == "de")
			{
				if(input.size() != 2)
					std::cout << "Error"<<std::endl;
				else
					delete_file(input[1],k);
			}
			else if(command == "cl")
			{
				if(input.size() != 2)
					std::cout << "Error"<<std::endl;
				else{
					int index;
					try {
						std::stringstream integer(input[1]);
						integer >> index;
						if (index < 4 && index > 0)
							close_file(index,k);
						else
							std::cout << "Error"<<std::endl;
					}
					catch(const std::ios_base::failure& e) {
						std::cout << "Error"<<std::endl;
					}
				}

			}
			else if (command == "cr")
			{
				if(input.size() != 2)
					std::cout << "Error"<<std::endl;
				else
				{
					create(input[1],k);
				}
			}
			else if (command == "op")
			{
				if(input.size() != 2)
					std::cout << "Error"<<std::endl;
				else
				{
					open(input[1],k);
				}
			}
			else if (command == "wr")
			{
				if(input.size() != 4)
					std::cout << "Error"<<std::endl;
				else if( input[2].size() != 1)
					std::cout << "Error" << std::endl;
				else
				{
					int index;
					int count;
					unsigned char value;
					try {
						std::stringstream integer(input[1]);
						integer >> index;
						std::stringstream second(input[3]);
						second >> count;
						std::stringstream c_value(input[2]);
						c_value >> value;
						write(index,value,count,k);
					}
					catch(const std::ios_base::failure& e) {
						std::cout << "Error";
					}
				}
			}
			else if (command == "sk")
			{
				if(input.size() != 3)
					std::cout << "Error"<<std::endl;
				else
				{
					int index;
					int pos;
					try {
						std::stringstream integer (input[1]);
						integer >> index;
						std::stringstream second(input[2]);
						second >> pos;
						seek(index,pos,k);
					} catch(const std::ios_base::failure& e) {
						std::cout << "Error";
					}
				}
			}
			else if (command == "rd")
			{
				if(input.size() != 3)
					std::cout << "Error"<<std::endl;
				else
				{
					int index;
					int count;
					try {
						std::istringstream integer(input[1]);
						integer >> index;
						std::istringstream second(input[2]);
						second >> count;
						read(index,count,k);
					} catch(const std::ios_base::failure& e) {
						std::cout << "Error";
					}
				}
			}
			else if (command == "dr")
			{
				k.directory();
			}
			else if (command == "sv")
			{
				k.save();
				std::cout << "disk saved" << std::endl;
			}
			else
				std::cout<<"Invalid command"<<std::endl;
		}
	}
}

void seek(int index,int pos, FileSystem &k)
{
	int error_code = k.lseek(index-1, pos);
	if(error_code == -1)
		std::cout<< "Error: File hasn't been open"<<std::endl;
	else{
		std::cout <<  "Current position " << error_code <<std::endl;
	}
}

void read(int index,int count, FileSystem &k)
{
	unsigned char* mem_area = new unsigned char [count];
	int error_code = k.read(index-1, mem_area, count);
	if(error_code == -2)
		std::cout<< "Error: Cannot read from the EOF"<<std::endl;
	else if(error_code == -1)
		std::cout<< "Error: File has not been open"<<std::endl;
	else{
		std:: string str= "";
		for(int i = 0; i < error_code;i++)
			str+= mem_area[i];
		std::cout <<  error_code << " bytes read "<< str <<std::endl;
		delete [] mem_area;
	}
}

void write(int index,char value,int count, FileSystem &k)
{
	if(count  >= 192)//last byte 192 would be _EOF cannot write to this
	{
		std::cout << "Error: Number of bytes to read is greater then allowed"<<std::endl;
		return;
	}
	int error_code = k.write(index-1, value,count);
	if(error_code == -2)
		std::cout<< "Error: Maximum file size reached"<<std::endl;
	else if(error_code == -1)
		std::cout<< "Error: No file with " << index <<" is not open"<<std::endl;
	else
		std::cout << error_code << " bytes written"<<std::endl;
}
void open(std::string file_name, FileSystem &k)
{
	if(file_name.size() > 10)
	{
		std::cout << "Error: File for file size must be at max 10";
		return;
	}
	int error_code = k.open(file_name);
	if(error_code == -2)
		std::cout<< "Error: Maximum number of files opened" <<std::endl;
	else if(error_code == -1)
		std::cout<< "Error: File "<< file_name << "does not exist"<<std::endl;
	else
		std::cout << "File "<< file_name <<" open," << " index = "<< error_code+1<<std::endl;
}
void close_file(int index, FileSystem &k)
{
	int status = k.close(index-1);
	if (status == -1)
		std::cout << "Error: File with index  "<< index << " not opened" <<std::endl;
	else
		std::cout << "File with index  "<< index << " closed" <<std::endl;
}

void delete_file(std::string file_name, FileSystem &k)
{
	if(file_name.size() > 10)
	{
		std::cout << "Error: File size for file must be at max 10";
		return;
	}
	int error_code = k.deleteFile(file_name);
	if(error_code == -1)
		std::cout<< "Error: File "<< file_name << " does not exist"<<std::endl;
	else
		std::cout << "File "<< file_name <<" deleted "<<std::endl;
}

void create(std::string file_name, FileSystem &k)
{
	if(file_name.size() > 10)
	{
		std::cout << "Error: File size for file must be at max 10";
		return;
	}
	int error_code = k.create(file_name);
	if(error_code == -2)
		std::cout<< "Error: File with name "<< file_name<< " exist" <<std::endl;
	else if(error_code == -1)
		std::cout<< "Error: Maximum number of files exist" <<std::endl;
	else
		std::cout << "File "<< file_name <<" created" << std::endl;
}
