#include "ChituFile.h"

//initializes the file for reading
bool ChituFile::InitFile()
{
	//try to open the file in read mode with the binary flag
	errno_t cFileError = fopen_s(&cFile, filePath.c_str(), "rb");

	//handle error opening the file
	if (cFileError != 0)
	{
		std::cout << "> Error opening '" << filePath << "'\n";

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
			std::cout << "> Error getting size of file " << filePath << ".\n";

			return false;
		}
	}

	//we successfully did everything we needed!
	return true;
}

//load/store the raw data into program memory
void ChituFile::LoadFile()
{
	//load the file header first:
	cFileHeader = new ChituFileHeader(cFile, CFILE_HEADER_SIZE);

	//load the preview image manager
	cPreviewImageManager = new ChituPreviewImageManager(cFile, cFileHeader);

	//load unknown data (the one right before the copyright)
	cUnknownData = new ChituUnknownDataBlock(cFile,
		cFileHeader->GetSubdata1Address(), UNKNOWN_DATA_SIZE);

	//load mystery data (the one right after the copyright)
	cMysteryData = new ChituMysteryDataBlock(cFile,
		cUnknownData->GetAddressOfMysteryData(), MYSTERY_DATA_SIZE);

	//load copyright:
	cCopyrightData = new ChituCopyRightData(cFile,
		cMysteryData->GetCopyrightAddress(), cMysteryData->GetCopyrightDataSize());

	//load G Code:
	cGCode = new ChituGCodeManager(cFile,
		cFileHeader->GetGCodeAddress(), cFileSize - cFileHeader->GetGCodeAddress(),
		cFileHeader->GetNumLayers());

	//load layer images (after creating the layer image manager):
	cLayerImageManager = new ChituLayerImageManager(cGCode->interLayerGCodeLines,
		cFileHeader->GetScreenX_PX(), cFileHeader->GetScreenY_PX());

	//actually load the layer images (raw data)
	cLayerImageManager->InitImages();

	//close the file since we've read everything we need
	//to from it
	fclose(cFile);

	//output some stuff to the console
	std::cout << "> File Read Complete.\n";
	std::cout << "\t> file contents are stored in memory\n";
	std::cout << "\t> file has been closed\n";
}

//decode the stored raw data
void ChituFile::DecodeFile()
{
	//preview images
	cPreviewImageManager->DecodeImages();

	//layer images
	cLayerImageManager->DecodeImages();
}

//generate a report with useful information
//about the file
void ChituFile::Report(std::string logFileName)
{
	//create the actual log stream
	std::ofstream logStream(logFileName.c_str(), std::ofstream::out);

	//check that the log stream didn't fail,
	//if it did we won't even bother doing anything else
	if (logStream.fail()) return;

	//File header info
	logStream << "\nFile Header:\n";
	cFileHeader->ReportData(&logStream, 1);

	//Preview Image info
	logStream << "\nPreview Images:\n";
	cPreviewImageManager->ReportData(&logStream, 1);

	//Unknown Data
	logStream << "\nUnknown Data:\n";
	cUnknownData->ReportData(&logStream, 1);

	//copyright stuff
	logStream << "\nCopyright Notice:\n";
	cCopyrightData->ReportData(&logStream, 1);

	//mystery data
	logStream << "\nMystery Data:\n";
	cMysteryData->ReportData(&logStream, 1);

	//G-Code info
	logStream << "\nG-Code:\n";
	cGCode->ReportData(&logStream, 1);

	//DISABLE THE FOLLOWING TWO LINES TO SAVE A LOT OF TIME:
	//we already know the image decoding algorithm works--
	//we don't need to print the raw data anymore. Just keeping
	//it here for the sake of having more information available
	//for debugging.

	//Layer Images
	//logStream << "\nLayer Images:\n";
	//cLayerImageManager->ReportImages(&logStream, 1);

	//we're done logging, so we can close the log
	logStream.close();
}


//saves the large and small preview images
void ChituFile::SavePreviewImages()
{
	cPreviewImageManager->SaveImages();
}

//saves the layer images
void ChituFile::SaveLayerImages()
{
	//DISABLE THE FOLLOWING LINE TO SAVE A LOT OF TIME:
	//we already know the image decoding algorithm works--
	//we don't need to save the raw data anymore. Just keeping
	//it here for the sake of having more information available
	//for debugging.
	
	cLayerImageManager->SaveImages();
}
