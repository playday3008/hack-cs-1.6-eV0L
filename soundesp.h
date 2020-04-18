#ifndef _SOUND_
#define _SOUND_

typedef struct
{
	Vector origin;
	DWORD timestamp;
} player_sound_t;

typedef struct
{
	Vector origin;
	DWORD timestamp;
} my_sound_t;

typedef void(*PreS_DynamicSound_t)(int, DWORD, char*, float*, float, float, int, int);
void PreS_DynamicSound(int entid, DWORD entchannel, char* szSoundFile, float* fOrigin, float fVolume, float fAttenuation, int iTimeOff, int iPitch);
extern deque<my_sound_t> mySounds;

#endif
