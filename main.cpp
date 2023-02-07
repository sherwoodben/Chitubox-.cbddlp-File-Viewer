/*VERY FIRST ATTEMPT
at getting actual information from
the chitubox file format, so I can
work towards making my own
for the express purpose
of generating lattices*/

#include <fstream>
#include <filesystem>
#include <string>
#include <iostream>

#include "ChituFile.h"

int main() {

	//clear the output folder to start, then make the
	//folder again:
	std::filesystem::remove_all("OUTPUT");
	std::filesystem::create_directory("OUTPUT");

	//string to store the file name, hard
	//coded for now
	std::string fileName = "INPUT\\smallCat.cbddlp";

	//log file information
	std::string logFileName = "OUTPUT\\log.txt";

	//create the actual Chitu File object (for .cbddlp files)
	ChituFile newCFile(fileName);

	//try to initiate the file
	if (!newCFile.InitFile())
	{
		//if it fails, say something!
		std::cout << "Unable to read '" << newCFile.GetFilePath() << "'" << std::endl;
		//then quit
		return -1;
	}

	//console output
	std::cout << "> Initiating File Read..." << std::endl;

	//load the file
	newCFile.LoadFile();

	//console output
	std::cout << "> Generating Report..." << std::endl;

	//generate a report, also saves images to disk
	newCFile.Report(logFileName);

	//console output
	std::cout << "> Finished." << std::endl;

	//end of program
	return 0;
}