#ifndef _PLAYERS_
#define _PLAYERS_

class PlayerInfoLocal
{
public:
	cl_entity_s* pEnt;
	bool bJumped;
	bool bAlive;
	int iIndex;
	int iFOV;
	int iMovetype;
	int iFlags;
	int iUsehull;
	int iTeam;
	int iHealthStrafe;
	int iHealth;
	int iMaxHitboxes;
	Vector vForward;
	Vector vStrafeForward;
	Vector vNoSpreadAngle;
	Vector vNoRecoilAngle;
	Vector vVelocity;
	Vector vT1, vT2;
	Vector vStartjumppos;
	Vector vOrigin;
	Vector vPunchangle;
	Vector vEye;
	Vector vView_ofs;
	Vector vViewAngles;
	float flVelocity;
	float flHeightorigin;
	float flFallVelocity;
	float flVelocityspeed;
	float flJumpdist;
	float flJumpmesstime;
	float flGroundangle;
	float flWaterlevel;
	float flEdgeDistance;
	float flDamage;
	float flVelocity2D;
	float flFrametime;
	CBasePlayerWeapon weapon;
};
extern PlayerInfoLocal g_Local;

class PlayerInfo
{
public:
	void Init() 
	{
		entinfo.name = "\\missing-name\\";
		entinfo.model = "missing-model";
	}

	hud_player_info_t entinfo;
	cl_entity_s* pEnt;
	Vector vVelocity;
	Vector vOrigin;
	player_sound_t sound;
	bool bAlive;
	bool bDucked;
	bool bVisible;
	bool bGotPlayer;
	bool bAliveInScoreTab;
	bool vip;
	int iHealth;
	int iTeam;
	float flFrametime;
	float flDist;

	PlayerInfo()
	{
		Init();
	}
};
extern PlayerInfo g_Player[33];

struct player_extra_info_t
{
	Vector vHitbox[21];
	Vector vHitboxMulti[21][8];

	bool bHitboxVisible[21];
	bool bHitboxPointsVisible[21][8];

	float fHitboxFOV[21];
	float fHitboxPointsFOV[21][8];
};
extern player_extra_info_t g_PlayerExtraInfoList[33];

unsigned int Cstrike_SequenceInfo[];

void UpdateLocalPlayer();
void UpdatePlayer();
void SetAntiAimAngles(cl_entity_s* ent);
void ThirdPerson(struct ref_params_s* pparams);
void RunHLCommands();
char* GetWeaponName(int weaponmodel);
bool CheckDrawEngine();
bool CheckDraw();
void pWeapon(cl_entity_t* ent, model_t* pmodel, int modelindex, bool rotate, float test1, float test2, float test3, float test4, float test5);
void wWeapon(cl_entity_t* ent, model_t* pmodel, int modelindex, bool rotate, float test1, float test2, float test3);
void PlayerWeapon(cl_entity_s* ent);
bool bPathFree(float* pflFrom, float* pflTo);

#endif