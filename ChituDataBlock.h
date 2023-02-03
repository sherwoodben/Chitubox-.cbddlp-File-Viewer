#pragma once


#include <map>
#include <string>
#include <vector>
#include <fstream>

#include "FileInterpreter.h"
#include"ChituDataTypes.h"



struct ChituDataBlock
{
	long int size = -1;
	char* rawData = nullptr;

	std::map<std::string, ChituData*> data;
	std::vector<std::string> dataInsertionOrder;
	int dataEntries = 0;

	ChituDataBlock(std::FILE* readFrom, long int readOffset, long int bytesToRead) : size{bytesToRead}
	{
		rawData = new char[size];
		fseek(readFrom, readOffset, SEEK_SET);
		fread(rawData, sizeof(char), size, readFrom);
	}

	~ChituDataBlock()
	{
		if (rawData != nullptr)
		{
			delete[] rawData;
		}

		for (std::map<std::string, ChituData*>::iterator mapEntry = data.begin(); mapEntry != data.end(); mapEntry++)
		{
			delete mapEntry->second;
		}

		dataInsertionOrder.clear();
		rawData = nullptr;
		size = -1;
		dataEntries = 0;
	}

	template <typename T>
	void RegisterData(const T& dataToRegister, std::string key)
	{
		data[key] = static_cast<ChituData*>(dataToRegister);
		dataInsertionOrder.push_back(key);
		dataEntries++;
	}

	void ReportData(std::ostream* targetStream, int tabLevel = 0)
	{

		for (std::vector<std::string>::iterator dataEntry = dataInsertionOrder.begin(); dataEntry != dataInsertionOrder.end(); dataEntry++)
		{
			std::string tabString = "";

			ChituData* datum = data[*dataEntry];

			for (int i = 0; i < tabLevel; i++) tabString.append(1, '\t');

			*targetStream << tabString << datum->name << ": ";

			if (datum->type == DataType::INT)
			{

				ChituInt* datumAsInt = static_cast<ChituInt*>(datum);
				*targetStream << datumAsInt->value << std::endl;
			}
			else if (datum->type == DataType::FLOAT)
			{
				ChituFloat* datumAsFloat = static_cast<ChituFloat*>(datum);
				float val = datumAsFloat->value;
				*targetStream << datumAsFloat->value << std::endl;
			}
			else if (datum->type == DataType::ADDRESS)
			{
				ChituAddress* datumAsAddress = static_cast<ChituAddress*>(datum);
				*targetStream << datumAsAddress->value << std::endl;
			}
			else if (datum->type == DataType::SHORT)
			{
				ChituShort* datumAsShort = static_cast<ChituShort*>(datum);
				*targetStream << datumAsShort->value << std::endl;
			}
		}
	}
};