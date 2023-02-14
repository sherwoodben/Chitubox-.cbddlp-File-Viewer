#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>

#include "ChituFileHeader.h"
#include "ChituPreviewImage.h"
#include "ChituGCode.h"
#include "ChituLayerImage.h"
#include "ChituCopyrightData.h"
#include "Helpers.h"

//this is for .cdbblp files; need to figure
//out how I want to handle the possibilty of
//dealing with .ctb files.
class ChituFile
{
private:

	/*IMPORTANT TO NOTE:
		When I say "pointer to ___" here I mean it in the literal
		c++ sense; a pointer to an object. Gets a bit confusing since
		there are also pointers within the data that point to locations
		in the file-- I call these ChituPointers to avoid confusion. */

	//the actual file on disk and its path
	std::FILE* cFile = nullptr;
	std::string filePath = "DEFAULT";

	//size of the file (bytes)
	long int cFileSize = -1;

	//size of various parts of the file
	//(determined through examination of several
	//files)
	long int CFILE_HEADER_SIZE = 112;
	long int PREVIEW_IMAGE_HEADER_SIZE = 16;
	long int UNKNOWN_DATA_SIZE = 136;
	long int MYSTERY_DATA_SIZE = 80;

	//a pointer to the "file header"
	ChituFileHeader* cFileHeader = nullptr;
	
	//pointers to the preview image headers and the actual
	//images:
	//large preview
	ChituPreviewImageHeader* cLargePreviewHeader = nullptr;
	ChituPreviewImage* cLargePreviewImage = nullptr;

	//and small preview
	ChituPreviewImageHeader* cSmallPreviewHeader = nullptr;
	ChituPreviewImage* cSmallPreviewImage = nullptr;

	//pointer to the G-Code
	ChituGCode* cGCode = nullptr;

	//pointer to the layer image manager
	ChituLayerImageManager* cLayerImageManager = nullptr;

	//pointer to the unknown data block
	ChituUnknownDataBlock* cUnknownData = nullptr;

	//pointer to the copyright stuff
	ChituCopyRightData* cCopyrightData = nullptr;

	//pointer to the mystery data
	ChituMysteryDataBlock* cMysteryData = nullptr;

	

public:

	//constructor just takes a file path
	ChituFile(std::string FILE_PATH)
		: filePath(FILE_PATH)
	{
	}

	~ChituFile()
	{
		//created components of the file with 'new'
		//so we need to also 'delete' them
		delete cFileHeader;
		delete cLargePreviewHeader;
		delete cLargePreviewImage;
		delete cSmallPreviewHeader;
		delete cSmallPreviewImage;
		delete cGCode;
		delete cLayerImageManager;
		delete cUnknownData;
		delete cCopyrightData;
		delete cMysteryData;
		
	}

	//initializes the file for reading
	bool InitFile();

	//load/store the raw data into program memory
	void LoadFile();

	//decode the stored raw data
	void DecodeFile();

	//generate a report with useful information
	//about the file
	void Report(std::string logFileName);

	//simple file path getter function
	std::string GetFilePath() { return filePath; }

	//saves the large and small preview images
	void SavePreviewImages();

	//saves the layer images
	void SaveLayerImages();
	
};