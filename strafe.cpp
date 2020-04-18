#include "client.h"

float FrameCount;
float FpsCount;
float InterpFps;
float PreStrafe;
float JumpOff;

float Keyforwardm = false;
float Keymoveleft = false;
float Keymoveright = false;
float Keyback = false;
float Keyduck = false;
float Keyjump = false;

bool Strafe = false;
bool Fastrun = false;
bool Gstrafe = false;
bool Bhop = false;
bool Jumpbug = false;

float YawForVec(float* fwd)
{
	if (fwd[1] == 0 && fwd[0] == 0)
		return 0;
	else
	{
		float yaw = (atan2(fwd[1], fwd[0]) * 180 / M_PI);
		if (yaw < 0)yaw += 360;
		return yaw;
	}
}

void StrafeHack(float frametime, struct usercmd_s *cmd)
{
	if(Strafe && !(g_Local.iFlags & FL_ONGROUND) && (g_Local.iMovetype != MOVETYPE_FLY) && !(cmd->buttons&IN_ATTACK) && !(cmd->buttons & IN_ATTACK2 && IsCurWeaponKnife()))
	{
		if(g_Local.flVelocityspeed < 15)
		{	
			cmd->forwardmove=400;
			cmd->sidemove=0;
		}

		float dir = 0;
		if (cmd->buttons & IN_MOVERIGHT)
			dir = 90;
		if (cmd->buttons & IN_BACK)
			dir = 180;
		if (cmd->buttons & IN_MOVELEFT)
			dir = -90;

		Vector va_real;
		g_Engine.GetViewAngles(va_real);
		va_real.y+=dir;
		float vspeed[3]={ g_Local.vVelocity.x/g_Local.flVelocity,g_Local.vVelocity.y/g_Local.flVelocity,0.0f};
		float va_speed=YawForVec(vspeed);
		float adif=va_speed-va_real[1];
		while(adif<-180)adif+=360;
		while(adif>180)adif-=360;
		cmd->sidemove=(437.8928)*(adif>0?1:-1);
		cmd->forwardmove=0;
		bool onlysidemove=(abs(adif)>=atan(30.f/g_Local.flVelocityspeed)/M_PI*180);
		cmd->viewangles[1] -= (-adif); 
		float fs = 0;
		if(!onlysidemove)
		{
			static float lv=0;
			Vector fw=g_Local.vStrafeForward;fw[2]=0;fw=fw.Normalize();
			float vel=POW(fw[0]* g_Local.vVelocity[0])+POW(fw[1]* g_Local.vVelocity[1]);

			fs=lv;
			lv=sqrt(69.f * 100000 / vel);
			static float lastang=0;
			float ca=abs(adif);		
			lastang=ca;
		}

		float ang = atan(fs/cmd->sidemove)/M_PI*180;
		cmd->viewangles.y+=ang;

		float sdmw=cmd->sidemove;
		float fdmw=cmd->forwardmove;

		if(cmd->buttons & IN_MOVERIGHT)
		{
			cmd->forwardmove = -sdmw;
			cmd->sidemove = fdmw;
		}
		if(cmd->buttons & IN_BACK)
		{
			cmd->forwardmove = -fdmw;
			cmd->sidemove = -sdmw;
		}
		if(cmd->buttons & IN_MOVELEFT)
		{
			cmd->forwardmove = sdmw;
			cmd->sidemove = -fdmw;
		}
	}
}

void FastRun(struct usercmd_s *cmd)
{
	if(Fastrun && g_Local.flVelocityspeed && g_Local.flFallVelocity == 0 && !Gstrafe && g_Local.iFlags&FL_ONGROUND)
	{
		static bool Run = false;
		if((cmd->buttons&IN_FORWARD && cmd->buttons&IN_MOVELEFT) || (cmd->buttons&IN_BACK && cmd->buttons&IN_MOVERIGHT))
		{
			if (Run)
			{
				Run = false;
				cmd->sidemove -= 89.6f;
				cmd->forwardmove -= 89.6f;
			}
			else
			{ 
				Run = true;
				cmd->sidemove += 89.6f; 
				cmd->forwardmove += 89.6f; 
			}
		} 
		else if((cmd->buttons&IN_FORWARD && cmd->buttons&IN_MOVERIGHT) || (cmd->buttons&IN_BACK && cmd->buttons&IN_MOVELEFT))
		{
			if (Run)
			{ 
				Run = false;
				cmd->sidemove -= 89.6f; 
				cmd->forwardmove += 89.6f; 
			}
			else			
			{ 
				Run = true;
				cmd->sidemove += 89.6f; 
				cmd->forwardmove -= 89.6f; 
			}
		} 
		else if(cmd->buttons&IN_FORWARD || cmd->buttons&IN_BACK)
		{
			if (Run)
			{ 
				Run = false;
				cmd->sidemove -= 126.6f; 
			}
			else			
			{ 
				Run = true;
				cmd->sidemove += 126.6f; 
			}
		} 
		else if(cmd->buttons&IN_MOVELEFT || cmd->buttons&IN_MOVERIGHT)
		{
			if (Run)
			{ 
				Run = false;
				cmd->forwardmove -= 126.6f; 
			}
			else			
			{ 
				Run = true;
				cmd->forwardmove += 126.6f; 
			}
		}
	}
}

void GroundStrafe(struct usercmd_s *cmd)
{
	if(Gstrafe && !Jumpbug)
	{
		static int gs_state = 0;
		if(gs_state == 0 && g_Local.iFlags&FL_ONGROUND)
		{
			cmd->buttons |=IN_DUCK;
			gs_state = 1;
		}
		else if(gs_state == 1)
		{
			cmd->buttons &= ~IN_DUCK;
			gs_state = 0;
		}
	}
}

void BHop(float frametime, struct usercmd_s *cmd)
{
	static bool lastFramePressedJump=false;
	static bool JumpInNextFrame=false;
	static float kz_bhop_cnt_rand_min = 0.f;
	static float kz_bhop_cnt_rand_max = 12.f;
	static int inAirBhopCnt = 0;

	bool isJumped = false;

	if(JumpInNextFrame)
	{
		JumpInNextFrame=false;
		cmd->buttons|=IN_JUMP;
		goto bhopfuncend;
	}
	if(Bhop && !Gstrafe)
	{
		cmd->buttons &= ~IN_JUMP;
		if(((!lastFramePressedJump)|| g_Local.iFlags&FL_ONGROUND || g_Local.flWaterlevel >= 2 || g_Local.iMovetype == MOVETYPE_FLY || g_Local.flHeightorigin<=2) && !Jumpbug)
		{
			static int bhop_jump_number = 0;
			bhop_jump_number++;
			if (bhop_jump_number >= 2)
			{
				bhop_jump_number = 0;
				JumpInNextFrame = true;
				goto bhopfuncend;
			}

			if(kz_bhop_cnt_rand_min > kz_bhop_cnt_rand_max)
				kz_bhop_cnt_rand_min = kz_bhop_cnt_rand_max;

			if(kz_bhop_cnt_rand_min <= 1)
				kz_bhop_cnt_rand_min = 1;

			if(kz_bhop_cnt_rand_min > 20)
				kz_bhop_cnt_rand_min = 20;

			if(kz_bhop_cnt_rand_max > 20)
				kz_bhop_cnt_rand_max = 20;

			if(kz_bhop_cnt_rand_max <= 1)
				kz_bhop_cnt_rand_max = 1;

			inAirBhopCnt = (int)g_Engine.pfnRandomFloat((int)kz_bhop_cnt_rand_min,(int)kz_bhop_cnt_rand_max);

			isJumped=true;
			cmd->buttons |= IN_JUMP;
		} 
	}
	if(!isJumped)
	{
		if(inAirBhopCnt>0)
		{
			if(inAirBhopCnt%2==0) 
			{
				cmd->buttons |= IN_JUMP;
			}
			else cmd->buttons &= ~IN_JUMP;
			inAirBhopCnt--;
		}
	}
    bhopfuncend:
	lastFramePressedJump = cmd->buttons&IN_JUMP;
}

double _my_abs(double n) 
{
	if (n >= 0)return n; //if positive, return without ant change
	else return 0 - n; //if negative, return a positive version
}

void JumpBug(float frametime, struct usercmd_s *cmd)
{
	static int state=0;

	bool autojb=false;
	if(cvar.kz_jump_bug_auto && g_Local.flFallVelocity>=404.8f)
		if(g_Local.flHeightorigin-(g_Local.flFallVelocity*frametime/cvar.misc_wav_speed *15)<=0)
			autojb=true;

	if((Jumpbug||autojb)&& g_Local.flFallVelocity>0)
	{
		bool curveang=false;
		float fpheight=0;
		if(g_Local.flGroundangle>1)
		{
			curveang=true;
			Vector vTemp = g_Local.vOrigin;
			vTemp[2] -= 8192;
			pmtrace_t *trace =  g_pEngine->PM_TraceLine(g_Local.vOrigin, vTemp, 1, 2, -1);
			fpheight=abs(g_Local.vOrigin.z-trace->endpos.z-(g_Local.iUsehull==1?18.0f:36.0f));
		}
		else fpheight=g_Local.flHeightorigin;
		

		static float last_h=0.0f;
		float cur_frame_zdist=abs((g_Local.flFallVelocity+(800*frametime))*frametime);
		cmd->buttons|=IN_DUCK;
		cmd->buttons&=~IN_JUMP;
		switch(state)
		{
		case 1:
			cmd->buttons&=~IN_DUCK;
			cmd->buttons|=IN_JUMP;
			state=2;
			break;
		case 2:
			state=0;
			break;
		default:
			if(_my_abs(fpheight-cur_frame_zdist*1.5)<=(20.0)&&cur_frame_zdist>0.0f)
			{
				float needspd=_my_abs(fpheight-(19.0));
				float scale=abs(needspd/cur_frame_zdist);
				AdjustSpeed(scale);
				state=1;
			}
			break;
		}
		last_h=fpheight;
	}
	else state=0;
}

void Kz(float frametime, struct usercmd_s *cmd)
{
	if (g_Local.bAlive)
	{
		if (cvar.kz_strafe)
			StrafeHack(frametime, cmd);
		if (cvar.kz_fast_run)
			FastRun(cmd);
		if (cvar.kz_gstrafe)
			GroundStrafe(cmd);
		if (cvar.kz_bhop)
			BHop(frametime, cmd);
		if (cvar.kz_jump_bug)
			JumpBug(frametime, cmd);
	}
	if (g_Local.bJumped && (g_Local.iFlags & FL_ONGROUND || g_Local.iMovetype == MOVETYPE_FLY))
	{
		Vector endpos = g_Local.vOrigin;
		endpos.z -= g_Local.iUsehull == 0 ? 36.0 : 18.0;
		g_Local.vT2 = endpos;
		if (endpos.z == g_Local.vStartjumppos.z)
		{
			Vector lj = endpos - g_Local.vStartjumppos;
			float dist = lj.Length() + 32.0625f + 0.003613;
			if (dist >= 200)
			{
				InterpFps = FpsCount / FrameCount;
				FpsCount = 0;
				FrameCount = 0;
				g_Local.flJumpdist = dist;
				g_Local.flJumpmesstime = GetTickCount() + 10000;
			}
		}
		g_Local.bJumped = false;
	}
	if (!g_Local.bJumped && (g_Local.iFlags & FL_ONGROUND) && cmd->buttons & IN_JUMP)
	{
		PreStrafe = g_Local.flVelocityspeed;
		if (g_Local.flEdgeDistance != 250)
			JumpOff = g_Local.flEdgeDistance;
		else JumpOff = 0;
		g_Local.vStartjumppos = g_Local.vOrigin;
		g_Local.vStartjumppos.z -= g_Local.iUsehull == 0 ? 36.0 : 18.0;
		g_Local.vT1 = g_Local.vStartjumppos;
		g_Local.bJumped = true;
	}
	if (cmd->buttons & IN_FORWARD) { Keyforwardm = true; }
	else { Keyforwardm = false; }
	if (cmd->buttons & IN_MOVELEFT) { Keymoveleft = true; }
	else { Keymoveleft = false; }
	if (cmd->buttons & IN_MOVERIGHT) { Keymoveright = true; }
	else { Keymoveright = false; }
	if (cmd->buttons & IN_BACK) { Keyback = true; }
	else { Keyback = false; }
	if (cmd->buttons & IN_DUCK) { Keyduck = true; }
	else { Keyduck = false; }
	if (cmd->buttons & IN_JUMP) { Keyjump = true;; }
	else { Keyjump = false; }
}

void KzFameCount()
{
	if (g_Local.bJumped)
	{
		FrameCount += 1;
		FpsCount += (1 / g_Local.flFrametime);
	}
}

void DrawLongJump()
{
	float WindowBorderSize = ImGui::GetStyle().WindowBorderSize;
	ImGui::GetStyle().WindowBorderSize = 0.0f;
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
	ImGui::Begin("Longjump", reinterpret_cast<bool*>(true), ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs);
	{
		ImGui::SetWindowPos(ImVec2(0, 0), ImGuiSetCond_Always);
		ImGui::SetWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y), ImGuiSetCond_Always);

		if (cvar.kz_show_kz && g_Local.bAlive && CheckDraw() && g_pEngine->GetMaxClients())
		{
			float ft1[2];
			float ft2[2];
			if (g_Local.flJumpmesstime > GetTickCount())
			{
				if (WorldToScreen(g_Local.vT1, ft1) && WorldToScreen(g_Local.vT2, ft2))
				{
					int ScreenX = ft1[0], ScreenY = ft1[1];
					ImGui::GetCurrentWindow()->DrawList->AddLine({ ft1[0] + 1, ft1[1] + 1 }, { ft2[0] + 1, ft2[1] + 1 }, black());
					ImGui::GetCurrentWindow()->DrawList->AddLine({ ft1[0], ft1[1] }, { ft2[0], ft2[1] }, green());
				}

				if (WorldToScreen(g_Local.vT1, ft1))
				{
					int label_size = ImGui::CalcTextSize("Start", NULL, true).x;
					int ScreenX = ft1[0], ScreenY = ft1[1];
					ImGui::GetCurrentWindow()->DrawList->AddRect({ (float)ScreenX - label_size / 2 - 2, (float)ScreenY - 23 }, { (float)ScreenX - label_size / 2 + label_size + 4, (float)ScreenY - 9 }, black(), cvar.visual_rounding);
					ImGui::GetCurrentWindow()->DrawList->AddRect({ (float)ScreenX - label_size / 2 - 3, (float)ScreenY - 24 }, { (float)ScreenX - label_size / 2 + label_size + 3, (float)ScreenY - 10}, wheel1(), cvar.visual_rounding);
					ImGui::GetCurrentWindow()->DrawList->AddText({ (float)ScreenX - label_size / 2, (float)ScreenY - 25 }, white(), "Start");
				}

				if (WorldToScreen(g_Local.vT2, ft2))
				{
					int label_size = ImGui::CalcTextSize("Stop", NULL, true).x;
					int ScreenX = ft2[0], ScreenY = ft2[1];
					ImGui::GetCurrentWindow()->DrawList->AddRect({ (float)ScreenX - label_size / 2 - 2, (float)ScreenY - 23 }, { (float)ScreenX - label_size / 2 + label_size + 4, (float)ScreenY - 9 }, black(), cvar.visual_rounding);
					ImGui::GetCurrentWindow()->DrawList->AddRect({ (float)ScreenX - label_size / 2 - 3, (float)ScreenY - 24 }, { (float)ScreenX - label_size / 2 + label_size + 3, (float)ScreenY - 10}, wheel1(), cvar.visual_rounding);
					ImGui::GetCurrentWindow()->DrawList->AddText({ (float)ScreenX - label_size / 2, (float)ScreenY - 25 }, white(), "Stop");
				}
			}
		}
		ImGui::End();
		ImGui::PopStyleColor();
		ImGui::GetStyle().WindowBorderSize = WindowBorderSize;
	}
}

void DrawKzKeys()
{
	if (cvar.kz_show_kz && g_Local.bAlive && CheckDraw() && g_pEngine->GetMaxClients())
	{
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.25f));
		ImGui::SetNextWindowPos(ImVec2(167, (ImGui::GetIO().DisplaySize.y / 2) - ImGui::GetIO().DisplaySize.y / 20 * 1), ImGuiCond_FirstUseEver);
		if (ImGui::Begin("kz-show-keys", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("%s", Keyforwardm ? "  W" : "  |");
			ImGui::Text("%s %s %s", Keymoveleft ? "A" : "-", Keyback ? "S" : "+", Keymoveright ? "D" : "-");
			ImGui::Text(Keyduck ? " Duck" : "  |");
			ImGui::Text(Keyjump ? " Jump" : "  |");

			ImGui::PopStyleColor();
			ImGui::End();
		}
	}
}

void DrawKzWindow()
{
	if (cvar.kz_show_kz && g_Local.bAlive && CheckDraw() && g_pEngine->GetMaxClients())
	{
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.25f));
		ImGui::SetNextWindowPos(ImVec2(20, (ImGui::GetIO().DisplaySize.y / 2) - ImGui::GetIO().DisplaySize.y / 20 * 1), ImGuiCond_Once);
		ImGui::Begin("kz-show-kz", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);
		{
			ImVec4 col_default_text = ImGui::GetStyleColorVec4(ImGuiCol_Text);
			ImVec4 col = col_default_text;

			static float fMaxPspeed = 0.f;
			if (g_Local.flVelocityspeed == 0)
				fMaxPspeed = 0.0;
			if (g_Local.flVelocityspeed > fMaxPspeed)
				fMaxPspeed = g_Local.flVelocityspeed;
			if (g_Local.flDamage >= g_Local.iHealthStrafe)
				col = ImColor(1.f, 0.f, 0.f, 1.0f);

			if (g_Local.flJumpmesstime > GetTickCount())
			{
				ImGui::TextColored(ImVec4(1.f, 0.f, 1.f, 1.f), "Time: %.1f", (g_Local.flJumpmesstime - GetTickCount()) / 1000);
				ImGui::TextColored(ImVec4(1.f, 0.f, 1.f, 1.f), "LongJump: %.3f", g_Local.flJumpdist);
				ImGui::TextColored(ImVec4(1.f, 0.f, 1.f, 1.f), "PreStrafe: %.3f", PreStrafe);
				ImGui::TextColored(ImVec4(1.f, 0.f, 1.f, 1.f), "JumpOff: %.3f", JumpOff);
				ImGui::TextColored(ImVec4(1.f, 0.f, 1.f, 1.f), "Fps: %.1f", InterpFps);
			}

			ImGui::Text("Speed: %.3f", g_Local.flVelocityspeed);
			ImGui::Text("SpeedMax: %.3f", fMaxPspeed);
			ImGui::Text("Height: %.3f", g_Local.flHeightorigin);
			ImGui::Text("FallSpeed: %.3f", g_Local.flFallVelocity);
			ImGui::Text("GroundAngle: %.3f", g_Local.flGroundangle);
			ImGui::TextColored(col, "Damage: %.1f", g_Local.flDamage);
			ImGui::TextColored(ImVec4(1.f, 0.f, g_Local.flEdgeDistance, 1.f), "Edge: %.3f", g_Local.flEdgeDistance);

			ImGui::PopStyleColor();
			ImGui::End();
		}
	}
}

inline float EndSpeed(float StartSpeed, float gravity, float frametime, float distance)
{
	while (distance > 0)
	{
		StartSpeed += gravity * frametime;
		float dist = StartSpeed * frametime;
		distance -= dist;
	}
	return StartSpeed;
}

inline float interp(float s1, float s2, float s3, float f1, float f3)
{
	if (s2 == s1)return f1;
	if (s2 == s3)return f3;
	if (s3 == s1)return f1;
	return f1 + ((s2 - s1) / (s3 - s1))*((f3 - f1)/*/1*/);
}

float Damage()
{
	Vector start = pmove->origin;
	vec3_t vForward, vecEnd;
	float va[3];
	g_Engine.GetViewAngles(va);
	g_Engine.pfnAngleVectors(va, vForward, NULL, NULL);
	vecEnd[0] = start[0] + vForward[0] * 8192; vecEnd[1] = start[1] + vForward[1] * 8192; vecEnd[2] = start[2] + vForward[2] * 8192;
	pmtrace_t *trace = g_pEngine->PM_TraceLine(start, vecEnd, 1, 2, -1);
	float fDistance = ((start.z) - (trace->endpos.z)) - (pmove->usehull == 0 ? (36) : (18));
	float endSpeed = EndSpeed(pmove->flFallVelocity, 800, 1 / 1000.0f, fDistance);
	if (interp(504.80001f, endSpeed, 1000, 1, 100) > 0)
		return interp(504.80001f, endSpeed, 1000, 1, 100);
	else return 0;
}

inline float EdgeDistance() {
#define TraceEdge(x,y){\
    Vector start=pmove->origin;\
	start[2]-=0.1f;\
	Vector end=start;\
	end[1]+=x*mind;\
	end[0]+=y*mind;\
	pmtrace_s* t1 = g_pEngine->PM_TraceLine(end,start,1,pmove->usehull,-1);\
	if(!(t1->startsolid))mind=(t1->endpos-start).Length2D();\
	}
	float mind = 250;
	TraceEdge(-1, 0);
	TraceEdge(1, 0);
	TraceEdge(0, 1);
	TraceEdge(0, -1);
	TraceEdge(-1, -1);
	TraceEdge(1, 1);
	TraceEdge(1, -1);
	TraceEdge(-1, 1);
	return mind;
}