// AudioStudy.cpp : Defines the entry point for the console application.
//



#include "stdafx.h"
#include <mmdeviceapi.h>
#include <stdio.h>
#include <FunctionDiscoveryKeys_devpkey.h>
#include <string.h>
#include "AudioFunctions.h"
#include "ParseWAVEFile.h"
#include "AudioTestFunctions.h"


#define CMD_TYPE_LIST	1
#define CMD_TYPE_PLAY	2
#define CMD_TYPE_RECORD 3

#define CMD_TYPE_UNKNOWN -1



void ShowUsage()
{
	printf("show usage:\n");
	printf("/list:show all render deivces.\n");
	printf("/play:all render devices play wavefiles.\n");
	printf("/record:use default capture device to record.\n");
}


void testMixer()
{
	MIXERCAPS		mixcaps;

	unsigned long	iNumDevs = 0, i = 0;

	iNumDevs = mixerGetNumDevs();
	printf("Mixer deiver count is %d\n",iNumDevs);
	for (i = 0; i < iNumDevs; i++)
	{
		if (!mixerGetDevCaps(i, &mixcaps, sizeof(MIXERCAPS)))
		{
			printf("Device ID #%u: %s\r\n", i, mixcaps.szPname);
		}
	}

	return;
}


int _tmain(int argc, _TCHAR* argv[])
{
	// parse commandline.
	int cmdType = CMD_TYPE_UNKNOWN;
	if(argc < 2)
	{
		ShowUsage();
		return 255;
	}
	else
	{
		if(_stricmp(argv[1],"/list") ==0)
		{
			cmdType = CMD_TYPE_LIST;
		}
		else if (_stricmp(argv[1],"/play") == 0)
		{
			cmdType = CMD_TYPE_PLAY;
		}
		else if(_stricmp(argv[1],"/record") == 0)
		{
			cmdType = CMD_TYPE_RECORD;
		}
		else
		{
			printf("Error:Unknown command .\n");
			ShowUsage();
			return 255;
		}
	}

		if(FAILED(CoInitialize(NULL)))
	{
		printf("CoInitialzie fail,error %d. \n",GetLastError());
		return 255;
	}


	switch (cmdType)
	{
	case CMD_TYPE_LIST:
		{
			IMMDevice * pDevices[MAX_RENDER_DEVICE_COUNT] = {NULL};
			EDataFlow edfDeviceType = eCapture;
			int DeviceCount = 0;
			float fDevVolume = 0;

			list_devices(pDevices,eAll,DEVICE_STATE_ACTIVE,DeviceCount);
			//printf("WardTest DeviceCount is:%d\n",DeviceCount);
			//for(int mm = 0;mm<DeviceCount;mm++)
			//{
				//showDeviceInfo(pDevices[mm]);
				//getDeviceVolume(pDevices[mm],fDevVolume);
				//printf("device volume get from device is %f .\n",fDevVolume);
				//setDeviceVolume(pDevices[mm],fDevVolume - 0.01);
				//setDeviceVolume(pDevices[mm],1);
			//}
		}
		break;

	case CMD_TYPE_PLAY:
		{
		DoAudioRenderTest();
		}
		break;
	case CMD_TYPE_RECORD :
		{
		//DoAudioCaptureTest();
		MyAudioSink mas;
		RecordAudioStream(&mas);
		mas.StoreToFile("Record.bin");
		}
		break;
	default:
		break;
	}




	// printf("Function end here.\n");
	return 0;

	//DoAudioRenderTest();

	//IMMDevice * pDevices[MAX_RENDER_DEVICE_COUNT] = {NULL};
	//EDataFlow edfDeviceType = eCapture;
	//int DeviceCount = 0;

	//list_devices(pDevices,edfDeviceType,DeviceCount);
	//printf("||||||||||||||||||||||||||||||||||||||||||||||||\n");
	//list_devices(pDevices,eRender,DeviceCount);

	//printf("find %d devices.\n",DeviceCount);

	/*
	float fDevVolume = 0;
	for(int mm = 0;mm<DeviceCount;mm++)
	{
		showDeviceInfo(pDevices[mm]);
		getDeviceVolume(pDevices[mm],fDevVolume);
		printf("device volume get from device is %f .\n",fDevVolume);
		//setDeviceVolume(pDevices[mm],fDevVolume - 0.01);
		setDeviceVolume(pDevices[mm],1);
	}
	
	*/

	//HRESULT hr = NULL;

	//char szFileName[256] = {"D_44K_1.WAV"};
	//HMMIO hFile = NULL;
	//WAVEFORMATEX *pWfx = NULL;
	//UINT32 nBytes = 0;
	//UINT32 nFrames = 0;
	//hr = open_file(szFileName, &hFile,&pWfx,&nBytes,&nFrames);
	//if(FAILED(hr))
	//{
	//	printf("Fail to open file %s ,error %d.\n",GetLastError());
	//	return 255;
	//}
	//else
	//{
	//	printf("Success to open file.\n");
	//}










	return 0;








	//PrintEndpointNames();

	// implement render functions.
	// 1. load wav file.
	//RIFF_FILE myRf("Distortion4k.wav");
	//myRf.ParseRIFFFile();
	//if(myRf.m_isErrorOccur == TRUE)
	//{
	//	printf("Fail to parse wav file.\n");
	//}
	//else
	//{
	//	printf("Success to parse wav file.\n");
	//	MyAudioSource mas;
	//	printf("1\n");

	//	mas.m_FileSize = myRf.m_DataChunk.SampleCount;
	//	printf("2\n");
	//	strcpy(mas.m_FileName,myRf.m_fileName);
	//	printf("3\n");
	//	//memcpy(mas.pFileData,(BYTE *)(myRf.m_DataChunk.SampleOfChannel[0]),myRf.m_DataChunk.SampleCount);		// default copy left channel.
	//	mas.pFileData = (BYTE *)(myRf.m_DataChunk.SampleOfChannel[0]);

	//	printf("Before play audio file.\n");
	//	//Sleep(2000);
	//	PlayAudioStream(&mas);
	//	
	//}
	return 0;
}

