//-------------------------------------------------------------------------------------------------------
// VST Plug-Ins SDK
//-
// Example ADelay (VST 2.0)
// Simple Delay plugin (Mono->Stereo)
//-
// � 2003, Steinberg Media Technologies, All Rights Reserved
//-------------------------------------------------------------------------------------------------------

#ifndef __ADelay__
#include "ADelay.h"
#endif

bool oome = false;

#if MAC
#pragma export on
#endif

//------------------------------------------------------------------------
// Prototype of the export function main
//------------------------------------------------------------------------
#if BEOS
#define main main_plugin
extern "C" __declspec(dllexport) AEffect *main_plugin (audioMasterCallback audioMaster);

#elif MACX
#define main main_macho
extern "C" AEffect *main_macho (audioMasterCallback audioMaster);

#elif WIN32
// jsl - avoids having to have a .def file
extern "C" __declspec(dllexport) int main (audioMasterCallback audioMaster);

#else
AEffect *main (audioMasterCallback audioMaster);
#endif

//------------------------------------------------------------------------
// jsl - VC8 started whining about this pray that int and pointer
// are the same size
//AEffect* main (audioMasterCallback audioMaster)
int main (audioMasterCallback audioMaster)
{
	// Get VST Version
	if (!audioMaster (0, audioMasterVersion, 0, 0, 0, 0))
		return 0;  // old version

	// Create the AudioEffect
	ADelay* effect = new ADelay (audioMaster);
	if (!effect)
		return 0;
	
	// Check if no problem in constructor of AGain
	if (oome)
	{
		delete effect;
		return 0;
	}
	return (int)(effect->getAeffect ());
}

#if MAC
#pragma export off
#endif

//------------------------------------------------------------------------
#if WIN32
#include <windows.h>
void* hInstance;
BOOL WINAPI DllMain (HINSTANCE hInst, DWORD dwReason, LPVOID lpvReserved)
{
	hInstance = hInst;
	return 1;
}
#endif