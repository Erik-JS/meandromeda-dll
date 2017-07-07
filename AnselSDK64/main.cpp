// AnselSDK64 - MAIN.CPP
#include "main.h"
#include "../sharedcode/Logging.h"
#include "../sharedcode/PluginLoader.h"

HINSTANCE originalAnselSDK64 = NULL;
UINT64 p[32];
BYTE bcrypcallpattern[] = {0xFF, 0x15, 0x00, 0x00, 0x00, 0x00, 0x48, 0x8B, 0x4C, 0x24, 0x40, 0x85, 0xC0};

bool DataCompare(const BYTE* OpCodes, const BYTE* Mask, const char* StrMask)
{
	while (*StrMask)
	{
		if(*StrMask == 'x' && *OpCodes != *Mask )
			return false;
		++StrMask;
		++OpCodes;
		++Mask;
	}
	return true;
}

DWORD64 FindPattern(DWORD64 StartAddress, DWORD CodeLen, BYTE* Mask, const char* StrMask, unsigned short ignore)
{
	unsigned short Ign = 0;
	DWORD i = 0;
	while (Ign <= ignore)
	{
		if(DataCompare((BYTE*)(StartAddress + i++), Mask,StrMask))
			++Ign;
		else if (i>=CodeLen)
			return 0;
	}
	return StartAddress + i - 1;
}

void loadOriginalAnselSDK64()
{
    originalAnselSDK64 = LoadLibrary("AnselSDK64.bak");
    if(!originalAnselSDK64)
        return;
    p[0] = (UINT64)GetProcAddress(originalAnselSDK64, "addUserControl");
    p[1] = (UINT64)GetProcAddress(originalAnselSDK64, "clearHdrBufferBindHint");
    p[2] = (UINT64)GetProcAddress(originalAnselSDK64, "clearHdrBufferFinishedHint");
    p[3] = (UINT64)GetProcAddress(originalAnselSDK64, "clearUserControlDescListDirtyFlag");
    p[4] = (UINT64)GetProcAddress(originalAnselSDK64, "getConfiguration");
    p[5] = (UINT64)GetProcAddress(originalAnselSDK64, "getConfigurationSize");
    p[6] = (UINT64)GetProcAddress(originalAnselSDK64, "getHdrBufferBindHintActive");
    p[7] = (UINT64)GetProcAddress(originalAnselSDK64, "getHdrBufferFinishedHintActive");
    p[8] = (UINT64)GetProcAddress(originalAnselSDK64, "getSessionConfigurationSize");
    p[9] = (UINT64)GetProcAddress(originalAnselSDK64, "getUserControlDescription");
    p[10] = (UINT64)GetProcAddress(originalAnselSDK64, "getUserControlDescriptionsSize");
    p[11] = (UINT64)GetProcAddress(originalAnselSDK64, "getVersion");
    p[12] = (UINT64)GetProcAddress(originalAnselSDK64, "initializeConfiguration");
    p[13] = (UINT64)GetProcAddress(originalAnselSDK64, "initializeSessionConfiguration");
    p[14] = (UINT64)GetProcAddress(originalAnselSDK64, "isAnselAvailable");
    p[15] = (UINT64)GetProcAddress(originalAnselSDK64, "isUserControlDescListDirty");
    p[16] = (UINT64)GetProcAddress(originalAnselSDK64, "lockUserControlDescriptions");
    p[17] = (UINT64)GetProcAddress(originalAnselSDK64, "markHdrBufferBind");
    p[18] = (UINT64)GetProcAddress(originalAnselSDK64, "markHdrBufferFinished");
    p[19] = (UINT64)GetProcAddress(originalAnselSDK64, "quaternionToRotationMatrixVectors");
    p[20] = (UINT64)GetProcAddress(originalAnselSDK64, "removeUserControl");
    p[21] = (UINT64)GetProcAddress(originalAnselSDK64, "rotationMatrixVectorsToQuaternion");
    p[22] = (UINT64)GetProcAddress(originalAnselSDK64, "setConfiguration");
    p[23] = (UINT64)GetProcAddress(originalAnselSDK64, "setSessionFunctions");
    p[24] = (UINT64)GetProcAddress(originalAnselSDK64, "setStopSessionCallback");
    p[25] = (UINT64)GetProcAddress(originalAnselSDK64, "setUpdateCameraFunc");
    p[26] = (UINT64)GetProcAddress(originalAnselSDK64, "setUserControlLabelLocalization");
    p[27] = (UINT64)GetProcAddress(originalAnselSDK64, "startSession");
    p[28] = (UINT64)GetProcAddress(originalAnselSDK64, "stopSession");
    p[29] = (UINT64)GetProcAddress(originalAnselSDK64, "unlockUserControlDescriptions");
    p[30] = (UINT64)GetProcAddress(originalAnselSDK64, "updateCamera");
    p[31] = (UINT64)GetProcAddress(originalAnselSDK64, "userControlValueChanged");
}

NTSTATUS WINAPI FakeBCryptVerifySignature(BCRYPT_KEY_HANDLE hKey, VOID *pPaddingInfo, PUCHAR pbHash, ULONG cbHash, PUCHAR pbSignature, ULONG cbSignature, ULONG dwFlags)
{
    return (NTSTATUS)0;
}

void patchBCryptFunc()
{
    DWORD64 functioncall = FindPattern((DWORD64)GetModuleHandle(NULL), 0xF00000, bcrypcallpattern, "xx????xxxxxxx", 0);
    if (!functioncall)
    {
        logprintf(">>BCryptVerifySignature: byte pattern not found.\n");
        return;
    }
    int distance = *(int*)(functioncall + 2);
    DWORD64 funcpointerlocation = functioncall + 6 + distance;
    DWORD dwProtect;
    VirtualProtect((void*)funcpointerlocation, 0x8, PAGE_READWRITE, &dwProtect );
    *(DWORD64*)funcpointerlocation = (DWORD64)FakeBCryptVerifySignature;
    VirtualProtect((void*)funcpointerlocation, 0x8, dwProtect, &dwProtect);
    logprintf(">>BCryptVerifySignature: pointer to fake function has been written.\n");
}

DWORD WINAPI Start(LPVOID lpParam)
{
    logStart("MEA_AnselSDK64.log");
    loadOriginalAnselSDK64();
    if(!originalAnselSDK64)
    {
        logprintf(">>Error loading AnselSDK64.bak!\n");
        logEnd();
        MessageBox(0, "Error loading AnselSDK64.bak!", "AnselSDK64 proxy DLL", 0);
        return 0;
    }
    patchBCryptFunc();
    AllocConsole();
    freopen("CON", "w", stdout);
    printf("< < < ASI plugin feedback > > >\n");
    loadPlugins(".");
    loadPlugins("ASI");
    if(getPluginCount() == 0)
    {
        FreeConsole();
    }
    else
    {
        SetConsoleTitle("ASI plugin feedback");
    }
    logEnd();
    return 0;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH:
            // attach to process
            // return FALSE to fail DLL load
            CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Start, 0, 0, 0);
            break;

        case DLL_PROCESS_DETACH:
            // detach from process
            if(originalAnselSDK64)
                FreeLibrary(originalAnselSDK64);
            break;

        case DLL_THREAD_ATTACH:
            // attach to thread
            break;

        case DLL_THREAD_DETACH:
            // detach from thread
            break;
    }
    return TRUE; // successful
}

extern "C" __declspec(dllexport) void addUserControl()
{
    asm("jmp *p + 0 * 4\n\t");
}

extern "C" __declspec(dllexport) void clearHdrBufferBindHint()
{
    asm("jmp *p + 1 * 4\n\t");
}

extern "C" __declspec(dllexport) void clearHdrBufferFinishedHint()
{
    asm("jmp *p + 2 * 4\n\t");
}

extern "C" __declspec(dllexport) void clearUserControlDescListDirtyFlag()
{
    asm("jmp *p + 3 * 4\n\t");
}

extern "C" __declspec(dllexport) void getConfiguration()
{
    asm("jmp *p + 4 * 4\n\t");
}

extern "C" __declspec(dllexport) void getConfigurationSize()
{
    asm("jmp *p + 5 * 4\n\t");
}

extern "C" __declspec(dllexport) void getHdrBufferBindHintActive()
{
    asm("jmp *p + 6 * 4\n\t");
}

extern "C" __declspec(dllexport) void getHdrBufferFinishedHintActive()
{
    asm("jmp *p + 7 * 4\n\t");
}

extern "C" __declspec(dllexport) void getSessionConfigurationSize()
{
    asm("jmp *p + 8 * 4\n\t");
}

extern "C" __declspec(dllexport) void getUserControlDescription()
{
    asm("jmp *p + 9 * 4\n\t");
}

extern "C" __declspec(dllexport) void getUserControlDescriptionsSize()
{
    asm("jmp *p + 10 * 4\n\t");
}

extern "C" __declspec(dllexport) void getVersion()
{
    asm("jmp *p + 11 * 4\n\t");
}

extern "C" __declspec(dllexport) void initializeConfiguration()
{
    asm("jmp *p + 12 * 4\n\t");
}

extern "C" __declspec(dllexport) void initializeSessionConfiguration()
{
    asm("jmp *p + 13 * 4\n\t");
}

extern "C" __declspec(dllexport) void isAnselAvailable()
{
    asm("jmp *p + 14 * 4\n\t");
}

extern "C" __declspec(dllexport) void isUserControlDescListDirty()
{
    asm("jmp *p + 15 * 4\n\t");
}

extern "C" __declspec(dllexport) void lockUserControlDescriptions()
{
    asm("jmp *p + 16 * 4\n\t");
}

extern "C" __declspec(dllexport) void markHdrBufferBind()
{
    asm("jmp *p + 17 * 4\n\t");
}

extern "C" __declspec(dllexport) void markHdrBufferFinished()
{
    asm("jmp *p + 18 * 4\n\t");
}

extern "C" __declspec(dllexport) void quaternionToRotationMatrixVectors()
{
    asm("jmp *p + 19 * 4\n\t");
}

extern "C" __declspec(dllexport) void removeUserControl()
{
    asm("jmp *p + 20 * 4\n\t");
}

extern "C" __declspec(dllexport) void rotationMatrixVectorsToQuaternion()
{
    asm("jmp *p + 21 * 4\n\t");
}

extern "C" __declspec(dllexport) void setConfiguration()
{
    asm("jmp *p + 22 * 4\n\t");
}

extern "C" __declspec(dllexport) void setSessionFunctions()
{
    asm("jmp *p + 23 * 4\n\t");
}

extern "C" __declspec(dllexport) void setStopSessionCallback()
{
    asm("jmp *p + 24 * 4\n\t");
}

extern "C" __declspec(dllexport) void setUpdateCameraFunc()
{
    asm("jmp *p + 25 * 4\n\t");
}

extern "C" __declspec(dllexport) void setUserControlLabelLocalization()
{
    asm("jmp *p + 26 * 4\n\t");
}

extern "C" __declspec(dllexport) void startSession()
{
    asm("jmp *p + 27 * 4\n\t");
}

extern "C" __declspec(dllexport) void stopSession()
{
    asm("jmp *p + 28 * 4\n\t");
}

extern "C" __declspec(dllexport) void unlockUserControlDescriptions()
{
    asm("jmp *p + 29 * 4\n\t");
}

extern "C" __declspec(dllexport) void updateCamera()
{
    asm("jmp *p + 30 * 4\n\t");
}

extern "C" __declspec(dllexport) void userControlValueChanged()
{
    asm("jmp *p + 31 * 4\n\t");
}
