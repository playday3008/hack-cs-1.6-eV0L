#include "client.h"

bool ReloadBar(int i, int BotX, int BotY, int box_height, ImU32 team, ImU32 green, ImU32 black)
{
	int seqinfo = Cstrike_SequenceInfo[g_Player[i].pEnt->curstate.sequence];

	if (cvar.visual_reload_bar && seqinfo == 2 && g_Player[i].bAlive)
	{
		int label_size = ImGui::CalcTextSize("Reloading", NULL, true).x;
		ImGui::GetCurrentWindow()->DrawList->AddRect({ (float)BotX - label_size / 2 - 2, (float)BotY + box_height - 13 }, { (float)BotX - label_size / 2 + label_size + 4, (float)BotY + box_height+1 }, black, cvar.visual_rounding);
		ImGui::GetCurrentWindow()->DrawList->AddRect({ (float)BotX - label_size / 2 - 3, (float)BotY + box_height - 14 }, { (float)BotX - label_size / 2 + label_size + 3, (float)BotY + box_height }, team, cvar.visual_rounding);
		ImGui::GetCurrentWindow()->DrawList->AddText({ (float)BotX - label_size / 2, (float)BotY + box_height - 15 }, green, "Reloading");
		return true;
	}
	return false;
}

bool Vip(int i, int BotX, int BotY, int box_height, ImU32 team, ImU32 green, ImU32 black)
{
	if (cvar.visual_vip && g_Player[i].vip)
	{
		int label_size = ImGui::CalcTextSize("VIP", NULL, true).x;
		ImGui::GetCurrentWindow()->DrawList->AddRect({ (float)BotX - label_size / 2 - 2, (float)BotY + box_height - 13 }, { (float)BotX - label_size / 2 + label_size + 4, (float)BotY + box_height + 1 }, black, cvar.visual_rounding);
		ImGui::GetCurrentWindow()->DrawList->AddRect({ (float)BotX - label_size / 2 - 3, (float)BotY + box_height - 14 }, { (float)BotX - label_size / 2 + label_size + 3, (float)BotY + box_height }, team, cvar.visual_rounding);
		ImGui::GetCurrentWindow()->DrawList->AddText({ (float)BotX - label_size / 2, (float)BotY + box_height - 15 }, green, "VIP");
		return true;
	}
	return false;
}

bool Weapon(int i, int BotX, int BotY, int box_height, ImU32 team, ImU32 black, ImU32 white)
{
	if (cvar.visual_weapon && g_Player[i].pEnt->curstate.weaponmodel)
	{
		int label_size = ImGui::CalcTextSize(GetWeaponName(g_Player[i].pEnt->curstate.weaponmodel), NULL, true).x;
		ImGui::GetCurrentWindow()->DrawList->AddRect({ (float)BotX - label_size / 2 - 2, (float)BotY + box_height - 13 }, { (float)BotX - label_size / 2 + label_size + 4, (float)BotY + box_height + 1 }, black, cvar.visual_rounding);
		ImGui::GetCurrentWindow()->DrawList->AddRect({ (float)BotX - label_size / 2 - 3, (float)BotY + box_height - 14 }, { (float)BotX - label_size / 2 + label_size + 3, (float)BotY + box_height }, team, cvar.visual_rounding);
		ImGui::GetCurrentWindow()->DrawList->AddText({ (float)BotX - label_size / 2, (float)BotY + box_height - 15 }, white, GetWeaponName(g_Player[i].pEnt->curstate.weaponmodel));
		return true;
	}
	return false;
}

bool Name(int i, int BotX, int BotY, int box_height, ImU32 team, ImU32 black, ImU32 white)
{
	if (cvar.visual_name && g_Player[i].entinfo.name)
	{
		int label_size = ImGui::CalcTextSize(g_Player[i].entinfo.name, NULL, true).x;
		ImGui::GetCurrentWindow()->DrawList->AddRect({ (float)BotX - label_size / 2 - 2, (float)BotY + box_height - 13 }, { (float)BotX - label_size / 2 + label_size + 4, (float)BotY + box_height + 1 }, black, cvar.visual_rounding);
		ImGui::GetCurrentWindow()->DrawList->AddRect({ (float)BotX - label_size / 2 - 3, (float)BotY + box_height - 14 }, { (float)BotX - label_size / 2 + label_size + 3, (float)BotY + box_height }, team, cvar.visual_rounding);
		ImGui::GetCurrentWindow()->DrawList->AddText({ (float)BotX - label_size / 2, (float)BotY + box_height - 15 }, white, g_Player[i].entinfo.name);
		return true;
	}
	return false;
}

void HealthBar(int i, int BotX, int BotY, int box_width, int box_height, ImU32 black)
{
	if (cvar.visual_health)
	{
		int hp = g_Player[i].iHealth;
		
		if (hp < 10)
			hp = 10;
		else if (hp > 100)
			hp = 100;

		if (hp)      ImGui::GetCurrentWindow()->DrawList->AddRect({ (float)BotX + box_width - 5, BotY + ((float)box_height / 100 * 10) + 2 }, { (float)BotX + box_width + 1, (float)BotY + 1 }, black, cvar.visual_rounding);
		if (hp > 10) ImGui::GetCurrentWindow()->DrawList->AddRect({ (float)BotX + box_width - 5, BotY + ((float)box_height / 100 * 20) + 2 }, { (float)BotX + box_width + 1, BotY + ((float)box_height / 100 * 10) + 1 }, black, cvar.visual_rounding);
		if (hp > 20) ImGui::GetCurrentWindow()->DrawList->AddRect({ (float)BotX + box_width - 5, BotY + ((float)box_height / 100 * 30) + 2 }, { (float)BotX + box_width + 1, BotY + ((float)box_height / 100 * 20) + 1 }, black, cvar.visual_rounding);
		if (hp > 30) ImGui::GetCurrentWindow()->DrawList->AddRect({ (float)BotX + box_width - 5, BotY + ((float)box_height / 100 * 40) + 2 }, { (float)BotX + box_width + 1, BotY + ((float)box_height / 100 * 30) + 1 }, black, cvar.visual_rounding);
		if (hp > 40) ImGui::GetCurrentWindow()->DrawList->AddRect({ (float)BotX + box_width - 5, BotY + ((float)box_height / 100 * 50) + 2 }, { (float)BotX + box_width + 1, BotY + ((float)box_height / 100 * 40) + 1 }, black, cvar.visual_rounding);
		if (hp > 50) ImGui::GetCurrentWindow()->DrawList->AddRect({ (float)BotX + box_width - 5, BotY + ((float)box_height / 100 * 60) + 2 }, { (float)BotX + box_width + 1, BotY + ((float)box_height / 100 * 50) + 1 }, black, cvar.visual_rounding);
		if (hp > 60) ImGui::GetCurrentWindow()->DrawList->AddRect({ (float)BotX + box_width - 5, BotY + ((float)box_height / 100 * 70) + 2 }, { (float)BotX + box_width + 1, BotY + ((float)box_height / 100 * 60) + 1 }, black, cvar.visual_rounding);
		if (hp > 70) ImGui::GetCurrentWindow()->DrawList->AddRect({ (float)BotX + box_width - 5, BotY + ((float)box_height / 100 * 80) + 2 }, { (float)BotX + box_width + 1, BotY + ((float)box_height / 100 * 70) + 1 }, black, cvar.visual_rounding);
		if (hp > 80) ImGui::GetCurrentWindow()->DrawList->AddRect({ (float)BotX + box_width - 5, BotY + ((float)box_height / 100 * 90) + 2 }, { (float)BotX + box_width + 1, BotY + ((float)box_height / 100 * 80) + 1 }, black, cvar.visual_rounding);
		if (hp > 90) ImGui::GetCurrentWindow()->DrawList->AddRect({ (float)BotX + box_width - 5, BotY + (float)box_height + 2 }, { (float)BotX + box_width + 1, BotY + ((float)box_height / 100 * 90) + 1 }, black, cvar.visual_rounding);
		
		if (hp)      ImGui::GetCurrentWindow()->DrawList->AddRect({ (float)BotX + box_width - 6, BotY + ((float)box_height / 100 * 10) + 1 }, { (float)BotX + box_width, (float)BotY }, ImColor(1.f, 0.0f, 0.0f, cvar.visual_alpha), cvar.visual_rounding);
		if (hp > 10) ImGui::GetCurrentWindow()->DrawList->AddRect({ (float)BotX + box_width - 6, BotY + ((float)box_height / 100 * 20) + 1 }, { (float)BotX + box_width, BotY + (float)box_height / 100 * 10 }, ImColor(255 / 255.f, 51 / 255.f, 0 / 255.f, cvar.visual_alpha), cvar.visual_rounding);
		if (hp > 20) ImGui::GetCurrentWindow()->DrawList->AddRect({ (float)BotX + box_width - 6, BotY + ((float)box_height / 100 * 30) + 1 }, { (float)BotX + box_width, BotY + (float)box_height / 100 * 20 }, ImColor(255 / 255.f, 102 / 255.f, 0 / 255.f, cvar.visual_alpha), cvar.visual_rounding);
		if (hp > 30) ImGui::GetCurrentWindow()->DrawList->AddRect({ (float)BotX + box_width - 6, BotY + ((float)box_height / 100 * 40) + 1 }, { (float)BotX + box_width, BotY + (float)box_height / 100 * 30 }, ImColor(255 / 255.f, 153 / 255.f, 0 / 255.f, cvar.visual_alpha), cvar.visual_rounding);
		if (hp > 40) ImGui::GetCurrentWindow()->DrawList->AddRect({ (float)BotX + box_width - 6, BotY + ((float)box_height / 100 * 50) + 1 }, { (float)BotX + box_width, BotY + (float)box_height / 100 * 40 }, ImColor(255 / 255.f, 204 / 255.f, 0 / 255.f, cvar.visual_alpha), cvar.visual_rounding);
		if (hp > 50) ImGui::GetCurrentWindow()->DrawList->AddRect({ (float)BotX + box_width - 6, BotY + ((float)box_height / 100 * 60) + 1 }, { (float)BotX + box_width, BotY + (float)box_height / 100 * 50 }, ImColor(204 / 255.f, 255 / 255.f, 0 / 255.f, cvar.visual_alpha), cvar.visual_rounding);
		if (hp > 60) ImGui::GetCurrentWindow()->DrawList->AddRect({ (float)BotX + box_width - 6, BotY + ((float)box_height / 100 * 70) + 1 }, { (float)BotX + box_width, BotY + (float)box_height / 100 * 60 }, ImColor(153 / 255.f, 255 / 255.f, 0 / 255.f, cvar.visual_alpha), cvar.visual_rounding);
		if (hp > 70) ImGui::GetCurrentWindow()->DrawList->AddRect({ (float)BotX + box_width - 6, BotY + ((float)box_height / 100 * 80) + 1 }, { (float)BotX + box_width, BotY + (float)box_height / 100 * 70 }, ImColor(102 / 255.f, 255 / 255.f, 0 / 255.f, cvar.visual_alpha), cvar.visual_rounding);
		if (hp > 80) ImGui::GetCurrentWindow()->DrawList->AddRect({ (float)BotX + box_width - 6, BotY + ((float)box_height / 100 * 90) + 1 }, { (float)BotX + box_width, BotY + (float)box_height / 100 * 80 }, ImColor(102 / 255.f, 255 / 255.f, 0 / 255.f, cvar.visual_alpha), cvar.visual_rounding);
		if (hp > 90) ImGui::GetCurrentWindow()->DrawList->AddRect({ (float)BotX + box_width - 6, BotY + (float)box_height + 1 }, { (float)BotX + box_width, BotY + (float)box_height / 100 * 90 }, ImColor(0.f, 1.f, 0.f, cvar.visual_alpha), cvar.visual_rounding);
	}
}

void Box(int BotX, int BotY, int box_width, int box_height, ImU32 team, ImU32 black)
{
	if (cvar.visual_box)
	{
		ImGui::GetCurrentWindow()->DrawList->AddRect({ (float)BotX + box_width + 2, (float)BotY + box_height + 2 }, { (float)BotX - box_width, (float)BotY + 1 }, black, cvar.visual_rounding);
		ImGui::GetCurrentWindow()->DrawList->AddRect({ (float)BotX + box_width + 1, (float)BotY + box_height + 1 }, { (float)BotX - box_width - 1, (float)BotY + 0 }, team, cvar.visual_rounding);
	}
}

void RadarRange(float* x, float* y, float range)
{
	if (fabs((*x)) > range || fabs((*y)) > range)
	{
		if ((*y) > (*x))
		{
			if ((*y) > -(*x))
			{
				(*x) = range * (*x) / (*y);
				(*y) = range;
			}
			else
			{
				(*y) = -range * (*y) / (*x);
				(*x) = -range;
			}
		}
		else
		{
			if ((*y) > -(*x))
			{
				(*y) = range * (*y) / (*x);
				(*x) = range;
			}
			else
			{
				(*x) = -range * (*x) / (*y);
				(*y) = -range;
			}
		}
	}
}

void CalcRadarPoint(Vector Origin, int& screenx, int& screeny, bool way)
{
	Vector vAngle;

	g_Engine.GetViewAngles(vAngle);

	float dx = Origin.x - g_Local.vEye.x;
	float dy = Origin.y - g_Local.vEye.y;

	float fYaw = float(vAngle.y * (M_PI / 180.0));

	float fsin_yaw = sin(fYaw);
	float fminus_cos_yaw = -cos(fYaw);

	float x = dy * fminus_cos_yaw + dx * fsin_yaw;
	float y = dx * fminus_cos_yaw - dy * fsin_yaw;

	float range = 2500.0f;

	RadarRange(&x, &y, range);

	ImVec2 DrawPos = ImGui::GetCursorScreenPos();
	ImVec2 DrawSize = ImGui::GetContentRegionAvail();

	int rad_x = (int)DrawPos.x;
	int rad_y = (int)DrawPos.y;

	float r_siz_x = DrawSize.x;
	float r_siz_y = DrawSize.y;

	int x_max = (int)r_siz_x + rad_x - 5;
	int y_max = (int)r_siz_y + rad_y - 5;

	if (way)
	{
		screenx = rad_x + ((int)r_siz_x / 2 + int(x / range * cvar.visual_radar_size * 2));
		screeny = rad_y + ((int)r_siz_y / 2 + int(y / range * cvar.visual_radar_size * 2));
	}
	else
	{
		screenx = rad_x + ((int)r_siz_x / 2 + int(x / range * r_siz_x));
		screeny = rad_y + ((int)r_siz_y / 2 + int(y / range * r_siz_y));
	}

	if (screenx > x_max)
		screenx = x_max;

	if (screenx < rad_x)
		screenx = rad_x;

	if (screeny > y_max)
		screeny = y_max;

	if (screeny < rad_y)
		screeny = rad_y;
}

void RotateTriangle(std::array<Vector2D, 3> & points, float rotation)
{
	const auto points_center = (points.at(0) + points.at(1) + points.at(2)) / 3;
	for (auto& point : points)
	{
		point = point - points_center;

		const auto temp_x = point.x;
		const auto temp_y = point.y;

		const auto theta = deg_2_rad(rotation);
		const auto c = cosf(theta);
		const auto s = sinf(theta);

		point.x = temp_x * c - temp_y * s;
		point.y = temp_x * s + temp_y * c;

		point = point + points_center;
	}
}

void VectorAnglesRadar(Vector& forward, Vector& angles)
{
	if (forward.x == 0.f && forward.y == 0.f)
	{
		angles.x = forward.z > 0.f ? -90.f : 90.f;
		angles.y = 0.f;
	}
	else
	{
		angles.x = rad_2_deg(atan2(-forward.z, forward.Length2D()));
		angles.y = rad_2_deg(atan2(forward.y, forward.x));
	}
	angles.z = 0.f;
}

void DrawRadarWay()
{
	for (unsigned int i = 0; i < autoroute.routecount; i++)
	{
		if (autoroute.route_line[i].enabled)
		{
			for (unsigned int j = 0; j < autoroute.route_line[i].count; j++)
			{
				if (!(j % 4))
				{
					int x, y;
					CalcRadarPoint(autoroute.route_line[i].point[j].origin, x, y, true);
					ImGui::GetCurrentWindow()->DrawList->AddRectFilled(ImVec2(x, y), ImVec2(x + 2, y + 2), black());
					ImGui::GetCurrentWindow()->DrawList->AddRectFilled(ImVec2(x - 1, y - 1), ImVec2(x + 1, y + 1), wheel1());
				}
			}
		}
	}
}

void DrawRadarScreen(Vector origin, ImU32 color, ImU32 black)
{
	int x, y;
	CalcRadarPoint(origin, x, y, false);
	auto angle = Vector();
	VectorAnglesRadar(Vector(ImGui::GetWindowSize().x / 2 - x, ImGui::GetWindowSize().y / 2 - y, 0.f), angle);

	const auto angle_yaw_rad = deg_2_rad(angle.y + 180.f);
	const auto new_point_x = ImGui::GetWindowSize().x / 2 +
		cvar.visual_radar_size / 2 * 8 * cosf(angle_yaw_rad);
	const auto new_point_y = ImGui::GetWindowSize().y / 2 +
		cvar.visual_radar_size / 2 * 8 * sinf(angle_yaw_rad);

	std::array<Vector2D, 3> points
	{
		Vector2D(new_point_x - (cvar.visual_radar_size / 4 + 3.5) / 2, new_point_y - (cvar.visual_radar_size / 4 + 3.5) / 2),
		Vector2D(new_point_x + (cvar.visual_radar_size / 4 + 3.5) / 4, new_point_y),
		Vector2D(new_point_x - (cvar.visual_radar_size / 4 + 3.5) / 2, new_point_y + (cvar.visual_radar_size / 4 + 3.5) / 2)
	};

	RotateTriangle(points, angle.y + 180.f);
	ImGui::GetCurrentWindow()->DrawList->AddTriangle(ImVec2(points.at(0).x + 1, points.at(0).y + 1), ImVec2(points.at(1).x + 1, points.at(1).y + 1), ImVec2(points.at(2).x + 1, points.at(2).y + 1), black);
	ImGui::GetCurrentWindow()->DrawList->AddTriangle(ImVec2(points.at(0).x, points.at(0).y), ImVec2(points.at(1).x, points.at(1).y), ImVec2(points.at(2).x, points.at(2).y), color);
}

void Esp(int i, Vector Player)
{
	if (cvar.visual_idhook_only ? idhook.FirstKillPlayer[i] == 1 : 1)
	{
		if (g_Player[i].iTeam != g_Local.iTeam || cvar.visual_visual_team)
		{
			Player.z -= 6;
			if (g_Player[i].bDucked)Player.z += 18;

			Vector point1 = Vector(Player.x, Player.y, Player.z - cvar.visual_box_size_bot/2 * cvar.visual_box_height);
			Vector point2 = Vector(Player.x, Player.y, Player.z + cvar.visual_box_size_top/2 * cvar.visual_box_height);

			float Bot[2], Top[2];

			bool m_bScreenTop = WorldToScreen(point2, Top);
			bool m_bScreenBot = WorldToScreen(point1, Bot);

			if (m_bScreenTop && m_bScreenBot)
			{
				float box_height = Top[1] - Bot[1];
				float box_width = box_height * cvar.visual_box_width / 3;

				Box(Bot[0], Bot[1], box_width, box_height, team(i), black());

				HealthBar(i, Bot[0], Bot[1], box_width, box_height, black());

				if (ReloadBar(i, Bot[0], Bot[1], box_height, team(i), green(), black()))
					Bot[1] -= 15;

				if (Name(i, Bot[0], Bot[1], box_height, team(i), black(), white()))
					Bot[1] -= 15;

				if (Weapon(i, Bot[0], Bot[1],  box_height, team(i), black(), white()))
					Bot[1] -= 15;

				if (Vip(i, Bot[0], Bot[1], box_height, team(i), green(), black()))
					Bot[1] -= 15;
			}
			else
				if (cvar.visual_radar && g_Local.bAlive) DrawRadarScreen(Player, team(i), black());
		}
	}
}

bool CheckEnt(int i)
{
	if (g_Player[i].bAlive && i != g_Local.iIndex)
		return true;
	return false;
}

bool CheckSound(int i)
{
	if (!g_Player[i].bAlive && g_Player[i].sound.timestamp + 300 >= GetTickCount())
		return true;
	return false;
}

void DrawPlayerEsp()
{
	float WindowBorderSize = ImGui::GetStyle().WindowBorderSize;
	ImGui::GetStyle().WindowBorderSize = 0.0f;
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
	ImGui::Begin("PlayerEsp", reinterpret_cast<bool*>(true), ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs);
	{
		ImGui::SetWindowPos(ImVec2(0, 0), ImGuiSetCond_Always);
		ImGui::SetWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y), ImGuiSetCond_Always);

		if (CheckDraw() && g_pEngine->GetMaxClients())
		{
			for (unsigned int i = 0; i < 33; i++)
			{
				if (CheckSound(i)) Esp(i, g_Player[i].sound.origin);
				if (CheckEnt(i)) Esp(i, g_Player[i].pEnt->origin);
			}
			if (cvar.route_draw && cvar.rage_active && g_Local.bAlive)DrawRadarWay();
		}
		ImGui::End();
		ImGui::PopStyleColor();
		ImGui::GetStyle().WindowBorderSize = WindowBorderSize;
	}
}

void SoundBox(Vector Player, DWORD timestamp)
{
	Vector point1 = Vector(Player.x, Player.y, Player.z - cvar.visual_box_size_bot / 2 * cvar.visual_box_height);
	Vector point2 = Vector(Player.x, Player.y, Player.z + cvar.visual_box_size_top / 2);

	float Bot[2], Top[2];

	bool m_bScreenTop = WorldToScreen(point2, Top);
	bool m_bScreenBot = WorldToScreen(point1, Bot);

	if (m_bScreenTop && m_bScreenBot)
	{
		if (cvar.visual_sound && !bPathFree(g_Local.vEye, Player) && GetTickCount() - timestamp <= 300)
		{
			float height = Top[1] - Bot[1];
			float box_height = height * cvar.visual_box_height;
			float box_width = box_height * cvar.visual_box_width / 3;

			Box(Bot[0], Bot[1], box_width, box_height, green(), black());
		}
	}
	else
		if (cvar.visual_radar && cvar.visual_sound && !bPathFree(g_Local.vEye, Player) && GetTickCount() - timestamp <= 300)
			DrawRadarScreen(Player, green(), black());
}

void SoundCircle(Vector position, float points, float radius)
{
	float step = (float)IM_PI * 2.0f / points;

	for (float a = 0; a < (IM_PI * 2.0f); a += step)
	{
		Vector start(radius * cosf(a) + position.x, radius * sinf(a) + position.y, position.z);
		Vector end(radius * cosf(a + step) + position.x, radius * sinf(a + step) + position.y, position.z);

		Vector start2d, end2d;

		if (cvar.visual_sound && WorldToScreen(start, start2d) && WorldToScreen(end, end2d))
		{
			ImGui::GetCurrentWindow()->DrawList->AddLine(ImVec2(start2d.x+1, start2d.y+1), ImVec2(end2d.x+1, end2d.y+1), black());
			ImGui::GetCurrentWindow()->DrawList->AddLine(ImVec2(start2d.x, start2d.y), ImVec2(end2d.x, end2d.y), green());
		}
	}
}

void DrawSoundEsp()
{
	float WindowBorderSize = ImGui::GetStyle().WindowBorderSize;
	ImGui::GetStyle().WindowBorderSize = 0.0f;
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
	ImGui::Begin("SoundEsp", reinterpret_cast<bool*>(true), ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs);
	{
		ImGui::SetWindowPos(ImVec2(0, 0), ImGuiSetCond_Always);
		ImGui::SetWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y), ImGuiSetCond_Always);

		while (mySounds.size() && GetTickCount() - mySounds.front().timestamp >= 900)
			mySounds.pop_front();

		if (CheckDraw() && g_pEngine->GetMaxClients())
		{
			for (my_sound_t sound : mySounds)
			{
				float size = 13.0f * (1200 - (GetTickCount() - sound.timestamp)) / 1200;
				Vector circle = Vector(sound.origin.x, sound.origin.y, sound.origin.z - 36);

				SoundCircle(circle, 15, size);
				SoundBox(sound.origin, sound.timestamp);
			}
		}
		ImGui::End();
		ImGui::PopStyleColor();
		ImGui::GetStyle().WindowBorderSize = WindowBorderSize;
	}
}

void DrawHitMark()
{
	float WindowBorderSize = ImGui::GetStyle().WindowBorderSize;
	ImGui::GetStyle().WindowBorderSize = 0.0f;
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
	ImGui::Begin("Hitmark", reinterpret_cast<bool*>(true), ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs);
	{
		ImGui::SetWindowPos(ImVec2(0, 0), ImGuiSetCond_Always);
		ImGui::SetWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y), ImGuiSetCond_Always);

		while (myFrags.size() && GetTickCount() - myFrags.front().timestamp >= 5000)
			myFrags.pop_front();

		if (CheckDraw() && g_pEngine->GetMaxClients())
		{
			if (cvar.visual_headshot_mark && myFrags.size() > 0)
			{
				for (unsigned int i = 0; i < myFrags.size(); i++)
				{
					Vector pos2D;
					if (WorldToScreen(myFrags[i].to, pos2D) && myFrags[i].to.Distance(g_Local.vOrigin) < 1000)
					{
						ImGui::GetCurrentWindow()->DrawList->AddLine({ pos2D.x - 8 + 1, pos2D.y - 8 + 1 }, { pos2D.x - (8 / 4) + 1, pos2D.y - (8 / 4) +1 }, black());
						ImGui::GetCurrentWindow()->DrawList->AddLine({ pos2D.x - 8 + 1, pos2D.y + 8 + 1 }, { pos2D.x - (8 / 4) + 1, pos2D.y + (8 / 4) + 1 }, black());
						ImGui::GetCurrentWindow()->DrawList->AddLine({ pos2D.x + 8 + 1, pos2D.y + 8 + 1 }, { pos2D.x + (8 / 4) + 1, pos2D.y + (8 / 4) + 1 }, black());
						ImGui::GetCurrentWindow()->DrawList->AddLine({ pos2D.x + 8 + 1, pos2D.y - 8 + 1 }, { pos2D.x + (8 / 4) + 1, pos2D.y - (8 / 4) + 1 }, black());

						ImGui::GetCurrentWindow()->DrawList->AddLine({ pos2D.x - 8, pos2D.y - 8 }, { pos2D.x - (8 / 4), pos2D.y - (8 / 4) }, green());
						ImGui::GetCurrentWindow()->DrawList->AddLine({ pos2D.x - 8, pos2D.y + 8 }, { pos2D.x - (8 / 4), pos2D.y + (8 / 4) }, green());
						ImGui::GetCurrentWindow()->DrawList->AddLine({ pos2D.x + 8, pos2D.y + 8 }, { pos2D.x + (8 / 4), pos2D.y + (8 / 4) }, green());
						ImGui::GetCurrentWindow()->DrawList->AddLine({ pos2D.x + 8, pos2D.y - 8 }, { pos2D.x + (8 / 4), pos2D.y - (8 / 4) }, green());
					}
				}
			}
		}
		ImGui::End();
		ImGui::PopStyleColor();
		ImGui::GetStyle().WindowBorderSize = WindowBorderSize;
	}
}

void DrawNoFlash()
{
	float WindowBorderSize = ImGui::GetStyle().WindowBorderSize;
	ImGui::GetStyle().WindowBorderSize = 0.0f;
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
	ImGui::Begin("NoFlash", reinterpret_cast<bool*>(true), ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs);
	{
		ImGui::SetWindowPos(ImVec2(0, 0), ImGuiSetCond_Always);
		ImGui::SetWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y), ImGuiSetCond_Always);

		if (CheckDraw() && g_pEngine->GetMaxClients())
		{
			if (cvar.visual_rem_flash > 0)
			{
				static float Flashed;
				static float FadeEnd;

				if (pScreenFade->fadeEnd > g_Engine.GetClientTime())
				{
					FadeEnd = pScreenFade->fadeEnd;

					if (pScreenFade->fadealpha > (int)cvar.visual_rem_flash)
					{
						pScreenFade->fadealpha = (int)cvar.visual_rem_flash;
					}
					if ((int)cvar.visual_rem_flash > pScreenFade->fadealpha)
					{
						pScreenFade->fadealpha = (int)cvar.visual_rem_flash;
					}
				}

				if (FadeEnd > g_Engine.GetClientTime())
				{
					Flashed = (FadeEnd - g_Engine.GetClientTime()) * pScreenFade->fadeSpeed;
				}
				else
				{
					Flashed = 0.0f;
				}

				if (Flashed > 255.0f)
				{
					Flashed = 255.0f;
				}

				if (Flashed < 0.0f)
				{
					Flashed = 0.0f;
				}

				float Percentage = (Flashed / 255.0f);

				if (Percentage > 0.0f)
				{
					ImGui::ProgressBar(Percentage, ImVec2(0.0f, 0.0f));
					ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
					ImGui::Text("Flashed");
				}
			}
		}
		ImGui::End();
		ImGui::PopStyleColor();
		ImGui::GetStyle().WindowBorderSize = WindowBorderSize;
	}
}

void TraceGrenade()
{
	if (cvar.visual_grenade_trajectory && IsCurWeaponNade() && g_Local.bAlive)
	{
		Vector vecAngles, vecForward, vecStart, vecEnd, vecOut;
		pmtrace_t pmtrace;

		float flGravity = pmove->movevars->gravity / SVGRAVITY;

		g_Engine.GetViewAngles(vecAngles);

		if (vecAngles[0] < 0)
			vecAngles[0] = -10 + vecAngles[0] * ((90.f - 10.f) / 90.0f);
		else
			vecAngles[0] = -10 + vecAngles[0] * ((90.f + 10.f) / 90.0f);

		float flVel = (90 - vecAngles[0]) * 4;

		if (flVel > 500)
			flVel = 500;

		g_Engine.pfnAngleVectors(vecAngles, vecForward, NULL, NULL);

		vecStart = g_Local.vEye + vecForward * 16;

		vecForward = (vecForward * flVel) + g_Local.vVelocity;
		typedef unsigned int uint;
		uint iCollisions = 0;
		float flTimeAlive;
		float flStep = (3.00f / 50.0f);

		for (flTimeAlive = 0; flTimeAlive < 3.00f; flTimeAlive += flStep)
		{
			vecEnd = vecStart + vecForward * flStep;

			g_Engine.pEventAPI->EV_SetTraceHull(2);
			g_Engine.pEventAPI->EV_PlayerTrace(vecStart, vecEnd, PM_STUDIO_BOX, -1, &pmtrace);

			if (pmtrace.ent != g_Local.iIndex && pmtrace.fraction < 1.0) //hits a wall
			{
				//hitpoint
				vecEnd = vecStart + vecForward * pmtrace.fraction * flStep;

				if (pmtrace.plane.normal[2] > 0.9 && vecForward[2] <= 0 && vecForward[2] >= (-1 * flGravity * 0.20f))
				{
					vecOut = vecEnd;
					return;
				}

				float flProj = vecForward.Dot(pmtrace.plane.normal);

				vecForward = (vecForward * 1.75f - flProj * 2 * pmtrace.plane.normal) * 0.75f; //reflection off the wall

				iCollisions++;

				if (iCollisions > 30)
					break;

				flTimeAlive -= (flStep * (1 - pmtrace.fraction));
			}

			float fScreenStart[2];
			float fScreenEnd[2];
			if (WorldToScreen(vecStart, fScreenStart) && WorldToScreen(vecEnd, fScreenEnd))
			{
				ImGui::GetCurrentWindow()->DrawList->AddLine({ fScreenStart[0] + 1, fScreenStart[1] + 1 }, { fScreenEnd[0] + 1,fScreenEnd[1] + 1 }, black());
				ImGui::GetCurrentWindow()->DrawList->AddLine({ fScreenStart[0], fScreenStart[1] }, { fScreenEnd[0],fScreenEnd[1] }, wheel1());
			}
			float fScreen[2];
			if (WorldToScreen(vecEnd, fScreen))
			{
				ImGui::GetCurrentWindow()->DrawList->AddRect({ fScreen[0] - 1, fScreen[1] - 1 }, { fScreen[0] + 4, fScreen[1] + 4 }, black(), cvar.visual_rounding);
				ImGui::GetCurrentWindow()->DrawList->AddRect({ fScreen[0] - 2, fScreen[1] - 2 }, { fScreen[0] + 3, fScreen[1] + 3 }, wheel2(), cvar.visual_rounding);
			}
			vecStart = vecEnd;

			vecForward[2] -= flGravity * pmtrace.fraction * flStep; //gravity
		}

		vecOut = vecStart;
	}
}

void DrawTraceGrenade()
{
	float WindowBorderSize = ImGui::GetStyle().WindowBorderSize;
	ImGui::GetStyle().WindowBorderSize = 0.0f;
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
	ImGui::Begin("DrawTrace", reinterpret_cast<bool*>(true), ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs);
	{
		ImGui::SetWindowPos(ImVec2(0, 0), ImGuiSetCond_Always);
		ImGui::SetWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y), ImGuiSetCond_Always);

		if (CheckDraw() && g_pEngine->GetMaxClients())
		{
			TraceGrenade();
		}
		ImGui::End();
		ImGui::PopStyleColor(); 
		ImGui::GetStyle().WindowBorderSize = WindowBorderSize;
	}
}

void CrossHair()
{
	float x = ImGui::GetWindowSize().x / 2, y = ImGui::GetWindowSize().y / 2;
	float ColorHSV[3];
	float ColorRGB[3];
	float ColorFloat;

	ImGui::ColorConvertRGBtoHSV(cvar.color_red, cvar.color_green, cvar.color_blue, ColorHSV[0], ColorHSV[1], ColorHSV[2]);
	ImRotateStart();
	ColorFloat = ColorHSV[0];
	for (unsigned int i = 1; i <= 15; i++)
	{
		ColorFloat += 1.0f / 15.0f;
		if (ColorFloat > 1.0f)
			ColorFloat -= 1.0f;
		ImGui::ColorConvertHSVtoRGB(ColorFloat, ColorHSV[1], ColorHSV[2], ColorRGB[0], ColorRGB[1], ColorRGB[2]);
		ImGui::GetCurrentWindow()->DrawList->AddLine({ x - i - cvar.visual_crosshair_offset, y - i - cvar.visual_crosshair_offset }, { x - i - 1 - cvar.visual_crosshair_offset, y - i - 1 - cvar.visual_crosshair_offset }, ImColor(ColorRGB[0], ColorRGB[1], ColorRGB[2], cvar.visual_alpha), 2.0f);
	}

	ColorFloat = ColorHSV[0];
	for (unsigned int i = 1; i <= 15; i++)
	{
		ColorFloat += 1.0f / 15.0f;
		if (ColorFloat > 1.0f)
			ColorFloat -= 1.0f;
		ImGui::ColorConvertHSVtoRGB(ColorFloat, ColorHSV[1], ColorHSV[2], ColorRGB[0], ColorRGB[1], ColorRGB[2]);
		ImGui::GetCurrentWindow()->DrawList->AddLine({ x + i + cvar.visual_crosshair_offset, y - i - cvar.visual_crosshair_offset }, { x + i + 1 + cvar.visual_crosshair_offset, y - i - 1 - cvar.visual_crosshair_offset }, ImColor(ColorRGB[0], ColorRGB[1], ColorRGB[2], cvar.visual_alpha), 2.0f);
	}

	ColorFloat = ColorHSV[0];
	for (unsigned int i = 1; i <= 15; i++)
	{
		ColorFloat += 1.0f / 15.0f;
		if (ColorFloat > 1.0f)
			ColorFloat -= 1.0f;
		ImGui::ColorConvertHSVtoRGB(ColorFloat, ColorHSV[1], ColorHSV[2], ColorRGB[0], ColorRGB[1], ColorRGB[2]);
		ImGui::GetCurrentWindow()->DrawList->AddLine({ x + i + cvar.visual_crosshair_offset, y + i + cvar.visual_crosshair_offset }, { x + i + 1 + cvar.visual_crosshair_offset, y + i + 1 + cvar.visual_crosshair_offset }, ImColor(ColorRGB[0], ColorRGB[1], ColorRGB[2], cvar.visual_alpha), 2.0f);
	}

	ColorFloat = ColorHSV[0];
	for (unsigned int i = 1; i <= 15; i++)
	{
		ColorFloat += 1.0f / 15.0f;
		if (ColorFloat > 1.0f)
			ColorFloat -= 1.0f;
		ImGui::ColorConvertHSVtoRGB(ColorFloat, ColorHSV[1], ColorHSV[2], ColorRGB[0], ColorRGB[1], ColorRGB[2]);
		ImGui::GetCurrentWindow()->DrawList->AddLine({ x - i - cvar.visual_crosshair_offset, y + i + cvar.visual_crosshair_offset }, { x - i - 1 - cvar.visual_crosshair_offset, y + i + 1 + cvar.visual_crosshair_offset }, ImColor(ColorRGB[0], ColorRGB[1], ColorRGB[2], cvar.visual_alpha), 2.0f);
	}
	static float RotateButton = 0.0f;
	static DWORD Tickcount = 0;
	static DWORD Tickcheck = 0;
	if (cvar.visual_crosshair_spin)
	{
		if (GetTickCount() - Tickcount >= 1)
		{
			if (Tickcheck != Tickcount)
			{
				RotateButton += (0.001f * (M_PI * 2)) * cvar.visual_crosshair_spin;
				Tickcheck = Tickcount;
			}
			Tickcount = GetTickCount();
		}

		if (RotateButton / (M_PI * 2) > 1.0f || RotateButton / (M_PI * 2) < -1.0f)
			RotateButton = 0.0f;
	}
	else
		RotateButton = 0.0f;

	ImRotateEnd(RotateButton);
}

void DrawCrossHair()
{
	float WindowBorderSize = ImGui::GetStyle().WindowBorderSize;
	ImGui::GetStyle().WindowBorderSize = 0.0f;
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
	ImGui::Begin("DrawCrossHair", reinterpret_cast<bool*>(true), ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs);
	{
		ImGui::SetWindowPos(ImVec2(0, 0), ImGuiSetCond_Always);
		ImGui::SetWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y), ImGuiSetCond_Always);

		if (cvar.visual_crosshair && g_Local.bAlive && CheckDraw() && g_pEngine->GetMaxClients())
		{
			CrossHair();
		}
		ImGui::End();
		ImGui::PopStyleColor();
		ImGui::GetStyle().WindowBorderSize = WindowBorderSize;
	}
}

void CustomFOV()
{
	int fov = cvar.visual_custom_fov;
	if (CheckDrawEngine() && g_Local.bAlive)
	{
		if (g_Local.iFOV == 90)(*pSetFOV)("SetFOV", 1, &fov);
	}
	else
	{
		fov = 90;
		if (g_Local.iFOV == 90)(*pSetFOV)("SetFOV", 1, &fov);
	}
}