#include "client.h"

void ClearIndex(struct ref_params_s* pparams)
{
	if (pparams->nextView == 0)
	{
		for (unsigned int i = 0; i < 33; i++)
		{
			g_Player[i].bGotPlayer = false;
		}
	}
}

void VectorTransform(Vector in1, float in2[3][4], float* out)
{
	out[0] = in1.Dot(in2[0]) + in2[0][3];
	out[1] = in1.Dot(in2[1]) + in2[1][3];
	out[2] = in1.Dot(in2[2]) + in2[2][3];
}

void ReplaceTextureIndex(char* texturename, int textureindex, char* modelname, int modeltype, float cvar)
{
	char filename[1024];
	static int RestoreIndex[1024];
	if (modeltype == 1)sprintf(filename, "models/player/%s/%s.mdl", modelname, modelname);
	if (modeltype == 2)sprintf(filename, "models/shield/%s.mdl", modelname);
	if (modeltype == 3)sprintf(filename, "models/%s.mdl", modelname);
	float checkdraw = cvar && CheckDrawEngine();
	if (!RestoreIndex[textureindex] && checkdraw || !checkdraw && RestoreIndex[textureindex])
	{
		model_t* pModel = g_Engine.CL_LoadModel(filename, NULL);
		if (pModel)
		{
			studiohdr_t* pStudioHeader = (studiohdr_t*)g_Studio.Mod_Extradata(pModel);
			mstudiotexture_t* ptexture = (mstudiotexture_t*)((byte*)pStudioHeader + pStudioHeader->textureindex);
			for (int i = 0; i < pStudioHeader->numtextures; i++)
			{
				if (strstr(ptexture[i].name, texturename))
				{
					if (checkdraw)
					{
						RestoreIndex[textureindex] = ptexture[i].index;
						ptexture[i].index = texture_id[textureindex];
					}
					else
					{
						ptexture[i].index = RestoreIndex[textureindex];
						RestoreIndex[textureindex] = 0;
					}
				}
			}
		}
	}
}

void ScanForTextureIndex(cl_entity_s* ent, char* texturename, int* array, int index)
{
	if (ent && ent->model)
	{
		studiohdr_t* pStudioHeader = (studiohdr_t*)g_Studio.Mod_Extradata(ent->model);
		mstudiotexture_t* ptexture = (mstudiotexture_t*)((byte*)pStudioHeader + pStudioHeader->textureindex);

		for (int i = 0; i < pStudioHeader->numtextures; i++)
		{
			if (strstr(ptexture[i].name, texturename))
				array[index] = ptexture[i].index;
		}
	}
}

void SetRemapColors(struct cl_entity_s* ent)
{
	if (ent && ent->model && ent->model->name && CheckDrawEngine())
	{
		studiohdr_t* pStudioHeader = (studiohdr_t*)g_Studio.Mod_Extradata(ent->model);
		mstudiobbox_t* pHitbox = (mstudiobbox_t*)((byte*)pStudioHeader + pStudioHeader->hitboxindex);
		mstudiobone_t* pbones = (mstudiobone_t*)((byte*)pStudioHeader + pStudioHeader->boneindex);
		BoneMatrix_t* pBoneMatrix = (BoneMatrix_t*)g_Studio.StudioGetBoneTransform();
		
		if (cvar.skeleton_world_bone && !strstr(ent->model->name, "player") && !strstr(ent->model->name, "v_") || cvar.skeleton_view_model_bone && strstr(ent->model->name, "v_"))
		{
			for (int i = 0; i < pStudioHeader->numbones; i++)
			{
				glDisable(GL_DEPTH_TEST);
				glDisable(GL_TEXTURE_2D);
				if (pbones[i].parent >= 0)
				{
					glLineWidth(1);
					glColor4f(cvar.color_blue, cvar.color_red, cvar.color_green, cvar.visual_alpha);
					glBegin(GL_LINES);
					glVertex3f((*pBoneMatrix)[pbones[i].parent][0][3], (*pBoneMatrix)[pbones[i].parent][1][3], (*pBoneMatrix)[pbones[i].parent][2][3]);
					glVertex3f((*pBoneMatrix)[i][0][3], (*pBoneMatrix)[i][1][3], (*pBoneMatrix)[i][2][3]);
					glEnd();

					glPointSize(3);
					glColor4f(cvar.color_red, cvar.color_green, cvar.color_blue, cvar.visual_alpha);
					glBegin(GL_POINTS);
					if (pbones[pbones[i].parent].parent != -1)
						glVertex3f((*pBoneMatrix)[pbones[i].parent][0][3], (*pBoneMatrix)[pbones[i].parent][1][3], (*pBoneMatrix)[pbones[i].parent][2][3]);
					glVertex3f((*pBoneMatrix)[i][0][3], (*pBoneMatrix)[i][1][3], (*pBoneMatrix)[i][2][3]);
					glEnd();
				}
				else
				{
					glPointSize(3);
					glColor4f(cvar.color_red, cvar.color_green, cvar.color_blue, cvar.visual_alpha);
					glBegin(GL_POINTS);
					glVertex3f((*pBoneMatrix)[i][0][3], (*pBoneMatrix)[i][1][3], (*pBoneMatrix)[i][2][3]);
					glEnd();
				}
				glEnable(GL_DEPTH_TEST);
				glEnable(GL_TEXTURE_2D);
			}
		}
		if (cvar.skeleton_world_hitbox && !strstr(ent->model->name, "player") && !strstr(ent->model->name, "v_") || cvar.skeleton_view_model_hitbox && strstr(ent->model->name, "v_"))
		{
			int index = 0;
			if (strstr(ent->model->name, "v_knife.mdl") || strstr(ent->model->name, "v_usp.mdl"))
				index = pStudioHeader->numhitboxes - 1;
			else
				index = pStudioHeader->numhitboxes;

			for (int i = 0; i < index; i++)
			{
				Vector vCubePointsTrans[8], vCubePoints[8];

				vCubePoints[0] = Vector(pHitbox[i].bbmin.x, pHitbox[i].bbmin.y, pHitbox[i].bbmin.z);
				vCubePoints[1] = Vector(pHitbox[i].bbmin.x, pHitbox[i].bbmax.y, pHitbox[i].bbmin.z);
				vCubePoints[2] = Vector(pHitbox[i].bbmax.x, pHitbox[i].bbmax.y, pHitbox[i].bbmin.z);
				vCubePoints[3] = Vector(pHitbox[i].bbmax.x, pHitbox[i].bbmin.y, pHitbox[i].bbmin.z);
				vCubePoints[4] = Vector(pHitbox[i].bbmax.x, pHitbox[i].bbmax.y, pHitbox[i].bbmax.z);
				vCubePoints[5] = Vector(pHitbox[i].bbmin.x, pHitbox[i].bbmax.y, pHitbox[i].bbmax.z);
				vCubePoints[6] = Vector(pHitbox[i].bbmin.x, pHitbox[i].bbmin.y, pHitbox[i].bbmax.z);
				vCubePoints[7] = Vector(pHitbox[i].bbmax.x, pHitbox[i].bbmin.y, pHitbox[i].bbmax.z);

				for (unsigned int x = 0; x < 8; x++)
					VectorTransform(vCubePoints[x], (*pBoneMatrix)[pHitbox[i].bone], vCubePointsTrans[x]);

				glDisable(GL_TEXTURE_2D);
				glLineWidth(1);
				glColor4f(cvar.color_blue, cvar.color_red, cvar.color_green, cvar.visual_alpha);
				for (unsigned int x = 0; x < 12; x++)
				{
					glBegin(GL_LINES);
					glVertex3fv(vCubePointsTrans[SkeletonHitboxMatrix[x][0]]);
					glVertex3fv(vCubePointsTrans[SkeletonHitboxMatrix[x][1]]);
					glEnd();
				}
				glPointSize(3);
				glColor4f(cvar.color_red, cvar.color_green, cvar.color_blue, cvar.visual_alpha);
				glBegin(GL_POINTS);
				for (unsigned int x = 0; x < 8; x++)
					glVertex3fv(vCubePointsTrans[x]);
				glEnd();
				glEnable(GL_TEXTURE_2D);
			}
		}
	}

	if (ent && ent->player)
	{
		model_t* pModel = NULL;
		char filename[1024];
		sprintf(filename, "models/player/%s/%s.mdl", g_Player[ent->index].entinfo.model, g_Player[ent->index].entinfo.model);
		if (g_pEngine->pfnGetCvarPointer("cl_minmodels")->value == 0)
			pModel = g_Engine.CL_LoadModel(filename, NULL);
		else
		{
			if (g_Player[ent->index].vip && g_Player[ent->index].iTeam == 2)
				pModel = g_Engine.CL_LoadModel("models/player/vip/vip.mdl", NULL);
			else
			{
				if (g_Player[ent->index].iTeam == 1)
					pModel = g_Engine.CL_LoadModel("models/player/leet/leet.mdl", NULL);
				if (g_Player[ent->index].iTeam == 2)
					pModel = g_Engine.CL_LoadModel("models/player/gign/gign.mdl", NULL);
			}
		}

		if (pModel)
		{
			studiohdr_t* pStudioHeader = (studiohdr_t*)g_Studio.Mod_Extradata(pModel);
			mstudiobbox_t* pHitbox = (mstudiobbox_t*)((byte*)pStudioHeader + pStudioHeader->hitboxindex);
			mstudiobone_t* pbones = (mstudiobone_t*)((byte*)pStudioHeader + pStudioHeader->boneindex);
			BoneMatrix_t* pBoneMatrix = (BoneMatrix_t*)g_Studio.StudioGetBoneTransform();
			float r, g, b;
			if (g_Player[ent->index].iTeam == 1) r = 1, g = 0, b = 0;
			else if (g_Player[ent->index].iTeam == 2) r = 0, g = 0, b = 1;
			else r = 1, g = 1, b = 1;
			if (cvar.skeleton_player_bone && (g_Player[ent->index].iTeam != g_Local.iTeam || cvar.visual_visual_team) && CheckDrawEngine())
			{
				for (int i = 0; i < pStudioHeader->numbones; i++)
				{
					glDisable(GL_DEPTH_TEST);
					glDisable(GL_TEXTURE_2D); 
					if (pbones[i].parent >= 0)
					{
						glLineWidth(1);
						glColor4f(r, g, b,cvar.visual_alpha);
						glBegin(GL_LINES);
						glVertex3f((*pBoneMatrix)[pbones[i].parent][0][3], (*pBoneMatrix)[pbones[i].parent][1][3], (*pBoneMatrix)[pbones[i].parent][2][3]);
						glVertex3f((*pBoneMatrix)[i][0][3], (*pBoneMatrix)[i][1][3], (*pBoneMatrix)[i][2][3]);
						glEnd();
					}
					glEnable(GL_DEPTH_TEST);
					glEnable(GL_TEXTURE_2D);
				}
			}
			if (cvar.skeleton_player_hitbox && (g_Player[ent->index].iTeam != g_Local.iTeam || cvar.visual_visual_team) && CheckDrawEngine())
			{
				for (int i = 0; i < pStudioHeader->numhitboxes; i++)
				{
					Vector vCubePointsTrans[8], vCubePoints[8];

					vCubePoints[0] = Vector(pHitbox[i].bbmin.x, pHitbox[i].bbmin.y, pHitbox[i].bbmin.z);
					vCubePoints[1] = Vector(pHitbox[i].bbmin.x, pHitbox[i].bbmax.y, pHitbox[i].bbmin.z);
					vCubePoints[2] = Vector(pHitbox[i].bbmax.x, pHitbox[i].bbmax.y, pHitbox[i].bbmin.z);
					vCubePoints[3] = Vector(pHitbox[i].bbmax.x, pHitbox[i].bbmin.y, pHitbox[i].bbmin.z);
					vCubePoints[4] = Vector(pHitbox[i].bbmax.x, pHitbox[i].bbmax.y, pHitbox[i].bbmax.z);
					vCubePoints[5] = Vector(pHitbox[i].bbmin.x, pHitbox[i].bbmax.y, pHitbox[i].bbmax.z);
					vCubePoints[6] = Vector(pHitbox[i].bbmin.x, pHitbox[i].bbmin.y, pHitbox[i].bbmax.z);
					vCubePoints[7] = Vector(pHitbox[i].bbmax.x, pHitbox[i].bbmin.y, pHitbox[i].bbmax.z);

					for (unsigned int x = 0; x < 8; x++)
						VectorTransform(vCubePoints[x], (*pBoneMatrix)[pHitbox[i].bone], vCubePointsTrans[x]);

					glDisable(GL_TEXTURE_2D);
					glLineWidth(1);
					glColor4f(r, g, b, cvar.visual_alpha);
					for (unsigned int x = 0; x < 12; x++)
					{
						glBegin(GL_LINES);
						glVertex3fv(vCubePointsTrans[SkeletonHitboxMatrix[x][0]]);
						glVertex3fv(vCubePointsTrans[SkeletonHitboxMatrix[x][1]]);
						glEnd();
					}
					glEnable(GL_TEXTURE_2D);
				}
			}
			//aimbot
			if (!g_Player[ent->index].bGotPlayer)
			{
				g_Local.iMaxHitboxes = pStudioHeader->numhitboxes - 1;

				for (int i = 0; i < pStudioHeader->numhitboxes; i++)
				{
					Vector vBBMax, vBBMin, vCubePointsTrans[8], vCubePoints[8];

					VectorTransform(pHitbox[i].bbmin, (*pBoneMatrix)[pHitbox[i].bone], vBBMin);
					VectorTransform(pHitbox[i].bbmax, (*pBoneMatrix)[pHitbox[i].bone], vBBMax);
					g_PlayerExtraInfoList[ent->index].vHitbox[i] = (vBBMax + vBBMin) * 0.5f + g_Player[ent->index].vVelocity * g_Player[ent->index].flFrametime;
					g_PlayerExtraInfoList[ent->index].fHitboxFOV[i] = g_Local.vForward.AngleBetween(g_PlayerExtraInfoList[ent->index].vHitbox[i] - g_Local.vEye);

					vCubePoints[0] = Vector(pHitbox[i].bbmin.x, pHitbox[i].bbmin.y, pHitbox[i].bbmin.z);
					vCubePoints[1] = Vector(pHitbox[i].bbmin.x, pHitbox[i].bbmax.y, pHitbox[i].bbmin.z);
					vCubePoints[2] = Vector(pHitbox[i].bbmax.x, pHitbox[i].bbmax.y, pHitbox[i].bbmin.z);
					vCubePoints[3] = Vector(pHitbox[i].bbmax.x, pHitbox[i].bbmin.y, pHitbox[i].bbmin.z);
					vCubePoints[4] = Vector(pHitbox[i].bbmax.x, pHitbox[i].bbmax.y, pHitbox[i].bbmax.z);
					vCubePoints[5] = Vector(pHitbox[i].bbmin.x, pHitbox[i].bbmax.y, pHitbox[i].bbmax.z);
					vCubePoints[6] = Vector(pHitbox[i].bbmin.x, pHitbox[i].bbmin.y, pHitbox[i].bbmax.z);
					vCubePoints[7] = Vector(pHitbox[i].bbmax.x, pHitbox[i].bbmin.y, pHitbox[i].bbmax.z);

					for (unsigned int x = 0; x < 8; x++)
					{
						VectorTransform(vCubePoints[x], (*pBoneMatrix)[pHitbox[i].bone], vCubePointsTrans[x]);
						g_PlayerExtraInfoList[ent->index].vHitboxMulti[i][x] = vCubePointsTrans[x] + g_Player[ent->index].vVelocity * g_Player[ent->index].flFrametime;
						g_PlayerExtraInfoList[ent->index].fHitboxPointsFOV[i][x] = g_Local.vForward.AngleBetween(g_PlayerExtraInfoList[ent->index].vHitboxMulti[i][x] - g_Local.vEye);
					}
				}
				g_Player[ent->index].bGotPlayer = true;
			}
		}
	}

	GetTextureModel();
	
	ScanForTextureIndex(ent, "view_glove", TexHandsIndex, 0);
	ScanForTextureIndex(ent, "view_finger", TexHandsIndex, 1);
	ScanForTextureIndex(ent, "view_skin", TexHandsIndex, 2);

	ReplaceTextureIndex("ARTIC_Working1", PLAYER1, "arctic", 1, cvar.visual_skins);
	ReplaceTextureIndex("GIGN_DMBASE2", PLAYER2, "gign", 1, cvar.visual_skins);
	ReplaceTextureIndex("GSG9_Working1", PLAYER3, "gsg9", 1, cvar.visual_skins);
	ReplaceTextureIndex("GUERILLA_DMBASE", PLAYER4, "guerilla", 1, cvar.visual_skins);
	ReplaceTextureIndex("Arab_dmbase1", PLAYER5, "leet", 1, cvar.visual_skins);
	ReplaceTextureIndex("SAS_DMBASE1", PLAYER6, "sas", 1, cvar.visual_skins);
	ReplaceTextureIndex("Terrorist_Working1", PLAYER7, "terror", 1, cvar.visual_skins);
	ReplaceTextureIndex("SEAL_Working1", PLAYER8, "urban", 1, cvar.visual_skins);

	ReplaceTextureIndex("Backpack1", BACKPACK1, "arctic", 1, cvar.visual_skins);
	ReplaceTextureIndex("Backpack1", BACKPACK2, "guerilla", 1, cvar.visual_skins);
	ReplaceTextureIndex("Backpack1", BACKPACK3, "leet", 1, cvar.visual_skins);
	ReplaceTextureIndex("Backpack1", BACKPACK4, "terror", 1, cvar.visual_skins);
	ReplaceTextureIndex("Backpack1", BACKPACK5, "w_backpack", 3, cvar.visual_skins);

	ReplaceTextureIndex("Backpack2", THIGHPACK1, "gign", 1, cvar.visual_skins);
	ReplaceTextureIndex("Backpack2", THIGHPACK2, "gsg9", 1, cvar.visual_skins);
	ReplaceTextureIndex("Backpack2", THIGHPACK3, "sas", 1, cvar.visual_skins);
	ReplaceTextureIndex("Backpack2", THIGHPACK4, "urban", 1, cvar.visual_skins);
	ReplaceTextureIndex("Backpack2", THIGHPACK5, "w_thighpack", 3, cvar.visual_skins);

	ReplaceTextureIndex("barrel", AK471, "v_ak47", 3, cvar.visual_skins);
	ReplaceTextureIndex("forearm", AK472, "v_ak47", 3, cvar.visual_skins);
	ReplaceTextureIndex("handle", AK473, "v_ak47", 3, cvar.visual_skins);
	ReplaceTextureIndex("lower_body", AK474, "v_ak47", 3, cvar.visual_skins);
	ReplaceTextureIndex("magazine", AK475, "v_ak47", 3, cvar.visual_skins);
	ReplaceTextureIndex("reticle", AK476, "v_ak47", 3, cvar.visual_skins);
	ReplaceTextureIndex("upper_body", AK477, "v_ak47", 3, cvar.visual_skins);
	ReplaceTextureIndex("wood", AK478, "v_ak47", 3, cvar.visual_skins);
	ReplaceTextureIndex("ak47_skin", AK479, "w_ak47", 3, cvar.visual_skins);
	ReplaceTextureIndex("ak47_skin", AK4710, "p_ak47", 3, cvar.visual_skins);
	ReplaceTextureIndex("ak47_skin", AK4711, "backweapons", 3, cvar.visual_skins);
	
	ReplaceTextureIndex("kevlar_vest", KEVLAR, "w_assault", 3, cvar.visual_skins);

	ReplaceTextureIndex("barrel", AUG1, "v_aug", 3, cvar.visual_skins);
	ReplaceTextureIndex("body", AUG2, "v_aug", 3, cvar.visual_skins);
	ReplaceTextureIndex("magazine", AUG3, "v_aug", 3, cvar.visual_skins);
	ReplaceTextureIndex("w_aug", AUG4, "w_aug", 3, cvar.visual_skins);
	ReplaceTextureIndex("w_aug", AUG5, "p_aug", 3, cvar.visual_skins);
	ReplaceTextureIndex("w_aug", AUG6, "backweapons", 3, cvar.visual_skins);

	ReplaceTextureIndex("barrel", AWP1, "v_awp", 3, cvar.visual_skins);
	ReplaceTextureIndex("base", AWP2, "v_awp", 3, cvar.visual_skins);
	ReplaceTextureIndex("bolt_handle", AWP3, "v_awp", 3, cvar.visual_skins);
	ReplaceTextureIndex("buttstock", AWP4, "v_awp", 3, cvar.visual_skins);
	ReplaceTextureIndex("ejector_port", AWP5, "v_awp", 3, cvar.visual_skins);
	ReplaceTextureIndex("magazine", AWP6, "v_awp", 3, cvar.visual_skins);
	ReplaceTextureIndex("newparts.bmp", AWP7, "v_awp", 3, cvar.visual_skins);
	ReplaceTextureIndex("newparts2", AWP8, "v_awp", 3, cvar.visual_skins);
	ReplaceTextureIndex("scope.bmp", AWP9, "v_awp", 3, cvar.visual_skins);
	ReplaceTextureIndex("scope_clamps", AWP10, "v_awp", 3, cvar.visual_skins);
	ReplaceTextureIndex("w_awp", AWP11, "w_awp", 3, cvar.visual_skins);
	ReplaceTextureIndex("w_awp", AWP12, "p_awp", 3, cvar.visual_skins);
	ReplaceTextureIndex("w_awp", AWP13, "backweapons", 3, cvar.visual_skins);

	ReplaceTextureIndex("c4base", C41, "v_c4", 3, cvar.visual_skins);
	ReplaceTextureIndex("c4buttons", C42, "v_c4", 3, cvar.visual_skins);
	ReplaceTextureIndex("c4timer", C43, "v_c4", 3, cvar.visual_skins);
	ReplaceTextureIndex("c4wires", C44, "v_c4", 3, cvar.visual_skins);
	ReplaceTextureIndex("cbase_front", C45, "v_c4", 3, cvar.visual_skins);
	ReplaceTextureIndex("c4base", C46, "p_c4", 3, cvar.visual_skins);
	ReplaceTextureIndex("c4timer", C47, "p_c4", 3, cvar.visual_skins);
	ReplaceTextureIndex("c4wires", C48, "p_c4", 3, cvar.visual_skins);
	ReplaceTextureIndex("c4base", C49, "w_c4", 3, cvar.visual_skins);
	ReplaceTextureIndex("c4timer", C410, "w_c4", 3, cvar.visual_skins);
	ReplaceTextureIndex("c4wires", C411, "w_c4", 3, cvar.visual_skins);

	ReplaceTextureIndex("DE_handle", DEAGLE1, "v_deagle", 3, cvar.visual_skins);
	ReplaceTextureIndex("DE_slide1", DEAGLE2, "v_deagle", 3, cvar.visual_skins);
	ReplaceTextureIndex("DE_slide2_eagle", DEAGLE3, "v_deagle", 3, cvar.visual_skins);
	ReplaceTextureIndex("deserteagle_skin", DEAGLE4, "p_deagle", 3, cvar.visual_skins);
	ReplaceTextureIndex("deserteagle_skin", DEAGLE5, "w_deagle", 3, cvar.visual_skins);
	ReplaceTextureIndex("deserteagle_skin", DEAGLE6, "p_shield_deagle", 2, cvar.visual_skins);
	ReplaceTextureIndex("DE_handle", DEAGLE7, "v_shield_deagle", 2, cvar.visual_skins);
	ReplaceTextureIndex("DE_slide1", DEAGLE8, "v_shield_deagle", 2, cvar.visual_skins);
	ReplaceTextureIndex("DE_slide2_eagle", DEAGLE9, "v_shield_deagle", 2, cvar.visual_skins);

	ReplaceTextureIndex("barrel", ELITE1, "v_elite", 3, cvar.visual_skins);
	ReplaceTextureIndex("handle", ELITE2, "v_elite", 3, cvar.visual_skins);
	ReplaceTextureIndex("magazine", ELITE3, "v_elite", 3, cvar.visual_skins);
	ReplaceTextureIndex("slide", ELITE4, "v_elite", 3, cvar.visual_skins);
	ReplaceTextureIndex("w_elite", ELITE5, "p_elite", 3, cvar.visual_skins);
	ReplaceTextureIndex("w_elite", ELITE6, "w_elite", 3, cvar.visual_skins);
	
	ReplaceTextureIndex("v_famas", FAMAS1, "v_famas", 3, cvar.visual_skins);
	ReplaceTextureIndex("p_famas", FAMAS2, "p_famas", 3, cvar.visual_skins);
	ReplaceTextureIndex("p_famas", FAMAS3, "w_famas", 3, cvar.visual_skins);
	ReplaceTextureIndex("p_famas", FAMAS4, "backweapons", 3, cvar.visual_skins);

	ReplaceTextureIndex("fs1", FIVESEVEN1, "v_fiveseven", 3, cvar.visual_skins);
	ReplaceTextureIndex("fs2", FIVESEVEN2, "v_fiveseven", 3, cvar.visual_skins);
	ReplaceTextureIndex("57_profile", FIVESEVEN3, "p_fiveseven", 3, cvar.visual_skins);
	ReplaceTextureIndex("57_profile", FIVESEVEN4, "w_fiveseven", 3, cvar.visual_skins);
	ReplaceTextureIndex("57_profile", FIVESEVEN5, "p_shield_fiveseven", 2, cvar.visual_skins);
	ReplaceTextureIndex("fs1", FIVESEVEN6, "v_shield_fiveseven", 2, cvar.visual_skins);
	ReplaceTextureIndex("fs2", FIVESEVEN7, "v_shield_fiveseven", 2, cvar.visual_skins);

	ReplaceTextureIndex("flash_body", FLASHBANG1, "v_flashbang", 3, cvar.visual_skins);
	ReplaceTextureIndex("flash_spoon", FLASHBANG2, "v_flashbang", 3, cvar.visual_skins);
	ReplaceTextureIndex("flash_top", FLASHBANG3, "v_flashbang", 3, cvar.visual_skins);
	ReplaceTextureIndex("f_body", FLASHBANG4, "w_flashbang", 3, cvar.visual_skins);
	ReplaceTextureIndex("f_top", FLASHBANG5, "w_flashbang", 3, cvar.visual_skins);
	ReplaceTextureIndex("f_body", FLASHBANG6, "p_flashbang", 3, cvar.visual_skins);
	ReplaceTextureIndex("f_top", FLASHBANG7, "p_flashbang", 3, cvar.visual_skins);
	ReplaceTextureIndex("f_body", FLASHBANG8, "p_shield_flashbang", 2, cvar.visual_skins);
	ReplaceTextureIndex("f_top", FLASHBANG9, "p_shield_flashbang", 2, cvar.visual_skins);
	ReplaceTextureIndex("flash_body", FLASHBANG10, "v_shield_flashbang", 2, cvar.visual_skins);
	ReplaceTextureIndex("flash_spoon", FLASHBANG11, "v_shield_flashbang", 2, cvar.visual_skins);
	ReplaceTextureIndex("flash_top", FLASHBANG12, "v_shield_flashbang", 2, cvar.visual_skins);

	ReplaceTextureIndex("barrel", G3SG11, "v_g3sg1", 3, cvar.visual_skins);
	ReplaceTextureIndex("body", G3SG12, "v_g3sg1", 3, cvar.visual_skins);
	ReplaceTextureIndex("buttstock", G3SG13, "v_g3sg1", 3, cvar.visual_skins);
	ReplaceTextureIndex("forearm", G3SG14, "v_g3sg1", 3, cvar.visual_skins);
	ReplaceTextureIndex("lowerbody", G3SG15, "v_g3sg1", 3, cvar.visual_skins);
	ReplaceTextureIndex("scope.bmp", G3SG16, "v_g3sg1", 3, cvar.visual_skins);
	ReplaceTextureIndex("scope_hold", G3SG17, "v_g3sg1", 3, cvar.visual_skins);
	ReplaceTextureIndex("scope_knob", G3SG18, "v_g3sg1", 3, cvar.visual_skins);
	ReplaceTextureIndex("w_g3sg1", G3SG19, "w_g3sg1", 3, cvar.visual_skins);
	ReplaceTextureIndex("w_g3sg1", G3SG110, "p_g3sg1", 3, cvar.visual_skins);
	ReplaceTextureIndex("w_g3sg1", G3SG111, "backweapons", 3, cvar.visual_skins);
	
	ReplaceTextureIndex("galil", GALIL1, "v_galil", 3, cvar.visual_skins);
	ReplaceTextureIndex("p_galil", GALIL2, "w_galil", 3, cvar.visual_skins);
	ReplaceTextureIndex("p_galil", GALIL3, "p_galil", 3, cvar.visual_skins);
	ReplaceTextureIndex("p_galil", GALIL4, "backweapons", 3, cvar.visual_skins);

	ReplaceTextureIndex("glock_barrel", GLOCK181, "v_glock18", 3, cvar.visual_skins);
	ReplaceTextureIndex("glock_base", GLOCK182, "v_glock18", 3, cvar.visual_skins);
	ReplaceTextureIndex("glock_mag", GLOCK183, "v_glock18", 3, cvar.visual_skins);
	ReplaceTextureIndex("glock_slide", GLOCK184, "v_glock18", 3, cvar.visual_skins);
	ReplaceTextureIndex("w_glock", GLOCK185, "w_glock18", 3, cvar.visual_skins);
	ReplaceTextureIndex("w_glock", GLOCK186, "p_glock18", 3, cvar.visual_skins);
	ReplaceTextureIndex("w_glock", GLOCK187, "p_shield_glock18", 2, cvar.visual_skins);
	ReplaceTextureIndex("glock_barrel", GLOCK188, "v_shield_glock18", 2, cvar.visual_skins);
	ReplaceTextureIndex("glock_base", GLOCK189, "v_shield_glock18", 2, cvar.visual_skins);
	ReplaceTextureIndex("glock_mag", GLOCK1810, "v_shield_glock18", 2, cvar.visual_skins);
	ReplaceTextureIndex("glock_slide", GLOCK1811, "v_shield_glock18", 2, cvar.visual_skins);

	ReplaceTextureIndex("he_body", HEGRENADE1, "v_hegrenade", 3, cvar.visual_skins);
	ReplaceTextureIndex("he_spoon", HEGRENADE2, "v_hegrenade", 3, cvar.visual_skins);
	ReplaceTextureIndex("he_top", HEGRENADE3, "v_hegrenade", 3, cvar.visual_skins);
	ReplaceTextureIndex("f_body", HEGRENADE4, "w_hegrenade", 3, cvar.visual_skins);
	ReplaceTextureIndex("f_top", HEGRENADE5, "w_hegrenade", 3, cvar.visual_skins);
	ReplaceTextureIndex("f_body", HEGRENADE6, "p_hegrenade", 3, cvar.visual_skins);
	ReplaceTextureIndex("f_top", HEGRENADE7, "p_hegrenade", 3, cvar.visual_skins);
	ReplaceTextureIndex("f_body", HEGRENADE8, "p_shield_hegrenade", 2, cvar.visual_skins);
	ReplaceTextureIndex("f_top", HEGRENADE9, "p_shield_hegrenade", 2, cvar.visual_skins);
	ReplaceTextureIndex("he_body", HEGRENADE10, "v_shield_hegrenade", 2, cvar.visual_skins);
	ReplaceTextureIndex("he_spoon", HEGRENADE11, "v_shield_hegrenade", 2, cvar.visual_skins);
	ReplaceTextureIndex("he_top", HEGRENADE12, "v_shield_hegrenade", 2, cvar.visual_skins);

	ReplaceTextureIndex("knifeskin", KNIFE1, "v_knife", 3, cvar.visual_skins);
	ReplaceTextureIndex("pknifeskin", KNIFE2, "p_knife", 3, cvar.visual_skins);
	ReplaceTextureIndex("knifeskin", KNIFE3, "v_shield_knife", 2, cvar.visual_skins);

	ReplaceTextureIndex("barrel", M31, "v_m3", 3, cvar.visual_skins);
	ReplaceTextureIndex("forearm", M32, "v_m3", 3, cvar.visual_skins);
	ReplaceTextureIndex("handle", M33, "v_m3", 3, cvar.visual_skins);
	ReplaceTextureIndex("sights", M34, "v_m3", 3, cvar.visual_skins);
	ReplaceTextureIndex("w_m3super90", M35, "w_m3", 3, cvar.visual_skins);
	ReplaceTextureIndex("w_m3super90", M36, "p_m3", 3, cvar.visual_skins);
	ReplaceTextureIndex("w_m3super90", M37, "backweapons", 3, cvar.visual_skins);

	ReplaceTextureIndex("barrel", M4A11, "v_m4a1", 3, cvar.visual_skins);
	ReplaceTextureIndex("buttstock", M4A12, "v_m4a1", 3, cvar.visual_skins);
	ReplaceTextureIndex("handle", M4A13, "v_m4a1", 3, cvar.visual_skins);
	ReplaceTextureIndex("magazine", M4A14, "v_m4a1", 3, cvar.visual_skins);
	ReplaceTextureIndex("receiver", M4A15, "v_m4a1", 3, cvar.visual_skins);
	ReplaceTextureIndex("silencer", M4A16, "v_m4a1", 3, cvar.visual_skins);
	ReplaceTextureIndex("m4a1_skin", M4A17, "w_m4a1", 3, cvar.visual_skins);
	ReplaceTextureIndex("m4a1_skin", M4A18, "p_m4a1", 3, cvar.visual_skins);
	ReplaceTextureIndex("m4a1_skin", M4A19, "backweapons", 3, cvar.visual_skins);

	ReplaceTextureIndex("ammobox", M2491, "v_m249", 3, cvar.visual_skins);
	ReplaceTextureIndex("barrel", M2492, "v_m249", 3, cvar.visual_skins);
	ReplaceTextureIndex("body", M2493, "v_m249", 3, cvar.visual_skins);
	ReplaceTextureIndex("bullet", M2494, "v_m249", 3, cvar.visual_skins);
	ReplaceTextureIndex("buttstock", M2495, "v_m249", 3, cvar.visual_skins);
	ReplaceTextureIndex("cover", M2496, "v_m249", 3, cvar.visual_skins);
	ReplaceTextureIndex("forearm", M2497, "v_m249", 3, cvar.visual_skins);
	ReplaceTextureIndex("handle", M2498, "v_m249", 3, cvar.visual_skins);
	ReplaceTextureIndex("sight", M2499, "v_m249", 3, cvar.visual_skins);
	ReplaceTextureIndex("w_m249", M24910, "w_m249", 3, cvar.visual_skins);
	ReplaceTextureIndex("w_m249", M24911, "p_m249", 3, cvar.visual_skins);
	ReplaceTextureIndex("w_m249", M24912, "backweapons", 3, cvar.visual_skins);
	
	ReplaceTextureIndex("body", MAC101, "v_mac10", 3, cvar.visual_skins);
	ReplaceTextureIndex("buttstock", MAC102, "v_mac10", 3, cvar.visual_skins);
	ReplaceTextureIndex("handle", MAC103, "v_mac10", 3, cvar.visual_skins);
	ReplaceTextureIndex("w_mac10", MAC104, "w_mac10", 3, cvar.visual_skins);
	ReplaceTextureIndex("w_mac10", MAC105, "p_mac10", 3, cvar.visual_skins);

	ReplaceTextureIndex("barrel", MP51, "v_mp5", 3, cvar.visual_skins);
	ReplaceTextureIndex("buttstock", MP52, "v_mp5", 3, cvar.visual_skins);
	ReplaceTextureIndex("clip", MP53, "v_mp5", 3, cvar.visual_skins);
	ReplaceTextureIndex("forearm", MP54, "v_mp5", 3, cvar.visual_skins);
	ReplaceTextureIndex("handle", MP55, "v_mp5", 3, cvar.visual_skins);
	ReplaceTextureIndex("lowerrec", MP56, "v_mp5", 3, cvar.visual_skins);
	ReplaceTextureIndex("rearsight", MP57, "v_mp5", 3, cvar.visual_skins);
	ReplaceTextureIndex("w_mp5", MP58, "w_mp5", 3, cvar.visual_skins);
	ReplaceTextureIndex("w_mp5", MP59, "p_mp5", 3, cvar.visual_skins);
	ReplaceTextureIndex("w_mp5", MP510, "backweapons", 3, cvar.visual_skins);

	ReplaceTextureIndex("buttstock", P901, "v_p90", 3, cvar.visual_skins);
	ReplaceTextureIndex("handle-1", P902, "v_p90", 3, cvar.visual_skins);
	ReplaceTextureIndex("handles", P903, "v_p90", 3, cvar.visual_skins);
	ReplaceTextureIndex("magazine", P904, "v_p90", 3, cvar.visual_skins);
	ReplaceTextureIndex("sights-1", P905, "v_p90", 3, cvar.visual_skins);
	ReplaceTextureIndex("w_p90", P906, "w_p90", 3, cvar.visual_skins);
	ReplaceTextureIndex("w_p90", P907, "p_p90", 3, cvar.visual_skins);
	ReplaceTextureIndex("w_p90", P908, "backweapons", 3, cvar.visual_skins);
	
	ReplaceTextureIndex("p228_handle", P2281, "v_p228", 3, cvar.visual_skins);
	ReplaceTextureIndex("p228_mag", P2282, "v_p228", 3, cvar.visual_skins);
	ReplaceTextureIndex("p228_slide", P2283, "v_p228", 3, cvar.visual_skins);
	ReplaceTextureIndex("w_p228", P2284, "w_p228", 3, cvar.visual_skins);
	ReplaceTextureIndex("w_p228", P2285, "p_p228", 3, cvar.visual_skins);
	ReplaceTextureIndex("w_p228", P2286, "p_shield_p228", 2, cvar.visual_skins);
	ReplaceTextureIndex("p228_handle", P2287, "v_shield_p228", 2, cvar.visual_skins);
	ReplaceTextureIndex("p228_mag", P2288, "v_shield_p228", 2, cvar.visual_skins);
	ReplaceTextureIndex("p228_slide", P2289, "v_shield_p228", 2, cvar.visual_skins);

	ReplaceTextureIndex("base", SCOUT1, "v_scout", 3, cvar.visual_skins);
	ReplaceTextureIndex("magazine", SCOUT2, "v_scout", 3, cvar.visual_skins);
	ReplaceTextureIndex("rail", SCOUT3, "v_scout", 3, cvar.visual_skins);
	ReplaceTextureIndex("scope.bmp", SCOUT4, "v_scout", 3, cvar.visual_skins);
	ReplaceTextureIndex("scope_clamps", SCOUT5, "v_scout", 3, cvar.visual_skins);
	ReplaceTextureIndex("w_scout", SCOUT6, "w_scout", 3, cvar.visual_skins);
	ReplaceTextureIndex("w_scout", SCOUT7, "p_scout", 3, cvar.visual_skins);
	ReplaceTextureIndex("w_scout", SCOUT8, "backweapons", 3, cvar.visual_skins);

	ReplaceTextureIndex("buttstock", SG5501, "v_sg550", 3, cvar.visual_skins);
	ReplaceTextureIndex("forearm", SG5502, "v_sg550", 3, cvar.visual_skins);
	ReplaceTextureIndex("handle", SG5503, "v_sg550", 3, cvar.visual_skins);
	ReplaceTextureIndex("magazine_transp", SG5504, "v_sg550", 3, cvar.visual_skins);
	ReplaceTextureIndex("receiver", SG5505, "v_sg550", 3, cvar.visual_skins);
	ReplaceTextureIndex("scope", SG5506, "v_sg550", 3, cvar.visual_skins);
	ReplaceTextureIndex("sg550_profile", SG5507, "w_sg550", 3, cvar.visual_skins);
	ReplaceTextureIndex("sg550_profile", SG5508, "p_sg550", 3, cvar.visual_skins);
	ReplaceTextureIndex("sg550_profile", SG5509, "backweapons", 3, cvar.visual_skins);
	
	ReplaceTextureIndex("acog", SG5521, "v_sg552", 3, cvar.visual_skins);
	ReplaceTextureIndex("buttstock", SG5522, "v_sg552", 3, cvar.visual_skins);
	ReplaceTextureIndex("forearm", SG5523, "v_sg552", 3, cvar.visual_skins);
	ReplaceTextureIndex("handle", SG5524, "v_sg552", 3, cvar.visual_skins);
	ReplaceTextureIndex("magazine_transp", SG5525, "v_sg552", 3, cvar.visual_skins);
	ReplaceTextureIndex("sg552_skin", SG5526, "w_sg552", 3, cvar.visual_skins);
	ReplaceTextureIndex("sg552_skin", SG5527, "p_sg552", 3, cvar.visual_skins);
	ReplaceTextureIndex("sg552_skin", SG5528, "backweapons", 3, cvar.visual_skins);
	
	ReplaceTextureIndex("shield_back", SHIELD1, "w_shield", 3, cvar.visual_skins);
	ReplaceTextureIndex("shield_front", SHIELD2, "w_shield", 3, cvar.visual_skins);
	ReplaceTextureIndex("shield_glass", SHIELD3, "w_shield", 3, cvar.visual_skins);
	ReplaceTextureIndex("shield_back", SHIELD4, "p_shield_deagle", 2, cvar.visual_skins);
	ReplaceTextureIndex("shield_front", SHIELD5, "p_shield_deagle", 2, cvar.visual_skins);
	ReplaceTextureIndex("shield_glass", SHIELD6, "p_shield_deagle", 2, cvar.visual_skins);
	ReplaceTextureIndex("shield_back", SHIELD7, "p_shield_fiveseven", 2, cvar.visual_skins);
	ReplaceTextureIndex("shield_front", SHIELD8, "p_shield_fiveseven", 2, cvar.visual_skins);
	ReplaceTextureIndex("shield_glass", SHIELD9, "p_shield_fiveseven", 2, cvar.visual_skins);
	ReplaceTextureIndex("shield_back", SHIELD10, "p_shield_flashbang", 2, cvar.visual_skins);
	ReplaceTextureIndex("shield_front", SHIELD11, "p_shield_flashbang", 2, cvar.visual_skins);
	ReplaceTextureIndex("shield_glass", SHIELD12, "p_shield_flashbang", 2, cvar.visual_skins);
	ReplaceTextureIndex("shield_back", SHIELD13, "p_shield_glock18", 2, cvar.visual_skins);
	ReplaceTextureIndex("shield_front", SHIELD14, "p_shield_glock18", 2, cvar.visual_skins);
	ReplaceTextureIndex("shield_glass", SHIELD15, "p_shield_glock18", 2, cvar.visual_skins);
	ReplaceTextureIndex("shield_back", SHIELD16, "p_shield_hegrenade", 2, cvar.visual_skins);
	ReplaceTextureIndex("shield_front", SHIELD17, "p_shield_hegrenade", 2, cvar.visual_skins);
	ReplaceTextureIndex("shield_glass", SHIELD18, "p_shield_hegrenade", 2, cvar.visual_skins);
	ReplaceTextureIndex("shield_back", SHIELD19, "p_shield_knife", 2, cvar.visual_skins);
	ReplaceTextureIndex("shield_front", SHIELD20, "p_shield_knife", 2, cvar.visual_skins);
	ReplaceTextureIndex("shield_glass", SHIELD21, "p_shield_knife", 2, cvar.visual_skins);
	ReplaceTextureIndex("shield_back", SHIELD22, "p_shield_p228", 2, cvar.visual_skins);
	ReplaceTextureIndex("shield_front", SHIELD23, "p_shield_p228", 2, cvar.visual_skins);
	ReplaceTextureIndex("shield_glass", SHIELD24, "p_shield_p228", 2, cvar.visual_skins);
	ReplaceTextureIndex("shield_back", SHIELD25, "p_shield_smokegrenade", 2, cvar.visual_skins);
	ReplaceTextureIndex("shield_front", SHIELD26, "p_shield_smokegrenade", 2, cvar.visual_skins);
	ReplaceTextureIndex("shield_glass", SHIELD27, "p_shield_smokegrenade", 2, cvar.visual_skins);
	ReplaceTextureIndex("shield_back", SHIELD28, "p_shield_usp", 2, cvar.visual_skins);
	ReplaceTextureIndex("shield_front", SHIELD29, "p_shield_usp", 2, cvar.visual_skins);
	ReplaceTextureIndex("shield_glass", SHIELD30, "p_shield_usp", 2, cvar.visual_skins);
	ReplaceTextureIndex("shield.bmp", SHIELD31, "v_shield_deagle", 2, cvar.visual_skins);
	ReplaceTextureIndex("shield_glass", SHIELD32, "v_shield_deagle", 2, cvar.visual_skins);
	ReplaceTextureIndex("shield.bmp", SHIELD33, "v_shield_fiveseven", 2, cvar.visual_skins);
	ReplaceTextureIndex("shield_glass", SHIELD34, "v_shield_fiveseven", 2, cvar.visual_skins);
	ReplaceTextureIndex("shield.bmp", SHIELD35, "v_shield_flashbang", 2, cvar.visual_skins);
	ReplaceTextureIndex("shield_glass", SHIELD36, "v_shield_flashbang", 2, cvar.visual_skins);
	ReplaceTextureIndex("shield.bmp", SHIELD37, "v_shield_glock18", 2, cvar.visual_skins);
	ReplaceTextureIndex("shield_glass", SHIELD38, "v_shield_glock18", 2, cvar.visual_skins);
	ReplaceTextureIndex("shield.bmp", SHIELD39, "v_shield_hegrenade", 2, cvar.visual_skins);
	ReplaceTextureIndex("shield_glass", SHIELD40, "v_shield_hegrenade", 2, cvar.visual_skins);
	ReplaceTextureIndex("shield.bmp", SHIELD41, "v_shield_knife", 2, cvar.visual_skins);
	ReplaceTextureIndex("shield.bmp", SHIELD42, "v_shield_p228", 2, cvar.visual_skins);
	ReplaceTextureIndex("shield_glass", SHIELD43, "v_shield_p228", 2, cvar.visual_skins);
	ReplaceTextureIndex("shield.bmp", SHIELD44, "v_shield_smokegrenade", 2, cvar.visual_skins);
	ReplaceTextureIndex("shield_glass", SHIELD45, "v_shield_smokegrenade", 2, cvar.visual_skins);
	ReplaceTextureIndex("shield.bmp", SHIELD46, "v_shield_usp", 2, cvar.visual_skins);
	ReplaceTextureIndex("shield_glass", SHIELD47, "v_shield_usp", 2, cvar.visual_skins);

	ReplaceTextureIndex("smoke_body", SMOKEGRENADE1, "v_smokegrenade", 3, cvar.visual_skins);
	ReplaceTextureIndex("smoke_spoon", SMOKEGRENADE2, "v_smokegrenade", 3, cvar.visual_skins);
	ReplaceTextureIndex("smoke_top", SMOKEGRENADE3, "v_smokegrenade", 3, cvar.visual_skins);
	ReplaceTextureIndex("f_body", SMOKEGRENADE4, "w_smokegrenade", 3, cvar.visual_skins);
	ReplaceTextureIndex("f_top", SMOKEGRENADE5, "w_smokegrenade", 3, cvar.visual_skins);
	ReplaceTextureIndex("f_body", SMOKEGRENADE6, "p_smokegrenade", 3, cvar.visual_skins);
	ReplaceTextureIndex("f_top", SMOKEGRENADE7, "p_smokegrenade", 3, cvar.visual_skins);
	ReplaceTextureIndex("f_body", SMOKEGRENADE8, "p_shield_smokegrenade", 2, cvar.visual_skins);
	ReplaceTextureIndex("f_top", SMOKEGRENADE9, "p_shield_smokegrenade", 2, cvar.visual_skins);
	ReplaceTextureIndex("smoke_body", SMOKEGRENADE10, "v_shield_smokegrenade", 2, cvar.visual_skins);
	ReplaceTextureIndex("smoke_spoon", SMOKEGRENADE11, "v_shield_smokegrenade", 2, cvar.visual_skins);
	ReplaceTextureIndex("smoke_top", SMOKEGRENADE12, "v_shield_smokegrenade", 2, cvar.visual_skins);

	ReplaceTextureIndex("barrel", TMP1, "v_tmp", 3, cvar.visual_skins);
	ReplaceTextureIndex("body", TMP2, "v_tmp", 3, cvar.visual_skins);
	ReplaceTextureIndex("ejector", TMP3, "v_tmp", 3, cvar.visual_skins);
	ReplaceTextureIndex("forearm", TMP4, "v_tmp", 3, cvar.visual_skins);
	ReplaceTextureIndex("handle", TMP5, "v_tmp", 3, cvar.visual_skins);
	ReplaceTextureIndex("magazine", TMP6, "v_tmp", 3, cvar.visual_skins);
	ReplaceTextureIndex("misc", TMP7, "v_tmp", 3, cvar.visual_skins);
	ReplaceTextureIndex("silencer", TMP8, "v_tmp", 3, cvar.visual_skins);
	ReplaceTextureIndex("top.bmp", TMP9, "v_tmp", 3, cvar.visual_skins);
	ReplaceTextureIndex("w_tmp", TMP10, "w_tmp", 3, cvar.visual_skins);
	ReplaceTextureIndex("w_tmp", TMP11, "p_tmp", 3, cvar.visual_skins);

	ReplaceTextureIndex("buttstock", UMP451, "v_ump45", 3, cvar.visual_skins);
	ReplaceTextureIndex("handle", UMP452, "v_ump45", 3, cvar.visual_skins);
	ReplaceTextureIndex("receiver", UMP453, "v_ump45", 3, cvar.visual_skins);
	ReplaceTextureIndex("ump_profile", UMP454, "w_ump45", 3, cvar.visual_skins);
	ReplaceTextureIndex("ump_profile", UMP455, "p_ump45", 3, cvar.visual_skins);
	ReplaceTextureIndex("ump_profile", UMP456, "backweapons", 3, cvar.visual_skins);
	
	ReplaceTextureIndex("handle", USP1, "v_usp", 3, cvar.visual_skins);
	ReplaceTextureIndex("mag", USP2, "v_usp", 3, cvar.visual_skins);
	ReplaceTextureIndex("silencer", USP3, "v_usp", 3, cvar.visual_skins);
	ReplaceTextureIndex("slide", USP4, "v_usp", 3, cvar.visual_skins);
	ReplaceTextureIndex("usp_skin", USP5, "w_usp", 3, cvar.visual_skins);
	ReplaceTextureIndex("usp_skin", USP6, "p_usp", 3, cvar.visual_skins);
	ReplaceTextureIndex("usp_skin", USP7, "p_shield_usp", 2, cvar.visual_skins);
	ReplaceTextureIndex("handle", USP8, "v_shield_usp", 2, cvar.visual_skins);
	ReplaceTextureIndex("mag", USP9, "v_shield_usp", 2, cvar.visual_skins);
	ReplaceTextureIndex("slide", USP10, "v_shield_usp", 2, cvar.visual_skins);
	
	ReplaceTextureIndex("barrel", XM10141, "v_xm1014", 3, cvar.visual_skins);
	ReplaceTextureIndex("body", XM10142, "v_xm1014", 3, cvar.visual_skins);
	ReplaceTextureIndex("buttstock", XM10143, "v_xm1014", 3, cvar.visual_skins);
	ReplaceTextureIndex("forearm", XM10144, "v_xm1014", 3, cvar.visual_skins);
	ReplaceTextureIndex("rear_sight", XM10145, "v_xm1014", 3, cvar.visual_skins);
	ReplaceTextureIndex("shell", XM10146, "v_xm1014", 3, cvar.visual_skins);
	ReplaceTextureIndex("xm1014", XM10147, "w_xm1014", 3, cvar.visual_skins);
	ReplaceTextureIndex("xm1014", XM10148, "p_xm1014", 3, cvar.visual_skins);
	ReplaceTextureIndex("xm1014", XM10149, "backweapons", 3, cvar.visual_skins);

	ReplaceTextureIndex("view_glove", HANDS1, "v_ak47", 3, cvar.visual_skins);
	ReplaceTextureIndex("view_glove", HANDS2, "v_aug", 3, cvar.visual_skins);
	ReplaceTextureIndex("view_glove", HANDS3, "v_awp", 3, cvar.visual_skins);
	ReplaceTextureIndex("view_glove", HANDS4, "v_c4", 3, cvar.visual_skins);
	ReplaceTextureIndex("view_glove", HANDS5, "v_deagle", 3, cvar.visual_skins);
	ReplaceTextureIndex("view_glove", HANDS6, "v_elite", 3, cvar.visual_skins);
	ReplaceTextureIndex("view_glove", HANDS7, "v_famas", 3, cvar.visual_skins);
	ReplaceTextureIndex("view_glove", HANDS8, "v_fiveseven", 3, cvar.visual_skins);
	ReplaceTextureIndex("view_glove", HANDS9, "v_flashbang", 3, cvar.visual_skins);
	ReplaceTextureIndex("view_glove", HANDS10, "v_g3sg1", 3, cvar.visual_skins);

	ReplaceTextureIndex("view_glove", HANDS11, "v_galil", 3, cvar.visual_skins);
	ReplaceTextureIndex("view_glove", HANDS12, "v_glock18", 3, cvar.visual_skins);
	ReplaceTextureIndex("view_glove", HANDS13, "v_hegrenade", 3, cvar.visual_skins);
	ReplaceTextureIndex("view_glove", HANDS14, "v_knife", 3, cvar.visual_skins);
	ReplaceTextureIndex("view_glove", HANDS15, "v_m3", 3, cvar.visual_skins);
	ReplaceTextureIndex("view_glove", HANDS16, "v_m4a1", 3, cvar.visual_skins);
	ReplaceTextureIndex("view_glove", HANDS17, "v_m249", 3, cvar.visual_skins);
	ReplaceTextureIndex("view_glove", HANDS18, "v_mac10", 3, cvar.visual_skins);
	ReplaceTextureIndex("view_glove", HANDS19, "v_mp5", 3, cvar.visual_skins);
	ReplaceTextureIndex("view_glove", HANDS20, "v_p90", 3, cvar.visual_skins);

	ReplaceTextureIndex("view_glove", HANDS21, "v_p228", 3, cvar.visual_skins);
	ReplaceTextureIndex("view_glove", HANDS22, "v_scout", 3, cvar.visual_skins);
	ReplaceTextureIndex("view_glove", HANDS23, "v_sg550", 3, cvar.visual_skins);
	ReplaceTextureIndex("view_glove", HANDS24, "v_sg552", 3, cvar.visual_skins);
	ReplaceTextureIndex("view_glove", HANDS25, "v_smokegrenade", 3, cvar.visual_skins);
	ReplaceTextureIndex("view_glove", HANDS26, "v_tmp", 3, cvar.visual_skins);
	ReplaceTextureIndex("view_glove", HANDS27, "v_ump45", 3, cvar.visual_skins);
	ReplaceTextureIndex("view_glove", HANDS28, "v_usp", 3, cvar.visual_skins);
	ReplaceTextureIndex("view_glove", HANDS29, "v_xm1014", 3, cvar.visual_skins);
	ReplaceTextureIndex("view_glove", HANDS30, "v_shield_deagle", 2, cvar.visual_skins);

	ReplaceTextureIndex("view_glove", HANDS31, "v_shield_fiveseven", 2, cvar.visual_skins);
	ReplaceTextureIndex("view_glove", HANDS32, "v_shield_flashbang", 2, cvar.visual_skins);
	ReplaceTextureIndex("view_glove", HANDS33, "v_shield_glock18", 2, cvar.visual_skins);
	ReplaceTextureIndex("view_glove", HANDS34, "v_shield_hegrenade", 2, cvar.visual_skins);
	ReplaceTextureIndex("view_glove", HANDS35, "v_shield_knife", 2, cvar.visual_skins);
	ReplaceTextureIndex("view_glove", HANDS36, "v_shield_p228", 2, cvar.visual_skins);
	ReplaceTextureIndex("view_glove", HANDS37, "v_shield_smokegrenade", 2, cvar.visual_skins);
	ReplaceTextureIndex("view_glove", HANDS38, "v_shield_usp", 2, cvar.visual_skins);
	ReplaceTextureIndex("view_finger", HANDS39, "v_ak47", 3, cvar.visual_skins);
	ReplaceTextureIndex("view_finger", HANDS40, "v_aug", 3, cvar.visual_skins);

	ReplaceTextureIndex("view_finger", HANDS41, "v_awp", 3, cvar.visual_skins);
	ReplaceTextureIndex("view_finger", HANDS42, "v_c4", 3, cvar.visual_skins);
	ReplaceTextureIndex("view_finger", HANDS43, "v_deagle", 3, cvar.visual_skins);
	ReplaceTextureIndex("view_finger", HANDS44, "v_elite", 3, cvar.visual_skins);
	ReplaceTextureIndex("view_finger", HANDS45, "v_famas", 3, cvar.visual_skins);
	ReplaceTextureIndex("view_finger", HANDS46, "v_fiveseven", 3, cvar.visual_skins);
	ReplaceTextureIndex("view_finger", HANDS47, "v_flashbang", 3, cvar.visual_skins);
	ReplaceTextureIndex("view_finger", HANDS48, "v_g3sg1", 3, cvar.visual_skins);
	ReplaceTextureIndex("view_finger", HANDS49, "v_galil", 3, cvar.visual_skins);
	ReplaceTextureIndex("view_finger", HANDS50, "v_glock18", 3, cvar.visual_skins);

	ReplaceTextureIndex("view_finger", HANDS51, "v_hegrenade", 3, cvar.visual_skins);
	ReplaceTextureIndex("view_finger", HANDS52, "v_knife", 3, cvar.visual_skins);
	ReplaceTextureIndex("view_finger", HANDS53, "v_m3", 3, cvar.visual_skins);
	ReplaceTextureIndex("view_finger", HANDS54, "v_m4a1", 3, cvar.visual_skins);
	ReplaceTextureIndex("view_finger", HANDS55, "v_m249", 3, cvar.visual_skins);
	ReplaceTextureIndex("view_finger", HANDS56, "v_mac10", 3, cvar.visual_skins);
	ReplaceTextureIndex("view_finger", HANDS57, "v_mp5", 3, cvar.visual_skins);
	ReplaceTextureIndex("view_finger", HANDS58, "v_p90", 3, cvar.visual_skins);
	ReplaceTextureIndex("view_finger", HANDS59, "v_p228", 3, cvar.visual_skins);
	ReplaceTextureIndex("view_finger", HANDS60, "v_scout", 3, cvar.visual_skins);

	ReplaceTextureIndex("view_finger", HANDS61, "v_sg550", 3, cvar.visual_skins);
	ReplaceTextureIndex("view_finger", HANDS62, "v_sg552", 3, cvar.visual_skins);
	ReplaceTextureIndex("view_finger", HANDS63, "v_smokegrenade", 3, cvar.visual_skins);
	ReplaceTextureIndex("view_finger", HANDS64, "v_tmp", 3, cvar.visual_skins);
	ReplaceTextureIndex("view_finger", HANDS65, "v_ump45", 3, cvar.visual_skins);
	ReplaceTextureIndex("view_finger", HANDS66, "v_usp", 3, cvar.visual_skins);
	ReplaceTextureIndex("view_finger", HANDS67, "v_xm1014", 3, cvar.visual_skins);
	ReplaceTextureIndex("view_finger", HANDS68, "v_shield_deagle", 2, cvar.visual_skins);
	ReplaceTextureIndex("view_finger", HANDS69, "v_shield_fiveseven", 2, cvar.visual_skins);
	ReplaceTextureIndex("view_finger", HANDS70, "v_shield_flashbang", 2, cvar.visual_skins);

	ReplaceTextureIndex("view_finger", HANDS71, "v_shield_glock18", 2, cvar.visual_skins);
	ReplaceTextureIndex("view_finger", HANDS72, "v_shield_hegrenade", 2, cvar.visual_skins);
	ReplaceTextureIndex("view_finger", HANDS73, "v_shield_knife", 2, cvar.visual_skins);
	ReplaceTextureIndex("view_finger", HANDS74, "v_shield_p228", 2, cvar.visual_skins);
	ReplaceTextureIndex("view_finger", HANDS75, "v_shield_smokegrenade", 2, cvar.visual_skins);
	ReplaceTextureIndex("view_finger", HANDS76, "v_shield_usp", 2, cvar.visual_skins);
	ReplaceTextureIndex("view_skin", HANDS77, "v_ak47", 3, cvar.visual_skins);
	ReplaceTextureIndex("view_skin", HANDS78, "v_aug", 3, cvar.visual_skins);
	ReplaceTextureIndex("view_skin", HANDS79, "v_awp", 3, cvar.visual_skins);
	ReplaceTextureIndex("view_skin", HANDS80, "v_c4", 3, cvar.visual_skins);

	ReplaceTextureIndex("view_skin", HANDS81, "v_deagle", 3, cvar.visual_skins);
	ReplaceTextureIndex("view_skin", HANDS82, "v_elite", 3, cvar.visual_skins);
	ReplaceTextureIndex("view_skin", HANDS83, "v_famas", 3, cvar.visual_skins);
	ReplaceTextureIndex("view_skin", HANDS84, "v_fiveseven", 3, cvar.visual_skins);
	ReplaceTextureIndex("view_skin", HANDS85, "v_flashbang", 3, cvar.visual_skins);
	ReplaceTextureIndex("view_skin", HANDS86, "v_g3sg1", 3, cvar.visual_skins);
	ReplaceTextureIndex("view_skin", HANDS87, "v_galil", 3, cvar.visual_skins);
	ReplaceTextureIndex("view_skin", HANDS88, "v_glock18", 3, cvar.visual_skins);
	ReplaceTextureIndex("view_skin", HANDS89, "v_hegrenade", 3, cvar.visual_skins);
	ReplaceTextureIndex("view_skin", HANDS90, "v_knife", 3, cvar.visual_skins);

	ReplaceTextureIndex("view_skin", HANDS91, "v_m3", 3, cvar.visual_skins);
	ReplaceTextureIndex("view_skin", HANDS92, "v_m4a1", 3, cvar.visual_skins);
	ReplaceTextureIndex("view_skin", HANDS93, "v_m249", 3, cvar.visual_skins);
	ReplaceTextureIndex("view_skin", HANDS94, "v_mac10", 3, cvar.visual_skins);
	ReplaceTextureIndex("view_skin", HANDS95, "v_mp5", 3, cvar.visual_skins);
	ReplaceTextureIndex("view_skin", HANDS96, "v_p90", 3, cvar.visual_skins);
	ReplaceTextureIndex("view_skin", HANDS97, "v_p228", 3, cvar.visual_skins);
	ReplaceTextureIndex("view_skin", HANDS98, "v_scout", 3, cvar.visual_skins);
	ReplaceTextureIndex("view_skin", HANDS99, "v_sg550", 3, cvar.visual_skins);
	ReplaceTextureIndex("view_skin", HANDS100, "v_sg552", 3, cvar.visual_skins);

	ReplaceTextureIndex("view_skin", HANDS101, "v_smokegrenade", 3, cvar.visual_skins);
	ReplaceTextureIndex("view_skin", HANDS102, "v_tmp", 3, cvar.visual_skins);
	ReplaceTextureIndex("view_skin", HANDS103, "v_ump45", 3, cvar.visual_skins);
	ReplaceTextureIndex("view_skin", HANDS104, "v_usp", 3, cvar.visual_skins);
	ReplaceTextureIndex("view_skin", HANDS105, "v_xm1014", 3, cvar.visual_skins);
	ReplaceTextureIndex("view_skin", HANDS106, "v_shield_deagle", 2, cvar.visual_skins);
	ReplaceTextureIndex("view_skin", HANDS107, "v_shield_fiveseven", 2, cvar.visual_skins);
	ReplaceTextureIndex("view_skin", HANDS108, "v_shield_flashbang", 2, cvar.visual_skins);
	ReplaceTextureIndex("view_skin", HANDS109, "v_shield_glock18", 2, cvar.visual_skins);
	ReplaceTextureIndex("view_skin", HANDS110, "v_shield_hegrenade", 2, cvar.visual_skins);

	ReplaceTextureIndex("view_skin", HANDS111, "v_shield_knife", 2, cvar.visual_skins);
	ReplaceTextureIndex("view_skin", HANDS112, "v_shield_p228", 2, cvar.visual_skins);
	ReplaceTextureIndex("view_skin", HANDS113, "v_shield_smokegrenade", 2, cvar.visual_skins);
	ReplaceTextureIndex("view_skin", HANDS114, "v_shield_usp", 2, cvar.visual_skins);

	ReplaceTextureIndex("rifle_goldshell", SHELL1, "rshell", 3, cvar.visual_skins);
	ReplaceTextureIndex("pistol_goldshell", SHELL2, "pshell", 3, cvar.visual_skins);
	ReplaceTextureIndex("shell", SHELL3, "shotgunshell", 3, cvar.visual_skins);
	ReplaceTextureIndex("rifle_goldshell", SHELL4, "rshell_big", 3, cvar.visual_skins);

	ReplaceTextureIndex("skin", CHICKEN, "chick", 3, cvar.visual_skins);

	ReplaceTextureIndex("newsvip", VIPBODY, "vip", 1, cvar.visual_skins);
	ReplaceTextureIndex("head2", VIPHEAD, "vip", 1, cvar.visual_skins);

	ReplaceTextureIndex("vip_militant_body", VIPBODY2, "vip", 1, cvar.visual_skins);
	ReplaceTextureIndex("vip_militant_head", VIPHEAD2, "vip", 1, cvar.visual_skins);

	ReplaceTextureIndex("arctic_body", ARCTICBODY, "arctic", 1, cvar.visual_skins);
	ReplaceTextureIndex("arctic_head2", ARCTICHEAD, "arctic", 1, cvar.visual_skins);

	ReplaceTextureIndex("body", GIGNBODY, "gign", 1, cvar.visual_skins);
	ReplaceTextureIndex("helmet", GIGNHEAD, "gign", 1, cvar.visual_skins);

	ReplaceTextureIndex("gsg9_body", GSG9BODY, "gsg9", 1, cvar.visual_skins);
	ReplaceTextureIndex("gsg9_head", GSG9HEAD, "gsg9", 1, cvar.visual_skins);

	ReplaceTextureIndex("guerilla_body", GUERILLABODY, "guerilla", 1, cvar.visual_skins);
	ReplaceTextureIndex("guerilla_head", GUERILLAHEAD, "guerilla", 1, cvar.visual_skins);

	ReplaceTextureIndex("leet_body5", LEETBODY, "leet", 1, cvar.visual_skins);
	ReplaceTextureIndex("guerilla_head3", LEETHEAD, "leet", 1, cvar.visual_skins);

	ReplaceTextureIndex("SAS_body", SASBODY, "sas", 1, cvar.visual_skins);
	ReplaceTextureIndex("SAS_head", SASHEAD, "sas", 1, cvar.visual_skins);

	ReplaceTextureIndex("terror_body", TERRORBODY, "terror", 1, cvar.visual_skins);
	ReplaceTextureIndex("terror_head3", TERRORHEAD, "terror", 1, cvar.visual_skins);

	ReplaceTextureIndex("seal_body", URBANBODY, "urban", 1, cvar.visual_skins);
	ReplaceTextureIndex("seal_head", URBANHEAD, "urban", 1, cvar.visual_skins);
	ReplaceTextureIndex("seal_chrome", URBANCHROME, "urban", 1, cvar.visual_skins);
}