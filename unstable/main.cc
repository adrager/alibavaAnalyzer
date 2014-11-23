#include "BinaryToRoot.h"

#include "iostream"

int main(int argc, char* argv[])
{
	if(argc < 2)
	{
		std::cout << "Usage: pedDataFile binaryFile" << std::endl;
		return 1;
	}
	std::vector<BinaryToRoot*> binaryToRoot_;
	for (unsigned int i=1; i<argc;i++)
	{
	  binaryToRoot_.push_back(new BinaryToRoot(argv[1],4));
	  binaryToRoot_[i-1]->ReadFile();
	}
	for (unsigned int i=0; i<binaryToRoot_.size();i++)
	{
	  delete binaryToRoot_[i];
	}

	return 0;
}
