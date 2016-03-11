
#pragma once 
#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <Windows.h>

using namespace std;

typedef struct {
	char	szChunkID[10];					// 如果Size为4 的话，copy会出问题
	DWORD	ChunkSize;						// 通常为16或者18（不连Size和ID）

	WORD	wFormatTag;
	WORD	wChannels;						// 声道个数
	DWORD	dwSamplesPerSec;				// 每秒采样次数
	DWORD	dwAvgBytesPerSec;				// 每秒多少BYTE
	WORD	wBlockAlign;
	WORD	wBitsPerSample;
	
	/* 根据wFormatTag的不同取值，这里还可能出现更多的fields */
	WORD	Reserved;
} FORMAT_CHUNK;

typedef struct{
	char	szChunckID[10];
	DWORD	ChunkSize;
	char	szType[10];

}RIFF_HEADER;

typedef struct {
	char	szChunkID[10];
	DWORD	ChunkSize;
	BYTE	Data[4];				// 这样看来是否Size已经固定了呢？因为data的Size为4；

}FACT_CHUNK;


// 注意：data chunk的结构不是按照File中的顺序，而是按照Channel将其划分，并
typedef struct{
	char	szChunkID[5];
	DWORD	ChunkSize;
	DWORD	*SampleOfChannel[2];
	DWORD	SampleCount;			// data 部分有多少个采样点。
}DATA_CHUNK;


class RIFF_FILE
{

public:
	RIFF_FILE(char * filename);
	~RIFF_FILE();
	

public:

	int		ParseRIFFFile();

	BOOL	m_isErrorOccur;					// if error occured ,this value is TRUE
	DWORD	m_ErrorCode;					// if error occured ,this value is not 0.

	char	m_fileName[256];

	DWORD	m_fileSize;
	
	BOOL	m_isExistFactChunk;				// optional

	RIFF_HEADER		m_header;
	FORMAT_CHUNK	m_FormatChunk;
	FACT_CHUNK		m_FactChunk;
	DATA_CHUNK		m_DataChunk;

private :
	//CString		m_debugStr;		// used to show debug message to dbgview.
	// parse different chunk
	int		ParseFactChunk(ifstream & ifs,char * chunkID ,DWORD ChunkSize);
	int		ParseDataChunk(ifstream & ifs,char * chunkID ,DWORD ChunkSize);
	int		ParseFormatChunk(ifstream & ifs,char * chunkID ,DWORD ChunkSize);


};