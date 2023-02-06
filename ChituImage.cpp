#include "ChituImage.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

void ChituPreviewImage::DecodeImage()
{
	//if we've already converted the image
	//there's no point doing it again
	if (converted) return;

	long int maxIndex = decodedWidth * decodedHeight;
	imageAsRGBA = new long int[maxIndex];

	//absolutley no clue what's happening here;
	//used the code I found from the "Photon File
	//Validator" and it correctly decoded the preview
	//images. The algorithm works so I won't question it!
	long int d = 0;
	for (long i = 0; i < encodedImgSize; i++)
	{
		long int b1, b2;
		ReadFromBinary(b1, encodedData, i);
		ReadFromBinary(b2, encodedData, i + 1);
		long int dot = b1 & 0xFF | ((b2 & 0xFF) << 8);
		i++;

		long int red = ((dot >> 11) & 0x1F) << 3;
		long int green = ((dot >> 6) & 0x1F) << 3;
		long int blue = (dot & 0x1F) << 3;
		long int alpha = 0xFF;
		long int color = alpha << 24 | blue << 16 | green << 8 | red;

		int repeat = 1;
		if ((dot & 0x0020) == 0x0020)
		{
			i++;
			ReadFromBinary(b1, encodedData, i);
			ReadFromBinary(b2, encodedData, i + 1);
			repeat += b1 & 0xFF | ((b2 & 0x0F) << 8);
			i++;
		}

		while (repeat > 0)
		{
			if (d < (maxIndex)) imageAsRGBA[d++] = color;
			repeat--;
		}
	}

	converted = true;
}

int ChituPreviewImage::SaveImage(std::string imgName)
{
	//if a name isn't supplied, just name the
	//file whatever the address of the image
	//data is-- ensures no duplicates!
	if (imgName == "") imgName = std::to_string(imgStartAddress);
	std::string fileName = "OUTPUT\\" + imgName + ".bmp";

	//only do the conversion if we haven't already!
	if (!converted)
	{
		DecodeImage();
	}

	//returns 0 if there was an error, so we can handle
	//that wherever this function returns
	return stbi_write_bmp(fileName.c_str(), decodedWidth, decodedHeight, 4, imageAsRGBA);	
}
