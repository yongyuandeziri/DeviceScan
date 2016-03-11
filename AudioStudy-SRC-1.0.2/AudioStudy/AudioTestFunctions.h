#pragma once

#include "stdafx.h"
#include "AudioFunctions.h"

struct PlayThreadArgs 
{
	HMMIO hFile;
	LPCWAVEFORMATEX pWfx;
	UINT32 nFrames;
	UINT32 nBytes;
	IMMDevice *pMMDevice;
	HRESULT hr;
};




int DoAudioRenderTest();
int DoAudioCaptureTest();

