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
	std::string fileName = "INPUT\\_cat.cbddlp";

	//log file information
	std::string logFileName = "OUTPUT\\log.txt";
	std::ofstream logStream(logFileName.c_str(), std::ofstream::out);

	//check that the log stream didn't fail
	if (logStream.fail())
	{
		//just quit the program if it did
		return -1;
	}

	//"beautify" the log file
	logStream << "Output for '" << fileName << "'" << std::endl;

	//create the actual Chitu File object (for .cbddlp files)
	ChituFile newCFile(fileName, &logStream);

	//console output
	std::cout << "> Initiating File Read" << std::endl;

	//try to initiate the file
	if (!newCFile.InitFile())
	{
		//if it fails, log it
		logStream << "Unable to read '" << newCFile.GetFilePath() << "'" << std::endl;
		logStream.close();

		//then quit
		return -1;
	}

	//console output
	std::cout << "> Initiating File Load" << std::endl;

	//load the file
	newCFile.LoadFile();

	//console output
	std::cout << "> Generating Report" << std::endl;

	//generate a report, also saves images to disk
	newCFile.Report();

	//we've done everything we need to with
	//the log file so we close it
	logStream.close();

	//console output
	std::cout << "> Finished." << std::endl;

	//end of program
	return 0;
}