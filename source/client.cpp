#include "client.h"

cl_clientfunc_t *g_pClient = NULL;
cl_enginefunc_t *g_pEngine = NULL;
engine_studio_api_t *g_pStudio = NULL;
StudioModelRenderer_t* g_pStudioModelRenderer = NULL;
playermove_t* pmove = NULL;
screenfade_t* pScreenFade;

CL_Move_t CL_Move_s = NULL;
PreS_DynamicSound_t PreS_DynamicSound_s = NULL;
cl_clientfunc_t g_Client;
cl_enginefunc_t g_Engine;
engine_studio_api_t g_Studio;
StudioModelRenderer_t g_StudioModelRenderer;

DWORD HudRedraw;

void HUD_Redraw(float time, int intermission)
{
	g_Client.HUD_Redraw(time, intermission);

	HudRedraw = GetTickCount();
	KzFameCount();
	Snapshot();
}

int HUD_Key_Event(int down, int keynum, const char* pszCurrentBinding)
{
	//preset keys
	bool keyrush = cvar.rage_active && keynum == cvar.route_rush_key;
	bool keyquick = cvar.misc_quick_change && keynum == cvar.misc_quick_change_key;
	bool keystrafe = cvar.kz_strafe && keynum == cvar.kz_strafe_key;
	bool keyfast = cvar.kz_fast_run && keynum == cvar.kz_fastrun_key;
	bool keygstrafe = cvar.kz_gstrafe && keynum == cvar.kz_gstrafe_key;
	bool keybhop = cvar.kz_bhop && keynum == cvar.kz_bhop_key;
	bool keyjump = cvar.kz_jump_bug && keynum == cvar.kz_jumpbug_key;
	bool keyrage = cvar.rage_active && !cvar.rage_auto_fire && keynum == cvar.rage_auto_fire_key;
	bool keylegit = !cvar.rage_active && cvar.legit[g_Local.weapon.m_iWeaponID].trigger_active && !cvar.legit[g_Local.weapon.m_iWeaponID].active && IsCurWeaponGun() && keynum == cvar.legit_trigger_key;
	bool keychat = cvar.gui_chat && keynum == cvar.gui_chat_key;
	bool keychatt = cvar.gui_chat && keynum == cvar.gui_chat_key_team;
	bool keyhud = keynum == cvar.gui_key_hud;

	//send key handle to menu
	keysmenu[keynum] = down;

	//hud menu bind
	if (keyhud && down)
	{
		bShowMenu = !bShowMenu;
		SaveCvar();
		return false;
	}

	//if menu active return the below
	if (bShowMenu)
		return false;

	//do key binding
	if (keyrush)
	{
		if (down)
			cvar.route_auto = 1, cvar.misc_wav_speed = 64;
		else
			cvar.route_auto = 0, cvar.misc_wav_speed = 1;
	}
	if (keyquick && down)
	{
		cvar.rage_active = !cvar.rage_active, SaveCvar(), ModeChangeDelay = GetTickCount();
		if (!cvar.rage_active)cvar.route_auto = 0, cvar.misc_wav_speed = 1, RageKeyStatus = false;
	}
	if (keystrafe)
		Strafe = down;
	if (keyfast)
		Fastrun = down;
	if (keygstrafe)
		Gstrafe = down;
	if (keybhop)
		Bhop = down;
	if (keyjump)
		Jumpbug = down;
	if (keyrage)
		RageKeyStatus = down;
	if (keylegit && down)
		TriggerKeyStatus = !TriggerKeyStatus;
	if (keychat && down)
		ActivateChat();
	if (keychatt && down)
		ActivateChatTeam();

	//block game bind
	if ((keystrafe || keyfast || keygstrafe || keybhop || keyjump || keyrage || keylegit || keychat || keychatt || keyquick || keyrush) && down)
		return false;

	return g_Client.HUD_Key_Event(down, keynum, pszCurrentBinding);
}

void CL_CreateMove(float frametime, struct usercmd_s* cmd, int active)
{
	g_Client.CL_CreateMove(frametime, cmd, active);
	
	AdjustSpeed(cvar.misc_wav_speed);
	ListIdHook();
	UpdateAimbot(frametime);
	UpdateWeaponData();
	FastZoom(cmd);
	AimBot(cmd);
	ContinueFire(cmd);
	ItemPostFrame(cmd);
	Kz(frametime, cmd);
	NoRecoil(cmd);
	NoSpread(cmd);
	Route(cmd);
	RouteDirection(cmd);
	AntiAim(cmd);
	FakeLag(cmd);
	CustomFOV();
}

void HUD_PostRunCmd(struct local_state_s* from, struct local_state_s* to, struct usercmd_s* cmd, int runfuncs, double time, unsigned int random_seed)
{
	g_Client.HUD_PostRunCmd(from, to, cmd, runfuncs, time, random_seed);
	ItemPreFrame(from, to, cmd, runfuncs, time, random_seed);
}

void V_CalcRefdef(struct ref_params_s* pparams)
{
	g_Local.vPunchangle = pparams->punchangle;
	g_Local.vForward = pparams->forward;
	V_CalcRefdefRecoil(pparams);

	g_Client.V_CalcRefdef(pparams);

	ThirdPerson(pparams);
	ClearIndex(pparams);
	g_Local.vStrafeForward = pparams->forward;
	g_Local.iHealthStrafe = pparams->health;
}

void AtRoundStart(void)
{
	RunHLCommands();
	ContinueRoute();
	myFrags.clear();
	mySounds.clear();
	for (unsigned int i = 0; i < 33; i++)
	{
		g_Player[i].sound.timestamp = 0;
		g_Player[i].iHealth = 100;
	}
}

void bSendpacket(bool status)
{
	static bool bsendpacket_status = true;
	static DWORD NULLTIME = NULL;
	if (status && !bsendpacket_status)
	{
		bsendpacket_status = true;
		*(DWORD*)(c_Offset.dwSendPacketPointer) = c_Offset.dwSendPacketBackup;
	}
	if (!status && bsendpacket_status)
	{
		bsendpacket_status = false;
		*(DWORD*)(c_Offset.dwSendPacketPointer) = (DWORD)& NULLTIME;
	}
}

void CL_Move()
{
	bSendpacket(true);
	CL_Move_s();
}

void AdjustSpeed(double speed)
{
	static double LastSpeed = 1;
	if (speed != LastSpeed)
	{
		*(double*)c_Offset.dwSpeedPointer = (speed * 1000);

		LastSpeed = speed;
	}
}

void HUD_PlayerMoveInit(struct playermove_s* ppmove)
{
	PM_InitTextureTypes(ppmove);
	return g_Client.HUD_PlayerMoveInit(ppmove);
}

void HUD_ProcessPlayerState(struct entity_state_s* dst, const struct entity_state_s* src)
{
	if (cvar.bypass_valid_blockers)
	{
		for (int i = 0; i < 3; i++)
			src->mins[i] = i == 2 ? -36 : -16;
		for (int i = 0; i < 3; i++)
			src->maxs[i] = i == 2 ? 36 : 16;
	}
	g_Client.HUD_ProcessPlayerState(dst, src);
}

int HUD_GetHullBounds(int hullnum, float* mins, float* maxs)
{
	if (hullnum == 1)
		maxs[2] = 32.0f;
	return 1;
}

int CL_IsThirdPerson(void)
{
	if (cvar.visual_chase_cam && g_Local.bAlive && CheckDrawEngine())
		return 1;
	return g_Client.CL_IsThirdPerson();
}

int HUD_AddEntity(int type, struct cl_entity_s* ent, const char* modelname)
{
	if (ent && ent->curstate.weaponmodel && cvar.visual_weapon_model && (g_Player[ent->index].iTeam != g_Local.iTeam || cvar.visual_visual_team) && CheckDrawEngine())
		PlayerWeapon(ent);

	if (ent && ent->player && ent->index == g_Local.iIndex && g_Local.bAlive && cvar.rage_active && cvar.visual_chase_cam && CheckDrawEngine())
	{
		SetAntiAimAngles(ent);
		g_Engine.CL_CreateVisibleEntity(ET_PLAYER, ent);
		return 0;
	}
	return g_Client.HUD_AddEntity(type, ent, modelname);
}

int StudioCheckBBox(void)
{
	if (cvar.rage_active)
		return 1;
	return g_Studio.StudioCheckBBox();
}

void StudioSetRemapColors(int top, int bottom)
{
	cl_entity_s* ent = g_Studio.GetCurrentEntity();
	SetRemapColors(ent);
	g_Studio.StudioSetRemapColors(top, bottom);
}

void HookFunction()
{
	g_pClient->HUD_Redraw = HUD_Redraw;
	g_pClient->HUD_AddEntity = HUD_AddEntity;
	g_pClient->CL_CreateMove = CL_CreateMove;
	g_pClient->HUD_PlayerMoveInit = HUD_PlayerMoveInit;
	g_pClient->V_CalcRefdef = V_CalcRefdef;
	g_pClient->HUD_PostRunCmd = HUD_PostRunCmd;
	g_pClient->HUD_Key_Event = HUD_Key_Event;
	g_pClient->HUD_ProcessPlayerState = HUD_ProcessPlayerState;
	g_pClient->HUD_GetHullBounds = HUD_GetHullBounds;
	g_pClient->CL_IsThirdPerson = CL_IsThirdPerson;
	g_pStudio->StudioSetRemapColors = StudioSetRemapColors;
	g_pStudio->StudioCheckBBox = StudioCheckBBox;

	c_Offset.EnablePageWrite((DWORD)g_pStudioModelRenderer, sizeof(StudioModelRenderer_t));
	g_pStudioModelRenderer->StudioRenderModel = StudioRenderModel_Gate;
	g_pStudioModelRenderer->StudioRenderFinal_Hardware = StudioRenderFinal_Hardware_Gate;
	c_Offset.RestorePageProtection((DWORD)g_pStudioModelRenderer, sizeof(StudioModelRenderer_t));
}
