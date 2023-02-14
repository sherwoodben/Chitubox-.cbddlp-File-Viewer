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


//CHANGE THESE TO CHANGE THE "DEBUG"
//LEVEL--can make flags eventually but
//will probably rework the whole
//main loop eventually so this works just dandy
//for now
const bool SAVE_PREVIEW_IMAGES = false;
const bool SAVE_LAYER_IMAGES = false;
const bool GENERATE_REPORT = true;

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

	fileName.append(temp);
	fileName.append(".cbddlp");

	//JUST FOR WINDOWS, JUST FOR NOW
	system("CLS");

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

	//more timing stuff
	auto loadEndTime = std::chrono::steady_clock::now();

	//console output
	std::cout << "> Decoding Data..." << std::endl;

	//decode all the data
	newCFile.DecodeFile();

	//more timing stuff
	auto decodeEndTime = std::chrono::steady_clock::now();

	//save images to disk
	if (SAVE_PREVIEW_IMAGES)
	{
		//console output
		std::cout << "> Saving Preview Images..." << std::endl;
		newCFile.SavePreviewImages();
	}
	
	if (SAVE_LAYER_IMAGES)
	{
		//console output
		std::cout << "> Saving Layer Images..." << std::endl;
		newCFile.SaveLayerImages();
	}
	
	//more timing stuff
	auto saveLayerImagesEndTime = std::chrono::steady_clock::now();
	
	if (GENERATE_REPORT)
	{
		//console output
		std::cout << "> Generating Report..." << std::endl;

		//generate a report
		newCFile.Report(logFileName);
	}
	
	//more timing stuff
	auto writeEndTime = std::chrono::steady_clock::now();

	//console output
	std::cout << "> Finished." << std::endl;

	//timing stuff
	double loadDiff = std::chrono::duration_cast<std::chrono::microseconds>(loadEndTime - startTime).count();
	double decodeDiff = std::chrono::duration_cast<std::chrono::microseconds>(decodeEndTime - loadEndTime).count();
	double saveLayerImageDiff = std::chrono::duration_cast<std::chrono::microseconds>(saveLayerImagesEndTime - decodeEndTime).count();
	double writeDiff = std::chrono::duration_cast<std::chrono::microseconds>(writeEndTime - saveLayerImagesEndTime).count();
	double totalDiff = std::chrono::duration_cast<std::chrono::microseconds>(writeEndTime - startTime).count();

	//even more timing stuff
	double loadPerc = (loadDiff / totalDiff) * 100;
	double decodePerc = (decodeDiff / totalDiff) * 100;
	double saveLayerImagePerc = (saveLayerImageDiff / totalDiff) * 100;
	double writePerc = (writeDiff / totalDiff) * 100;

	//console output
	std::cout << "\n> File read in " << std::chrono::duration <double, std::milli>(loadDiff).count() << " us (" << loadPerc << "%)" << std::endl;
	std::cout << "> Decoded in " << std::chrono::duration <double, std::milli>(decodeDiff).count() << " us (" << decodePerc << "%)" << std::endl;
	if (SAVE_PREVIEW_IMAGES || SAVE_LAYER_IMAGES) std::cout << "> Images Saved in " << std::chrono::duration <double, std::milli>(saveLayerImageDiff).count() << " us (" << saveLayerImagePerc << "%)" << std::endl;
	if (GENERATE_REPORT) std::cout << "> Log written in " << std::chrono::duration <double, std::milli>(writeDiff).count() << " us (" << writePerc << "%)" << std::endl;
	std::cout << "> Total " << std::chrono::duration <double, std::milli>(totalDiff).count() << " us" << std::endl;
	
	//end of program
	return 0;
}