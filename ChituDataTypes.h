#pragma once

#include <string>
#include <iostream>

#include "FileInterpreter.h"

enum DataType
{
	INT = 0,
	FLOAT = 1,
	ADDRESS = 2,
	SHORT = 3,
};

struct ChituData
{
	DataType type;
	long int size = -1;
	long int offset = -1;
	std::string name = "";
	char* rawData;

	ChituData(char* readFrom, long int offsetInBlock, DataType typeOfData, std::string humanReadableName = "UNKNOWN")
		: type{ typeOfData }, offset{ offsetInBlock }, name{ humanReadableName }
	{
		switch (type)
		{
		case INT:
		case FLOAT:
		case ADDRESS:
			size = 4;
			break;
		case SHORT:
			size = 2;
			break;
		}
		this->rawData = readFrom;// +offsetInBlock;

		for (int i = 0; i < size; i++)
		{
			std::cout << this->rawData[i + offset];
		}
		std::cout << std::endl;
	}

	ChituData(const ChituData& copyFrom)
	{
		type = copyFrom.type;
		size = copyFrom.size;
		offset = copyFrom.offset;
		name = copyFrom.name;
		rawData = copyFrom.rawData;
	}

	~ChituData()
	{
		size = -1;
		offset = -1;
		//name.clear();
		rawData = nullptr;
	}
};

struct ChituInt : public ChituData
{
	long int value = -1;

	ChituInt(char* readFrom, long int offsetInBlock, std::string humanReadableName = "UNKNOWN")
		: ChituData(readFrom, offsetInBlock, DataType::INT, humanReadableName)
	{
		ReadFromBinary(this->value, this->rawData, this->offset);
		std::cout << value << std::endl;
	}

	ChituInt(const ChituInt& copyFrom) : ChituData(copyFrom)
	{
		value = copyFrom.value;
	}

	~ChituInt()
	{
		value = -1;
	}
};

struct ChituFloat : public ChituData
{
	float value = -1;

	ChituFloat(char* readFrom, long int offsetInBlock, std::string humanReadableName = "UNKNOWN")
		: ChituData(readFrom, offsetInBlock, DataType::FLOAT, humanReadableName)
	{
		ReadFromBinary(this->value, this->rawData, this->offset);
		std::cout << value << std::endl;
	}

	ChituFloat(const ChituFloat& copyFrom) : ChituData(copyFrom)
	{
		value = copyFrom.value;
	}

	~ChituFloat()
	{
		value = -1;
	}
};

struct ChituAddress : public ChituData
{
	long int value = -1;

	ChituAddress(char* readFrom, long int offsetInBlock, std::string humanReadableName = "UNKNOWN")
		: ChituData(readFrom, offsetInBlock, DataType::ADDRESS, humanReadableName)
	{
		ReadFromBinary(this->value, this->rawData, this->offset);
		std::cout << value << std::endl;
	}

	ChituAddress(const ChituAddress& copyFrom) : ChituData(copyFrom)
	{
		value = copyFrom.value;
	}

	~ChituAddress()
	{
		value = -1;
	}
};

struct ChituShort : public ChituData
{
	short int value = -1;

	ChituShort(char* readFrom, long int offsetInBlock, std::string humanReadableName = "UNKNOWN")
		: ChituData(readFrom, offsetInBlock, DataType::SHORT, humanReadableName)
	{
		ReadFromBinary(this->value, this->rawData, this->offset);
		std::cout << value << std::endl;
	}

	ChituShort(const ChituShort& copyFrom) : ChituData(copyFrom)
	{
		value = copyFrom.value;
	}

	~ChituShort()
	{
		value = -1;
	}
};