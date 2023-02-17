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


#define SAVE_PREVIEW_IMAGES true
#define SAVE_LAYER_IMAGES false
#define GENERATE_REPORT true

//CHANGE THESE TO CHANGE THE "DEBUG"
//LEVEL--can make flags eventually but
//will probably rework the whole
//main loop eventually so this works just dandy
//for now


int main() {

	//clear the output folder to start, then make the
	//folder again:
	std::filesystem::remove_all("OUTPUT");
	std::filesystem::create_directory("OUTPUT");
	std::filesystem::create_directory("OUTPUT\\LAYER IMAGES");

	//string to store the file name (built from input)
	std::string fileName = "INPUT\\";

	std::string temp;

	std::cout << "Enter the name of the file to analyze (without the file extension)\n";
	
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
		std::cout << "Unable to read '" << newCFile.GetFilePath() << "'\n";
		//then quit
		return -1;
	}

	//console output
	std::cout << "> Loading File...\n";

	//load the file
	newCFile.LoadFile();

	//more timing stuff
	auto loadEndTime = std::chrono::steady_clock::now();

	//console output
	std::cout << "> Decoding Data...\n";

	//decode all the data
	newCFile.DecodeFile();

	//more timing stuff
	auto decodeEndTime = std::chrono::steady_clock::now();

	//save images to disk
	if (SAVE_PREVIEW_IMAGES)
	{
		//console output
		std::cout << "> Saving Preview Images...\n";
		newCFile.SavePreviewImages();
	}
	
	if (SAVE_LAYER_IMAGES)
	{
		//console output
		std::cout << "> Saving Layer Images...\n";
		newCFile.SaveLayerImages();
	}
	
	//more timing stuff
	auto saveLayerImagesEndTime = std::chrono::steady_clock::now();
	
	if (GENERATE_REPORT)
	{
		//console output
		std::cout << "> Generating Report...\n";

		//generate a report
		newCFile.Report(logFileName);
	}
	
	//more timing stuff
	auto writeEndTime = std::chrono::steady_clock::now();

	//console output
	std::cout << "> Finished.\n";

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
	std::cout << "\n> File read in " << std::chrono::duration <double, std::milli>(loadDiff).count() << " us (" << loadPerc << "%)\n";
	std::cout << "> Decoded in " << std::chrono::duration <double, std::milli>(decodeDiff).count() << " us (" << decodePerc << "%)\n";
	if (SAVE_PREVIEW_IMAGES || SAVE_LAYER_IMAGES) std::cout << "> Images Saved in " << std::chrono::duration <double, std::milli>(saveLayerImageDiff).count() << " us (" << saveLayerImagePerc << "%)\n";
	if (GENERATE_REPORT) std::cout << "> Log written in " << std::chrono::duration <double, std::milli>(writeDiff).count() << " us (" << writePerc << "%)\n";
	std::cout << "> Total " << std::chrono::duration <double, std::milli>(totalDiff).count() << " us\n";
	
	//end of program
	return 0;
}