
#pragma once 
#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <Windows.h>

using namespace std;

typedef struct {
	char	szChunkID[10];					// ���SizeΪ4 �Ļ���copy�������
	DWORD	ChunkSize;						// ͨ��Ϊ16����18������Size��ID��

	WORD	wFormatTag;
	WORD	wChannels;						// ��������
	DWORD	dwSamplesPerSec;				// ÿ���������
	DWORD	dwAvgBytesPerSec;				// ÿ�����BYTE
	WORD	wBlockAlign;
	WORD	wBitsPerSample;
	
	/* ����wFormatTag�Ĳ�ͬȡֵ�����ﻹ���ܳ��ָ����fields */
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
	BYTE	Data[4];				// ���������Ƿ�Size�Ѿ��̶����أ���Ϊdata��SizeΪ4��

}FACT_CHUNK;


// ע�⣺data chunk�Ľṹ���ǰ���File�е�˳�򣬶��ǰ���Channel���仮�֣���
typedef struct{
	char	szChunkID[5];
	DWORD	ChunkSize;
	DWORD	*SampleOfChannel[2];
	DWORD	SampleCount;			// data �����ж��ٸ������㡣
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