#pragma once

#include "FileInterpreter.h"
#include "FileHeader.h"
#include "NewFileHeader.h"
#include "ChituImage.h"
#include "ChituGCode.h"

extern bool DEBUG;

class ChituFile
{
private:

	int HEADER_SIZE = 112;
	int IMG_HEADER_SIZE = 16;

	ChituPointer* fileHeaderPointer = nullptr;
	FileHeader* fileHeader = nullptr;
	NewFileHeader* newFileHeader = nullptr;

	ChituPointer* largePreviewImagePointer = nullptr;
	ChituImage* largePreviewImage = nullptr;

	ChituPointer* gCodePointer = nullptr;
	ChituGCode* gCode = nullptr;

	ChituPointer* smallPreviewImagePointer = nullptr;
	ChituImage* smallPreviewImage = nullptr;

	ChituPointer* copyrightPointer = nullptr;
	long int* additionalCopyrightOffset = nullptr;
	
public:

	std::FILE* file;
	std::ofstream* logFile;

	ChituFile(std::FILE* _file, std::ofstream* _logFile = nullptr)
	{
		file = _file;
		logFile = _logFile;

		fileHeader = new FileHeader(file, HEADER_SIZE);
		newFileHeader = new NewFileHeader(file, HEADER_SIZE);
		newFileHeader->InitFileHeader();
		newFileHeader->ReportData(logFile);

	}

	~ChituFile()
	{
		delete fileHeader;
		delete newFileHeader;

		delete largePreviewImage;
		delete smallPreviewImage;

		delete gCode;
	}

	void LoadHeader()
	{
		fileHeader->ParseRawData();

		largePreviewImagePointer = &fileHeader->largePreviewImage;
		gCodePointer = &fileHeader->gCodePointer;
		smallPreviewImagePointer = &fileHeader->smallPreviewImage;

		copyrightPointer = &fileHeader->copyrightPointer;
		additionalCopyrightOffset = &fileHeader->copyrightOffset;

		if (logFile)
		{
			*logFile << "\nFile Header:\n" << std::endl;
			fileHeader->Report(logFile, 1);

			newFileHeader->ReportData(logFile, 1);
		}
	}

	void LoadImages()
	{
		
		largePreviewImage = new ChituImage(file, *largePreviewImagePointer, IMG_HEADER_SIZE, 0);
		smallPreviewImage = new ChituImage(file, *smallPreviewImagePointer, IMG_HEADER_SIZE, 1);

		if (logFile)
		{
			*logFile << "\nImage Headers:\n" << std::endl;
			largePreviewImage->Report(logFile, 1);
			smallPreviewImage->Report(logFile, 1);
		}

		GetImages();
	}

	void GetImages()
	{
		std::string tabString;
		tabString.append(1, '\t');

		largePreviewImage->DecodeImage();
		if (!largePreviewImage->SaveDecodedImage("largePreview") && logFile)
		{
			*logFile << tabString << "Image " << largePreviewImage->imageID << " could not be saved." << std::endl;
		}
		else if (logFile)
		{
			*logFile << tabString << "Image " << largePreviewImage->imageID << " successfully saved to disk." << std::endl;
		}

		smallPreviewImage->DecodeImage();
		if (!smallPreviewImage->SaveDecodedImage("smallPreview") && logFile)
		{
			*logFile << tabString << "Image " << smallPreviewImage->imageID << " could not be saved." << std::endl;
		}
		else if (logFile)
		{
			*logFile << tabString << "Image " << smallPreviewImage->imageID << " successfully saved to disk." << std::endl;
		}
	}

	void LoadGCode()
	{
		gCode = new ChituGCode(file, fileHeader->numLayers, gCodePointer->chituAddress);
		if (logFile)
		{
			*logFile << "\nG-Code:\n" << std::endl;
			gCode->Report(logFile, 1);
		}
	}
};