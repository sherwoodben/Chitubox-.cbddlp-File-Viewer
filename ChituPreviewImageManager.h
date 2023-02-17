#pragma once

#include "ChituPreviewImageHeader.h"
#include "ChituPreviewImage.h"
#include "ChituFileHeader.h"

struct ChituPreviewImageManager
{
	//large preview
	ChituPreviewImageHeader* cLargePreviewHeader = nullptr;
	ChituPreviewImage* cLargePreviewImage = nullptr;

	//small preview
	ChituPreviewImageHeader* cSmallPreviewHeader = nullptr;
	ChituPreviewImage* cSmallPreviewImage = nullptr;

	//useful values to have; seems the size
	//of a preview image header is always
	//16 bytes
	long int PREVIEW_IMAGE_HEADER_SIZE = 16;

	//constructor
	ChituPreviewImageManager(std::FILE* cFile, ChituFileHeader* cFileHeader)
	{
		LoadPreviewImageHeaders(cFile, cFileHeader);
		LoadPreviewImages(cFile);
	}

	//destructor
	~ChituPreviewImageManager()
	{
		//since we allocated with new, we need to delete
		delete cLargePreviewHeader;
		delete cLargePreviewImage;
		delete cSmallPreviewHeader;
		delete cSmallPreviewImage;
	}

	//load image headers:
	void LoadPreviewImageHeaders(std::FILE* cFile, ChituFileHeader* cFileHeader)
	{
		cLargePreviewHeader = new ChituPreviewImageHeader(cFile,
			cFileHeader->GetLargePreviewHeaderAddress(), PREVIEW_IMAGE_HEADER_SIZE);
		cSmallPreviewHeader = new ChituPreviewImageHeader(cFile,
			cFileHeader->GetSmallPreviewHeaderAddress(), PREVIEW_IMAGE_HEADER_SIZE);
	}

	//load images:
	void LoadPreviewImages(std::FILE* cFile)
	{
		cLargePreviewImage = new ChituPreviewImage(cFile,
			cLargePreviewHeader->GetWidth(), cLargePreviewHeader->GetHeight(),
			cLargePreviewHeader->GetAddress(), cLargePreviewHeader->GetSize());

		cSmallPreviewImage = new ChituPreviewImage(cFile,
			cSmallPreviewHeader->GetWidth(), cSmallPreviewHeader->GetHeight(),
			cSmallPreviewHeader->GetAddress(), cSmallPreviewHeader->GetSize());
	}

	//decode the images
	void DecodeImages()
	{
		cLargePreviewImage->DecodeImage();
		cSmallPreviewImage->DecodeImage();
	}

	void ReportData(std::ostream* targetStream, int tabLevel = 0)
	{
		//we build a tab string for formatting purposes;
		//we change the tab level to assist with organization.
		std::string tabString = "";
		for (int i = 0; i < tabLevel; i++) tabString.append(1, '\t');

		//Large preview image info
		*targetStream << tabString << "Large Preview Image Header:\n";
		cLargePreviewHeader->ReportData(targetStream, tabLevel + 1);

		//Small preview image info
		*targetStream << tabString << "Small Preview Image Header:\n";
		cSmallPreviewHeader->ReportData(targetStream, tabLevel + 1);
	}

	void SaveImages()
	{
		//gets the status of the saving of the large preview image
		if (!cLargePreviewImage->SaveImage())
		{
			std::cout << "\t> Large Preview Image could not be saved.\n";
		}
		else
		{
			std::cout << "\t> Large Preview Image successfully saved to disk.\n";
		}

		//gets the status of the saving of the small preview image
		if (!cSmallPreviewImage->SaveImage())
		{
			std::cout << "\t> Small Preview Image could not be saved.\n";
		}
		else
		{
			std::cout << "\t> Small Preview Image successfully saved to disk.\n";
		}
	}

};