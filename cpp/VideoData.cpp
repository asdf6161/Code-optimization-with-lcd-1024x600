#include "main.h"
#include "VideoData.h"

VideoData::VideoData()
{
}


VideoData::~VideoData()
{
	for (auto &line : lines) {
		delete[] line;
	}
	lines.clear();
}

#ifdef WINDOWS
bool VideoData::openFile(std::string filename)
{
	std::string path = STD_PATH + filename;
	this->fs.open(path);
	if (!fs) {
		return false;
	}
#else
bool VideoData::openFile(const char *filename){
	FRESULT res = this->rf.open(filename, FA_READ);
	if (res != FR_OK){
		return false;
	}
#endif // WINDOWS
	// read L + 2 frames ans save to mem
	for (uint8_t i = 0; i < this->linesCnt; i++)
	{
		bType *Bufferline = new bType[pixelBufferRead];
		this->pixelCnt = 0;
#ifdef WINDOWS
		std::string str;
		std::getline(fs, str);
#else
		std::string str = this->rf.read_line();
#endif
		std::string digit = "";
		for (auto &ch : str)
		{
			if (ch == ',') {
				Bufferline[this->pixelCnt++] = std::stoi(digit);
				digit = "";
			}
			else {
				digit += ch;
			}
		}
		Bufferline[this->pixelCnt++] = (bType)std::stoi(digit);
		this->lines.push_back(Bufferline);
	}

	return true;
}

bool VideoData::readNext()
{
	bType *Bufferline = new bType[pixelCnt];
	uint16_t index = 0;
#ifdef WINDOWS
	std::string str;
	std::getline(fs, str);
#else
	std::string str = this->rf.read_line();
#endif
	std::string digit = "";
	for (auto &ch : str)
	{
		if (ch == ',') {
			Bufferline[index++] = std::stoi(digit);
			digit = "";
		}
		else {
			digit += ch;
		}
	}
	delete[] lines.front();
	this->lines.erase(this->lines.begin());  // remove first elem
	Bufferline[index++] = (bType)std::stoi(digit);
	this->lines.push_back(Bufferline);

	return false;
}

void VideoData::closeFile()
{
#ifdef WINDOWS
	this->fs.close();
#else
	this->rf.close();
#endif // WINDOWS
}

bType * VideoData::getLine()
{
	return this->lines.front();
}

bType * VideoData::getLowLine()
{
	return this->lines.back();
}

uint16_t VideoData::getPixelCnt()
{
	return uint16_t(this->pixelCnt);
}
