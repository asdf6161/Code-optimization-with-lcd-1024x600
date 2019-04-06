
/*
 *  Example
 *  VideoData *data = new VideoData();
 *  line = data->getLine();
	lineLow = data->getLowLine();

	data->readNext();
	line2 = data->getLine();
	lineLow2 = data->getLowLine();

	data->closeFile();
	delete data;
 * */

#include <vector>
#include <string>
#include "ReadFile.h"
#pragma once

class VideoData
{
public:
	VideoData();
	virtual ~VideoData();

#ifdef WINDOWS
	bool openFile(std::string filename);
#else
	bool openFile(const char *filename);
#endif
	bool readNext();
	void closeFile();

	bType *getLine();
	bType *getLowLine();
	uint16_t getPixelCnt();

	// vars
public:

// methods
private:


	// vars
private:
	uint16_t pixelBufferRead = 4096;

	std::vector<bType *> lines;
#ifdef WINDOWS
	std::fstream fs;
#else
	ReadFile rf;
#endif

	// for diff with deviation
	uint8_t H = 2;
	uint8_t G = H + 1;
	uint8_t shift_len = 2;

	uint8_t L = 2;  // distance between lines
	uint16_t pixelCnt = 0;
	uint8_t linesCnt = L + 2;
};

