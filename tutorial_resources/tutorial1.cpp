#include <iostream>

/***************
///////////////
BUILD COMMANDS
///////////////

Basic build:
g++ main.cpp -o name.out

Create object file:
g++ -c file.cpp -o file.o

Create executable from object files
g++ file1.o file2.o -o name.out

Create executable from source files
g++ file1.cpp file2.cpp -o name.out
****************/

using namespace std;

int* getVarPointer()
{
	int variable = 20;
	return &variable;		//Returns pointer to variable
								//Variable is gone when function returns, leaving the pointer pointing to nothing
}

int main(int argc, char* argv[]){

	//Basic types
	unsigned int aUInt = 7;
	int anInt = -3;
	float aFloat = 1.2f;
	double aDouble = 2.7;

	char aChar = 'a';
	short aShort = 30;

	bool aBool = true;

	//No guarantee on value if uninitialized

        //////////////////////
        // Printing
        //////////////////////

        printf("Hello world\n");

        printf("aUInt = %u, anInt = %d, aFloat = %f, aDouble = %f, aChar = %c\n",  aUInt, anInt, aFloat, aDouble, aChar); 

        std::cout << "Cout output" << std::endl;

        //////////////////////
        // Control Statements
        //////////////////////

	bool condition = (aChar == 'a') && (aDouble != 3.0) || (aFloat <= 2.f);

	int counter = 1;

	while(counter < 10){
		counter+= counter;
	}

	do{
		counter--;
	}while(counter < 1);

	for(int i=0; i<10; i++){
		//Do something
	}

	if(condition){
		//Do something
	}
	else if(aBool){
		//Do something else
	}
	else{
		//Do another thing
	}

	int switchNum = 2;
	char switchChar;
	switch(switchNum){
		case 1:
			switchChar = 'a';
			break;
		case 2:
			switchChar = 'b';
			break;
		default:
			switchChar = ' ';

	}

        int val = (counter == 0) ? 30 : counter; 

        //////////////////////////
        // Pointers and references
        ///////////////////////////

	///////////////
	//POINTERS - Not covered in tutorial
	///////////////
	cout << "POINTERS______" << endl;

	int intVal = 5;
	int* intPtr;				// * after type declares as pointer
	intPtr = &intVal;			// & before variable gets pointer to the variable
	intVal++;		//intVal is now 6	

	cout << "intVal = " << intVal << endl;
	cout << "intPtr = " << intPtr << endl;		//Address of pointer
	cout << "*intPtr = " << *intPtr << endl;	//* before variable accesses value inside pointer

	int& intRef = intVal;		//& after type declares reference to another variable
										//intRef is effectively another name for intVal
	cout << "intRef = " << intRef << endl;
	intVal++;		//Increase intVal by one
	intRef++;		//Increase intRef by one
	cout << "intVal = " << intVal << endl;		//Prints out 8
	cout << "intRef = " << intRef << endl;		//Prints out 8

	cout << "-----------" << endl;

	int** intPtrPtr = &intPtr;		//Can also have pointers to pointers

        ////////////
        // Arrays
        ////////////

	int intArray[20];
	int* intArrayPtr = intArray;		//Can use pointers to point to an array
	int* intArrayHalf = &intArray[10];		//Points at the halfway point in the array


	
	cout << "First half of array" << endl;
	for(int i=0; i<10; i++)
	{
		cout << intArrayPtr[i] << endl;		//Index from pointer like array
														//- Make sure you 'own' the data you're indexing to
	}


	
	cout << "Second half of array" << endl;
	for(int i=0; i<10; i++)
	{
		cout << intArrayHalf[i] << endl;		//Index from halfway point
	}	

	//Danging pointer
	int* danglingPointer = getVarPointer();		//"Tries" to return pointer to a variable = 20
	cout << "*danglingPointer = " << *danglingPointer << endl;		//Trying to access the value will result in segmentation fault


	return 0;
}
