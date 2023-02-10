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

	//string to store the file name (built from input)
	std::string fileName = "INPUT\\";

	std::string temp;

	std::cout << "Enter the name of the file to analyze (without the file extension)" << std::endl;
	
	std::cin >> temp;

	fileName.append(temp);
	fileName.append(".cbddlp");

	//log file information
	std::string logFileName = "OUTPUT\\log.txt";

	auto startTime = std::chrono::steady_clock::now();

	//create the actual Chitu File object (for .cbddlp files ONLY right now)
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

	auto readEndTime = std::chrono::steady_clock::now();

	//console output
	std::cout << "> Generating Report..." << std::endl;

	//generate a report, also saves images to disk
	newCFile.Report(logFileName);

	auto writeEndTime = std::chrono::steady_clock::now();

	//console output
	std::cout << "> Finished." << std::endl;

	double readDiff = std::chrono::duration_cast<std::chrono::microseconds>(readEndTime - startTime).count();
	double writeDiff = std::chrono::duration_cast<std::chrono::microseconds>(writeEndTime - readEndTime).count();
	double totalDiff = std::chrono::duration_cast<std::chrono::microseconds>(writeEndTime - startTime).count();

	double readPerc = (readDiff / totalDiff) * 100;
	double writePerc = (writeDiff / totalDiff) * 100;

	//console output
	std::cout << ">File read in " << std::chrono::duration <double, std::milli>(readDiff).count() << " us (" << readPerc << "%)" << std::endl;
	std::cout << ">Log written in " << std::chrono::duration <double, std::milli>(writeDiff).count() << " us (" << writePerc << "%)" << std::endl;
	std::cout << ">Total " << std::chrono::duration <double, std::milli>(totalDiff).count() << " us" << std::endl;
	//end of program
	return 0;
}