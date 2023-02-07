#include "ChituFile.h"

bool ChituFile::InitFile()
{
	//try to open the file in read mode with the binary flag
	errno_t cFileError = fopen_s(&cFile, filePath.c_str(), "rb");

	//handle error opening the file
	if (cFileError != 0)
	{
		std::cout << "Error opening '" << filePath << "'" << std::endl;

		return false;
	}
	else {
		//first we get the length of the file
		std::error_code ec;
		cFileSize = std::filesystem::file_size(filePath, ec);

		//handles the case where we can't read the size of the file for some
		//reason
		if (ec)
		{
			std::cout << "Error getting size of file " << filePath << "." << std::endl;

			return false;
		}
	}

	return true;
}

void ChituFile::LoadFile()
{
	//load the file header first:
	cFileHeader = new ChituFileHeader(cFile, CFILE_HEADER_SIZE);

	//load image headers:
	cLargePreviewHeader = new ChituPreviewImageHeader(cFile,
		cFileHeader->GetLargePreviewHeaderAddress(), PREVIEW_IMAGE_HEADER_SIZE);
	cSmallPreviewHeader = new ChituPreviewImageHeader(cFile,
		cFileHeader->GetSmallPreviewHeaderAddress(), PREVIEW_IMAGE_HEADER_SIZE);
	
	//load images:
	cLargePreviewImage = new ChituPreviewImage(cFile,
		cLargePreviewHeader->GetWidth(), cLargePreviewHeader->GetHeight(),
		cLargePreviewHeader->GetAddress(), cLargePreviewHeader->GetSize());

	cSmallPreviewImage = new ChituPreviewImage(cFile,
		cSmallPreviewHeader->GetWidth(), cSmallPreviewHeader->GetHeight(),
		cSmallPreviewHeader->GetAddress(), cSmallPreviewHeader->GetSize());

	//load unknown data (the one right before the copyright)
	cUnknownData = new ChituUnknownDataBlock(cFile, cFileHeader->GetUnknownDataAddress(), UNKNOWN_DATA_SIZE);

	//load copyright:
	cCopyrightData = new ChituCopyRightData(cFile,
		cUnknownData->GetCopyrightAddress(), cUnknownData->GetCopyrightDataSize());

	//load mystery data (the one right after the copyright)
	cMysteryData = new ChituMysteryDataBlock(cFile, cUnknownData->GetAddressOfMysteryData(), MYSTERY_DATA_SIZE);

	//load G Code:
	cGCode = new ChituGCode(cFile,
		cFileHeader->GetGCodeAddress(), cFileSize - cFileHeader->GetGCodeAddress(),
		cFileHeader->GetNumLayers());

	//load layer images:
	cLayerImageManager = new ChituLayerImageManager(&(cGCode->interLayerGCodeLines),
		cFileHeader->GetScreenX_PX(), cFileHeader->GetScreenY_PX());

	//close the file since we've read everything we need
	//to from it
	fclose(cFile);
	std::cout << "> File Read Complete:" << std::endl;
	std::cout << "\t> file contents are stored in memory" << std::endl;
	std::cout << "\t> file has been closed" << std::endl;
}

void ChituFile::Report(std::string logFileName)
{
	std::ofstream logStream(logFileName.c_str(), std::ofstream::out);

	//check that the log stream didn't fail,
	//if it did we won't even bother doing anything else
	if (logStream.fail()) return;


	logStream << "\nFile Header:" << std::endl;
	cFileHeader->ReportData(&logStream, 1);
	logStream << "\nLarge Preview Image Header:" << std::endl;
	cLargePreviewHeader->ReportData(&logStream, 1);
	logStream << "\nSmall Preview Image Header:" << std::endl;
	cSmallPreviewHeader->ReportData(&logStream, 1);
	logStream << "\nPreview Images:" << std::endl;
	SavePreviewImages();
	logStream << "\nUnknown Data:" << std::endl;
	cUnknownData->ReportData(&logStream, 1);
	logStream << "\nCopyright Notice:" << std::endl;
	cCopyrightData->ReportData(&logStream, 1);
	logStream << "\nMystery Data:" << std::endl;
	cMysteryData->ReportData(&logStream, 1);
	logStream << "\nG-Code:" << std::endl;
	cGCode->ReportData(&logStream, 1);
	logStream << "\nLayer Images:" << std::endl;
	cLayerImageManager->ReportImages(&logStream, 1);

	//we're done logging, so we can close the log
	logStream.close();
}

void ChituFile::SavePreviewImages()
{
	
	if (!cLargePreviewImage->SaveImage())
	{
		std::cout << "\tLarge Preview Image could not be saved." << std::endl;
	}
	else
	{
		std::cout << "\tLarge Preview Image successfully saved to disk." << std::endl;
	}

	if (!cSmallPreviewImage->SaveImage())
	{
		std::cout << "\tSmall Preview Image could not be saved." << std::endl;
	}
	else
	{
		std::cout << "\tSmall Preview Image successfully saved to disk." << std::endl;
	}
}
