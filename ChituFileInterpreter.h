#pragma once

#include <vector>

template <typename T>
static void ReadFromBinary(T& readInto, char* data, long int readAt = 0)
{
	//std::cout << readAt << "\n";
	std::memcpy(&readInto, &data[readAt], sizeof(T));
	//std::cout << readInto << "\n";
}

struct ChituPointer
{
	long int chituAddress = 0;
};

template <>
static void ReadFromBinary<ChituPointer>(ChituPointer& readInto, char* data, long int readAt)
{
	std::memcpy(&readInto.chituAddress, data + readAt, 4);
}

static char BinToHex(char val)
{
	char hexValues[16] = {
		'0', '1', '2', '3', '4', '5', '6', '7',
		'8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
	};

	return hexValues[val];
}

static std::string BinToHex(char* data, int bytesToRead)
{
	std::string asString = "";

	for (int i = 0; i < bytesToRead; i++)
	{
		int msb = (data[i] & 0xF0) >> 4;
		int lsb = (data[i] & 0x0F);

		asString.append(1, BinToHex(msb));
		asString.append(1, BinToHex(lsb));
	}

	return asString;
}

static void RawDataToStream(char* rawData, int bytesToConvert, std::ostream* stream, int tabLevel = 0)
{
	std::string tabString = "";
	for (int i = 0; i < tabLevel; i++) tabString.append(1, '\t');

	std::string hexAsString = BinToHex(rawData, bytesToConvert);

	int charactersPerGroup = 16;
	for (int i = 0; i < bytesToConvert; i++)
	{
		std::string temp;

		if ((i % charactersPerGroup) == 0)
		{
			*stream << "\n" << tabString;
		}
		else if ((i % 8) == 0)
		{
			*stream << "| ";
		}
		*stream << hexAsString.substr(i * 2, 2) << " ";
	}
	*stream << "\n\n";
}
