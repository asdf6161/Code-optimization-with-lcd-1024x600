/*
 * ReadFile.h
 *
 *  Created on: 15 мар. 2019 г.
 *      Author: yura
 */

#ifndef READFILE_H_
#define READFILE_H_

#include "fatfs.h"
#include "string"

extern "C"{
#include "main.h"
}

#define BUFFER_READ_SIZE 512

using namespace std;

class ReadFile {
public:
	ReadFile();
	virtual ~ReadFile();
	FRESULT formatting();
	FRESULT open(const char *filepath, BYTE type);
	FRESULT write(BYTE *arr, UINT arr_size);
	string read_line();
	FRESULT close();
	bool file_is_end();
	bool file_is_open();

private:
	FRESULT __mount();

private:
	FATFS SDFatFs;
	FIL MyFile;
	UINT byteswritten;
	BYTE buffer[BUFFER_READ_SIZE];
	uint32_t read_bytes;
	UINT br;
	bool end_file;
	bool is_open;
};

#endif /* READFILE_H_ */
