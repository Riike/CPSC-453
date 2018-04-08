#include <cstdio>		//For printf and sscanf
#include <fstream>		//For ifstream
#include <iostream>		//For cout

//Use g++ main.cpp to compile

using namespace std;

//Prints out each line in the file
void printLines(const char* filename){
	ifstream f (filename);

	const int BUFF_SIZE = 256;
	char buffer [BUFF_SIZE];

	//ifstream will return true as long as something is left in the stream
	while(f){
		//Reads a line and stores it in the buffer
		f.getline(buffer, BUFF_SIZE);
		cout << buffer << endl;
	}

	f.close();
}


//Finds "Find %d" in file
void findNumber(const char* filename){
	ifstream f (filename);

	while(f){
		string word;	//Variable to be read into
		f >> word;		//Put the next word into the variable "word"
		if(word.compare("Find") == 0){
			int n;
			f >> n;		//Put the next intenger into the variable "n"
			printf("Find %d\n", n);
			return;
		}
	}

	f.close();
}

//Finds "Find %d %d %d" in file
void findNumbers(const char* filename){
	ifstream f (filename);

	const int BUFF_SIZE = 256;
	char buffer [BUFF_SIZE];

	while(f){
		f.getline(buffer, BUFF_SIZE);
		int x, y, z;
		//sscanf will look for "Find %d %d %d" inside buffer, and try and match three integers
		//Returns the number of arguments it successfully matched
		if(sscanf(buffer, "Find %d %d %d", &x, &y, &z) == 3){
			printf("Found %d %d %d\n", x, y, z);
		}
	}

	f.close();


}

//argc - The number of command line arguments
//argv - An array of char* "c-strings" with the name of the program being the first
int main(int argc, char *argv[]){
	if(argc <= 1){
		printf("Needs filename\n");
		return -1;
	}

	findNumbers(argv[1]);

}
