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
	long int offset = -1;
	bool isSubsetOfLarger = false;
	char* rawData = nullptr;

	std::map<std::string, ChituData*> data;
	std::vector<std::string> dataInsertionOrder;
	int dataEntries = 0;

	ChituDataBlock(std::FILE* readFrom, long int readOffset, long int bytesToRead) : size{bytesToRead}, offset{readOffset}
	{
		rawData = new char[size];
		fseek(readFrom, readOffset, SEEK_SET);
		fread(rawData, sizeof(char), size, readFrom);
	}

	ChituDataBlock(char* readFrom, long int readOffset, long int bytesToRead) : size{ bytesToRead }
	{
		isSubsetOfLarger = true;
		rawData = readFrom + readOffset;
	}

	~ChituDataBlock()
	{
		if (rawData != nullptr && !isSubsetOfLarger)
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

	template <typename T>
	T GetValueByKey(std::string key)
	{
		try
		{
			ChituData* datum = data.at(key);

			if (datum->type == DataType::INT)
			{
				return (static_cast<ChituInt*>(datum))->value;
			}
			else if (datum->type == DataType::FLOAT)
			{
				return (static_cast<ChituFloat*>(datum))->value;
			}
			else if (datum->type == DataType::ADDRESS)
			{
				return (static_cast<ChituAddress*>(datum))->value;
			}
			else if (datum->type == DataType::SHORT)
			{
				return (static_cast<ChituShort*>(datum))->value;
			}

			return -1;
		}
		catch (const std::out_of_range& outOfRangeError)
		{
			return -1;
		}
	}

	template <>
	long int GetValueByKey<long int>(std::string key)
	{
		try
		{
			ChituData* datum = data.at(key);

			if (datum->type == DataType::INT)
			{
				return (static_cast<ChituInt*>(datum))->value;
			}
			else if (datum->type == DataType::ADDRESS)
			{
				return (static_cast<ChituAddress*>(datum))->value;
			}

			return -1;
		}
		catch (const std::out_of_range& outOfRangeError)
		{
			return -1;
		}
	}

	template <>
	float GetValueByKey<float>(std::string key)
	{
		try
		{
			ChituData* datum = data.at(key);

			if (datum->type == DataType::FLOAT)
			{
				return (static_cast<ChituFloat*>(datum))->value;
			}

			return -1;
		}
		catch (const std::out_of_range& outOfRangeError)
		{
			return -1;
		}
	}

	template <>
	short int GetValueByKey<short int>(std::string key)
	{
		try
		{
			ChituData* datum = data.at(key);

			if (datum->type == DataType::SHORT)
			{
				return (static_cast<ChituShort*>(datum))->value;
			}

			return -1;
		}
		catch (const std::out_of_range& outOfRangeError)
		{
			return -1;
		}
	}

	virtual void ReportData(std::ostream* targetStream, int tabLevel = 0)
	{

		for (std::vector<std::string>::iterator dataEntry = dataInsertionOrder.begin(); dataEntry != dataInsertionOrder.end(); dataEntry++)
		{
			std::string tabString = "";

			ChituData* datum = data[*dataEntry];

			for (int i = 0; i < tabLevel; i++) tabString.append(1, '\t');

			*targetStream << tabString << datum->name << ": ";

			if (datum->type == DataType::INT)
			{
				*targetStream << GetValueByKey<long int>(*dataEntry) << std::endl;
			}
			else if (datum->type == DataType::FLOAT)
			{
				*targetStream << GetValueByKey<float>(*dataEntry) << std::endl;
			}
			else if (datum->type == DataType::ADDRESS)
			{
				*targetStream << GetValueByKey<long int>(*dataEntry) << std::endl;
			}
			else if (datum->type == DataType::SHORT)
			{
				*targetStream << GetValueByKey<short int>(*dataEntry) << std::endl;
			}
		}
	}
};