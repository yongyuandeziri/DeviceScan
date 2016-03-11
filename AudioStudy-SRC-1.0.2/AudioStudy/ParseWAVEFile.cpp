
#include "stdafx.h"
#include "ParseWAVEFile.h"
//#include "myFFT.h"



RIFF_FILE::RIFF_FILE(char * fileName)
{
	OutputDebugString("Enter RIFF_FILE Construction ");
	m_isErrorOccur= FALSE;
	m_ErrorCode = 0;

	//m_debugStr = "";
	
	if(fileName == NULL || strlen(fileName) <=4) // extension.
	{
		cout<<"file name is invalid.\n"<<endl;
		
	//	m_debugStr.Format("File name is invalid .");
		OutputDebugString("File name is invalid");
	//	dbgprintf(dbg,"%s\n",m_debugStr);

		m_ErrorCode = 255;
		m_isErrorOccur = TRUE;
	}
	strcpy_s(m_fileName,fileName);
	ZeroMemory(&m_header,sizeof(RIFF_HEADER));
	ZeroMemory(&m_FormatChunk,sizeof(FORMAT_CHUNK));
	ZeroMemory(&m_FactChunk,sizeof(FACT_CHUNK));
	ZeroMemory(&m_DataChunk,sizeof(DATA_CHUNK));

	m_fileSize = 0;
	m_isExistFactChunk = 0;
}


RIFF_FILE::~RIFF_FILE()
{
	cout<<"enter RIFF_FILE destruct"<<endl;
	//m_debugStr = "Enter RIFF_FILE destruction";
	//OutputDebugString(m_debugStr);
	//dbgprintf(dbg,"%s\n",m_debugStr);

	if(m_DataChunk.SampleOfChannel[0] != NULL)
	{
		cout<<"free sample of channel  0."<<endl;
		OutputDebugString("Free samples of channel 0");
		delete m_DataChunk.SampleOfChannel[0];
		m_DataChunk.SampleOfChannel[0] = NULL;
	}

	if(m_DataChunk.SampleOfChannel[1] != NULL)
	{
		cout<<"free sample of channel 1 ."<<endl;
		OutputDebugString("Free samples of channel 1");
		delete m_DataChunk.SampleOfChannel[1];
		m_DataChunk.SampleOfChannel[1] = NULL;
	}

	cout<<"exit RIFF_FILE destruct"<<endl;
	//m_debugStr.Format("Exit RIFF_FILE destruction");
	///OutputDebugString(m_debugStr);
	//dbgprintf(dbg,"%s\n",m_debugStr);
}


int RIFF_FILE::ParseRIFFFile()
{
	cout<<"enter ParseRIFFFile() ."<<endl;
	//m_debugStr = "Enter function ParseRIFFFile()";
	//OutputDebugString(m_debugStr);
	//dbgprintf(dbg,"%s\n",m_debugStr);

	if(m_isErrorOccur )
	{
		cout<<"Error occured "<<endl;
	//	m_debugStr = "Error Occured";
		OutputDebugString("Error occured ");
	//	dbgprintf(dbg,"%s\n",m_debugStr);
		return 255;
	}
	int retval = 0;
	ifstream	ifs;
	ifs.open(m_fileName,ios::binary);

	if(ifs.fail() == true)
	{
		cout<<"Open file fail"<<endl;
	//	m_debugStr = "Open file fail";
		OutputDebugString("Open file fail");
	//	dbgprintf(dbg,"%s\n",m_debugStr);
		m_ErrorCode = 255;
		m_isErrorOccur = TRUE;
		return 255;
	}

	BYTE temp[256];
	cout<<"Read file header ."<<endl;
	OutputDebugString("Read file header ");
	//m_debugStr = "Read file header ";
	//dbgprintf(dbg,"%s\n",m_debugStr);
	ifs.read((char *)temp,12);					// ¹Ì¶¨¸ñÊ½ ,RIFF file header .
	
	for(int mm = 0 ;mm<4;mm++)
	{
		m_header.szChunckID[mm] = temp[mm];
	}

	m_fileSize = (DWORD)temp[7] << 24 | (DWORD)temp[6]<<16 | (DWORD)temp[5]<<8 | temp[4];
	m_header.ChunkSize = m_fileSize;
	m_fileSize += 8;		// RIFF and size .

	for(int mm = 0 ;mm<4;mm++)
	{
		m_header.szType[mm] = temp[mm+8];
	}

	cout << "file size is : "<<m_fileSize<<endl;
	cout<<"header :"<<m_header.szChunckID<<endl;
	cout << "file type is : "<<m_header.szType<<endl;
	cout << "chunk size is : "<<m_header.ChunkSize<<endl;
	cout<<endl;

	OutputDebugString("File info:");
	//m_debugStr.Format("File size : %d \nHeader ID : %s \nFile Type : %s \nChunkSize : %d(0x%lx)"
	//	,m_fileSize,m_header.szChunckID,m_header.szType,m_header.ChunkSize,m_header.ChunkSize);
	//dbgprintf(dbg,"%s\n",m_debugStr);


	DWORD	ChunkSize = 0;
	char	ChunkID[10] = {0};

	cout<<"before while loop to parse chunks ."<<endl;
	OutputDebugString("Before Parse Chunks");
	//m_debugStr = "before parse chunks .";
	//dbgprintf(dbg,"%s\n",m_debugStr);

	while (ifs.eof() == false)
	{
		ZeroMemory(temp,10);
		ChunkSize = 0;

		ifs.read((char*)temp,8);				// format chunk id and size .
		if(ifs.eof() == true)
		{
			cout<<endl<<"read id eof "<<endl<<endl;
			OutputDebugString("Read ID EOF");
	//		m_debugStr = "Read ID EOF";
	//		dbgprintf(dbg,"%s\n",m_debugStr);
			break;
		}

		for(int mm = 0;mm<4;mm++)
		{
			ChunkID[mm] = temp[mm];
		}

		ChunkSize = (DWORD)temp[7]<< 24 | (DWORD)temp[6]<<16 | (DWORD)temp[5]<<8 | (DWORD)temp[4];
		
		if(_stricmp(ChunkID,"fmt ") == 0)
		{
			retval = ParseFormatChunk(ifs,ChunkID,ChunkSize);
		}else if(_stricmp(ChunkID,"data") == 0)
		{
			retval = ParseDataChunk(ifs,ChunkID,ChunkSize);
		}else if(_stricmp(ChunkID,"fact") == 0)
		{
			retval = ParseFactChunk(ifs,ChunkID,ChunkSize);
		}else 
		{
			cout<<"Unknown chunk ID :"<<ChunkID<<endl;
		//	m_debugStr.Format("Unknow Chunk ID:%s ,size %d(0x%lx ) ,Fmt Chunk reserved data is : %lx ",ChunkID,ChunkSize,ChunkSize,m_FormatChunk.Reserved);
		//	OutputDebugString(m_debugStr);
		//	dbgprintf(dbg,"%s\n",m_debugStr);
			//m_isErrorOccur = TRUE;
			//m_ErrorCode = 255;
			break;
		}
	}
	cout<<"exit parse RIFF file !"<<endl<<endl<<endl;
	OutputDebugString("Exit ParseRIFFFile()");
//	m_debugStr = "Exit ParseRIFFFile()";
	//dbgprintf(dbg,"%s\n",m_debugStr);

	return retval;
}


int RIFF_FILE::ParseDataChunk(ifstream & ifs,char * ChunkID ,DWORD ChunkSize)
{
	cout<<endl<<endl<<"enter parse data chunk ."<<endl;
	cout<<"Chunk ID is : "<<ChunkID<<endl;
	cout <<"chunk size hex is : 0x"<<(hex)<<ChunkSize<<(dec)<<endl;
	
	//m_debugStr.Format("Enter Parse Data Chunk.\nChunk ID : %s \nChunkSize : %d(0x%lx)",ChunkID,ChunkSize,ChunkSize);
	//OutputDebugString(m_debugStr);
	//dbgprintf(dbg,"%s\n",m_debugStr);

	if(m_isErrorOccur == TRUE)
	{
		cout<<"error occured ."<<endl;
		OutputDebugString("Error Occured !");
		return 255;
	}

	int retval = 0;

	BYTE * temp = NULL;					// do not use char * for << will add flag 
	temp = new BYTE[ChunkSize];
	if(temp == NULL)
	{
		cout <<"Fail to alloc data chunk"<<endl;
		OutputDebugString("Fail to alloc data chunk");
	//	m_debugStr = "Fail to alloc data chunk";
	//	dbgprintf(dbg,"%s\n",m_debugStr);

		m_isErrorOccur = TRUE;
		m_ErrorCode = 255;
		return 255;
	}
	
	ZeroMemory(temp,ChunkSize);
	
	// seperate to read .every time read 4K bytes .

	ifs.read((char *)temp,ChunkSize);
	cout<<"Read "<<ifs.gcount()<<"bytes"<<endl;
	streamsize bytesread = ifs.gcount();
	//m_debugStr.Format("Read %d(0x%lx ) bytes",bytesread,bytesread);
	//OutputDebugString(m_debugStr);
	//dbgprintf(dbg,"%s\n",m_debugStr);

	if(ifs.fail() == true)
	{
		cout<<"read file fail "<<endl;
		OutputDebugString("Read file fail ");
	//	m_debugStr = "Read file Fail";
	//	dbgprintf(dbg,"%s\n",m_debugStr);

		m_isErrorOccur = TRUE;
		m_ErrorCode = 255;
		delete temp;
		temp = NULL;
		return 0255;
	}

	m_DataChunk.ChunkSize = ChunkSize;
	strcpy_s(m_DataChunk.szChunkID,ChunkID);
	
	if(m_FormatChunk.wBitsPerSample == 8)
	{
	}else if(m_FormatChunk.wBitsPerSample == 16)
	{	
	}else
	{
		cout <<"Invalid bitsPerSample "<<m_FormatChunk.wBitsPerSample<<" !"<<endl;
	//	m_debugStr.Format("invalid bitsPerSample %d ",m_FormatChunk.wBitsPerSample);
	//	OutputDebugString(m_debugStr);
	//	dbgprintf(dbg,"%s\n",m_debugStr);

		m_isErrorOccur = TRUE;
		m_ErrorCode = 255;
		delete temp;
		temp = NULL;
		return 255;
	}

	if(m_FormatChunk.wChannels == 1)
	{
	}else if(m_FormatChunk.wChannels == 2)
	{
	}
	else
	{
		cout<<"invlaid channels "<< m_FormatChunk.wChannels<<" !"<<endl;
	//	m_debugStr.Format("Invalid Channels : %d ",m_FormatChunk.wChannels);
	//	OutputDebugString(m_debugStr);
	//	dbgprintf(dbg,"%s\n",m_debugStr);

		m_isErrorOccur = TRUE;
		m_ErrorCode = 255;
		delete temp;
		temp = NULL;
		return 255;
	}

	m_DataChunk.SampleCount = m_DataChunk.ChunkSize /(m_FormatChunk.wChannels * m_FormatChunk.wBitsPerSample/8);
	cout<<"data chunk sample count is :"<<m_DataChunk.SampleCount<<endl;
	//m_debugStr.Format("Data Chunk Samples count is :%d",m_DataChunk.SampleCount);
	//OutputDebugString(m_debugStr);
	//dbgprintf(dbg,"%s\n",m_debugStr);

	for(int mm = 0;mm<m_FormatChunk.wChannels;mm++)
	{
		m_DataChunk.SampleOfChannel[mm] = new DWORD[m_DataChunk.SampleCount];
		if(m_DataChunk.SampleOfChannel[mm] == NULL)
		{
			cout <<"Fail to allocate data chunk sample array!"<<endl;
			OutputDebugString("Fail to alloc Data Chunk Sample Array!");
		//	m_debugStr = "Fail to alloc Data Chunk Sample Array!";
		//	dbgprintf(dbg,"%s\n",m_debugStr);

			delete temp;
			temp = NULL;
			if(mm != 0) // generally 2 channels.
			{
				delete m_DataChunk.SampleOfChannel[0];
				m_DataChunk.SampleOfChannel[0] = NULL;
			}
			m_isErrorOccur = TRUE;
			m_ErrorCode = 255;
			return 255;
		}
		ZeroMemory(m_DataChunk.SampleOfChannel[mm],m_DataChunk.SampleCount);
	}

	DWORD sampleOffset  =0 ;
	DWORD sampleSize  = m_FormatChunk.wBitsPerSample/8;
	cout<<"sample size is : "<<sampleSize<<endl;
	//m_debugStr.Format("Sample size is %d bytes ",sampleSize);
	//OutputDebugString(m_debugStr);
	//dbgprintf(dbg,"%s\n",m_debugStr);

	if(sampleSize != 1&& sampleSize != 2&& sampleSize != 4)
	{
	//	m_debugStr.Format("Unknown sample size %d \n",sampleSize);
	//	OutputDebugString(m_debugStr);
	//	dbgprintf(dbg,"%s\n",m_debugStr);
		if(temp!= NULL)
		{
			delete temp;
		}
		delete m_DataChunk.SampleOfChannel[0];
		m_DataChunk.SampleOfChannel[0] = NULL;
		delete m_DataChunk.SampleOfChannel[1];
		m_DataChunk.SampleOfChannel[1] = NULL;
	}

	for(int mm = 0;mm<m_DataChunk.SampleCount;mm++)
	{
		// copy data to channel sample array  .
		for(int channelIndex = 0;channelIndex<m_FormatChunk.wChannels;channelIndex++)
		{
			sampleOffset = mm *m_FormatChunk.wChannels* sampleSize;
			switch(sampleSize)
			{
			case 1:
				m_DataChunk.SampleOfChannel[channelIndex][mm] = temp[sampleOffset];
				break;

			case 2:
				m_DataChunk.SampleOfChannel[channelIndex][mm] = (WORD)temp[sampleOffset+1]<<8|temp[sampleOffset];
				break;
			case 4:
				m_DataChunk.SampleOfChannel[channelIndex][mm] = (DWORD)temp[sampleOffset+3]<<24|(DWORD)temp[sampleOffset+2]<<16|(DWORD)temp[sampleOffset+1]|temp[sampleOffset];
				break;

			default:
				cout<<"Unknown sample size :"<<sampleSize<<endl;

				m_isErrorOccur = TRUE;
				m_ErrorCode = 255;
				break;
			}
		}
	}

	if(temp != NULL)
	{
		delete temp;
	}

	double * tempbuffer = new double[m_DataChunk.SampleCount];
	if(tempbuffer == NULL)
	{
		return 255;
	}

	for(int mm = 0;mm<m_DataChunk.SampleCount;mm++)
	{
		tempbuffer[mm] = m_DataChunk.SampleOfChannel[0][mm]*1.0/32768;
	}

	//StoreToFile(tempbuffer,m_DataChunk.SampleCount,"oirginaldata.txt");
	delete tempbuffer;
	

	return retval;
}


int RIFF_FILE::ParseFactChunk(ifstream & ifs,char * ChunkID ,DWORD ChunkSize)
{
	cout <<"enter parse fact chunk"<<endl;
	OutputDebugString("Enter Parse Fact Chunk");
	//m_debugStr.Format("Enter Parse Fact Chunk");
	//dbgprintf(dbg,"%s\n",m_debugStr);

	if(m_isErrorOccur == TRUE)
	{
		cout<<"error occured."<<endl;
		OutputDebugString("Error Occured ");
		return 255;
	}

	if(ChunkSize < 12)
	{
		cout <<"Fact chunk size "<< ChunkSize <<" < 12"<<endl;
	//	m_debugStr.Format("Chunk Size %d < 12",ChunkSize);
	//	OutputDebugString(m_debugStr);
	//	dbgprintf(dbg,"%s\n",m_debugStr);

		m_isErrorOccur = TRUE;
		m_ErrorCode = 255;
		return 255;
	}

	int retval = 0;

	BYTE * temp = NULL;
	temp = new BYTE[ChunkSize];
	if(temp == NULL)
	{
		cout <<"Fail to allock fact chunk"<<endl;
		OutputDebugString("Fail to alloc Fact Chunk buffer");
	//	m_debugStr.Format("Fail to alloc Fact Chunk buffer");
	//	dbgprintf(dbg,"%s\n",m_debugStr);

		m_isErrorOccur = TRUE;
		m_ErrorCode = 255;
		return 255;
	}

	ifs.read((char *)temp,ChunkSize);
	
	m_FactChunk.ChunkSize = ChunkSize;
	strcpy_s(m_FactChunk.szChunkID,ChunkID);

	for(int mm = 0;mm<4;mm++)
	{
		m_FactChunk.Data[mm] = temp[mm+8];
	}

	if(temp != NULL)
	{
		OutputDebugString("Free data buffer");
		cout<<"delete temp buffer"<<endl;
		cout<<"bofore delete :"<<(DWORD)temp<<endl;
		delete temp;
		cout<<"after delete :"<<(DWORD)temp<<endl;
	}

	cout<<"show Fact chunk information:"<<endl;
	cout<<" ID :"<<m_FactChunk.szChunkID<<endl;
	cout<<" Length: "<<m_FactChunk.ChunkSize<<endl;
	cout<<" Data: "<<m_FactChunk.Data<<endl;

	//m_debugStr.Format("show Fact chunk info :\nChunk ID: %s \nChunkSize : %d \nChunk Data :%2.2x %2.2x %2.2x \n"
	//	,m_FactChunk.szChunkID,m_FactChunk.ChunkSize,m_FactChunk.Data[0],m_FactChunk.Data[1],m_FactChunk.Data[2],m_FactChunk.Data[3]);
	//OutputDebugString(m_debugStr);
	//dbgprintf(dbg,"%s\n",m_debugStr);

	return retval;
}


int RIFF_FILE::ParseFormatChunk(ifstream & ifs,char * ChunkID ,DWORD ChunkSize)
{
	cout <<endl<<endl<<"enter function parseformat chunk"<<endl;

	cout<<"ID :"<<ChunkID<<", Size :"<<ChunkSize<<endl;
	//m_debugStr.Format("Enter Function ParseFormatChunk()\nChunk ID: %s ,Chunk Size : %d ",ChunkID,ChunkSize);
	//OutputDebugString(m_debugStr);
	//dbgprintf(dbg,"%s\n",m_debugStr);
	if(m_isErrorOccur == TRUE)
	{
		cout<<"error occured"<<endl;
		OutputDebugString("Error Occured");
		return 255;
	}

	int retval = 0;

	if(ChunkSize <0x10)
	{
		cout<<"format chunk size "<< ChunkSize<<" < 16"<<endl;
		OutputDebugString("Chunk size is nvalid ");
	//	m_debugStr.Format("Chunk size is nvalid ");
	//	dbgprintf(dbg,"%s\n",m_debugStr);
		m_isErrorOccur = TRUE;
		m_ErrorCode = 255;
		return 255;
	}

	BYTE * temp = NULL;
	temp = new BYTE[ChunkSize+1];
	if(temp == NULL)
	{
		cout <<"Fail to alloc fact chunk"<<endl;
		OutputDebugString("Fail to alloc Format chunk buffer");
	//	m_debugStr.Format("Fail to alloc Format chunk buffer");
	//	dbgprintf(dbg,"%s\n",m_debugStr);
		m_isErrorOccur = TRUE;
		m_ErrorCode = 255;
		return 255;
	}
	
	ZeroMemory(temp,ChunkSize+1);

	ifs.read((char *)temp,ChunkSize);

	m_FormatChunk.ChunkSize = ChunkSize;
	strcpy_s(m_FormatChunk.szChunkID,ChunkID);

	m_FormatChunk.wFormatTag = (WORD)temp[1]<<8| temp[0];

	m_FormatChunk.wChannels = (WORD)temp[3]<<8|temp[2];

	m_FormatChunk.dwSamplesPerSec = (DWORD)temp[7]<<24 | (DWORD)temp[6]<<16 | (DWORD)temp[5]<<8 | temp[4];

	m_FormatChunk.dwAvgBytesPerSec = (DWORD)temp[11]<<24 | (DWORD)temp[10]<<16 | (DWORD)temp[9]<<8 | temp[8];

	m_FormatChunk.wBlockAlign = (WORD)temp[13]<<8|temp[12];

	m_FormatChunk.wBitsPerSample = (WORD)temp[15]<<8|temp[14];

	if(ChunkSize >16)
	{	
		m_FormatChunk.Reserved = (WORD)temp[17]<<8|temp[16];
		cout <<"copy additional field : "<<m_FormatChunk.Reserved<<endl;
	//	m_debugStr.Format("Copy additional field : %4.4x",m_FormatChunk.Reserved);
	///	OutputDebugString(m_debugStr);
	//	dbgprintf(dbg,"%s\n",m_debugStr);
	}

	if(temp != NULL)
	{
		delete temp;
	}
	

	cout<<"show format chunk info: "<<endl;
	cout<<"chunk ID :"<<m_FormatChunk.szChunkID<<endl;
	cout<<"chunk size : "<<m_FormatChunk.ChunkSize<<endl;
	cout<<"Format tag : "<<m_FormatChunk.wFormatTag<<endl;
	cout<<"Channels : "<<m_FormatChunk.wChannels<<endl;
	cout<<"sample Frequency : "<<m_FormatChunk.dwSamplesPerSec<<endl;
	cout<<"bytes per second : "<<m_FormatChunk.dwAvgBytesPerSec<<endl;
	cout<<"block align : "<<m_FormatChunk.wBlockAlign<<endl;
	cout<<"bits per sample : "<<m_FormatChunk.wBitsPerSample<<endl;
	
	cout<<endl;
	//m_debugStr.Format("Show Format Chunk Info :");
	//OutputDebugString(m_debugStr);
	//dbgprintf(dbg,"%s\n",m_debugStr);

	//m_debugStr.Format("Chunk id : %s \nChunk Size : %d(0x%lx)\nFormat Tag : %lx\nChannels : %d \nsamples Frequency: %d \nbytes per second :%d\nblock Align : %d\nbits per sample %d \n"
	//	,m_FormatChunk.szChunkID,m_FormatChunk.ChunkSize,m_FormatChunk.ChunkSize,
	//	m_FormatChunk.wFormatTag,m_FormatChunk.wChannels,
	//	m_FormatChunk.dwSamplesPerSec,m_FormatChunk.dwAvgBytesPerSec,
	//	m_FormatChunk.wBlockAlign,m_FormatChunk.wBitsPerSample);

	//OutputDebugString(m_debugStr);
	//dbgprintf(dbg,"%s\n",m_debugStr);

	return retval;
}