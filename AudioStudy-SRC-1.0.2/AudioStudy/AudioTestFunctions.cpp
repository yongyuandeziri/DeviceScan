#include "stdafx.h"
#include "AudioFunctions.h"
#include "AudioTestFunctions.h"


int DoAudioRenderTest()
{
	printf("\n\nEnter function of %s.\n",__FUNCTION__);
    HRESULT		hr = S_OK;
	int			retval = 0;
	HMMIO		m_hFile[MAX_RENDER_DEVICE_COUNT];
	IMMDevice *	m_pMMDevice[MAX_RENDER_DEVICE_COUNT] = {NULL};
	IMMDevice * pTempDevice = NULL;
	WAVEFORMATEX *m_pWfx[MAX_RENDER_DEVICE_COUNT] = {NULL};
	UINT32		m_nBytes[MAX_RENDER_DEVICE_COUNT] = {0};
	UINT32		m_nFrames[MAX_RENDER_DEVICE_COUNT] = {0};
	WCHAR		m_FileName[MAX_RENDER_DEVICE_COUNT][100] ={L"play1.WAV",L"play1.WAV",L"D_44K_3.WAV",L"D_44K_4.WAV"};
	HANDLE		m_hThread[MAX_RENDER_DEVICE_COUNT] = {INVALID_HANDLE_VALUE};
	int			iDeviceCount = 0;

	// 1. select all render devices.
	list_devices(m_pMMDevice,eRender,DEVICE_STATE_ACTIVE,iDeviceCount);
	
	for(int mm = 0;mm<iDeviceCount;mm++)
	{
		char FileName[120] = {0};
		sprintf(FileName,"play%d.WAV",mm+1);
		printf("\n\nfile name is %s.\n",FileName);

		HRESULT hrFile		=	open_file(FileName, &m_hFile[mm], &m_pWfx[mm], &m_nBytes[mm], &m_nFrames[mm]);

		if(hrFile != S_OK)
		{
			printf("Fail to open file %s\n\n",FileName);
		}
	}
	// create threads for all render devices.
	PlayThreadArgs pta[MAX_RENDER_DEVICE_COUNT] = {0};

	for(int mm = 0;mm<iDeviceCount;mm++)
	{		
		printf("\n\n");
		showDeviceInfo(m_pMMDevice[mm]);
		pta[mm].hFile		= m_hFile[mm];
		pta[mm].pWfx		= m_pWfx[mm];
		pta[mm].nFrames		= m_nFrames[mm];
		pta[mm].nBytes		= m_nBytes[mm];
		pta[mm].pMMDevice	= m_pMMDevice[mm];
		pta[mm].hr			= E_UNEXPECTED;

		m_hThread[mm] = CreateThread(
			NULL,
			0,
			PlayThreadFunction,
			&(pta[mm]),
			0,
			NULL
		);

		if (NULL == m_hThread[mm]) 
		{
			printf("CreateThread failed: GetLastError = %u\n", GetLastError());
			retval = 255;
			break;
		}
	}

	printf("After create thread.\n");
	if(retval != 0)
	{
		printf("Fail to create thread.\n");
		return retval;
	}

	printf("Before wait for thread end.\n");
	for(int mm = 0;mm < iDeviceCount;mm++)
	{
		WaitForSingleObject(m_hThread[mm], INFINITE);
		CloseHandle(m_hThread[mm]);
		printf("Wait thread %d end!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!.\n",mm);
	}

	printf("After wait for thread end.\n");
	printf("Exit function %s.\n"__FUNCTION__);

    return 0;
}


int DoAudioCaptureTest()
{
	HRESULT		hr = S_OK;
	int			retval = 0;
	HMMIO		m_hFile[MAX_RENDER_DEVICE_COUNT];
	IMMDevice *	m_pMMDevice[MAX_RENDER_DEVICE_COUNT] = {NULL};
	IMMDevice * pTempDevice = NULL;
	WAVEFORMATEX *m_pWfx[MAX_RENDER_DEVICE_COUNT] = {NULL};
	UINT32		m_nBytes[MAX_RENDER_DEVICE_COUNT] = {0};
	UINT32		m_nFrames[MAX_RENDER_DEVICE_COUNT] = {0};
	WCHAR		m_FileName[MAX_RENDER_DEVICE_COUNT][100] ={L"D_44K_1.WAV",L"D_44K_2.WAV",L"D_44K_3.WAV",L"D_44K_4.WAV"};
	HANDLE		m_hThread[MAX_RENDER_DEVICE_COUNT] = {INVALID_HANDLE_VALUE};
	int			iDeviceCount = 0;

	// 1. select all render devices.
	list_devices(m_pMMDevice,eCapture,DEVICE_STATEMASK_ALL,iDeviceCount);
	
	
	printf("find %d capture device.\n",iDeviceCount);
	if(iDeviceCount <=0)
	{
		printf("invalid count of capture device %d.\n",iDeviceCount);
		return 255;
	}

	// 2. chose an capture deivce to record.
	for(int mm = 0;mm<iDeviceCount;mm++)
	{

	}



	return retval;
}


DWORD WINAPI PlayThreadFunction(void *  pContext)
{
    struct PlayThreadArgs * pArgs = (struct PlayThreadArgs*)pContext;
	printf("Enter therad of PlayThreadFunction\n");

    pArgs->hr = CoInitialize(NULL);
    if (FAILED(pArgs->hr)) 
	{
        return 0;
    }

    pArgs->hr = Play(
        pArgs->hFile,
        pArgs->pWfx,
        pArgs->nFrames,
        pArgs->nBytes,
        pArgs->pMMDevice
    );

    CoUninitialize();

    return 0;
}

