#pragma once

#include "stdafx.h"
#include <Windows.h>

//#define MAX_AUDIO_DEVICE_COUNT 4



HRESULT list_devices(IMMDevice ** pDevices,EDataFlow edfDeviceType,DWORD dwDeviceState,int & DeviceCount);
void	showDeviceInfo(IMMDevice * pMMDevice);
int		setDeviceVolume(IMMDevice * pIMMDevice, float fVolume);
int		getDeviceVolume(IMMDevice * pIMMDevice, float & fVolume);

HRESULT open_file(char * szFileName,HMMIO *phFile,WAVEFORMATEX **ppWfx,UINT32 *pnBytes,UINT32 *pnFrames);

DWORD WINAPI PlayThreadFunction(void *  pContext);

HRESULT Play(HMMIO hFile,LPCWAVEFORMATEX pWfx,UINT32 nFramesInFile,UINT32 nBytesInFile,IMMDevice *pMMDevice);

HRESULT Record(char * fileName,LPCWAVEFORMATEX pWfx,int timeSpan,IMMDevice * pMMDevice);


class MyAudioSource
{
public:
	MyAudioSource();
	~MyAudioSource();
	void ParseSourceFile(char * fileName);
	HRESULT SetFormat(WAVEFORMATEX * pfmt);
	HRESULT LoadData(UINT32 buffCount,BYTE * pData,DWORD *Flags);

	char	m_FileName[256];
	DWORD	m_FileSize;
	DWORD	m_CurrentOffset;
	BOOL	m_IsEnd;

	//RIFF_FILE	FileInfo;

	BYTE * pFileData;
};



class MyAudioSink
{
public:
	MyAudioSink();
	~MyAudioSink();


public:
	HRESULT SetFormat(WAVEFORMATEX * pwfx);

	HRESULT CopyData(BYTE * pData, UINT32 numFramesAvailable, BOOL * bDone);

	HRESULT StoreToFile(char * fileanme);



	BOOL			m_isErrorOccured;		// if true:error occured, if False: all function passed.
	WAVEFORMATEX	m_WaveFormat;			// store recorded data format.
	BYTE *			m_pData;				// store recorded data.
	UINT32			m_DtataLength;			// how many bytes recorded.

	UINT32			m_MaxAllowedSize;		// init buffer size here2M

	UINT32			m_LoopCount;			// how many buffers recorded		12
	char			m_FileName[256];		// store recored data in this file


};






void PrintEndpointNames();
HRESULT PlayAudioStream(MyAudioSource *pMySource);

HRESULT RecordAudioStream(MyAudioSink *pMySink);


