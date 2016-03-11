#include "stdafx.h"
#include "AudioFunctions.h"
#include <mmdeviceapi.h>
#include <stdio.h>
#include <FunctionDiscoveryKeys_devpkey.h>
#include <AudioClient.h>
#include <AudioPolicy.h>
#include <Windows.h>
#include <audioendpoints.h>
#include <endpointvolume.h>

#include "ParseWAVEFile.h"


//-----------------------------------------------------------
// This function enumerates all active (plugged in) audio
// rendering endpoint devices. It prints the friendly name
// and endpoint ID string of each endpoint device.
//-----------------------------------------------------------

int checkCount = 0;


#define EXIT_ON_ERROR(hres)  \
				checkCount++;\
              if (FAILED(hres)) { goto Exit; }

#define SAFE_RELEASE(punk)  \
              if ((punk) != NULL)  \
                { (punk)->Release(); (punk) = NULL; }

const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
const IID IID_IAudioEndpointVolume = __uuidof(IAudioEndpointVolume);
const IID IID_Endpoint = __uuidof(IMMEndpoint);


void PrintEndpointNames()
{
    HRESULT hr = S_OK;
    IMMDeviceEnumerator *pEnumerator = NULL;
    IMMDeviceCollection *pCollection = NULL;
    IMMDevice *pEndpoint = NULL;
	IMMEndpoint	* pEP = NULL;
    IPropertyStore *pProps = NULL;
	IAudioEndpointVolume *volume = NULL;
    LPWSTR pwszID = NULL;
	//Need to add call before CoCreateInstance:
//------------------------------------------------------

	CoInitialize(NULL);

    hr = CoCreateInstance(
           CLSID_MMDeviceEnumerator, NULL,
           CLSCTX_ALL, IID_IMMDeviceEnumerator,
           (void**)&pEnumerator);
    EXIT_ON_ERROR(hr)

    hr = pEnumerator->EnumAudioEndpoints(
	eAll, DEVICE_STATE_ACTIVE ,
                        &pCollection);
    EXIT_ON_ERROR(hr)

    UINT  count;
    hr = pCollection->GetCount(&count);
    EXIT_ON_ERROR(hr)

    if (count == 0)
    {
        printf("No endpoints found.\n");
    }
	else
	{
		printf("Find device %d .\n",count);
	}

    // Each loop prints the name of an endpoint device.
    for (ULONG i = 0; i < count; i++)
    {
        // Get pointer to endpoint number i.
        hr = pCollection->Item(i, &pEndpoint);
        EXIT_ON_ERROR(hr)

        // Get the endpoint ID string.
        hr = pEndpoint->GetId(&pwszID);
        EXIT_ON_ERROR(hr)
			printf("Endpoint ID is : %s.\n",pwszID);
        hr = pEndpoint->OpenPropertyStore(
                          STGM_READ, &pProps);
        EXIT_ON_ERROR(hr)

        PROPVARIANT varName;
        // Initialize container for property value.
        PropVariantInit(&varName);

        // Get the endpoint's friendly-name property.
        hr = pProps->GetValue(
                       PKEY_Device_FriendlyName, &varName);
        EXIT_ON_ERROR(hr)

        // Print endpoint friendly name and endpoint ID.
        printf("Endpoint %d: \n\"%S\" \n(%S)\n",
               i, varName.pwszVal, pwszID);
		//TODO here.
		// get endpoint direction.
		printf("Before get endpoint interface.\n");
		hr = pEndpoint->QueryInterface(IID_Endpoint,(void **)&pEP);
		printf("After get endpoint interface.\n");
		EXIT_ON_ERROR(hr)
		EDataFlow ef;
		pEP->GetDataFlow(&ef);
		switch (ef)
		{
		case eRender:
			printf("is render device.\n");
			break;
		case eCapture:
			printf("is capture device.\n");
			break;
		default:
			printf("Warning :::: Unknown device type %d.\n",ef);
			break;
		}

		if(ef == eRender)
		{

		}
		else
		{
			// TODO :: skip capture device now.
		}



	//	pEndpoint->QueryInterface(IID_IMMEndpoint);
		hr=pEndpoint->Activate(IID_IAudioEndpointVolume, CLSCTX_ALL, NULL, (void **)(&volume));
		  EXIT_ON_ERROR(hr)
		float level = 0;
		hr = volume->GetMasterVolumeLevelScalar(&level); //获取音量值
		EXIT_ON_ERROR(hr)
		printf("original volume value is %f.\n",level);

		hr = volume->SetMasterVolumeLevelScalar( 0.1,NULL); //设置音量值
		EXIT_ON_ERROR(hr)
		printf("after set volume level to increase 10 percent.\n");
		// if output put ,output wave info.




		
//		hr = volume->SetChannelVolumeLevel();
//		EXIT_ON_ERROR(hr)


		//hr = volume->SetMute(TRUE,NULL);
		//EXIT_ON_ERROR(hr)
		//printf("After set mute.\n");


        CoTaskMemFree(pwszID);
        pwszID = NULL;
        PropVariantClear(&varName);
        SAFE_RELEASE(pProps)
        SAFE_RELEASE(pEndpoint)
		SAFE_RELEASE(volume);

		printf("\n\n");
    }
    SAFE_RELEASE(pEnumerator)
    SAFE_RELEASE(pCollection)
    return;

Exit:
	printf("\n\nWarning ::: Error Occured .\n");
    printf("Error!\n");
	system("color 47");
    CoTaskMemFree(pwszID);
    SAFE_RELEASE(pEnumerator)
    SAFE_RELEASE(pCollection)
    SAFE_RELEASE(pEndpoint)
    SAFE_RELEASE(pProps)
}


void MyAudioSource:: ParseSourceFile(char * fileName)
{

}


HRESULT MyAudioSource:: SetFormat(WAVEFORMATEX * pfmt)
{
	printf("Enter function set format.\n");
	printf("Show set format:\n");
	printf("wFormatTag %d\n",pfmt->wFormatTag);
	printf("nChannels %d\n",pfmt->nChannels);
	printf("nSamplesPerSec %d\n",pfmt->nSamplesPerSec);
	printf("nAvgBytesPerSec %d\n",pfmt->nAvgBytesPerSec);
	printf("nBlockAlign %d\n",pfmt->nBlockAlign);
	printf("wBitsPerSample %d\n",pfmt->wBitsPerSample);
	printf("cbSize %d\n",pfmt->cbSize);
	return S_OK;
}


HRESULT MyAudioSource:: LoadData(UINT32 buffCount,BYTE * pData,DWORD * Flags)
{
	printf("Enter function load data.\n");
	printf("Buffer size is %d.\n",buffCount);
	printf("Current offset is %d.\n",m_CurrentOffset);
	printf("file size is %d.\n",m_FileSize);
	*Flags == FALSE;
	if(m_CurrentOffset + buffCount >= m_FileSize)
	{
		printf("Reach the end of file.\n");
		*Flags = AUDCLNT_BUFFERFLAGS_SILENT;
		return S_OK;
	}

	memcpy(pData,pFileData + m_CurrentOffset,buffCount);
	m_CurrentOffset += buffCount;
	return S_OK;
}


MyAudioSource::MyAudioSource()
{
	printf("Enter construction.\n");
	m_CurrentOffset = 0;
}


MyAudioSource::~MyAudioSource()
{
	printf("Enter destruction.\n");
}



//-----------------------------------------------------------
// Play an audio stream on the default audio rendering
// device. The PlayAudioStream function allocates a shared
// buffer big enough to hold one second of PCM audio data.
// The function uses this buffer to stream data to the
// rendering device. The inner loop runs every 1/2 second.
//-----------------------------------------------------------

// REFERENCE_TIME time units per second and per millisecond
#define REFTIMES_PER_SEC  10000000
#define REFTIMES_PER_MILLISEC  10000
//
//#define EXIT_ON_ERROR(hres)  \
//              if (FAILED(hres)) { goto Exit; }
//#define SAFE_RELEASE(punk)  \
//              if ((punk) != NULL)  \
//                { (punk)->Release(); (punk) = NULL; }

//const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
//const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
//const IID IID_IAudioClient = __uuidof(IAudioClient);
//const IID IID_IAudioRenderClient = __uuidof(IAudioRenderClient);

HRESULT PlayAudioStream(MyAudioSource *pMySource)
{
	printf("Enter PlayAudioStream.\n");

    HRESULT hr;
    REFERENCE_TIME hnsRequestedDuration = REFTIMES_PER_SEC;
    REFERENCE_TIME hnsActualDuration;
    IMMDeviceEnumerator *pEnumerator = NULL;
    IMMDevice *pDevice = NULL;
    IAudioClient *pAudioClient = NULL;
    IAudioRenderClient *pRenderClient = NULL;
    WAVEFORMATEX *pwfx = NULL;
    UINT32 bufferFrameCount;
    UINT32 numFramesAvailable;
    UINT32 numFramesPadding;
    BYTE *pData;
    DWORD flags = 0;

	CoInitialize(NULL);

    hr = CoCreateInstance(
           CLSID_MMDeviceEnumerator, NULL,
           CLSCTX_ALL, IID_IMMDeviceEnumerator,
           (void**)&pEnumerator);
    EXIT_ON_ERROR(hr)
	printf("After get enumerator.\n");

    hr = pEnumerator->GetDefaultAudioEndpoint(
                        eRender, eConsole, &pDevice);
    EXIT_ON_ERROR(hr)
	printf("After get default endpoint.\n");

    hr = pDevice->Activate(
                    IID_IAudioClient, CLSCTX_ALL,
                    NULL, (void**)&pAudioClient);
    EXIT_ON_ERROR(hr)
	printf("After activate device.\n");

    hr = pAudioClient->GetMixFormat(&pwfx);
    EXIT_ON_ERROR(hr)

	printf("Show Default mix Format:\n");
	printf("wFormatTag %d\n",pwfx->wFormatTag);
	printf("nChannels %d\n",pwfx->nChannels);
	printf("nSamplesPerSec %d\n",pwfx->nSamplesPerSec);
	printf("nAvgBytesPerSec %d\n",pwfx->nAvgBytesPerSec);
	printf("nBlockAlign %d\n",pwfx->nBlockAlign);
	printf("wBitsPerSample %d\n",pwfx->wBitsPerSample);
	printf("cbSize %d\n",pwfx->cbSize);

    hr = pAudioClient->Initialize(
                         AUDCLNT_SHAREMODE_SHARED,
                         0,
                         hnsRequestedDuration,
                         0,
                         pwfx,
                         NULL);
    EXIT_ON_ERROR(hr)

    // Tell the audio source which format to use.
    hr = pMySource->SetFormat(pwfx);
    EXIT_ON_ERROR(hr)

    // Get the actual size of the allocated buffer.
    hr = pAudioClient->GetBufferSize(&bufferFrameCount);
    EXIT_ON_ERROR(hr)

    hr = pAudioClient->GetService(
                         IID_IAudioRenderClient,
                         (void**)&pRenderClient);
    EXIT_ON_ERROR(hr)

    // Grab the entire buffer for the initial fill operation.
    hr = pRenderClient->GetBuffer(bufferFrameCount, &pData);
    EXIT_ON_ERROR(hr)

    // Load the initial data into the shared buffer.
    hr = pMySource->LoadData(bufferFrameCount, pData, &flags);
    EXIT_ON_ERROR(hr)

    hr = pRenderClient->ReleaseBuffer(bufferFrameCount, flags);
    EXIT_ON_ERROR(hr)

    // Calculate the actual duration of the allocated buffer.
    hnsActualDuration = (double)REFTIMES_PER_SEC *
                        bufferFrameCount / pwfx->nSamplesPerSec;

    hr = pAudioClient->Start();  // Start playing.
    EXIT_ON_ERROR(hr)

    // Each loop fills about half of the shared buffer.
    while (flags != AUDCLNT_BUFFERFLAGS_SILENT)
    {
        // Sleep for half the buffer duration.
        Sleep((DWORD)(hnsActualDuration/REFTIMES_PER_MILLISEC/2));

        // See how much buffer space is available.
        hr = pAudioClient->GetCurrentPadding(&numFramesPadding);
        EXIT_ON_ERROR(hr)

        numFramesAvailable = bufferFrameCount - numFramesPadding;

        // Grab all the available space in the shared buffer.
        hr = pRenderClient->GetBuffer(numFramesAvailable, &pData);
        EXIT_ON_ERROR(hr)

        // Get next 1/2-second of data from the audio source.
        hr = pMySource->LoadData(numFramesAvailable, pData, &flags);
        EXIT_ON_ERROR(hr)

        hr = pRenderClient->ReleaseBuffer(numFramesAvailable, flags);
        EXIT_ON_ERROR(hr)
    }

    // Wait for last data in buffer to play before stopping.
    Sleep((DWORD)(hnsActualDuration/REFTIMES_PER_MILLISEC/2));

    hr = pAudioClient->Stop();  // Stop playing.
    EXIT_ON_ERROR(hr)

Exit:
	printf("Error occured ,checkCount is %d.\n",checkCount);
    CoTaskMemFree(pwfx);
    SAFE_RELEASE(pEnumerator)
    SAFE_RELEASE(pDevice)
    SAFE_RELEASE(pAudioClient)
    SAFE_RELEASE(pRenderClient)

    return hr;
}




MyAudioSink::MyAudioSink()
{
	printf("Enter construction.\n");
	m_isErrorOccured = FALSE;
	ZeroMemory(&m_WaveFormat,sizeof(WAVEFORMATEX));
	m_DtataLength = 0;
	
	m_MaxAllowedSize = 2*1024*1024;
	m_pData = (BYTE*)malloc(m_MaxAllowedSize);
	if(m_pData == NULL)
	{
		printf("Fail to alloc data buffer.\n");
		m_isErrorOccured = TRUE;
	}
}


MyAudioSink::~MyAudioSink()
{
	printf("Enter destruction.\n");
	if(m_pData != NULL)
	{
		free(m_pData);
		m_pData = NULL;
	}
}


HRESULT MyAudioSink::SetFormat(WAVEFORMATEX * pwfx)
{
	HRESULT hr = S_OK;
	if(pwfx == NULL)
	{
		printf("input parameter pwfx is null.\n");
		m_isErrorOccured = TRUE;
		return S_FALSE;
	}

	memcpy(&m_WaveFormat,pwfx,sizeof(WAVEFORMATEX));

	return hr;
}


HRESULT MyAudioSink::CopyData(BYTE * pData, UINT32 numFramesAvailable, BOOL * bDone)
{
	printf("Enter copy data.\n");
	HRESULT hr = S_OK;
	if(pData == NULL)
	{
		printf("input parameter pdata is null.\n");
		*bDone = FALSE;
		return hr;
	}

	if(numFramesAvailable == 0)
	{
		printf("Warning :: numFramesAvailable is 0\n");
		*bDone = FALSE;
		return hr;
	}

	// Record data to buffer.
	if(m_DtataLength + numFramesAvailable > m_MaxAllowedSize)
	{
		printf("Buffer size is too large.\n");
		*bDone = TRUE;
		return hr;
	}
	else
	{
		memcpy(m_pData+ m_DtataLength,pData,numFramesAvailable);
		m_DtataLength += numFramesAvailable;
		m_LoopCount ++;
		if(m_LoopCount ==12)
		{
			printf("record %d buffers, finish.\n",m_LoopCount);
			*bDone  = TRUE;
		}
		else
		{
			printf("Current copy data index is %d.\n",m_LoopCount);
		}
		*bDone = FALSE;
	}
	
	printf("Exit function copy data.\n");
	return hr;
}

HRESULT MyAudioSink::StoreToFile(char * filename)
{
	printf("Enter function storetofile.\n");
	HRESULT hr = S_OK;

	if(filename == NULL)
	{
		m_isErrorOccured = TRUE;
		printf("file name is NULL.\n");
		return S_FALSE;
	}

	strcpy(m_FileName,filename);
	

	// note:: store wave data to file first 
	HANDLE hfile = INVALID_HANDLE_VALUE;
	hfile = CreateFile(filename,GENERIC_READ|GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,CREATE_ALWAYS,NULL,NULL);
	if(hfile == INVALID_HANDLE_VALUE || hfile == NULL)
	{
		m_isErrorOccured=  TRUE;
		printf("Fail to store recored data to %s.\n",filename);
		return S_FALSE;
	}
	else
	{
		printf("open file success.\n");
	}

	DWORD bytesWroten = 0;
	// write data to file.
	if(WriteFile(hfile,m_pData,m_DtataLength,&bytesWroten,NULL) == 0)
	{
		printf("Write data to file success.\n");
	}
	else
	{
		printf("Fail to write to file %s.\n",filename);
		m_isErrorOccured = TRUE;
		hr = S_FALSE;
	}

	CloseHandle(hfile);
	printf("Exit storetofile.\n");
	return hr;
}






//-----------------------------------------------------------
// Record an audio stream from the default audio capture
// device. The RecordAudioStream function allocates a shared
// buffer big enough to hold one second of PCM audio data.
// The function uses this buffer to stream data from the
// capture device. The main loop runs every 1/2 second.
//-----------------------------------------------------------


// REFERENCE_TIME time units per second and per millisecond
#define REFTIMES_PER_SEC  10000000
#define REFTIMES_PER_MILLISEC  10000

#define EXIT_ON_ERROR(hres)  \
              if (FAILED(hres)) { goto Exit; }
#define SAFE_RELEASE(punk)  \
              if ((punk) != NULL)  \
                { (punk)->Release(); (punk) = NULL; }
/*
const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
const IID IID_IAudioClient = __uuidof(IAudioClient);
*/
const IID IID_IAudioCaptureClient = __uuidof(IAudioCaptureClient);

HRESULT RecordAudioStream(MyAudioSink *pMySink)
{
	CoInitialize(NULL);

	printf("enter function %s.\n"__FUNCTION__);
    HRESULT hr;
    REFERENCE_TIME hnsRequestedDuration = REFTIMES_PER_SEC;
    REFERENCE_TIME hnsActualDuration;
    UINT32 bufferFrameCount;
    UINT32 numFramesAvailable;
    IMMDeviceEnumerator *pEnumerator = NULL;
    IMMDevice *pDevice = NULL;
    IAudioClient *pAudioClient = NULL;
    IAudioCaptureClient *pCaptureClient = NULL;
    WAVEFORMATEX *pwfx = NULL;
    UINT32 packetLength = 0;
    BOOL bDone = FALSE;
    BYTE *pData;
    DWORD flags;
	printf("before CoCreateInstance\n");
    hr = CoCreateInstance(
           CLSID_MMDeviceEnumerator, NULL,
           CLSCTX_ALL, IID_IMMDeviceEnumerator,
           (void**)&pEnumerator);
    EXIT_ON_ERROR(hr)

	printf("before GetDefaultAudioEndpoint\n");
    hr = pEnumerator->GetDefaultAudioEndpoint(eCapture, eConsole, &pDevice);
    EXIT_ON_ERROR(hr)

	printf("before Activate\n");
    hr = pDevice->Activate(IID_IAudioClient, CLSCTX_ALL,NULL, (void**)&pAudioClient);
    EXIT_ON_ERROR(hr)

	printf("before GetMixFormat\n");
    hr = pAudioClient->GetMixFormat(&pwfx);
    EXIT_ON_ERROR(hr)
	
	printf("before Initialize\n");
    hr = pAudioClient->Initialize(
                         AUDCLNT_SHAREMODE_SHARED,
                         0,
                         hnsRequestedDuration,
                         0,
                         pwfx,
                         NULL);
    EXIT_ON_ERROR(hr)

	printf("before GetBufferSize\n");
    // Get the size of the allocated buffer.
    hr = pAudioClient->GetBufferSize(&bufferFrameCount);
    EXIT_ON_ERROR(hr)

	printf("before GetService\n");
    hr = pAudioClient->GetService(
                         IID_IAudioCaptureClient,
                         (void**)&pCaptureClient);
    EXIT_ON_ERROR(hr)

	printf("before SetFormat\n");
    // Notify the audio sink which format to use.
    hr = pMySink->SetFormat(pwfx);
    EXIT_ON_ERROR(hr)

	printf("before \n");
    // Calculate the actual duration of the allocated buffer.
    hnsActualDuration = (double)REFTIMES_PER_SEC *
                     bufferFrameCount / pwfx->nSamplesPerSec;

    hr = pAudioClient->Start();  // Start recording.
    EXIT_ON_ERROR(hr)
	printf("before first while loop\n");
    // Each loop fills about half of the shared buffer.
    while (bDone == FALSE)
    {
        // Sleep for half the buffer duration.
        Sleep(hnsActualDuration/REFTIMES_PER_MILLISEC/2);

		printf("before GetNextPacketSize\n");
        hr = pCaptureClient->GetNextPacketSize(&packetLength);
        EXIT_ON_ERROR(hr)

        while (packetLength != 0)
        {
			printf("before GetBuffer\n");
            // Get the available data in the shared buffer.
            hr = pCaptureClient->GetBuffer(
                                   &pData,
                                   &numFramesAvailable,
                                   &flags, NULL, NULL);
            EXIT_ON_ERROR(hr)

            if (flags & AUDCLNT_BUFFERFLAGS_SILENT)
            {
                pData = NULL;  // Tell CopyData to write silence.
            }

			printf("before CopyData\n");
            // Copy the available capture data to the audio sink.
            hr = pMySink->CopyData(pData, numFramesAvailable, &bDone);
            EXIT_ON_ERROR(hr)

			printf("before ReleaseBuffer\n");
            hr = pCaptureClient->ReleaseBuffer(numFramesAvailable);
            EXIT_ON_ERROR(hr)

			printf("before GetNextPacketSize\n");
            hr = pCaptureClient->GetNextPacketSize(&packetLength);
            EXIT_ON_ERROR(hr)
        }
    }

	printf("before \n");
    hr = pAudioClient->Stop();  // Stop recording.
    EXIT_ON_ERROR(hr)

Exit:

	printf("Last error %d.\n",GetLastError());


    CoTaskMemFree(pwfx);
    SAFE_RELEASE(pEnumerator)
    SAFE_RELEASE(pDevice)
    SAFE_RELEASE(pAudioClient)
    SAFE_RELEASE(pCaptureClient)
	printf("Exit function %s.\n",__FUNCTION__);
    return hr;
}








#include<stdio.h>

#include<Objbase.h>

//#include<Audioclient.h>

//#include<Mmdeviceapi.h>

#include<Mmreg.h>

#include <MMDeviceAPI.h>

#include <AudioClient.h>

#include <AudioPolicy.h>

// #define WAVE_FORMAT_PCM

//typedef IID* REFIID; 

#pragma comment(lib, "ole32.lib")

//const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator); 

//const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator); 

const IID IID_IAudioClient = __uuidof(IAudioClient); 

const IID IID_IAudioRenderClient = __uuidof(IAudioRenderClient); 

void PlayAnAudioStream()
{

HRESULT hresult;

IMMDeviceEnumerator *pDeviceEnumerator=NULL;

IMMDevice *pDevice=NULL;

IAudioClient *pAudioInterface=NULL;

IAudioRenderClient *pAudioRenderClient=NULL;

WAVEFORMATEX *pDeviceFormat = NULL;

DWORD flags = 0;

REFERENCE_TIME hnsBufferDuration = 10000000,hnsActualDuration;

UINT32 pNumBufferFrames;

UINT32 pNumPaddingFrames,pNumAvFrames;

BYTE *pData;

FILE *InputStreamData = NULL; 

//char *intermediatedata= NULL;

//intermediatedata = (char*) malloc(44100*sizeof(char));

/* Creating a object for Enumerating a device */

hresult = CoCreateInstance(CLSID_MMDeviceEnumerator,

NULL, /*Object is not created as the part of the aggregate */

CLSCTX_ALL,IID_IMMDeviceEnumerator,(void**)&pDeviceEnumerator);

/* Retrieve the endpoint device using the Enumerator */

hresult = pDeviceEnumerator->GetDefaultAudioEndpoint(eRender,eMultimedia,&pDevice);

hresult = pDevice->Activate(IID_IAudioClient,CLSCTX_ALL,NULL,(void**)&pAudioInterface);

hresult = pAudioInterface->GetMixFormat((WAVEFORMATEX**)&pDeviceFormat); 

hresult = pAudioInterface->Initialize( AUDCLNT_SHAREMODE_SHARED,0,hnsBufferDuration,0,pDeviceFormat,NULL ); 

hresult = pAudioInterface->GetService(IID_IAudioRenderClient,(void**)&pAudioRenderClient); 

hresult = pAudioInterface->GetBufferSize(&pNumBufferFrames);

printf("\n the no of frames are %d",pNumBufferFrames);

hresult = pAudioRenderClient->GetBuffer( pNumBufferFrames, &pData ); 

/* Set Audio Format */

pDeviceFormat->wFormatTag = WAVE_FORMAT_PCM;

pDeviceFormat->nChannels = 1;

pDeviceFormat->nSamplesPerSec = 44100;

pDeviceFormat->wBitsPerSample = 16;

pDeviceFormat->nBlockAlign = (pDeviceFormat->nChannels * pDeviceFormat->wBitsPerSample)/8;

pDeviceFormat->nAvgBytesPerSec = pDeviceFormat->nSamplesPerSec * pDeviceFormat->nBlockAlign;

pDeviceFormat->cbSize = 0;

/* Load the buffer to pdata */

InputStreamData = fopen("44k_mono_16bit.pcm","r");

fread(pData,sizeof(char),pNumBufferFrames,InputStreamData);// + intermediatedata 0x005F3CA8 "" char*

// memcpy(pData,intermediatedata,pNumBufferFrames);+ pData 0x00D81C00 unsigned char*

/* Release this buffer to audio device */

hresult = pAudioRenderClient->ReleaseBuffer(pNumBufferFrames,flags); 

hnsActualDuration = (double)10000000 *

pNumBufferFrames / pDeviceFormat->nSamplesPerSec;

/* Play the data */

hresult = pAudioInterface->Start();

//lseek(InputStreamData,pNumBufferFrames,SEEK_CUR);

//sleep(3);

/* Stop playing */

while(EOF)

{

//Sleep(hnsActualDuration/10000/2);


hresult = pAudioInterface->GetCurrentPadding(&pNumPaddingFrames);

pNumAvFrames = pNumBufferFrames - pNumPaddingFrames;

hresult = pAudioRenderClient->GetBuffer( pNumAvFrames, &pData ); 

fread(pData,sizeof(char),pNumAvFrames,InputStreamData);

hresult = pAudioRenderClient->ReleaseBuffer(pNumAvFrames,flags); 

//lseek(InputStreamData,pNumAvFrames,SEEK_CUR);

}

printf("I am here");

hresult = pAudioInterface->Stop();

CoTaskMemFree(pDeviceFormat);

fclose(InputStreamData);

}


// list audio devices.

HRESULT list_devices(IMMDevice ** pDevices,EDataFlow edfDeviceType,DWORD dwDeviceState,int &DeviceCount)
{
	HRESULT hr = S_OK;
	//printf("Enter function %s.\n",__FUNCTION__);
	DeviceCount = 0;
	CoInitialize(NULL);
    // get an enumerator
    IMMDeviceEnumerator *pMMDeviceEnumerator;

    hr = CoCreateInstance(
        __uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, 
        __uuidof(IMMDeviceEnumerator),
        (void**)&pMMDeviceEnumerator);

    if (FAILED(hr)) 
	{
        printf("CoCreateInstance(IMMDeviceEnumerator) failed: hr = 0x%08x\n", hr);
        return hr;
    }

    IMMDeviceCollection *pMMDeviceCollection;

    // get all the active render endpoints
	hr = pMMDeviceEnumerator->EnumAudioEndpoints(edfDeviceType, dwDeviceState, &pMMDeviceCollection);
    pMMDeviceEnumerator->Release();
    if (FAILED(hr)) 
	{
        printf("IMMDeviceEnumerator::EnumAudioEndpoints failed: hr = 0x%08x\n", hr);
        return hr;
    }

    UINT count;
    hr = pMMDeviceCollection->GetCount(&count);
    if (FAILED(hr)) 
	{
        pMMDeviceCollection->Release();
        printf("IMMDeviceCollection::GetCount failed: hr = 0x%08x\n", hr);
        return hr;
    }

	printf("%u\n",count);
    pMMDeviceCollection->Release();
    return S_OK; 
/*
    for (UINT i = 0; i < count; i++) 
	{
		//printf("\n\nEnter loop for render device %d.\n",i);
        IMMDevice *pMMDevice;

        // get the "n"th device
        hr = pMMDeviceCollection->Item(i, &pMMDevice);
        if (FAILED(hr)) 
		{
            pMMDeviceCollection->Release();
            printf("IMMDeviceCollection::Item failed: hr = 0x%08x\n", hr);
            return hr;
        }

        // open the property store on that device
        IPropertyStore *pPropertyStore;
        hr = pMMDevice->OpenPropertyStore(STGM_READ, &pPropertyStore);
        //pMMDevice->Release();
        if (FAILED(hr)) 
		{
			pMMDevice->Release();
            pMMDeviceCollection->Release();
            printf("IMMDevice::OpenPropertyStore failed: hr = 0x%08x\n", hr);
            return hr;
        }
		else
		{
			// record the device instead of release them.
		//	printf("record device.\n");
			pDevices[DeviceCount] = pMMDevice;
			
			//showDeviceInfo(pDevices[DeviceCount]);
			DeviceCount++;
		}

		// Wesley remove this part to an single function of show_deivceInfo(IMMDeivce * pDevice);
  //      // get the long name property
  //      PROPVARIANT pv; PropVariantInit(&pv);
  //      hr = pPropertyStore->GetValue(PKEY_Device_FriendlyName, &pv);
  //      pPropertyStore->Release();
  //      if (FAILED(hr)) 
		//{
  //          pMMDeviceCollection->Release();
  //          printf("IPropertyStore::GetValue failed: hr = 0x%08x\n", hr);
  //          return hr;
  //      }

  //      if (VT_LPWSTR != pv.vt)
		//{
  //          printf("PKEY_Device_FriendlyName variant type is %u - expected VT_LPWSTR", pv.vt);

  //          PropVariantClear(&pv);
  //          pMMDeviceCollection->Release();
  //          return E_UNEXPECTED;
  //      }

		//printf("find device friendly name.\n");
  //      printf("    %ls\n\n", pv.pwszVal);
  //      
  //      PropVariantClear(&pv);

    }

    pMMDeviceCollection->Release();
    

	//printf("Return all function ok.\n\n\n");
    return S_OK;
	*/
}


void showDeviceInfo(IMMDevice * pMMDevice)
{
	if(pMMDevice == NULL)
	{
		printf("Error::device pointer is NULL.\n");
		return;
	}
	
	HRESULT hr = S_OK;
	IPropertyStore *pPropertyStore;
	hr = pMMDevice->OpenPropertyStore(STGM_READ, &pPropertyStore);
	if (FAILED(hr)) 
	{
        printf("IPropertyStore::OpenPropertyStore failed: hr = 0x%08x\n", hr);
        return;
    }

	// get the long name property
    PROPVARIANT pv; 
	PropVariantInit(&pv);
    hr = pPropertyStore->GetValue(PKEY_Device_FriendlyName, &pv);
    
    if (FAILED(hr)) 
	{
		pPropertyStore->Release();
        printf("IPropertyStore::GetValue failed: hr = 0x%08x\n", hr);
        return;
    }

    if (VT_LPWSTR != pv.vt)	
	{
		pPropertyStore->Release();
        printf("PKEY_Device_FriendlyName variant type is %u - expected VT_LPWSTR", pv.vt);

        PropVariantClear(&pv);
        return;
    }

//	printf("\n*******find device friendly name*******\n");
  //  printf("    %ls\n\n", pv.pwszVal);
        
		
	PropVariantInit(&pv);
    hr = pPropertyStore->GetValue(PKEY_Device_DeviceDesc, &pv);
    
    if (FAILED(hr)) 
	{
		pPropertyStore->Release();
        printf("IPropertyStore::GetValue failed: hr = 0x%08x\n", hr);
        return;
    }

    if (VT_LPWSTR != pv.vt)	
	{
		pPropertyStore->Release();
    //    printf("PKEY_Device_DeviceDesc variant type is %u - expected VT_LPWSTR", pv.vt);

        PropVariantClear(&pv);
        return;
    }

	printf("find device PKEY_Device_DeviceDesc name.\n");
    printf("    %ls\n\n", pv.pwszVal);

	PropVariantInit(&pv);
    hr = pPropertyStore->GetValue(PKEY_DeviceInterface_FriendlyName, &pv);
    
    if (FAILED(hr)) 
	{
		pPropertyStore->Release();
        printf("IPropertyStore::GetValue failed: hr = 0x%08x\n", hr);
        return;
    }

    if (VT_LPWSTR != pv.vt)	
	{
		pPropertyStore->Release();
        printf("PKEY_DeviceInterface_FriendlyName variant type is %u - expected VT_LPWSTR", pv.vt);

        PropVariantClear(&pv);
        return;
    }

	printf("find device PKEY_Device_DeviceDesc name.\n");
    printf("    %ls\n\n", pv.pwszVal);

    PropVariantClear(&pv);
	pPropertyStore->Release();
	return;
}


void releaseDevice(IMMDevice *pMMDevice)
{
	SAFE_RELEASE(pMMDevice);
	return;
}


int setDeviceVolume(IMMDevice * pIMMDevice, float fVolume)
{
	if(pIMMDevice == NULL)
	{
		printf("Invalide parameter of null device pointer in function %s.\n",__FUNCTION__);
	}
	int retval = 0;
	float level = 0;
	if(fVolume < 0  || fVolume >1)
	{
		printf("Invalid volume data %f .\n",fVolume);
		return -1;
	}

	IAudioEndpointVolume * volume = NULL;

	HRESULT hr= pIMMDevice->Activate(IID_IAudioEndpointVolume, CLSCTX_ALL, NULL, (void **)(&volume));
	if(FAILED(hr))
	{
		printf("Fail to activate device volume.\n");
		return -1;
	}
	
	hr = volume->GetMasterVolumeLevelScalar(&level); //获取音量值
	if(FAILED(hr))
	{
		printf("Fail to get device volume.\n");
		SAFE_RELEASE(volume);
		return -1;
	}

	printf("Device Original volume value is %f.\n",level);

	hr = volume->SetMasterVolumeLevelScalar(fVolume,NULL); //设置音量值
	if(FAILED(hr))
	{
		printf("Fail to get device volume.\n");
		SAFE_RELEASE(volume);
		return -1;
	}
	printf("After set device volume\n");

	// check again.
	hr = volume->GetMasterVolumeLevelScalar(&level); //获取音量值
	if(FAILED(hr))
	{
		printf("Fail to get device volume.\n");
		SAFE_RELEASE(volume);
		return -1;
	}

	if(level == fVolume)
	{
		printf("Set Device Volume to %f success.\n",fVolume);
	}
	else
	{
		printf("Set Device Volume to %f fail.\n");
		SAFE_RELEASE(volume);
		return -1;
	}

	SAFE_RELEASE(volume);

	return 0;
}


int getDeviceVolume(IMMDevice * pIMMDevice, float & fVolume)
{
	if(pIMMDevice == NULL)
	{
		printf("Invalide parameter of null device pointer in function %s.\n",__FUNCTION__);
	}

	float level = 0;

	IAudioEndpointVolume * volume = NULL;

	HRESULT hr= pIMMDevice->Activate(IID_IAudioEndpointVolume, CLSCTX_ALL, NULL, (void **)(&volume));
	if(FAILED(hr))
	{
		printf("Fail to activate device volume.\n");
		return -1;
	}
	
	hr = volume->GetMasterVolumeLevelScalar(&level); //获取音量值
	if(FAILED(hr))
	{
		printf("Fail to get device volume.\n");
		return -1;
	}

	fVolume = level;
	printf("Device volume value is %f.\n",level);
	SAFE_RELEASE(volume);

	return 0;
}


HRESULT open_file(char * szFileName,HMMIO *phFile,WAVEFORMATEX **ppWfx,UINT32 *pnBytes,UINT32 *pnFrames)
{
    MMIOINFO mi = {0};

    printf("Opening .wav file \"%s\"...\n", szFileName);
    HMMIO hFile = mmioOpen(szFileName,&mi,MMIO_READ);

    if (NULL == hFile)
	{
        printf("mmioOpen(\"%s\", ...) failed. wErrorRet == %u,last error %d\n", szFileName, mi.wErrorRet,GetLastError());
        return E_FAIL;
    }

    // parse file
    MMCKINFO ckRiff = {0};
    ckRiff.ckid = MAKEFOURCC('W', 'A', 'V', 'E');
    MMRESULT mmr = mmioDescend(hFile,&ckRiff,NULL,MMIO_FINDRIFF);

    if (MMSYSERR_NOERROR != mmr) 
	{
		printf("Could not find a RIFF/WAVE chunk: mmr = 0x%08x\n", mmr);
		mmioClose(hFile, 0);
		return E_INVALIDARG;
	}

	// find the "fmt " chunk
	MMCKINFO ckFmt = {0};
	ckFmt.ckid = MAKEFOURCC('f', 'm', 't', ' ');
	mmr = mmioDescend(hFile,&ckFmt,&ckRiff,MMIO_FINDCHUNK);

	if (MMSYSERR_NOERROR != mmr) 
	{
		printf("Could not find a \"fmt \" chunk in the RIFF/WAVE chunk: mmr = 0x%08x\n", mmr);
		mmioClose(hFile, 0);
		return HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
	}

	// actually read the fmt data from the fmt chunk
	UINT nBytes = ckFmt.cksize;
	printf("format chunk size is %d.\n",nBytes);
	printf("sizeof(PCMWAVEFORMAT) is %d.\n",sizeof(PCMWAVEFORMAT));

	if (0 == nBytes) 
	{
		printf("\"fmt \" chunk has size 0!\n");
		mmioClose(hFile, 0);
		return E_INVALIDARG;
	}

    if (sizeof(PCMWAVEFORMAT) > nBytes)
	{
        printf("\"fmt \" chunk has size %u which is less than sizeof(PCMWAVEFORMAT) (%u)\n", nBytes,sizeof(PCMWAVEFORMAT));
        mmioClose(hFile, 0);
        return E_INVALIDARG;
    }

	printf("sizeof(WAVEFORMATEX) is %d.\n",sizeof(WAVEFORMATEX));

    UINT32 nBytesToAllocate = nBytes;
    if (nBytes < sizeof(WAVEFORMATEX))
	{
        nBytesToAllocate = sizeof(WAVEFORMATEX);
        printf("Wave format in file is smaller than a WAVEFORMATEX; will zero-pad the last %u bytes.\n", nBytesToAllocate - nBytes);
    }
    
    WAVEFORMATEX *pWfx = (WAVEFORMATEX*)CoTaskMemAlloc(nBytesToAllocate);
    if (NULL == pWfx)
	{
        printf("Could not allocate %u bytes for WAVEFORMATEX\n", nBytes);
        mmioClose(hFile, 0);
        return E_OUTOFMEMORY;
    }

    ZeroMemory(pWfx, nBytesToAllocate);

    LONG nBytesRead = mmioRead(hFile, (HPSTR)pWfx, nBytes);
    if (0 == nBytesRead)
	{
        printf("Unexpected EOF in fmt chunk - tried to read %u bytes\n", nBytes);
        CoTaskMemFree(pWfx);
        mmioClose(hFile, 0);
        return E_INVALIDARG;
    } 
	else if (-1 == nBytesRead)
	{
        printf("Could not read from file when reading format data - tried to read %u bytes\n", nBytes);
        CoTaskMemFree(pWfx);
        mmioClose(hFile, 0);
        return E_INVALIDARG;
    }
	else if (nBytes != (UINT)nBytesRead)
	{
        printf("Tried to read %u bytes but read %u bytes\n", nBytes, nBytesRead);
        CoTaskMemFree(pWfx);
        mmioClose(hFile, 0);
        return E_INVALIDARG;
    }

    // do some sanity checking on the wave format
    if (sizeof(WAVEFORMATEX) > nBytes) 
	{
        // well, it had better be a PCMWAVEFORMAT
        if (sizeof(PCMWAVEFORMAT) == nBytes && WAVE_FORMAT_PCM == pWfx->wFormatTag) 
		{
            // fine
        }
		else 
		{
            printf("Only WAVE_FORMAT_PCM formats are allowed to be smaller than sizeof(WAVEFORMATEX) - wFormatTag == %u, size == %u which is < sizeof(WAVEFORMATEX) (%u)\n",
                pWfx->wFormatTag, nBytes, (int)sizeof(WAVEFORMATEX));
            CoTaskMemFree(pWfx);
            mmioClose(hFile, 0);
            return E_INVALIDARG;
        }
    } 
	else 
	{
        // cbSize had better be sane
        if (sizeof(WAVEFORMATEX) + pWfx->cbSize != nBytes) 
		{
            printf("Format chunk size does not match format size - sizeof(WAVEFORMATEX) + cbSize == %u + %u = %u, chunk size == %u\n",
                (int)sizeof(WAVEFORMATEX), pWfx->cbSize, (int)sizeof(WAVEFORMATEX) + pWfx->cbSize, nBytes
            );
            CoTaskMemFree(pWfx);
            mmioClose(hFile, 0);
            return E_INVALIDARG;
        }
    }

    mmr = mmioAscend(hFile, &ckFmt, 0);
    if (MMSYSERR_NOERROR != mmr) 
	{
        printf("Could not mmioAscend out of \"fmt \" chunk: mmr = 0x%08x\n", mmr);
        CoTaskMemFree(pWfx);
        mmioClose(hFile, 0);
        return E_INVALIDARG;
    }

    // unless the wave format is WAVE_FORMAT_PCM, find the "fact" chunk
    UINT nFrames = 0;
    if (WAVE_FORMAT_PCM != pWfx->wFormatTag) 
	{
        MMCKINFO ckFact = {0};
        ckFact.ckid = MAKEFOURCC('f', 'a', 'c', 't');
        mmr = mmioDescend(hFile,&ckFact,&ckRiff,MMIO_FINDCHUNK);
        if (MMSYSERR_NOERROR != mmr) 
		{
            printf("wFormatTag == %u but no \"fact\" chunk follows the \"fmt \" chunk - \"fact\" is required for wFormatTag != %u: mmr = 0x%08x\n",
                pWfx->wFormatTag, WAVE_FORMAT_PCM, mmr
            );
            CoTaskMemFree(pWfx);
            mmioClose(hFile, 0);
            return E_INVALIDARG;
        }

        nBytes = ckFact.cksize;
        if (sizeof(UINT32) != nBytes) 
		{
            printf("\"fact\" chunk contains %u bytes - expected %u\n", nBytes, (int)sizeof(UINT32));
            CoTaskMemFree(pWfx);
            mmioClose(hFile, 0);
            return E_INVALIDARG;
        }

        nBytesRead = mmioRead(hFile, (HPSTR)&nFrames, nBytes);
        if (0 == nBytesRead) 
		{
            printf("Unexpected EOF in fmt chunk - tried to read %u bytes\n", nBytes);
            CoTaskMemFree(pWfx);
            mmioClose(hFile, 0);
            return E_INVALIDARG;
		}
		else if (-1 == nBytesRead) 
		{
            printf("Could not read from file when reading format data - tried to read %u bytes\n", nBytes);
            CoTaskMemFree(pWfx);
            mmioClose(hFile, 0);
            return E_INVALIDARG;
        }
		else if ((UINT)nBytesRead != nBytes)
		{
            printf("Tried to read %u bytes but read %u bytes\n", nBytes, nBytesRead);
            CoTaskMemFree(pWfx);
            mmioClose(hFile, 0);
            return E_INVALIDARG;
        }

        mmr = mmioAscend(hFile, &ckFact, 0);
        if (MMSYSERR_NOERROR != mmr) 
		{
            printf("Could not mmioAscend out of \"fact\" chunk: mmr = 0x%08x\n", mmr);
            CoTaskMemFree(pWfx);
            mmioClose(hFile, 0);
            return E_INVALIDARG;
        }
    } // fact chunk
    
    // find the "data" chunk
    MMCKINFO ckData = {0};
    ckData.ckid = MAKEFOURCC('d', 'a', 't', 'a');
    mmr = mmioDescend(hFile,&ckData,&ckRiff,MMIO_FINDCHUNK);
    if (MMSYSERR_NOERROR != mmr) 
	{
        printf("Could not find \"data\" after \"fmt \" (or \"fact\") chunk: mmr = 0x%08x\n", mmr);
        CoTaskMemFree(pWfx);
        mmioClose(hFile, 0);
        return E_INVALIDARG;
    }

    UINT nWaveDataBytes = ckData.cksize;

    // number of bytes of data should be a multiple of the frame size
    UINT32 nBytesPerFrame = pWfx->nBlockAlign;
    if (0 != nWaveDataBytes % nBytesPerFrame) 
	{
        printf("\"data\" chunk contains %u bytes - format is %u bytes per frame; but %u %% %u == %u != 0!\n",
            nWaveDataBytes, nBytesPerFrame, nWaveDataBytes, nBytesPerFrame,
            nWaveDataBytes % nBytesPerFrame
        );
        CoTaskMemFree(pWfx);
        mmioClose(hFile, 0);
        return E_INVALIDARG;
    }

    // if there was a fact chunk, verify that the number of bytes is expected
    if (WAVE_FORMAT_PCM != pWfx->wFormatTag) 
	{
        if (nFrames * nBytesPerFrame != nWaveDataBytes) 
		{
            printf(
                "\"fact\" chunk reports %u frames\n"
                "format is %u bytes per frame\n"
                "data chunk has %u bytes\n"
                "but %u * %u == %u != %u\n",
                nFrames, nBytesPerFrame, nWaveDataBytes,
                nFrames, nBytesPerFrame, nFrames * nBytesPerFrame, nWaveDataBytes
            );
            CoTaskMemFree(pWfx);
            mmioClose(hFile, 0);
            return E_INVALIDARG;
        }
    } 
	else 
	{
        // set the number of frames by the number of bytes
        nFrames = nWaveDataBytes / nBytesPerFrame;
    }
    
    *phFile = hFile;
    *ppWfx = pWfx;
    *pnBytes = nWaveDataBytes;
    *pnFrames = nFrames;
	printf("open file ok.\n");
    return S_OK;
}


HRESULT Play(HMMIO hFile,LPCWAVEFORMATEX pWfx,UINT32 nFramesInFile,UINT32 nBytesInFile,IMMDevice *pMMDevice) 
{
	printf("enter function play.\n");
    if (nFramesInFile == 0) 
	{
        printf("No frames in file.\n");
        return E_INVALIDARG;
    }
    
    if (nFramesInFile * pWfx->nBlockAlign != nBytesInFile) 
	{
        printf("Unexpected number of bytes in the file (%u) - expected %u.\n",nBytesInFile,nFramesInFile * pWfx->nBlockAlign);
        return E_INVALIDARG;
    }

    HRESULT hr;
	printf("Before activate audio client.\n");
	if(pMMDevice == NULL)
	{
		printf("device is null\n");
		return S_FALSE;
	}
	else
	{
		printf("device is not null\n");
		printf("Show device info here.\n");
		// open the property store on that device

        IPropertyStore *pPropertyStore;
        hr = pMMDevice->OpenPropertyStore(STGM_READ, &pPropertyStore);
        if (FAILED(hr)) 
		{
            printf("IMMDevice::OpenPropertyStore failed: hr = 0x%08x\n", hr);
            return hr;
        }

        // get the long name property
        PROPVARIANT pv; PropVariantInit(&pv);
        hr = pPropertyStore->GetValue(PKEY_Device_FriendlyName, &pv);
        pPropertyStore->Release();
        if (FAILED(hr))
		{
			printf("IPropertyStore::GetValue failed: hr = 0x%08x\n", hr);
			return hr;
		}
		if (VT_LPWSTR != pv.vt) {
            printf("PKEY_Device_FriendlyName variant type is %u - expected VT_LPWSTR", pv.vt);
            return E_UNEXPECTED;
        }
		printf("!!!!!!!!device friendly name is :%ls.\n",pv.pwszVal);
	}



    // activate an IAudioClient
    IAudioClient *pAudioClient;
    hr = pMMDevice->Activate(
        __uuidof(IAudioClient),
        CLSCTX_ALL, NULL,
        (void**)&pAudioClient
    );

    if (FAILED(hr)) 
	{
        printf("IMMDevice::Activate(IAudioClient) failed: hr = 0x%08x\n", hr);
        return hr;
    }

	printf("before check format support.\n");
    // check to see if the format is supported
    hr = pAudioClient->IsFormatSupported(
        AUDCLNT_SHAREMODE_EXCLUSIVE,
//AUDCLNT_SHAREMODE_SHARED,
        pWfx,
        NULL // can't suggest a "closest match" in exclusive mode
    );

    if (AUDCLNT_E_UNSUPPORTED_FORMAT == hr)
	{
        printf("Audio device does not support the requested format.\n");
        pAudioClient->Release();
        return hr;
    }
	else if (FAILED(hr)) 
	{
        printf("IAudioClient::IsFormatSupported failed: hr = 0x%08x.\n", hr);
        pAudioClient->Release();
        return hr;
    }

    // get the periodicity of the device
    REFERENCE_TIME hnsPeriod;
    hr = pAudioClient->GetDevicePeriod(
        NULL, // don't care about the engine period
        &hnsPeriod // only the device period
    );

    if (FAILED(hr)) 
	{
        printf("IAudioClient::GetDevicePeriod failed: hr = 0x%08x.\n", hr);
        pAudioClient->Release();
        return hr;
    }

    // need to know how many frames that is
    UINT32 nFramesInBuffer = (UINT32)( // frames =
        1.0 * hnsPeriod * // hns *
        pWfx->nSamplesPerSec / // (frames / s) /
        1000 / // (ms / s) /
        10000 // (hns / s) /
        + 0.5 // rounding
    );


    printf("The default period for this device is %I64u hundred-nanoseconds, or %u frames.\n", hnsPeriod, nFramesInBuffer);

    // call IAudioClient::Initialize the first time
    // this may very well fail
    // if the device period is unaligned
    hr = pAudioClient->Initialize(
        AUDCLNT_SHAREMODE_EXCLUSIVE,
		//AUDCLNT_SHAREMODE_SHARED,
        AUDCLNT_STREAMFLAGS_EVENTCALLBACK,
        hnsPeriod, hnsPeriod, pWfx, NULL
    );

    // if you get a compilation error on AUDCLNT_E_BUFFER_SIZE_NOT_ALIGNED,
    // uncomment the #define below
//#define AUDCLNT_E_BUFFER_SIZE_NOT_ALIGNED      AUDCLNT_ERR(0x019)
    if (AUDCLNT_E_BUFFER_SIZE_NOT_ALIGNED == hr) 
	{
        // if the buffer size was not aligned, need to do the alignment dance
        printf("Buffer size not aligned - doing the alignment dance.\n");
        
        // get the buffer size, which will be aligned
        hr = pAudioClient->GetBufferSize(&nFramesInBuffer);
        if (FAILED(hr)) 
		{
            printf("IAudioClient::GetBufferSize failed: hr = 0x%08x\n", hr);
            return hr;
        }
        printf("frams in buffer is %d.\n",nFramesInBuffer);
        // throw away this IAudioClient
        pAudioClient->Release();

        // calculate the new aligned periodicity
        hnsPeriod = // hns =
            (REFERENCE_TIME)(
                10000.0 * // (hns / ms) *
                1000 * // (ms / s) *
                nFramesInBuffer / // frames /
                pWfx->nSamplesPerSec  // (frames / s)
                + 0.5 // rounding
            );

        // activate a new IAudioClient
        hr = pMMDevice->Activate(
            __uuidof(IAudioClient),
            CLSCTX_ALL, NULL,
            (void**)&pAudioClient
        );
        if (FAILED(hr)) 
		{
            printf("IMMDevice::Activate(IAudioClient) failed: hr = 0x%08x\n", hr);
            return hr;
        }

        // try initialize again
        printf("Trying again with periodicity of %I64u hundred-nanoseconds, or %u frames.\n", hnsPeriod, nFramesInBuffer);
        hr = pAudioClient->Initialize(
            AUDCLNT_SHAREMODE_EXCLUSIVE,
            AUDCLNT_STREAMFLAGS_EVENTCALLBACK,
            hnsPeriod, hnsPeriod, pWfx, NULL
        );

        if (FAILED(hr))
		{
            printf("IAudioClient::Initialize failed, even with an aligned buffer: hr = 0x%08x\n", hr);
            pAudioClient->Release();
            return hr;
        }
    } 
	else if (FAILED(hr))
	{
        printf("IAudioClient::Initialize failed: hr = 0x%08x\n", hr);
        pAudioClient->Release();
        return hr;
    }

    // OK, IAudioClient::Initialize succeeded
    // let's see what buffer size we actually ended up with
    hr = pAudioClient->GetBufferSize(&nFramesInBuffer);
    if (FAILED(hr)) 
	{
        printf("IAudioClient::GetBufferSize failed: hr = 0x%08x\n", hr);
        pAudioClient->Release();
        return hr;
    }

    // calculate the new period
    hnsPeriod = // hns =
        (REFERENCE_TIME)(
            10000.0 * // (hns / ms) *
            1000 * // (ms / s) *
            nFramesInBuffer / // frames /
            pWfx->nSamplesPerSec  // (frames / s)
            + 0.5 // rounding
        );
    
    printf("We ended up with a period of %I64u hns or %u frames.\n", hnsPeriod, nFramesInBuffer);

    // make an event
    HANDLE hNeedDataEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (NULL == hNeedDataEvent) 
	{
        DWORD dwErr = GetLastError();
        printf("CreateEvent failed: GetLastError = %u\n", dwErr);
        pAudioClient->Release();
        return HRESULT_FROM_WIN32(dwErr);
    }

    // set it as the event handle
    hr = pAudioClient->SetEventHandle(hNeedDataEvent);
    if (FAILED(hr))
	{
        printf("IAudioClient::SetEventHandle failed: hr = 0x%08x\n", hr);
        CloseHandle(hNeedDataEvent);
        pAudioClient->Release();
        return hr;
    }
    
    // activate an IAudioRenderClient
    IAudioRenderClient *pAudioRenderClient;
    hr = pAudioClient->GetService(
        __uuidof(IAudioRenderClient),
        (void**)&pAudioRenderClient
    );
    if (FAILED(hr))
	{
        printf("IAudioClient::GetService(IAudioRenderClient) failed: hr 0x%08x\n", hr);
        CloseHandle(hNeedDataEvent);
        pAudioClient->Release();
        return hr;
    }
    
    // pre-roll a buffer of silence
    BYTE *pData;
    hr = pAudioRenderClient->GetBuffer(nFramesInBuffer, &pData); // just a "ping" buffer
    if (FAILED(hr)) 
	{
        printf("IAudioRenderClient::GetBuffer failed trying to pre-roll silence: hr = 0x%08x\n", hr);
        pAudioRenderClient->Release();        
        CloseHandle(hNeedDataEvent);
        pAudioClient->Release();
        return hr;
    }

    hr = pAudioRenderClient->ReleaseBuffer(nFramesInBuffer, AUDCLNT_BUFFERFLAGS_SILENT);
    if (FAILED(hr)) 
	{
        printf("IAudioRenderClient::ReleaseBuffer failed trying to pre-roll silence: hr = 0x%08x\n", hr);
        pAudioRenderClient->Release();        
        CloseHandle(hNeedDataEvent);
        pAudioClient->Release();
        return hr;
    }

	

    // register with MMCSS
    DWORD nTaskIndex = 0;
    HANDLE hTask = AvSetMmThreadCharacteristics("Playback", &nTaskIndex);
    if (NULL == hTask) 
	{
        DWORD dwErr = GetLastError();
        printf("AvSetMmThreadCharacteristics failed: last error = %u\n", dwErr);
        pAudioRenderClient->Release();        
        CloseHandle(hNeedDataEvent);
        pAudioClient->Release();
        return HRESULT_FROM_WIN32(dwErr);
    }

    // call IAudioClient::Start
    hr = pAudioClient->Start();
    if (FAILED(hr)) 
	{
        printf("IAudioClient::Start failed: hr = 0x%08x", hr);
        AvRevertMmThreadCharacteristics(hTask);
        pAudioRenderClient->Release();
        CloseHandle(hNeedDataEvent);
        pAudioClient->Release();
    }

	int renderLoopCount = 0;

    // render loop
    for (
        UINT32 nFramesPlayed = 0,
            nFramesThisPass = nFramesInBuffer;
        nFramesPlayed < nFramesInFile;
        nFramesPlayed += nFramesThisPass
    ) 
	{
        // in a production app there would be a timeout here
        WaitForSingleObject(hNeedDataEvent, INFINITE);
		
		//printf("Enter render loop count %d.\n",renderLoopCount++);
        
		// need data
        hr = pAudioRenderClient->GetBuffer(nFramesInBuffer, &pData);
        if (FAILED(hr)) 
		{
            printf("IAudioRenderClient::GetBuffer failed: hr = 0x%08x\n", hr);
            pAudioClient->Stop();
            AvRevertMmThreadCharacteristics(hTask);
            pAudioRenderClient->Release();        
            CloseHandle(hNeedDataEvent);
            pAudioClient->Release();
            return hr;
        }

        // is there a full buffer's worth of data left in the file?
        if (nFramesPlayed + nFramesInBuffer > nFramesInFile) 
		{
            // nope - this is the last buffer
            nFramesThisPass = nFramesInFile - nFramesPlayed;
        }
        UINT32 nBytesThisPass = nFramesThisPass * pWfx->nBlockAlign;

        LONG nBytesGotten = mmioRead(hFile, (HPSTR)pData, nBytesThisPass);
        if (0 == nBytesGotten)
		{
            printf("Unexpectedly reached the end of the file.\n");
            pAudioClient->Stop();
            AvRevertMmThreadCharacteristics(hTask);
            pAudioRenderClient->Release();        
            CloseHandle(hNeedDataEvent);
            pAudioClient->Release();
            return E_UNEXPECTED;            
        } 
		else if (-1 == nBytesGotten) 
		{
            printf("Error reading from the file.\n");
            pAudioClient->Stop();
            AvRevertMmThreadCharacteristics(hTask);
            pAudioRenderClient->Release();        
            CloseHandle(hNeedDataEvent);
            pAudioClient->Release();
            return E_UNEXPECTED;            
        } else if (nBytesGotten != (LONG)nBytesThisPass) 
		{
            printf("mmioRead got %d bytes instead of %u\n", nBytesGotten, nBytesThisPass);
            pAudioClient->Stop();
            AvRevertMmThreadCharacteristics(hTask);
            pAudioRenderClient->Release();        
            CloseHandle(hNeedDataEvent);
            pAudioClient->Release();
            return E_UNEXPECTED;
        }

        // if there's leftover buffer space, zero it out
        // it would be much better if we could intelligently fill this with silence
        // ah well, c'est la vie
        if (nFramesThisPass < nFramesInBuffer)
		{
            UINT32 nBytesToZero = (nFramesInBuffer * pWfx->nBlockAlign) - nBytesThisPass;
            ZeroMemory(pData + nBytesGotten, nBytesToZero);
        }

        hr = pAudioRenderClient->ReleaseBuffer(nFramesInBuffer, 0); // no flags
        if (FAILED(hr)) 
		{
            printf("IAudioRenderClient::ReleaseBuffer failed: hr = 0x%08x\n", hr);
            pAudioClient->Stop();
            AvRevertMmThreadCharacteristics(hTask);
            pAudioRenderClient->Release();        
            CloseHandle(hNeedDataEvent);
            pAudioClient->Release();
            return hr;
        }
    } // render loop

    // add a buffer of silence for good measure
    WaitForSingleObject(hNeedDataEvent, INFINITE);
    hr = pAudioRenderClient->GetBuffer(nFramesInBuffer, &pData);
    if (FAILED(hr))
	{
        printf("IAudioRenderClient::GetBuffer failed trying to post-roll silence: hr = 0x%08x\n", hr);
        pAudioClient->Stop();
        AvRevertMmThreadCharacteristics(hTask);
        pAudioRenderClient->Release();        
        CloseHandle(hNeedDataEvent);
        pAudioClient->Release();
        return hr;
    }

    hr = pAudioRenderClient->ReleaseBuffer(nFramesInBuffer, AUDCLNT_BUFFERFLAGS_SILENT);
    if (FAILED(hr)) 
	{
        printf("IAudioRenderClient::ReleaseBuffer failed trying to post-roll silence: hr = 0x%08x\n", hr);
        pAudioClient->Stop();
        AvRevertMmThreadCharacteristics(hTask);
        pAudioRenderClient->Release();        
        CloseHandle(hNeedDataEvent);
        pAudioClient->Release();
        return hr;
    }

    printf("Successfully played all %u frames.\n", nFramesInFile);

    pAudioClient->Stop();
    AvRevertMmThreadCharacteristics(hTask);
    pAudioRenderClient->Release();
    CloseHandle(hNeedDataEvent);
    pAudioClient->Release();
    
    return S_OK;
}


HRESULT Record(char * fileName,LPCWAVEFORMATEX pWfx,int timeSpan,IMMDevice * pMMDevice)
{
	printf("Enter function %s.\n",__FUNCTION__);
	HRESULT hr = S_FALSE;
	if(fileName == NULL || pWfx == NULL || timeSpan <=0 || pMMDevice == NULL)
	{
		printf("Invlaid paramreter.\n");
		return hr;
	}



	return hr;
}
