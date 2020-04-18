#ifndef _OFFSET_
#define _OFFSET_

class AutoOffset
{
private:
	DWORD dwOldPageProtection;

public:
	DWORD HwBase, HwSize, HwEnd;
	DWORD ClBase, ClSize, ClEnd;
	DWORD UiBase, UiSize, UiEnd;
	DWORD HlBase, HlSize, HlEnd;

	BOOL __comparemem(const UCHAR *buff1, const UCHAR *buff2, UINT size);
	ULONG __findmemoryclone(const ULONG start, const ULONG end, const ULONG clone, UINT size);
	ULONG __findreference(const ULONG start, const ULONG end, const ULONG address);

	DWORD FarProc(const DWORD Address, DWORD LB, DWORD HB);
	BYTE HLType;

	void GlobalTime();
	void Error(const PCHAR Msg);
	void PatchInterpolation(void);

	unsigned Absolute(DWORD Addr);

	bool GetRendererInfo();
	bool EnablePageWrite(DWORD addr, DWORD size);
	bool RestorePageProtection(DWORD addr, DWORD size);

	PVOID ClientFuncs();
	PVOID EngineFuncs();
	PVOID StudioFuncs();
	PVOID FindStudioModelRenderer(void);
	PVOID FindPlayerMove(void);
	DWORD GetModuleSize(const DWORD Address);
	DWORD dwSendPacketPointer, dwSendPacketBackup, dwSpeedPointer;
	DWORD CL_Move(void);
	DWORD FindSpeed(void);
	DWORD PreS_DynamicSound(void);
	DWORD Steam_GSInitiateGameConnection(void);
	PUserMsg FindUserMsgBase();

	static uintptr_t GetDestination(uintptr_t callPtr){return (intptr_t)(callPtr + 5) + *(intptr_t*)(callPtr + 1);}

	static void SetDestination(uintptr_t callPtr, uintptr_t destPtr)
	{
		DWORD dwOldProtect;
		VirtualProtect(LPVOID(callPtr + 1), sizeof(intptr_t), PAGE_EXECUTE_READWRITE, &dwOldProtect);
		*(intptr_t*)(callPtr + 1) = destPtr - (callPtr + 5);
		VirtualProtect(LPVOID(callPtr + 1), sizeof(intptr_t), dwOldProtect, &dwOldProtect);
	}
};
extern AutoOffset c_Offset;

int Steam_GSInitiateGameConnection_Hooked(void* pData, int iMaxBytes, uint64_t g_SteamID, uint32_t g_ServerIP, uint16_t g_ServerPort, bool bIsSecure);
extern int(*g_pfnSteam_GSInitiateGameConnection)(void* pData, int iMaxBytes, uint64_t SteamID, uint32_t ServerIP, uint16_t ServerPort, bool bIsSecure);

#endif