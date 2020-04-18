#include "client.h"

PlayerInfoLocal g_Local;
PlayerInfo g_Player[33];
player_extra_info_t g_PlayerExtraInfoList[33];

bool IsLocalAlive(cl_entity_s* pLocal)
{
	if ((pLocal->curstate.iuser1 == OBS_NONE) && 
		(g_Local.iTeam == 1 || g_Local.iTeam == 2) && 
		(g_Local.vView_ofs[2] != PM_DEAD_VIEWHEIGHT) && 
		(g_Local.iHealth >= 1))
		return true;
	else
		return false;
}

void UpdateLocalPlayer()
{
	if (g_pEngine->GetMaxClients())
	{
		cl_entity_s* pLocal = g_Engine.GetLocalPlayer();

		g_Local.pEnt = g_pEngine->GetMaxClients() && pLocal ? pLocal : NULL;
		g_Local.iIndex = g_Local.pEnt ? g_Local.pEnt->index : NULL;
		g_Local.bAlive = g_Local.pEnt ? IsLocalAlive(g_Local.pEnt) : NULL;
		g_Local.flEdgeDistance = g_Local.pEnt ? EdgeDistance() : NULL;
		g_Local.flDamage = g_Local.pEnt ? Damage() : NULL;

		if (g_Local.pEnt)
		{
			g_Local.vVelocity = pmove->velocity;
			g_Local.flFallVelocity = pmove->flFallVelocity;
			g_Local.flVelocityspeed = sqrt(POW(pmove->velocity[0]) + POW(pmove->velocity[1]));
			g_Local.flVelocity = pmove->velocity.Length();
			g_Local.iUsehull = pmove->usehull;
			g_Local.flVelocity2D = pmove->velocity.Length2D();
			g_Local.vOrigin = pmove->origin;
			g_Local.iMovetype = pmove->movetype;
			g_Local.vView_ofs = pmove->view_ofs;
			g_Local.vEye = pmove->origin + pmove->view_ofs;
			g_Local.iFlags = pmove->flags;
			g_Local.flWaterlevel = pmove->waterlevel;

			//Get Distance to Ground
			{
				Vector vTemp1 = g_Local.vOrigin;
				vTemp1[2] -= 8192;

				pmtrace_t* trace = g_Engine.PM_TraceLine(g_Local.vOrigin, vTemp1, 1, (g_Local.iFlags & FL_DUCKING) ? 1 : 0, -1);

				g_Local.flHeightorigin = abs(trace->endpos.z - g_Local.vOrigin.z);
				g_Local.flGroundangle = acos(trace->plane.normal[2]) / M_PI * 180;

				//Get Distance to Player under me
				{
					pmtrace_t pTrace;

					g_pEngine->pEventAPI->EV_SetTraceHull((g_Local.iFlags & FL_DUCKING) ? 1 : 0);
					g_pEngine->pEventAPI->EV_PlayerTrace(g_Local.vOrigin, trace->endpos, PM_GLASS_IGNORE | PM_STUDIO_BOX, pLocal->index, &pTrace);

					if (pTrace.fraction < 1.0f)
					{
						g_Local.flHeightorigin = abs(pTrace.endpos.z - g_Local.vOrigin.z);

						int i = g_pEngine->pEventAPI->EV_IndexFromTrace(&pTrace);
						if (i > 0 && i < 33)
						{
							float dst = g_Local.vOrigin.z - (((g_Local.iFlags & FL_DUCKING) ? 1 : 0) == 0 ? 32 : 18) - g_Player[i].vOrigin.z - g_Local.flHeightorigin;
							if (dst < 30)
							{
								g_Local.flHeightorigin -= 14.0;
							}
						}
					}
				}
			}
		}
	}
}

bool IsPlayerAlive(int i)
{
	if (g_pEngine->GetMaxClients())
	if (g_Player[i].pEnt &&
		g_Player[i].pEnt->player &&
		g_Player[i].pEnt->index != g_Local.iIndex &&
		(g_Player[i].pEnt->index < 33 && g_Player[i].pEnt->index >= 0) &&
		(g_Player[i].iTeam == 1 || g_Player[i].iTeam == 2) &&
		!g_Player[i].pEnt->curstate.mins.IsZero() &&
		!g_Player[i].pEnt->curstate.maxs.IsZero() &&
		!(Cstrike_SequenceInfo[g_Player[i].pEnt->curstate.sequence] == SEQUENCE_DIE) &&
		(g_Player[i].pEnt->model->name != 0 || g_Player[i].pEnt->model->name != "") &&
		g_Player[i].bAliveInScoreTab &&
		(g_Local.pEnt) &&
		!(g_Player[i].pEnt->curstate.messagenum < g_Local.pEnt->curstate.messagenum) &&
		!(g_Local.pEnt->curstate.iuser1 == OBS_IN_EYE && g_Local.pEnt->curstate.iuser2 == g_Player[i].pEnt->index))
		return true;
	else
		return false;
}

void GetPlayerInfo(int i)
{
	g_Engine.pfnGetPlayerInfo(i, &g_Player[i].entinfo);
	if (!g_Player[i].entinfo.name) { g_Player[i].entinfo.name = "\\missing-name\\"; }
	if (!g_Player[i].entinfo.model) { g_Player[i].entinfo.model = "unknown model"; }
}

void UpdatePlayer()
{
	if(g_pEngine->GetMaxClients())
	for (unsigned int i = 0; i < 33; ++i)
	{
		if (i != g_Local.iIndex)
		{
			cl_entity_s* ent = g_Engine.GetEntityByIndex(i);

			GetPlayerInfo(i);

			g_Player[i].pEnt = ent;
			g_Player[i].bAlive = g_Player[i].pEnt ? IsPlayerAlive(i) : NULL;
			g_Player[i].vVelocity = g_Player[i].pEnt ? (g_Player[i].pEnt->curstate.origin - g_Player[i].pEnt->prevstate.origin) : NULL;
			g_Player[i].flDist = g_Player[i].pEnt ? (g_Player[i].pEnt->curstate.origin.Distance(g_Local.vOrigin)) : NULL;
			g_Player[i].vOrigin = g_Player[i].pEnt ? (g_Player[i].pEnt->curstate.origin) : NULL;
			g_Player[i].bDucked = g_Player[i].pEnt ? (ent->curstate.usehull? true : false) : NULL;
			g_Player[i].flFrametime = g_Player[i].pEnt ? (g_Player[i].pEnt->curstate.animtime - g_Player[i].pEnt->prevstate.animtime) : NULL;
			
			if (g_Player[i].flFrametime == NULL)
				g_Player[i].flFrametime = g_Local.flFrametime;
		}
	}
}

void ThirdPerson(struct ref_params_s* pparams)
{
	if (cvar.visual_chase_cam && g_Local.bAlive && CheckDrawEngine())
	{
		Vector Offset(0, 0, 0);
		Vector r, u, b;
		VectorMul(pparams->right, 0, r);
		VectorMul(pparams->up, cvar.visual_chase_up, u);
		VectorMul(pparams->forward, -cvar.visual_chase_back, b);
		Offset = Offset + r;
		Offset = Offset + u;
		Offset = Offset + b;
		pparams->vieworg[0] += Offset[0];
		pparams->vieworg[1] += Offset[1];
		pparams->vieworg[2] += Offset[2];
	}
}

void SetAntiAimAngles(cl_entity_s* ent)
{
	if (!IsCurWeaponNonAttack())
	{
		int id = NULL;
		float flDist = 8192.f;

		for (unsigned int i = 0; i < 33; i++)
		{
			if (i == g_Local.iIndex)
				continue;

			if (!g_Player[id].bAlive)
				continue;

			if (!cvar.rage_team && g_Player[i].iTeam == g_Local.iTeam)
				continue;

			if (g_Player[i].flDist < flDist || id == NULL)
			{
				flDist = g_Player[i].flDist;
				id = i;
			}
		}

		Vector vAngles = g_Local.vViewAngles;

		if (id > 0)
			VectorAngles(g_Player[id].pEnt->origin - g_Local.vEye, vAngles);

		//Yaw
		if (g_Local.flVelocity2D > 0)
		{
			if (cvar.aa_yaw_while_running > 0)
			{
				if (cvar.aa_yaw_while_running == 1)
				{//180
					ent->angles[1] = vAngles[1] + 180;
				}
				else if (cvar.aa_yaw_while_running == 2)
				{//180 Jitter
					static bool jitter = false;

					if (jitter)
						ent->angles[1] = vAngles[1] + 180;
					else
						ent->angles[1] = vAngles[1];

					jitter = !jitter;
				}
				else if (cvar.aa_yaw_while_running == 3)
				{//Spin
					int spin = 30;
					ent->angles[1] = fmod(g_Engine.GetClientTime() * spin * 360.0f, 360.0f);
				}
				else if (cvar.aa_yaw_while_running == 4)
				{//Jitter
					static unsigned int m_side = 0;

					if (m_side == 0)
						ent->angles[1] = vAngles[1] + 0;
					else if (m_side == 1)
						ent->angles[1] = vAngles[1] + 90;
					else if (m_side == 2)
						ent->angles[1] = vAngles[1] + 180;
					else if (m_side >= 3) {
						ent->angles[1] = vAngles[1] + -90;
						m_side = 0;
					}

					m_side++;
				}
				else if (cvar.aa_yaw_while_running == 5) {//Sideway
					static bool jitter = false;

					if (jitter)
						ent->angles[1] = vAngles[1] + 90;
					else
						ent->angles[1] = vAngles[1] - 90;

					jitter = !jitter;
				}
				else if (cvar.aa_yaw_while_running == 6) {//Random
					ent->angles[1] = rand() % 361;
					if (ent->angles[1] > 180)
						ent->angles[1] -= 360;
				}
				else if (cvar.aa_yaw_while_running == 7) {//Static
					ent->angles[1] = vAngles[1] + cvar.aa_yaw_static;
				}
			}

			float angle = ent->angles[1];

			if ((cvar.aa_edge == 2 || cvar.aa_edge == 3) && FakeEdge(angle))
				ent->angles[1] = angle;
		}
		else
		{
			if (cvar.aa_yaw > 0)
			{
				if (cvar.aa_yaw == 1)
				{//180
					ent->angles[1] = vAngles[1] + 180;
				}
				else if (cvar.aa_yaw == 2)
				{//180 Jitter
					static bool jitter = false;

					if (jitter)
						ent->angles[1] = vAngles[1] + 180;
					else
						ent->angles[1] = vAngles[1];

					jitter = !jitter;
				}
				else if (cvar.aa_yaw == 3)
				{//Spin
					int spin = 30;
					ent->angles[1] = fmod(g_Engine.GetClientTime() * spin * 360.0f, 360.0f);
				}
				else if (cvar.aa_yaw == 4)
				{//Jitter
					static unsigned int m_side = 0;

					if (m_side == 0)
						ent->angles[1] = vAngles[1] + 0;
					else if (m_side == 1)
						ent->angles[1] = vAngles[1] + 90;
					else if (m_side == 2)
						ent->angles[1] = vAngles[1] + 180;
					else if (m_side >= 3) {
						ent->angles[1] = vAngles[1] + -90;
						m_side = 0;
					}

					m_side++;
				}
				else if (cvar.aa_yaw == 5)
				{//Sideway
					static bool jitter = false;

					if (jitter)
						ent->angles[1] = vAngles[1] + 90;
					else
						ent->angles[1] = vAngles[1] - 90;

					jitter = !jitter;
				}
				else if (cvar.aa_yaw == 6)
				{//Random
					ent->angles[1] = rand() % 361;
					if (ent->angles[1] > 180)
						ent->angles[1] -= 360;
				}
				else if (cvar.aa_yaw == 7)
				{//Static
					ent->angles[1] = vAngles[1] + cvar.aa_yaw_static;
				}
			}

			float angle = ent->angles[1];

			if ((cvar.aa_edge == 1 || cvar.aa_edge == 3 || cvar.aa_edge == 4) && FakeEdge(angle))
			{
				static float timer = g_Local.weapon.curtime;

				if (g_Local.weapon.curtime - timer > 3 && cvar.aa_edge == 4)
				{
					timer = g_Local.weapon.curtime;

					ent->angles[1] = angle + 180;
				}
				else {
					ent->angles[1] = angle;
				}
			}
		}
		//Roll
		if (g_Local.flVelocity2D > 0)
		{
			if (cvar.aa_roll_while_running > 0)
			{
				if (cvar.aa_roll_while_running == 1)
				{//180
					ent->angles[2] = 180;
				}
				else if (cvar.aa_roll_while_running == 2)
				{//180 Jitter
					static bool jitter = false;

					if (jitter)
						ent->angles[2] = 180;

					jitter = !jitter;
				}
				else if (cvar.aa_roll_while_running == 3)
				{//Spin
					int spin = 30;
					ent->angles[2] = fmod(g_Engine.GetClientTime() * spin * 360.0f, 360.0f);
				}
				else if (cvar.aa_roll_while_running == 4)
				{//Jitter
					static unsigned int m_side = 0;

					if (m_side == 1)
						ent->angles[2] = 90;
					else if (m_side == 2)
						ent->angles[2] = 180;
					else if (m_side >= 3) {
						ent->angles[2] = -90;
						m_side = 0;
					}

					m_side++;
				}
				else if (cvar.aa_roll_while_running == 5) {//Sideway
					static bool jitter = false;

					if (jitter)
						ent->angles[2] = 90;
					else
						ent->angles[2] = -90;

					jitter = !jitter;
				}
				else if (cvar.aa_roll_while_running == 6) {//Random
					ent->angles[2] = rand() % 361;
					if (ent->angles[2] > 180)
						ent->angles[2] -= 360;
				}
				else if (cvar.aa_roll_while_running == 7) {//Static
					ent->angles[2] = cvar.aa_yaw_static;
				}
			}

			float angle = ent->angles[2];

			if ((cvar.aa_edge == 2 || cvar.aa_edge == 3) && FakeEdge(angle))
				ent->angles[2] = angle;
		}
		else
		{
			if (cvar.aa_roll > 0)
			{
				if (cvar.aa_roll == 1)
				{//180
					ent->angles[2] = 180;
				}
				else if (cvar.aa_roll == 2)
				{//180 Jitter
					static bool jitter = false;

					if (jitter)
						ent->angles[2] = 180;

					jitter = !jitter;
				}
				else if (cvar.aa_roll == 3)
				{//Spin
					int spin = 30;
					ent->angles[2] = fmod(g_Engine.GetClientTime() * spin * 360.0f, 360.0f);
				}
				else if (cvar.aa_roll == 4)
				{//Jitter
					static unsigned int m_side = 0;

					if (m_side == 1)
						ent->angles[2] = 90;
					else if (m_side == 2)
						ent->angles[2] = 180;
					else if (m_side >= 3) {
						ent->angles[2] = -90;
						m_side = 0;
					}

					m_side++;
				}
				else if (cvar.aa_roll == 5)
				{//Sideway
					static bool jitter = false;

					if (jitter)
						ent->angles[2] = 90;
					else
						ent->angles[2] = -90;

					jitter = !jitter;
				}
				else if (cvar.aa_roll == 6)
				{//Random
					ent->angles[2] = rand() % 361;
					if (ent->angles[2] > 180)
						ent->angles[2] -= 360;
				}
				else if (cvar.aa_roll == 7)
				{//Static
					ent->angles[2] = cvar.aa_roll_static;
				}
			}

			float angle = ent->angles[2];

			if ((cvar.aa_edge == 1 || cvar.aa_edge == 3 || cvar.aa_edge == 4) && FakeEdge(angle))
			{
				static float timer = g_Local.weapon.curtime;

				if (g_Local.weapon.curtime - timer > 3 && cvar.aa_edge == 4)
				{
					timer = g_Local.weapon.curtime;

					ent->angles[2] = angle + 180;
				}
				else
				{
					ent->angles[2] = angle;
				}
			}
		}
		//Pitch
		if (cvar.aa_pitch > 0)
		{
			if (cvar.aa_pitch == 1)
			{//Fakedown
				ent->angles[0] = -180;
			}
			if (cvar.aa_pitch == 2)
			{//Random
				ent->angles[0] = rand() % 361;
				if (ent->angles[0] > 180)
					ent->angles[0] -= 360;
			}
		}
	}
}

char* GetWeaponName(int weaponmodel)
{
	static char weapon[50];
	weapon[0] = 0;

	model_s* mdl = g_pStudio->GetModelByIndex(weaponmodel);
	if (!mdl) { return weapon; }

	char* name = mdl->name;  if (!name) { return weapon; }
	int len = strlen(name);  if (len > 48 || len < 10) { return weapon; }

	strcpy(weapon, name + 9); len -= 9;
	if (len > 4)weapon[len - 4] = (char)0;

	return weapon;
}

unsigned int Cstrike_SequenceInfo[] =
{
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0, // 0..9   
	0,	1,	2,	0,	1,	2,	0,	1,	2,	0, // 10..19 
	1,	2,	0,	1,	1,	2,	0,	1,	1,	2, // 20..29 
	0,	1,	2,	0,	1,	2,	0,	1,	2,	0, // 30..39 
	1,	2,	0,	1,	2,	0,	1,	2,	0,	1, // 40..49 
	2,	0,	1,	2,	0,	0,	0,	8,	0,	8, // 50..59 
	0, 16,	0, 16,	0,	0,	1,	1,	2,	0, // 60..69 
	1,	1,	2,	0,	1,	0,	1,	0,	1,	2, // 70..79 
	0,	1,	2, 	32, 40, 32, 40, 32, 32, 32, // 80..89
	33, 64, 33, 34, 64, 65, 34, 32, 32, 4, // 90..99
	4,	4,	4,	4,	4,	4,	4,	4,	4,	4, // 100..109
	4                                      	// 110
};

void RunHLCommands()
{
	static bool run = true;
	if (run)
	{
		g_Engine.pfnClientCmd("alias get_b67 \"primammo;secammo;vesthelm;sgren;hegren;flash\"");
		g_Engine.pfnClientCmd("alias +rrr \"+reload;get_b67\"");
		g_Engine.pfnClientCmd("alias -rrr \"-reload\"");
		g_Engine.pfnClientCmd("bind r \"+rrr\"");
		g_Engine.pfnClientCmd("hud_fastswitch 1");
		g_Engine.pfnClientCmd("cl_showfps 1");
		g_Engine.pfnClientCmd("unbind ins");
		g_Engine.pfnClientCmd("unbind del");
		g_Engine.pfnClientCmd("unbind alt");
		g_Engine.pfnClientCmd("unbind f12");
		g_Engine.pfnClientCmd("rate 999999");
		g_Engine.pfnClientCmd("cl_updaterate 1000");
		g_Engine.pfnClientCmd("cl_cmdrate 1000");
		g_Engine.pfnClientCmd("cl_rate 9999");
		g_Engine.pfnClientCmd("ex_interp 0.1");
		run = false;
	}
}

bool CheckDrawEngine()
{
	if (DrawVisuals && (!cvar.route_auto || cvar.route_draw_visual) && GetTickCount() - HudRedraw <= 100)
		return true;
	else
		return false;
}

bool CheckDraw()
{
	if ((!cvar.route_auto || cvar.route_draw_visual) && GetTickCount() - HudRedraw <= 100)
		return true;
	else
		return false;
}

void pWeapon(cl_entity_t* ent, model_t* pmodel, int modelindex, bool rotate, float test1, float test2, float test3, float test4, float test5)
{
	Vector vAngles, vF, vR, vU;
	static cl_entity_t mymodel[33];
	mymodel[ent->index].model = pmodel;
	mymodel[ent->index].curstate = ent->curstate;
	mymodel[ent->index].curstate.modelindex = modelindex;
	if (rotate)mymodel[ent->index].angles[1] = fmod(g_Engine.GetClientTime() * 0.5 * 360.0f, 360.0f);
	mymodel[ent->index].angles[2] = test1;
	mymodel[ent->index].angles[3] = test2;
	vAngles = Vector(0.0f, mymodel[ent->index].angles[1], 0.0f);
	g_Engine.pfnAngleVectors(vAngles, vF, vR, vU);
	mymodel[ent->index].origin = ent->origin + vF * test3 + vR * test4 + vU * test5;
	g_Engine.CL_CreateVisibleEntity(ET_NORMAL, &mymodel[ent->index]);
}

void wWeapon(cl_entity_t* ent, model_t* pmodel, int modelindex, bool rotate, float test1, float test2, float test3)
{
	static cl_entity_t mymodel[33];
	mymodel[ent->index].model = pmodel;
	mymodel[ent->index].curstate = ent->curstate;
	mymodel[ent->index].curstate.modelindex = modelindex;
	if (rotate)mymodel[ent->index].angles[1] = fmod(g_Engine.GetClientTime() * 0.5 * 360.0f, 360.0f);
	mymodel[ent->index].angles[2] = test1;
	mymodel[ent->index].angles[3] = test2;
	mymodel[ent->index].origin = ent->origin;
	mymodel[ent->index].origin[2] = ent->origin[2] + test3;
	g_Engine.CL_CreateVisibleEntity(ET_NORMAL, &mymodel[ent->index]);
}

void PlayerWeapon(cl_entity_s* ent)
{
	model_t* pweaponmodel = g_Studio.GetModelByIndex(ent->curstate.weaponmodel);
	if (pweaponmodel)
	{
		char filename[256];
		if (strstr(pweaponmodel->name, "p_shield_deagle.mdl")) sprintf(filename, "models/w_deagle.mdl");
		else if (strstr(pweaponmodel->name, "p_shield_fiveseven.mdl")) sprintf(filename, "models/w_fiveseven.mdl");
		else if (strstr(pweaponmodel->name, "p_shield_flashbang.mdl")) sprintf(filename, "models/w_flashbang.mdl");
		else if (strstr(pweaponmodel->name, "p_shield_glock18.mdl")) sprintf(filename, "models/w_glock18.mdl");
		else if (strstr(pweaponmodel->name, "p_shield_hegrenade.mdl")) sprintf(filename, "models/w_hegrenade.mdl");
		else if (strstr(pweaponmodel->name, "p_shield_knife.mdl")) sprintf(filename, "models/p_knife.mdl");
		else if (strstr(pweaponmodel->name, "p_shield_p228.mdl")) sprintf(filename, "models/w_p228.mdl");
		else if (strstr(pweaponmodel->name, "p_shield_smokegrenade.mdl")) sprintf(filename, "models/w_smokegrenade.mdl");
		else if (strstr(pweaponmodel->name, "p_shield_usp.mdl")) sprintf(filename, "models/w_usp.mdl");
		else if (strstr(pweaponmodel->name, "p_knife.mdl")) sprintf(filename, "models/p_knife.mdl");
		else
		{
			sprintf(filename, "%s", pweaponmodel->name);
			filename[7] = 'w';
		}

		int modelindex;
		model_t* pmodel = g_Engine.CL_LoadModel(filename, &modelindex);
		if (pmodel)
		{
			int height;
			if (ent->curstate.usehull) height = 18;
			else height = 0;

			if (strstr(pmodel->name, "w_ak47.mdl"))wWeapon(ent, pmodel, modelindex, true, 271, 282, 50 + height);
			if (strstr(pmodel->name, "w_aug.mdl"))wWeapon(ent, pmodel, modelindex, true, 271, 298, 50 + height);
			if (strstr(pmodel->name, "w_awp.mdl"))wWeapon(ent, pmodel, modelindex, true, 271, 286, 50 + height);
			if (strstr(pmodel->name, "w_c4.mdl"))wWeapon(ent, pmodel, modelindex, true, -180, 180, 44 + height);
			if (strstr(pmodel->name, "w_deagle.mdl"))wWeapon(ent, pmodel, modelindex, true, 271, 286, 46 + height);
			if (strstr(pmodel->name, "w_elite.mdl"))wWeapon(ent, pmodel, modelindex, true, 271, 286, 46 + height);
			if (strstr(pmodel->name, "w_famas.mdl"))wWeapon(ent, pmodel, modelindex, true, 271, 207, 50 + height);
			if (strstr(pmodel->name, "w_fiveseven.mdl"))wWeapon(ent, pmodel, modelindex, true, 271, 231, 46 + height);
			if (strstr(pmodel->name, "w_flashbang.mdl"))pWeapon(ent, pmodel, modelindex, true, 0, 0, -6, 0, 44 + height);
			if (strstr(pmodel->name, "w_g3sg1.mdl"))wWeapon(ent, pmodel, modelindex, true, 271, 289, 50 + height);
			if (strstr(pmodel->name, "w_galil.mdl"))wWeapon(ent, pmodel, modelindex, true, 271, 164, 50 + height);
			if (strstr(pmodel->name, "w_glock18.mdl"))wWeapon(ent, pmodel, modelindex, true, 271, 289, 46 + height);
			if (strstr(pmodel->name, "w_hegrenade.mdl"))pWeapon(ent, pmodel, modelindex, true, 0, 0, -6, 0, 44 + height);
			if (strstr(pmodel->name, "p_knife.mdl"))pWeapon(ent, pmodel, modelindex, true, 0, -25, -30, -6, 17 + height);
			if (strstr(pmodel->name, "w_m3.mdl"))wWeapon(ent, pmodel, modelindex, true, 271, 302, 50 + height);
			if (strstr(pmodel->name, "w_m4a1.mdl"))wWeapon(ent, pmodel, modelindex, true, 271, 236, 50 + height);
			if (strstr(pmodel->name, "w_m249.mdl"))wWeapon(ent, pmodel, modelindex, true, 20, -100, 50 + height);
			if (strstr(pmodel->name, "w_mac10.mdl"))wWeapon(ent, pmodel, modelindex, true, 271, 90, 50 + height);
			if (strstr(pmodel->name, "w_mp5.mdl"))wWeapon(ent, pmodel, modelindex, true, 271, -10, 50 + height);
			if (strstr(pmodel->name, "w_p90.mdl"))wWeapon(ent, pmodel, modelindex, true, 271, 302, 50 + height);
			if (strstr(pmodel->name, "w_p228.mdl"))wWeapon(ent, pmodel, modelindex, true, 271, 236, 46 + height);
			if (strstr(pmodel->name, "w_scout.mdl"))wWeapon(ent, pmodel, modelindex, true, 271, 247, 50 + height);
			if (strstr(pmodel->name, "w_sg550.mdl"))wWeapon(ent, pmodel, modelindex, true, 271, 295, 50 + height);
			if (strstr(pmodel->name, "w_sg552.mdl"))wWeapon(ent, pmodel, modelindex, true, 271, 309, 50 + height);
			if (strstr(pmodel->name, "w_smokegrenade.mdl"))pWeapon(ent, pmodel, modelindex, true, 0, 0, -6, 0, 44 + height);
			if (strstr(pmodel->name, "w_tmp.mdl"))wWeapon(ent, pmodel, modelindex, true, 271, 314, 50 + height);
			if (strstr(pmodel->name, "w_ump45.mdl"))wWeapon(ent, pmodel, modelindex, true, 271, 247, 50 + height);
			if (strstr(pmodel->name, "w_usp.mdl"))wWeapon(ent, pmodel, modelindex, true, 271, 243, 46 + height);
			if (strstr(pmodel->name, "w_xm1014.mdl"))wWeapon(ent, pmodel, modelindex, true, 271, 8, 50 + height);
		}
	}
}

bool bPathFree(float* pflFrom, float* pflTo)
{
	pmtrace_t pTrace;
	g_Engine.pEventAPI->EV_SetTraceHull(2);
	g_Engine.pEventAPI->EV_PlayerTrace(pflFrom, pflTo, PM_GLASS_IGNORE | PM_STUDIO_BOX, g_Local.iIndex, &pTrace);
	return (bool)(pTrace.fraction >= 1.0f);
}