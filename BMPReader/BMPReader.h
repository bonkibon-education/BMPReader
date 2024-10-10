#pragma once


#ifndef _BMP_READER // _BMP_READER
#define _BMP_READER


#include "BinaryFileManager.h"
#include <windows.h>
#include <vector>


// Color item structure 
struct ColorSymbol {
	const unsigned char symbol;
	const unsigned short int colors[3];
};

// Colors / Symbol: RGB
static const ColorSymbol CONST_COLOR_STORE[] = {
	{'.', {255, 255, 255}},
	{'#', {0, 0, 0}},
};

// Symbol for the error event
static const unsigned char CONST_SYMBOL_ERROR = '?';

// Color store size
static const size_t CONST_COLOR_STORE_SIZE = sizeof(CONST_COLOR_STORE) / sizeof(ColorSymbol);


class BMPReader : public BinaryFileManager {
private:
	std::unique_ptr<char[]> _data = nullptr;
	std::ifstream::pos_type _dataSize = 0;

	BITMAPFILEHEADER bmpHeader;
	BITMAPINFOHEADER bmpInfoHeader;

	/**
	 * Calculates the row padding required for a BMP image
	 *
	 * @param width The width of the BMP image in pixels.
	 * @param bytesPerPixel The number of bytes per pixel in the BMP image.
	 * @return The number of bytes of padding required for each row of the BMP image.
	*/
	static const unsigned short int _getRowPadding(const unsigned long int& width, const unsigned short int& bytesPerPixel) {
		return (4 - (width * bytesPerPixel) % 4) % 4;
	}

	/**
	 * Compares two 1-dimensional arrays.
	 *
	 * @param array1 The first array to compare.
	 * @param array2 The second array to compare.
	 * @return true if the two arrays are equal, false otherwise.
	*/
	static const bool _arraysEqual1D(const unsigned short int(&array1)[3], const unsigned short int(&array2)[3]) {
		return array1[0] == array2[0] && array1[1] == array2[1] && array1[2] == array2[2];
	}

	/**
	 * Retrieves the color symbol equal to the given RGB color values.
	 *
	 * @param colors array RGB.
	 * @return The color symbol equal to the provided RGB color values.
	*/
	static const unsigned char _findColorSymbol(const unsigned short int(&colors)[3]) {
		for (int i = 0; i < CONST_COLOR_STORE_SIZE; i++) {
			if (_arraysEqual1D(CONST_COLOR_STORE[i].colors, colors)) {
				return CONST_COLOR_STORE[i].symbol;
			}
		}

		return CONST_SYMBOL_ERROR;
	}

public:
	BMPReader(const std::string& filename) : BinaryFileManager(filename, "bmp"), bmpHeader(), bmpInfoHeader() {}

	/**
	 * Opens a BMP file.
	 *
	 * @throws std::runtime_error If an error occurs while opening, reading, or closing the file.
	*/
	const void openBMP() {
		if (!this->openFile()) {
			throw "InvalidOpenBMPFile: " + BinaryFileManager::fileName();
		}

		if (!this->getFileSize(_dataSize)) {
			this->closeFile();
			throw "InvalidGetFileSize: " + BinaryFileManager::fileName();
		}

		if (!this->readFile(_dataSize, _data)) {
			this->closeFile();
			throw "InvalidReadFile: " + BinaryFileManager::fileName();
		}

		if (!this->closeFile()) {
			throw "InvalidCloseFile: " + BinaryFileManager::fileName();
		}

		std::memcpy(&bmpHeader, _data.get(), sizeof(BITMAPFILEHEADER));
		std::memcpy(&bmpInfoHeader, _data.get() + sizeof(BITMAPFILEHEADER), sizeof(BITMAPINFOHEADER));
	}

	/**
	 * Displays BMP image
	 * @note This function parses pixels and replace the RGB color value into a character value.
	*/
	template <typename T1>
	void displayBMP(T1& logStream) const {
		static const int height = bmpInfoHeader.biHeight;
		static const unsigned int heightAbs = abs(height);
		static const unsigned long int width = bmpInfoHeader.biWidth;
		static const unsigned long int bytesPerPixel = bmpInfoHeader.biBitCount / 8;
		static const unsigned long int rowPadded = ((bmpInfoHeader.biWidth * bytesPerPixel) + 3) / 4 * 4;

		// Pixels vector (for remove line reordering)
		std::vector<std::vector<uint8_t>> pixelsVector(heightAbs, std::vector<uint8_t>(width * bytesPerPixel));

		// Parse pixels / fill pixelVector
		for (unsigned int y = 0; y < heightAbs; y++) {

			// Calculate correct move for pixels
			int offsetRow = (height > 0) ? (heightAbs - 1 - y) : y;

			for (unsigned int x = 0; x < width; x++) {

				// Calculate pixel offsets
				int offsetPixel = bmpHeader.bfOffBits + y * rowPadded + x * bytesPerPixel;

				// Get colors 
				uint8_t color1 = static_cast<uint8_t>(_data[offsetPixel]);
				uint8_t color2 = static_cast<uint8_t>(_data[offsetPixel + 1]);
				uint8_t color3 = static_cast<uint8_t>(_data[offsetPixel + 2]);

				// Set colors to pixels 
				pixelsVector[offsetRow][x * bytesPerPixel] = color1;
				pixelsVector[offsetRow][x * bytesPerPixel + 1] = color2;
				pixelsVector[offsetRow][x * bytesPerPixel + 2] = color3;
			}
		}

		// Show correct image. Log to console/file symbol
		for (unsigned int i = 0; i < heightAbs; i++) {
			for (unsigned int j = 0; j < width; j++) {
				unsigned short int blue = pixelsVector[i][j * bytesPerPixel];
				unsigned short int green = pixelsVector[i][j * bytesPerPixel + 1];
				unsigned short int red = pixelsVector[i][j * bytesPerPixel + 2];

				logStream << _findColorSymbol({ red , green , blue });
			}

			logStream << std::endl;
		}

		logStream << std::endl;
	}

	/**
	 * Displays the BMP file structure:
	 * - BITMAPFILEHEADER;
	 * - BITMAPINFOHEADER;
	*/
	template <typename T1, typename T2>
	void displayInfo(T1& logStream, const T2& name) const {
		logStream << "\n\n --------------- displayInfo: " << name << " " << "--------------- \n\n" << std::endl;

		logStream << "BITMAPFILEHEADER:" << std::endl;
		logStream << "  bfType: " << bmpHeader.bfType << std::endl;
		logStream << "  bfSize: " << bmpHeader.bfSize << std::endl;
		logStream << "  bfReserved1: " << bmpHeader.bfReserved1 << std::endl;
		logStream << "  bfReserved2: " << bmpHeader.bfReserved2 << std::endl;
		logStream << "  bfOffBits: " << bmpHeader.bfOffBits << std::endl;

		logStream << "BITMAPINFOHEADER:" << std::endl;
		logStream << "  biSize: " << bmpInfoHeader.biSize << std::endl;
		logStream << "  biWidth: " << bmpInfoHeader.biWidth << std::endl;
		logStream << "  biHeight: " << bmpInfoHeader.biHeight << std::endl;
		logStream << "  biPlanes: " << bmpInfoHeader.biPlanes << std::endl;
		logStream << "  biBitCount: " << bmpInfoHeader.biBitCount << std::endl;
		logStream << "  biCompression: " << bmpInfoHeader.biCompression << std::endl;
		logStream << "  biSizeImage: " << bmpInfoHeader.biSizeImage << std::endl;
		logStream << "  biXPelsPerMeter: " << bmpInfoHeader.biXPelsPerMeter << std::endl;
		logStream << "  biYPelsPerMeter: " << bmpInfoHeader.biYPelsPerMeter << std::endl;
		logStream << "  biClrUsed: " << bmpInfoHeader.biClrUsed << std::endl;
		logStream << "  biClrImportant: " << bmpInfoHeader.biClrImportant << std::endl;
		logStream << std::endl;
	}

	const void closeBMP() {
		this->closeFile();
	}

	~BMPReader() {
		this->closeBMP();
	}
};

#endif // _BMP_READER