// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//
#define _CRT_SECURE_NO_DEPRECATE 1
#pragma warning(disable:4996);
#pragma warning(once:4996);
#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>



// TODO: reference additional headers your program requires here
#include <Windows.h>
#include <stdio.h>
#include <mmsystem.h>
#include <mmdeviceapi.h>
#include <functiondiscoverykeys_devpkey.h>
#include <mmiscapi.h>
#include <minwindef.h>
#include <audioclient.h>
#include <avrt.h>


#pragma   comment(lib,"Winmm.lib") 
#pragma   comment(lib,"Avrt.lib") 

#define		MAX_RENDER_DEVICE_COUNT		100