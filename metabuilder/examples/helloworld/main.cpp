#include <stdio.h>

#include "mylib.h"
#include <iostream>
#include <string>

int main(int argc, char *argv[])
{
	int result = thisIsMyFunctionThereAreManyLikeItButThisOneIsMine(21);
	printf("Hello World %i!\n", result);

	std::string testStr = "A test string";
	std::cout << "Let's test the C++ standard lib! " << testStr << "\n";

	return 0;
}
