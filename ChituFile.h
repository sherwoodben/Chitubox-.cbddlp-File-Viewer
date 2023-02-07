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

class ChituFile
{
private:

	std::FILE* cFile = nullptr;
	std::string filePath = "DEFAULT";

	long int cFileSize = -1;

	long int CFILE_HEADER_SIZE = 112;
	long int PREVIEW_IMAGE_HEADER_SIZE = 16;
	long int UNKNOWN_DATA_SIZE = 136;
	long int MYSTERY_DATA_SIZE = 80;

	ChituFileHeader* cFileHeader = nullptr;
	
	ChituPreviewImageHeader* cLargePreviewHeader = nullptr;
	ChituPreviewImage* cLargePreviewImage = nullptr;

	ChituPreviewImageHeader* cSmallPreviewHeader = nullptr;
	ChituPreviewImage* cSmallPreviewImage = nullptr;

	ChituGCode* cGCode = nullptr;

	ChituLayerImageManager* cLayerImageManager = nullptr;

	ChituUnknownDataBlock* cUnknownData = nullptr;

	ChituCopyRightData* cCopyrightData = nullptr;

	ChituMysteryDataBlock* cMysteryData = nullptr;

	

public:

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

	bool InitFile();

	void LoadFile();

	void Report(std::string logFileName);

	std::string GetFilePath() { return filePath; }

	void SavePreviewImages();
	
};