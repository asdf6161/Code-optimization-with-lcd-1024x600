/*
 * ReadFile.cpp
 *
 *  Created on: 15 мар. 2019 г.
 *      Author: yura
 */

#include <ReadFile.h>

ReadFile::ReadFile() {
	// TODO Auto-generated constructor stub
	this->byteswritten = 0;
	this->is_open = false;
	this->end_file = true;  // file not open
	this->read_bytes = 0;
	this->br = 0;
	if (this->__mount() != FR_OK){
		Error_Handler();
	}
}

ReadFile::~ReadFile() {
	// TODO Auto-generated destructor stub
}

FRESULT ReadFile::formatting(){
	return f_mkfs((TCHAR const*)SDPath, FM_ANY, 0, this->buffer, (UINT)512);
}

FRESULT ReadFile::open(const char *filepath, BYTE type){
	FRESULT r = f_open(&MyFile, filepath, type);
	if (r == FR_OK){
		this->end_file = false;
		this->is_open = true;
	} else {
		this->is_open = false;
	}
	return r;
}

FRESULT ReadFile::write(BYTE *arr, UINT arr_size){
	return f_write(&MyFile, arr, arr_size, &this->byteswritten);
}

string ReadFile::read_line(){
	if (this->is_open && !this->end_file){
		string str;
		bool line_end = false;
		while(!line_end){
			if (this->read_bytes == this->br){  // if all bytes from buffer are read
				this->read_bytes = 0;
				this->br = 0;
				f_read(&MyFile, &this->buffer, BUFFER_READ_SIZE, &this->br);
				if (br < BUFFER_READ_SIZE){
					// File fully read
					this->end_file = true;
					line_end = true;
				}
			}

			// read to newline
			for (;this->read_bytes < this->br; ++this->read_bytes) {
				str += this->buffer[this->read_bytes];
				if (this->buffer[this->read_bytes] == '\n'){
					this->read_bytes++;
					line_end = true;
					break;
				}
			}
		}
		return str;
	} else {
		return nullptr;
	}
	return nullptr;
}

FRESULT ReadFile::close(){
	FRESULT res = f_close(&MyFile);
//	FATFS_UnLinkDriver(SDPath);
	this->is_open = false;
	return res;
}

bool ReadFile::file_is_end(){
	return this->end_file;
}

bool ReadFile::file_is_open(){
	return this->is_open;
}

FRESULT ReadFile::__mount(){
	return f_mount(&SDFatFs, (TCHAR const*)SDPath, 0);
}
