#include "client.h"

ImColor red() { return ImColor(1.f, 0.f, 0.f, cvar.visual_alpha); }
ImColor green() { return ImColor(0.f, 1.f, 0.f, cvar.visual_alpha); }
ImColor blue() { return ImColor(0.f, 0.f, 1.f, cvar.visual_alpha); }
ImColor black() { return ImColor(0.f, 0.f, 0.f, cvar.visual_alpha); }
ImColor white() { return ImColor(1.f, 1.f, 1.f, cvar.visual_alpha); }
ImColor wheel1() { return  ImColor(cvar.color_red, cvar.color_green, cvar.color_blue, cvar.visual_alpha); }
ImColor wheel2() { return  ImColor(cvar.color_green, cvar.color_blue, cvar.color_red, cvar.visual_alpha); }
ImColor wheel3() { return  ImColor(cvar.color_blue, cvar.color_red, cvar.color_green, cvar.visual_alpha); }

ImColor team(int i)
{
	if (cvar.rage_active && i == iTargetRage ||
		!cvar.rage_active && cvar.legit[g_Local.weapon.m_iWeaponID].active && i == iTargetLegit ||
		!cvar.rage_active && cvar.legit[g_Local.weapon.m_iWeaponID].trigger_active && i == iTargetTrigger ||
		!cvar.rage_active && cvar.knifebot_active && i == iTargetKnife &&
		g_Local.bAlive && !IsCurWeaponNonAttack())
		return green();
	if (g_Player[i].iTeam == 1)
		return red();
	else if (g_Player[i].iTeam == 2)
		return blue();
	else
		return white();
}