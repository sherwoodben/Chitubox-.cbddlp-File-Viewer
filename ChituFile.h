#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>

#include "ChituFileHeader.h"
#include "ChituPreviewImage.h"
#include "ChituGCode.h"
#include "ChituLayerImage.h"

class ChituFile
{
private:

	std::FILE* cFile = nullptr;
	std::string filePath = "DEFAULT";
	std::ofstream* logStream = nullptr;

	long int cFileSize = -1;

	long int CFILE_HEADER_SIZE = 112;
	long int PREVIEW_IMAGE_HEADER_SIZE = 16;

	ChituFileHeader* cFileHeader = nullptr;
	
	ChituPreviewImageHeader* cLargePreviewHeader = nullptr;
	ChituPreviewImage* cLargePreviewImage = nullptr;

	ChituPreviewImageHeader* cSmallPreviewHeader = nullptr;
	ChituPreviewImage* cSmallPreviewImage = nullptr;

	ChituGCode* cGCode = nullptr;

	ChituLayerImageManager* cLayerImageManager = nullptr;

public:

	ChituFile(std::string FILE_PATH, std::ofstream* LOG_STREAM = nullptr)
		: filePath(FILE_PATH), logStream(LOG_STREAM)
	{
	}

	~ChituFile()
	{
		//created components of the file with 'new'
		//so we need to also 'delete' them [see LoadFile()
		//for 'new' usage]:
		delete cFileHeader;
		delete cLargePreviewHeader;
		delete cLargePreviewImage;
		delete cSmallPreviewHeader;
		delete cSmallPreviewImage;
		delete cGCode;
		delete cLayerImageManager;
	}

	bool InitFile();

	void LoadFile();

	void Report();

	std::string GetFilePath() { return filePath; }

	void SavePreviewImages();
	
};