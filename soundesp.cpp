#include "client.h"

deque<my_sound_t> mySounds;

int CheckSoundEsp(char* szSoundFile)
{
	if (!strstr(szSoundFile, "pl_shell") &&
		!strstr(szSoundFile, "ric") &&
		!strstr(szSoundFile, "die") &&
		!strstr(szSoundFile, "glass") &&
		!strstr(szSoundFile, "debris") &&
		!strstr(szSoundFile, "death") &&
		strstr(szSoundFile, "player"))
		return true;
	return false;
}

void PreS_DynamicSound(int entid, DWORD entchannel, char* szSoundFile, float* fOrigin, float fVolume, float fAttenuation, int iTimeOff, int iPitch)
{
	if (cvar.visual_sound && g_pEngine->GetMaxClients() && szSoundFile && fOrigin)
	{
		if (CheckSoundEsp(szSoundFile))
		{
			if (entid > 0 && entid < 33 && entid != g_Local.iIndex)
			{
				if (strstr(szSoundFile, "bhit_helmet"))
					g_Player[entid].iHealth -= 80;
				else if (strstr(szSoundFile, "bhit_kevlar"))
					g_Player[entid].iHealth -= 20;
				else if (strstr(szSoundFile, "bhit_flesh"))
					g_Player[entid].iHealth -= 30;
				else if (strstr(szSoundFile, "headshot"))
					g_Player[entid].iHealth -= 80;
				else if (strstr(szSoundFile, "die") || strstr(szSoundFile, "death"))
					g_Player[entid].iHealth = 100;

				if (!(Cstrike_SequenceInfo[g_Player[entid].pEnt->curstate.sequence] == SEQUENCE_DIE))
				{
					g_Player[entid].sound.origin = fOrigin;
					g_Player[entid].sound.timestamp = GetTickCount();
				}
			}
			if (g_Player[entid].sound.origin[0] != fOrigin[0] &&
				g_Player[entid].sound.origin[1] != fOrigin[1] &&
				g_Player[entid].sound.origin[2] != fOrigin[2])
			{
				my_sound_t sound;
				sound.origin = fOrigin;
				sound.timestamp = GetTickCount();
				mySounds.push_back(sound);
			}
		}
	}
	PreS_DynamicSound_s(entid, entchannel, szSoundFile, fOrigin, fVolume, fAttenuation, iTimeOff, iPitch);
}