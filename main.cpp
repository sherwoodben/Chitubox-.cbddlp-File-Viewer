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
	std::filesystem::create_directory("OUTPUT\\LAYER IMAGES");

	//string to store the file name (built from input)
	std::string fileName = "INPUT\\";

	std::string temp;

	std::cout << "Enter the name of the file to analyze (without the file extension)" << std::endl;
	
	std::cin >> temp;

	//JUST FOR WINDOWS, JUST FOR NOW
	system("CLS");

	fileName.append(temp);
	fileName.append(".cbddlp");

	//log file information
	std::string logFileName = "OUTPUT\\log.txt";

	//start timer
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
	std::cout << "> Loading File..." << std::endl;

	//load the file
	newCFile.LoadFile();


	auto loadEndTime = std::chrono::steady_clock::now();

	//console output
	std::cout << "> Decoding Data..." << std::endl;

	//decode all the data
	newCFile.DecodeFile();

	auto decodeEndTime = std::chrono::steady_clock::now();

	//console output
	std::cout << "> Saving Images..." << std::endl;

	//save images to disk
	newCFile.SavePreviewImages();
	newCFile.SaveLayerImages();

	auto saveLayerImagesEndTime = std::chrono::steady_clock::now();

	//console output
	std::cout << "> Generating Report..." << std::endl;

	//generate a report
	newCFile.Report(logFileName);

	auto writeEndTime = std::chrono::steady_clock::now();

	//console output
	std::cout << "> Finished." << std::endl;

	//timing stuff
	double loadDiff = std::chrono::duration_cast<std::chrono::microseconds>(loadEndTime - startTime).count();
	double decodeDiff = std::chrono::duration_cast<std::chrono::microseconds>(decodeEndTime - loadEndTime).count();
	double saveLayerImageDiff = std::chrono::duration_cast<std::chrono::microseconds>(saveLayerImagesEndTime - decodeEndTime).count();
	double writeDiff = std::chrono::duration_cast<std::chrono::microseconds>(writeEndTime - saveLayerImagesEndTime).count();
	double totalDiff = std::chrono::duration_cast<std::chrono::microseconds>(writeEndTime - startTime).count();

	//more timing stuff
	double loadPerc = (loadDiff / totalDiff) * 100;
	double decodePerc = (decodeDiff / totalDiff) * 100;
	double saveLayerImagePerc = (saveLayerImageDiff / totalDiff) * 100;
	double writePerc = (writeDiff / totalDiff) * 100;

	//console output
	std::cout << "\n> File read in " << std::chrono::duration <double, std::milli>(loadDiff).count() << " us (" << loadPerc << "%)" << std::endl;
	std::cout << "> Decoded in " << std::chrono::duration <double, std::milli>(decodeDiff).count() << " us (" << decodePerc << "%)" << std::endl;
	std::cout << "> Layer Images Saved in " << std::chrono::duration <double, std::milli>(saveLayerImageDiff).count() << " us (" << saveLayerImagePerc << "%)" << std::endl;
	std::cout << "> Log written in " << std::chrono::duration <double, std::milli>(writeDiff).count() << " us (" << writePerc << "%)" << std::endl;
	std::cout << "> Total " << std::chrono::duration <double, std::milli>(totalDiff).count() << " us" << std::endl;
	//end of program
	return 0;
}