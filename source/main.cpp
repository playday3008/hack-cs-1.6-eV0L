#include "client.h"

char hackdir[256];

void SetupHook()
{
	while (!c_Offset.GetRendererInfo())
		Sleep(100);

	g_pClient = (cl_clientfunc_t*)c_Offset.ClientFuncs();

	g_pEngine = (cl_enginefunc_t*)c_Offset.EngineFuncs();

	g_pStudio = (engine_studio_api_t*)c_Offset.StudioFuncs();

	g_pStudioModelRenderer = (StudioModelRenderer_t*)c_Offset.FindStudioModelRenderer();

	glReadPixels_s = (glReadPixels_t)DetourFunction((PBYTE)GetProcAddress(GetModuleHandleA("opengl32.dll"), "glReadPixels"), (PBYTE)m_glReadPixels);

	PreS_DynamicSound_s = (PreS_DynamicSound_t)DetourFunction((LPBYTE)c_Offset.PreS_DynamicSound(), (LPBYTE)& PreS_DynamicSound);

	CL_Move_s = (CL_Move_t)DetourFunction((LPBYTE)c_Offset.CL_Move(), (LPBYTE)&CL_Move);
	
	c_Offset.GlobalTime();

	c_Offset.dwSpeedPointer = (DWORD)c_Offset.FindSpeed();

	c_Offset.PatchInterpolation();

	Init_Command_Interpreter();

	while (!g_Client.V_CalcRefdef)
		RtlCopyMemory(&g_Client, g_pClient, sizeof(cl_clientfunc_t));

	while (!g_Engine.V_CalcShake)
		RtlCopyMemory(&g_Engine, g_pEngine, sizeof(cl_enginefunc_t));

	while (!g_Studio.StudioSetupSkin)
		RtlCopyMemory(&g_Studio, g_pStudio, sizeof(engine_studio_api_t));

	while (!g_StudioModelRenderer.StudioSlerpBones)
		RtlCopyMemory(&g_StudioModelRenderer, g_pStudioModelRenderer, sizeof(StudioModelRenderer_t));

	while (!pmove)
		pmove = (playermove_t*)c_Offset.FindPlayerMove();

	while (!pUserMsgBase)
		pUserMsgBase = c_Offset.FindUserMsgBase();

	pScreenFade = *(screenfade_t**)((DWORD)g_Engine.pfnSetScreenFade + 0x17);
	if (IsBadReadPtr(pScreenFade, sizeof(screenfade_t)))
		pScreenFade = *(screenfade_t**)((DWORD)g_Engine.pfnSetScreenFade + 0x18);
	
	DWORD SteamBase = (DWORD)GetModuleHandle("hw.dll");
	if (SteamBase)
	{
		g_pfnSteam_GSInitiateGameConnection = (decltype(g_pfnSteam_GSInitiateGameConnection))c_Offset.GetDestination(c_Offset.Steam_GSInitiateGameConnection());
		c_Offset.SetDestination(c_Offset.Steam_GSInitiateGameConnection(), (uintptr_t)& Steam_GSInitiateGameConnection_Hooked);
	}
	
	HookFunction();

	HookUserMessages();

	HookOpenGL();

	InitRadio();
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
	switch (fdwReason) 
	{
	case DLL_PROCESS_ATTACH:
		if (GetLastError() != ERROR_ALREADY_EXISTS)
		{
			DisableThreadLibraryCalls(hinstDLL);
			GetModuleFileName(hinstDLL, hackdir, 255);
			char* pos = hackdir+strlen(hackdir);
			while(pos>=hackdir && *pos!='\\')
				--pos;
			pos[1]=0;
			CreateThread(0, 0, (LPTHREAD_START_ROUTINE)SetupHook, 0, 0, 0);
		}
		break;
	}
	return TRUE;
}