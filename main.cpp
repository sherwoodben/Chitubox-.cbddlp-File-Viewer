/*VERY FIRST ATTEMPT
at getting actual information from
the chitubox file format, so I can
work towards making my own
for the express purpose
of generating lattices*/

#include <fstream>
#include <string>
#include <sstream>
#include <iostream>

#include "ChituFile.h"

int main() {
	//string to store the file name, hard
	//coded for now
	std::string fileName = "INPUT\\_cat2.cbddlp";

	//log file information
	std::string logFileName = "OUTPUT\\log.txt";
	std::ofstream logStream(logFileName.c_str(), std::ofstream::out);

	//check that the log stream didn't fail
	if (logStream.fail())
	{
		return -1;
	}

	logStream << "Output for '" << fileName << "'" << std::endl;
	
	//try to open the file in read mode
	std::FILE* file;
	errno_t fileError = fopen_s(&file, fileName.c_str(), "rb");
	
	//check that the file is open; if it's
	//not, we'll just quit
	if (fileError != 0)
	{	
		logStream << "Error opening '" << fileName << "'" << std::endl;
		logStream.close();
		return -1;
	}

	//create a chitufile object
	//with a pointer to the file and
	//the log
	ChituFile cFile(file, &logStream);

	cFile.LoadHeader();

	cFile.LoadImages();

	cFile.LoadGCode();

	//we've done everything we need to with
	//the file and we've stored all the data
	//we need from it so we close it and the log
	fclose(file);
	logStream.close();

	return 0;
}