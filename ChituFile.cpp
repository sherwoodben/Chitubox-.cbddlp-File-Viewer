#include "ChituFile.h"

bool ChituFile::InitFile()
{
	//try to open the file in read mode with the binary flag
	errno_t cFileError = fopen_s(&cFile, filePath.c_str(), "rb");

	//handle error opening the file
	if (cFileError != 0)
	{
		if (logStream) *logStream << "Error opening '" << filePath << "'" << std::endl;

		return false;
	}
	else {
		//first we get the length of the file
		std::error_code ec;
		cFileSize = std::filesystem::file_size(filePath, ec);

		//handles the case where we can't read the size of the file for some
		//reason
		if (ec && logStream)
		{
			if (logStream) *logStream << "Error getting size of file " << filePath << "." << std::endl;

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
}

void ChituFile::Report()
{
	*logStream << "\nFile Header:" << std::endl;
	cFileHeader->ReportData(logStream, 1);
	*logStream << "\nLarge Preview Image Header:" << std::endl;
	cLargePreviewHeader->ReportData(logStream, 1);
	*logStream << "\nSmall Preview Image Header:" << std::endl;
	cSmallPreviewHeader->ReportData(logStream, 1);
	*logStream << "\nPreview Images:" << std::endl;
	SavePreviewImages();
	*logStream << "\nG-Code:" << std::endl;
	cGCode->ReportData(logStream, 1);
	*logStream << "\nLayer Images:" << std::endl;
	cLayerImageManager->ReportImages(logStream, 1);
}

void ChituFile::SavePreviewImages()
{
	
	if (!cLargePreviewImage->SaveImage())
	{
		*logStream << "\tLarge Preview Image could not be saved." << std::endl;
	}
	else
	{
		*logStream << "\tLarge Preview Image successfully saved to disk." << std::endl;
	}

	if (!cSmallPreviewImage->SaveImage())
	{
		*logStream << "\tSmall Preview Image could not be saved." << std::endl;
	}
	else
	{
		*logStream << "\tSmall Preview Image successfully saved to disk." << std::endl;
	}
}
