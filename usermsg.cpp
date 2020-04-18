#include "client.h"

PUserMsg pUserMsgBase;
pfnUserMsgHook pResetHUD;
pfnUserMsgHook pTeamInfo;
pfnUserMsgHook pDeathMsg;
pfnUserMsgHook pSetFOV;
pfnUserMsgHook pHealth;
pfnUserMsgHook pScoreAttrib;

deque<DeathInfos> myFrags;

int ScoreAttrib(const char* pszName, int iSize, void* pbuf)
{
	BEGIN_READ(pbuf, iSize);
	int id = READ_BYTE();
	int info = READ_BYTE();
	g_Player[id].vip = (info & (1 << 2));
	g_Player[id].bAliveInScoreTab = !(info & (1 << 0));
	return pScoreAttrib(pszName, iSize, pbuf);
}

int SetFOV(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);
	int iFOV = READ_BYTE();
	g_Local.iFOV = iFOV;
	return (*pSetFOV)(pszName, iSize, pbuf);
}

int ResetHUD(const char *pszName, int iSize, void *pbuf)
{
	AtRoundStart();
	return pResetHUD(pszName, iSize, pbuf);
}

int DeathMsg(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);
	int killer = READ_BYTE();
	int victim = READ_BYTE();
	int headshot = READ_BYTE();

	if (killer != victim && killer == g_Local.iIndex && victim > 0 && victim < 33)
		dwReactionTime = GetTickCount();

	if (victim != g_Local.iIndex && victim > 0 && victim < 33)
		g_Player[victim].iHealth = 100;

	if (victim != g_Local.iIndex && killer == g_Local.iIndex && victim > 0 && victim < 33)
	{
		if (cvar.visual_headshot_mark && headshot)
		{
			DeathInfos deathInfos;
			deathInfos.to = g_PlayerExtraInfoList[victim].vHitbox[11];
			deathInfos.timestamp = GetTickCount();
			myFrags.push_back(deathInfos);
		}
		if (cvar.visual_kill_lightning && CheckDrawEngine())
		{
			for (unsigned int i = 0; i < 5; i++)
			{
				Vector origin;
				Vector direction;

				origin[0] = g_Player[victim].pEnt->origin[0];
				origin[1] = g_Player[victim].pEnt->origin[1];
				origin[2] = g_Player[victim].pEnt->origin[2] + 30;

				direction[0] = g_Player[victim].pEnt->origin[0];
				direction[1] = g_Player[victim].pEnt->origin[1];
				direction[2] = g_Player[victim].pEnt->origin[2] + 6000;

				int beamindex = g_Engine.pEventAPI->EV_FindModelIndex("sprites/laserbeam.spr");
				g_Engine.pEfxAPI->R_BeamPoints(origin, direction, beamindex, 1, 1.f, 1, 1, 0, 0, 0, cvar.color_red, cvar.color_green, cvar.color_blue);
			}
		}
		if (cvar.visual_kill_sound)
		{
			static DWORD doublekill = 0;
			static DWORD triplekill = 0;
			static DWORD monsterkill = 0;

			if (GetTickCount() - monsterkill < 1000)
			{
				BASS_ChannelSetAttribute(BASS::stream_sounds.monsterkill, BASS_ATTRIB_VOL, cvar.visual_kill_volume / 100.f);
				BASS_ChannelPlay(BASS::stream_sounds.monsterkill, true);
				monsterkill = GetTickCount();
			}
			if (GetTickCount() - triplekill < 1000 && GetTickCount() - monsterkill > 1000)
			{
				BASS_ChannelSetAttribute(BASS::stream_sounds.triplekill, BASS_ATTRIB_VOL, cvar.visual_kill_volume / 100.f);
				BASS_ChannelPlay(BASS::stream_sounds.triplekill, true);
				monsterkill = GetTickCount();
			}
			if (GetTickCount() - doublekill < 1000 && GetTickCount() - triplekill > 1000)
			{
				BASS_ChannelSetAttribute(BASS::stream_sounds.doublekill, BASS_ATTRIB_VOL, cvar.visual_kill_volume / 100.f);
				BASS_ChannelPlay(BASS::stream_sounds.doublekill, true);
				triplekill = GetTickCount();
			}
			if (headshot && (GetTickCount() - doublekill > 1000))
			{
				BASS_ChannelSetAttribute(BASS::stream_sounds.headshot, BASS_ATTRIB_VOL, cvar.visual_kill_volume / 100.f);
				BASS_ChannelPlay(BASS::stream_sounds.headshot, true);
				doublekill = GetTickCount();
			}
			if (GetTickCount() - doublekill > 1000)
			{
				doublekill = GetTickCount();
			}
		}
	}

	return pDeathMsg(pszName, iSize, pbuf);
}

int TeamInfo(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);
	int id = READ_BYTE();
	char *szTeam = READ_STRING();

	if (id >= 0 && id < 33)
	{
		if (!lstrcmpA(szTeam, "TERRORIST"))
		{
			g_Player[id].iTeam = 1;
			if (id == g_Local.iIndex) { g_Local.iTeam = 1; }
		}
		else if (!lstrcmpA(szTeam, "CT"))
		{
			g_Player[id].iTeam = 2;
			if (id == g_Local.iIndex) { g_Local.iTeam = 2; }
		}
		else
		{
			g_Player[id].iTeam = 0;
			if (id == g_Local.iIndex) { g_Local.iTeam = 0; }
		}
	}
	return pTeamInfo(pszName, iSize, pbuf);
}

int Health(const char *pszName, int iSize, void *pbuf)
{
	BEGIN_READ(pbuf, iSize);
	g_Local.iHealth = READ_BYTE();
	return pHealth(pszName, iSize, pbuf);
}

PUserMsg UserMsgByName(char* szMsgName)
{
	PUserMsg Ptr = NULL;
	Ptr = pUserMsgBase;
	while (Ptr->next)
	{
		if (!strcmp(Ptr->name, szMsgName))
			return Ptr;
		Ptr = Ptr->next;
	}
	Ptr->pfn = 0;
	return Ptr;
}

pfnUserMsgHook HookUserMsg(char* szMsgName, pfnUserMsgHook pfn)
{
	PUserMsg Ptr = NULL;
	pfnUserMsgHook Original = NULL;
	Ptr = UserMsgByName(szMsgName);
	if (Ptr->pfn != 0) {
		Original = Ptr->pfn;
		Ptr->pfn = pfn;
		return Original;
	}
	else
	{
		char Message[256];
		strcpy(Message, "ERROR: Couldn't find ");
		strcat(Message, szMsgName);
		strcat(Message, " message.");
		c_Offset.Error(Message);
	}
}

void HookUserMessages()
{
#define HOOK_MSG(n) \
	p##n = HookUserMsg(#n, ##n);

	HOOK_MSG(ResetHUD);
	HOOK_MSG(TeamInfo);
	HOOK_MSG(DeathMsg);
	HOOK_MSG(SetFOV);
	HOOK_MSG(Health);
	HOOK_MSG(ScoreAttrib);
}