#include "client.h"

void Spacing()
{
	ImVec2 LastSapcing = ImGui::GetStyle().ItemSpacing;
	ImGui::GetStyle().ItemSpacing = ImVec2(0, 0);
	ImGui::SameLine();
	ImGui::GetStyle().ItemSpacing = LastSapcing;
}

void Menutabs(int tab, int texture, bool sameline)
{
	static float RotateImage = 0.0f;
	static float RotateButton = 0.0f;
	static float RotateReset = 0.0f;
	static DWORD Tickcount = 0;
	static DWORD Tickcheck = 0;
	if (cvar.gui_menu_button_spin)
	{
		if (GetTickCount() - Tickcount >= 1)
		{
			if (Tickcheck != Tickcount)
			{
				RotateImage += 0.001f * cvar.gui_menu_button_spin;
				RotateButton += (0.001f * (M_PI * 2)) * cvar.gui_menu_button_spin;
				RotateReset += (0.001f * (M_PI * 2)) * cvar.gui_menu_button_spin;
				Tickcheck = Tickcount;
			}
			Tickcount = GetTickCount();
		}

		if (RotateReset / (M_PI * 2) > 1.0f || RotateReset / (M_PI * 2) < -1.0f)
		{
			RotateImage = 0.0f;
			RotateButton = M_PI * 2 / 4;
			RotateReset = 0.0f;
		}
	}
	else
	{
		RotateImage = 0.0f;
		RotateButton = M_PI * 2 / 4;
		RotateReset = 0.0f;
	}

	if (MenuTab == tab)
	{
		ImRotateStart();
		if (sameline) ImGui::SameLine();
		if (ImGui::ImageButton((GLuint*)texture_id[texture], ImVec2(42, 44), ImVec2(0, 0.0f + RotateImage), ImVec2(1, 1.0f + RotateImage)))MenuTab = 0;
		ImRotateEnd(RotateButton);
	}
	else
	{
		if (sameline) ImGui::SameLine();
		if (ImGui::ImageButton((GLuint*)texture_id[texture], ImVec2(42, 44)))
		{
			MenuTab = tab;
			RotateImage = 0.0f;
			RotateButton = M_PI * 2 / 4;
			RotateReset = 0.0f;
		}
	}
}

void ImageBorder(int from, int to)
{
	static float RotateImage = 0.0f;
	static DWORD Tickcount = 0;
	static DWORD Tickcheck = 0;
	if (cvar.gui_menu_image_spin)
	{
		if (GetTickCount() - Tickcount >= 1)
		{
			if (Tickcheck != Tickcount)
			{
				RotateImage += 0.001f * cvar.gui_menu_image_spin;
				Tickcheck = Tickcount;
			}
			Tickcount = GetTickCount();
		}

		if (RotateImage > 1.0f || RotateImage < -1.0f)
			RotateImage = 0.0f;
	}
	else
		RotateImage = 0.0f;

	float xfrom = ImGui::GetCurrentWindow()->Pos.x;
	float xto = ImGui::GetCurrentWindow()->Pos.x + ImGui::GetCurrentWindow()->Size.x;
	const ImRect bb(ImVec2(xfrom, ImGui::GetCurrentWindow()->DC.CursorPos.y + from), ImVec2(xto, ImGui::GetCurrentWindow()->DC.CursorPos.y + to));
	if (cvar.gui_menu_image)ImGui::GetWindowDrawList()->AddImage((GLuint*)texture_id[MENUBACK], ImVec2(bb.Min.x, bb.Min.y), ImVec2(bb.Max.x, bb.Max.y), ImVec2(0.0f + RotateImage, 0), ImVec2(1.0f + RotateImage, 1));
}

bool ComboBox(const char* label, float* current_item, const char* const items[], int items_count, int height_in_items) 
{
	ImGui::PushItemWidth(-1);
	char str[256];
	sprintf(str, "##%s", label);
	const bool value_changed = ImGui::Combo(str, current_item, Items_ArrayGetter, (void*)items, items_count, height_in_items);
	ImGui::PopItemWidth();
	return value_changed;
}

bool SliderFloat(const char* label, float* v, float v_min, float v_max, const char* format)
{
	ImGui::PushItemWidth(-1);
	char str[256];
	sprintf(str, "##%s", label);
	const bool value_changed = ImGui::SliderFloat(str, v, v_min, v_max, format);
	ImGui::PopItemWidth();
	return value_changed;
}

void KeyBind(float& key, char* keyname)
{
	bool clicked = false;
	char str[256];

	if (key == -2)
	{
		for (unsigned int i = 0; i < 255; i++)
		{
			if (keys[i])
			{
				if (i == VK_ESCAPE || i == VK_LBUTTON || i == cvar.gui_key)
				{
					key = -2;
					break;
				}
				key = i;
			}
		}
	}

	if (key == -1)
	{
		ImVec4 prevColor = ImGui::GetStyle().Colors[ImGuiCol_Text];
		ImGui::GetStyle().Colors[ImGuiCol_Text] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
		sprintf(str, "[No key]##%s", keyname);
		if (ImGui::Button(str))
			clicked = true;
		ImGui::GetStyle().Colors[ImGuiCol_Text] = prevColor;
		if (keyname != NULL)ImGui::SameLine();
		if (keyname != NULL)ImGui::Text(keyname);
	}
	else if (key == -2)
	{
		ImVec4 prevColor = ImGui::GetStyle().Colors[ImGuiCol_Text];
		ImGui::GetStyle().Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
		sprintf(str, "[Press key]##%s", keyname);
		if (ImGui::Button(str))
			clicked = true;
		ImGui::GetStyle().Colors[ImGuiCol_Text] = prevColor;
		if (keyname != NULL)ImGui::SameLine();
		if (keyname != NULL)ImGui::Text(keyname);
	}
	else
	{
		ImVec4 prevColor = ImGui::GetStyle().Colors[ImGuiCol_Text];
		ImGui::GetStyle().Colors[ImGuiCol_Text] = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
		sprintf(str, "[%s]##%s", GetKeyName((int)key).c_str(), keyname);
		if (ImGui::Button(str))
			clicked = true;
		ImGui::GetStyle().Colors[ImGuiCol_Text] = prevColor;
		if (keyname != NULL)ImGui::SameLine();
		if (keyname != NULL)ImGui::Text(keyname);
	}

	if (clicked)
	{
		if (key == -1)
			key = -2;
		else
			key = -1;
	}
}

void HudKeyBind(float& key, char* keyname, bool menukey)
{
	bool clicked = false;
	char str[256];

	if (key == -2)
	{
		for (unsigned int i = 0; i < 255; i++)
		{
			if (keysmenu[i])
			{
				if (i == K_ESCAPE || i == cvar.gui_key_hud)
				{
					key = -2;
					break;
				}
				if (menukey && i == K_MOUSE1)
				{
					key = -2;
					break;
				}
				key = i;
			}
		}
	}

	if (key == -1)
	{
		ImVec4 prevColor = ImGui::GetStyle().Colors[ImGuiCol_Text];
		ImGui::GetStyle().Colors[ImGuiCol_Text] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
		sprintf(str, "[No key]##%s", keyname);
		if (ImGui::Button(str))
			clicked = true;
		ImGui::GetStyle().Colors[ImGuiCol_Text] = prevColor;
		if (keyname != NULL)ImGui::SameLine();
		if (keyname != NULL)ImGui::Text(keyname);
	}
	else if (key == -2)
	{
		ImVec4 prevColor = ImGui::GetStyle().Colors[ImGuiCol_Text];
		ImGui::GetStyle().Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
		sprintf(str, "[Press key]##%s", keyname);
		if (ImGui::Button(str))
			clicked = true;
		ImGui::GetStyle().Colors[ImGuiCol_Text] = prevColor;
		if (keyname != NULL)ImGui::SameLine();
		if (keyname != NULL)ImGui::Text(keyname);
	}
	else
	{
		ImVec4 prevColor = ImGui::GetStyle().Colors[ImGuiCol_Text];
		ImGui::GetStyle().Colors[ImGuiCol_Text] = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
		sprintf(str, "[%s]##%s", KeyEventToChar(key), keyname);
		if (ImGui::Button(str))
			clicked = true;
		ImGui::GetStyle().Colors[ImGuiCol_Text] = prevColor;
		if (keyname != NULL)ImGui::SameLine();
		if (keyname != NULL)ImGui::Text(keyname);
	}

	if (clicked)
	{
		if (key == -1)
			key = -2;
		else
			key = -1;
	}
}

int CheckWeapon(float global, float sub)
{
	if (global == 0 && (sub == 0 || sub == 1 || sub == 2 || sub == 3 || sub == 4 || sub == 5))
	{
		if (sub == 0)
			return WEAPON_GLOCK18;
		if (sub == 1)
			return WEAPON_P228;
		if (sub == 2)
			return WEAPON_DEAGLE;
		if (sub == 3)
			return WEAPON_ELITE;
		if (sub == 4)
			return WEAPON_FIVESEVEN;
		if (sub == 5)
			return WEAPON_USP;
	}
	else if (global == 1 && (sub == 0 || sub == 1 || sub == 2 || sub == 3))
	{
		if (sub == 0)
			return WEAPON_AWP;
		if (sub == 1)
			return WEAPON_SCOUT;
		if (sub == 2)
			return WEAPON_G3SG1;
		if (sub == 3)
			return WEAPON_SG550;
	}
	else if (global == 2 && (sub == 0 || sub == 1 || sub == 2 || sub == 3 || sub == 4 || sub == 5))
	{
		if (sub == 0)
			return WEAPON_M4A1;
		if (sub == 1)
			return WEAPON_GALIL;
		if (sub == 2)
			return WEAPON_FAMAS;
		if (sub == 3)
			return WEAPON_AUG;
		if (sub == 4)
			return WEAPON_AK47;
		if (sub == 5)
			return WEAPON_SG552;
	}
	else if (global == 3 && (sub == 0 || sub == 1))
	{
		if (sub == 0)
			return WEAPON_XM1014;
		if (sub == 1)
			return WEAPON_M3;
	}
	else if (global == 4 && sub == 0)
	{
		if (sub == 0)
			return WEAPON_M249;
	}
	else if (global == 5 && (sub == 0 || sub == 1 || sub == 2 || sub == 3 || sub == 4))
	{
		if (sub == 0)
			return WEAPON_TMP;
		if (sub == 1)
			return WEAPON_P90;
		if (sub == 2)
			return WEAPON_MP5N;
		if (sub == 3)
			return WEAPON_MAC10;
		if (sub == 4)
			return WEAPON_UMP45;
	}
	else return 0;
}

void CheckSubSection(float& global, float& sub)
{
	if (global == 0 && sub > 5)
		sub = 5;
	if (global == 1 && sub > 3)
		sub = 3;
	if (global == 2 && sub > 5)
		sub = 5;
	if (global == 3 && sub > 1)
		sub = 1;
	if (global == 4 && sub > 0)
		sub = 0;
	if (global == 5 && sub > 4)
		sub = 4;
}

void MenuRage()
{
	ImGui::BeginChild("Aimbot - General", ImVec2(225, 400), true);
	{
		ImGui::Text("Rage Aimbot"), ImGui::Separator();

		ImGui::Checkbox("Activate", &cvar.rage_active);
		ImGui::Checkbox("Aim Team", &cvar.rage_team);
		ImGui::Checkbox("Auto Fire", &cvar.rage_auto_fire);
		HudKeyBind(cvar.rage_auto_fire_key, "Auto Fire Key", false); ImGui::SameLine(); ShowHelpMarker("Only working in game");
		ImGui::Checkbox("Perfect Silent", &cvar.rage_perfect_silent);
		ImGui::Checkbox("Silent Aim", &cvar.rage_silent);
		ImGui::Checkbox("Shield Attack", &cvar.rage_shield_attack);
		ImGui::Checkbox("Wall", &cvar.rage_wall);
		SliderFloat("Fov", &cvar.rage_fov, 0.f, 360.f, "Fov: %.1f");
		ImGui::Text("Target Selection");
		const char* listbox_target[] = { "Field Of View", "Distance", "Cycle" };
		ComboBox("Target Selection", &cvar.rage_target_selection, listbox_target, IM_ARRAYSIZE(listbox_target), 3);
		ImGui::Text("Target Hitbox");
		const char* listbox_hitbox[] = { "Head", "Chest", "Stomach", "Waist", "All", "Vital" };
		ComboBox("Target Hitbox", &cvar.rage_hitbox, listbox_hitbox, IM_ARRAYSIZE(listbox_hitbox), 6);
		ImGui::Text("Target Multi Hitbox");
		const char* listbox_multi[] = { "None", "Only Head", "Below Head", "All" };
		ComboBox("Multi Hitbox", &cvar.rage_multipoint, listbox_multi, IM_ARRAYSIZE(listbox_multi), 4);
		ImGui::Text("Knife Attack Method");
		const char* listbox_knife[] = { "Slash", "Stab" };
		ComboBox("Knife Attack Method", &cvar.rage_knife_attack, listbox_knife, IM_ARRAYSIZE(listbox_knife), 2);
	}
	ImGui::EndChild();

	Spacing();

	ImGui::BeginChild("Aimbot - Other", ImVec2(225, 400), true);
	{
		ImGui::Text("Rage Weapon Settings"); ImGui::Separator();

		ImGui::Text("Select Weapon");
		const char* listbox_edge[] = { "Pistol", "Sniper", "Rifle", "Shotgun", "Machine Gun", "Submachine Gun" };
		ComboBox("Section", &cvar.menu_rage_global_section, listbox_edge, IM_ARRAYSIZE(listbox_edge), 6);

		CheckSubSection(cvar.menu_rage_global_section, cvar.menu_rage_sub_section);
		
		if (cvar.menu_rage_global_section == 0)
		{
			const char* listbox_sub1[] = { "GLOCK18", "P228", "DEAGLE", "ELITE", "FIVESEVEN", "USP" };
			ComboBox("Weapon", &cvar.menu_rage_sub_section, listbox_sub1, IM_ARRAYSIZE(listbox_sub1), 6);
		}
		else if (cvar.menu_rage_global_section == 1)
		{
			const char* listbox_sub2[] = { "AWP", "SCOUT", "G3SG1", "SG550" };
			ComboBox("Weapon", &cvar.menu_rage_sub_section, listbox_sub2, IM_ARRAYSIZE(listbox_sub2), 4);
		}
		else if (cvar.menu_rage_global_section == 2)
		{
			const char* listbox_sub3[] = { "M4A1", "GALIL", "FAMAS", "AUG", "AK47", "SG552" };
			ComboBox("Weapon", &cvar.menu_rage_sub_section, listbox_sub3, IM_ARRAYSIZE(listbox_sub3), 6);
		}
		else if (cvar.menu_rage_global_section == 3)
		{
			const char* listbox_sub4[] = { "XM1014", "M3" };
			ComboBox("Weapon", &cvar.menu_rage_sub_section, listbox_sub4, IM_ARRAYSIZE(listbox_sub4), 2);
		}
		else if (cvar.menu_rage_global_section == 4)
		{
			const char* listbox_sub5[] = { "M249" };
			ComboBox("Weapon", &cvar.menu_rage_sub_section, listbox_sub5, IM_ARRAYSIZE(listbox_sub5), 1);
		}
		else if (cvar.menu_rage_global_section == 5)
		{
			const char* listbox_sub6[] = { "TMP", "P90", "MP5N", "MAC10", "UMP45" };
			ComboBox("Weapon", &cvar.menu_rage_sub_section, listbox_sub6, IM_ARRAYSIZE(listbox_sub6), 5);
		}
		if (CheckWeapon(cvar.menu_rage_global_section, cvar.menu_rage_sub_section))
		{
			ImGui::Text("Adjust Delay");
			SliderFloat("Shot Delay", &cvar.rage[CheckWeapon(cvar.menu_rage_global_section, cvar.menu_rage_sub_section)].rage_delay_shot, 0.f, 1000.f, "Shot Delay: %.0fms");

			const char* listbox_count[] = { "Count", "Random" };
			ComboBox("Shot Type", &cvar.rage[CheckWeapon(cvar.menu_rage_global_section, cvar.menu_rage_sub_section)].rage_shot_type, listbox_count, IM_ARRAYSIZE(listbox_count), 2);
			if (!cvar.rage[CheckWeapon(cvar.menu_rage_global_section, cvar.menu_rage_sub_section)].rage_shot_type) SliderFloat("Shot Count", &cvar.rage[CheckWeapon(cvar.menu_rage_global_section, cvar.menu_rage_sub_section)].rage_shot_count, 1.f, 5.f, "Shot Count: %.0f");
			if (cvar.rage[CheckWeapon(cvar.menu_rage_global_section, cvar.menu_rage_sub_section)].rage_shot_type) SliderFloat("Random Max", &cvar.rage[CheckWeapon(cvar.menu_rage_global_section, cvar.menu_rage_sub_section)].rage_random_max, 2.f, 5.f, "Random Max: %.0f");
			ImGui::Text("Adjust Recoil");
			SliderFloat("Recoil", &cvar.rage[CheckWeapon(cvar.menu_rage_global_section, cvar.menu_rage_sub_section)].rage_recoil, 0.f, 2.f, "Recoil: %.4f");
			ImGui::Text("Adjust Spread Method");
			const char* listbox_spread[] = { "Pitch / Yaw", "Roll" };
			ComboBox("Remove Spread Method", &cvar.rage_nospread_method, listbox_spread, IM_ARRAYSIZE(listbox_spread), 2);
		}
		ImGui::Separator();
	}
	ImGui::EndChild();

	ImGui::BeginChild("Aimbot - Fakelag", ImVec2(225, 319), true);
	{
		ImGui::Text("Fake Lag"), ImGui::Separator();

		ImGui::Checkbox("Activate", &cvar.fakelag_active);
		ImGui::Checkbox("Fake Lag While Shooting", &cvar.fakelag_while_shooting);
		const char* listbox_type[] = { "Dynamic", "Maximum", "Flucture", "Break Lag Compensation" };
		ComboBox("Fakelag Type", &cvar.fakelag_type, listbox_type, IM_ARRAYSIZE(listbox_type), 4);
		const char* listbox_move[] = { "On Land", "On Move", "In Air", "All" };
		ComboBox("Fakelag Move", &cvar.fakelag_move, listbox_move, IM_ARRAYSIZE(listbox_move), 4);
		SliderFloat("Variance", &cvar.fakelag_variance, 0.f, 100.f, "Variance: %.0f");
		SliderFloat("Limit", &cvar.fakelag_limit, 0.f, 30.f, "Limit: %.0f");
		ImGui::Separator();
	}
	ImGui::EndChild();

	Spacing();

	ImGui::BeginChild("Aimbot - Anti-Aimbot", ImVec2(225, 319), true);
	{
		ImGui::Text("Anti Aim"), ImGui::Separator();

		ImGui::Text("Pitch");
		const char* listbox_pitch[] = { "None", "Fakedown", "Random" };
		ComboBox("Pitch", &cvar.aa_pitch, listbox_pitch, IM_ARRAYSIZE(listbox_pitch), 3);
		ImGui::Text("Yaw");
		const char* listbox_yaw[] = { "None", "180", "180 Jitter", "Spin", "Jitter", "Sideways", "Random", "Static" };
		ComboBox("Yaw", &cvar.aa_yaw, listbox_yaw, IM_ARRAYSIZE(listbox_yaw), 8);
		ImGui::Text("Yaw While Running");
		const char* listbox_yawrun[] = { "None", "180", "180 Jitter", "Spin", "Jitter", "Sideways", "Random", "Static" };
		ComboBox("Yaw While Running", &cvar.aa_yaw_while_running, listbox_yawrun, IM_ARRAYSIZE(listbox_yawrun), 8);
		SliderFloat("Static Yaw", &cvar.aa_yaw_static, -180.f, 180.f, "Static Yaw: %.0f");
		ImGui::Text("Roll");
		const char* listbox_roll[] = { "None", "180", "180 Jitter", "Spin", "Jitter", "Sideways", "Random", "Static" };
		ComboBox("Roll", &cvar.aa_roll, listbox_roll, IM_ARRAYSIZE(listbox_roll), 8);
		ImGui::Text("Roll While Running");
		const char* listbox_rollrun[] = { "None", "180", "180 Jitter", "Spin", "Jitter", "Sideways", "Random", "Static" };
		ComboBox("Roll While Running", &cvar.aa_roll_while_running, listbox_rollrun, IM_ARRAYSIZE(listbox_rollrun), 8);
		SliderFloat("Static Roll", &cvar.aa_roll_static, -180.f, 180.f, "Static Roll: %.0f");
		const char* listbox_edge[] = { "None", "Standing", "In Move", "All", "All + Fake" };
		ImGui::Text("Edge");
		ComboBox("Edge", &cvar.aa_edge, listbox_edge, IM_ARRAYSIZE(listbox_edge), 5);
	}
	ImGui::EndChild();
}

void MenuLegit()
{
	ImGui::BeginChild("Legit - Section", ImVec2(225, 35), true);
	{
		const char* listbox_section[] = { "Pistol", "Sniper", "Rifle", "Shotgun", "Machine Gun", "Submachine Gun" };
		ComboBox("Section", &cvar.menu_legit_global_section, listbox_section, IM_ARRAYSIZE(listbox_section), 6);
	}
	ImGui::EndChild();

	Spacing();

	ImGui::BeginChild("Legit - Weapon", ImVec2(225, 35), true);
	{
		CheckSubSection(cvar.menu_legit_global_section, cvar.menu_legit_sub_section);

		if (cvar.menu_legit_global_section == 0)
		{
			const char* listbox_sub1[] = { "GLOCK18", "P228", "DEAGLE", "ELITE", "FIVESEVEN", "USP" };
			ComboBox("Weapon", &cvar.menu_legit_sub_section, listbox_sub1, IM_ARRAYSIZE(listbox_sub1), 6);
		}
		else if (cvar.menu_legit_global_section == 1)
		{
			const char* listbox_sub2[] = { "AWP", "SCOUT", "G3SG1", "SG550" };
			ComboBox("Weapon", &cvar.menu_legit_sub_section, listbox_sub2, IM_ARRAYSIZE(listbox_sub2), 4);
		}
		else if (cvar.menu_legit_global_section == 2)
		{
			const char* listbox_sub3[] = { "M4A1", "GALIL", "FAMAS", "AUG", "AK47", "SG552" };
			ComboBox("Weapon", &cvar.menu_legit_sub_section, listbox_sub3, IM_ARRAYSIZE(listbox_sub3), 6);
		}
		else if (cvar.menu_legit_global_section == 3)
		{
			const char* listbox_sub4[] = { "XM1014", "M3" };
			ComboBox("Weapon", &cvar.menu_legit_sub_section, listbox_sub4, IM_ARRAYSIZE(listbox_sub4), 2);
		}
		else if (cvar.menu_legit_global_section == 4)
		{
			const char* listbox_sub5[] = { "M249" };
			ComboBox("Weapon", &cvar.menu_legit_sub_section, listbox_sub5, IM_ARRAYSIZE(listbox_sub5), 1);
		}
		else if (cvar.menu_legit_global_section == 5)
		{
			const char* listbox_sub6[] = { "TMP", "P90", "MP5N", "MAC10", "UMP45" };
			ComboBox("Weapon", &cvar.menu_legit_sub_section, listbox_sub6, IM_ARRAYSIZE(listbox_sub6), 5);
		}
	}
	ImGui::EndChild();

	if (CheckWeapon(cvar.menu_legit_global_section, cvar.menu_legit_sub_section))
	{
		ImGui::BeginChild("Legit - General", ImVec2(225, 464), true);
		{
			ImGui::Text("Legit Aimbot"), ImGui::Separator();

			ImGui::Checkbox("Activate", &cvar.legit[CheckWeapon(cvar.menu_legit_global_section, cvar.menu_legit_sub_section)].active);
			ImGui::Checkbox("Aim Team", &cvar.legit_team);
			ImGui::Checkbox("Humanize", &cvar.legit[CheckWeapon(cvar.menu_legit_global_section, cvar.menu_legit_sub_section)].humanize);
			ImGui::Checkbox("Perfect Silent", &cvar.legit[CheckWeapon(cvar.menu_legit_global_section, cvar.menu_legit_sub_section)].perfect_silent);
			ImGui::Checkbox("Head Hitbox", &cvar.legit[CheckWeapon(cvar.menu_legit_global_section, cvar.menu_legit_sub_section)].head);
			ImGui::Checkbox("Chest Hitbox", &cvar.legit[CheckWeapon(cvar.menu_legit_global_section, cvar.menu_legit_sub_section)].chest);
			ImGui::Checkbox("Waist Hitbox", &cvar.legit[CheckWeapon(cvar.menu_legit_global_section, cvar.menu_legit_sub_section)].waist);
			ImGui::Checkbox("All Other Hitbox", &cvar.legit[CheckWeapon(cvar.menu_legit_global_section, cvar.menu_legit_sub_section)].all);
			SliderFloat("Auto Speed", &cvar.legit[CheckWeapon(cvar.menu_legit_global_section, cvar.menu_legit_sub_section)].speed, 0.f, 100.f, "Auto Speed: %.0f%%");
			SliderFloat("Attack Speed", &cvar.legit[CheckWeapon(cvar.menu_legit_global_section, cvar.menu_legit_sub_section)].speed_in_attack, 0.f, 100.f, "Attack Speed: %.0f%%");
			SliderFloat("Block Attack", &cvar.legit[CheckWeapon(cvar.menu_legit_global_section, cvar.menu_legit_sub_section)].block_attack_after_kill, 0.f, 1000.f, "Block Attack: %.0fms");
			SliderFloat("Maximum FOV", &cvar.legit[CheckWeapon(cvar.menu_legit_global_section, cvar.menu_legit_sub_section)].fov, 0.f, 45.f, "Maximum FOV: %.0f");
			SliderFloat("Recoil Pitch", &cvar.legit[CheckWeapon(cvar.menu_legit_global_section, cvar.menu_legit_sub_section)].recoil_compensation_pitch, 0.f, 100.f, "Recoil Pitch: %.0f%%");
			SliderFloat("Recoil Yaw", &cvar.legit[CheckWeapon(cvar.menu_legit_global_section, cvar.menu_legit_sub_section)].recoil_compensation_yaw, 0.f, 100.f, "Recoil Yaw: %.0f%%");
			SliderFloat("Recoil Compensate", &cvar.legit[CheckWeapon(cvar.menu_legit_global_section, cvar.menu_legit_sub_section)].recoil_compensation_after_shots_fired, 0.f, 10.f, "Recoil Compensate: %.0f");
			SliderFloat("Speed Scale FOV", &cvar.legit[CheckWeapon(cvar.menu_legit_global_section, cvar.menu_legit_sub_section)].speed_scale_fov, 0.f, 100.f, "Speed Scale FOV: %.0f%%");
			SliderFloat("Shot Delay", &cvar.legit[CheckWeapon(cvar.menu_legit_global_section, cvar.menu_legit_sub_section)].reaction_time, 0.f, 1000.f, "Shot Delay: %.0fms");
			ImGui::Text("Accuracy Boost");
			const char* listbox_accuracy[] = { "None", "Aiming", "Recoil", "Recoil / Spread" };
			ComboBox("Accuracy Boost", &cvar.legit[CheckWeapon(cvar.menu_legit_global_section, cvar.menu_legit_sub_section)].accuracy, listbox_accuracy, IM_ARRAYSIZE(listbox_accuracy), 4);
		}
		ImGui::EndChild();

		Spacing();

		ImGui::BeginChild("Aimbot - Trigger", ImVec2(225, 464), true);
		{
			ImGui::Text("Trigger Aimbot"), ImGui::Separator();

			ImGui::Checkbox("Activate", &cvar.legit[CheckWeapon(cvar.menu_legit_global_section, cvar.menu_legit_sub_section)].trigger_active);
			ImGui::Checkbox("Aim Team", &cvar.legit_trigger_team);
			ImGui::Checkbox("Head Hitbox", &cvar.legit[CheckWeapon(cvar.menu_legit_global_section, cvar.menu_legit_sub_section)].trigger_head);
			ImGui::Checkbox("Chest Hitbox", &cvar.legit[CheckWeapon(cvar.menu_legit_global_section, cvar.menu_legit_sub_section)].trigger_chest);
			ImGui::Checkbox("Waist Hitbox", &cvar.legit[CheckWeapon(cvar.menu_legit_global_section, cvar.menu_legit_sub_section)].trigger_waist);
			ImGui::Checkbox("All Other Hitbox", &cvar.legit[CheckWeapon(cvar.menu_legit_global_section, cvar.menu_legit_sub_section)].trigger_all);
			ImGui::Checkbox("Only Zoom", &cvar.legit_trigger_only_zoom);
			ImGui::Checkbox("Wall", &cvar.legit[CheckWeapon(cvar.menu_legit_global_section, cvar.menu_legit_sub_section)].trigger_wall);
			HudKeyBind(cvar.legit_trigger_key, "Trigger Key", false); ImGui::SameLine(); ShowHelpMarker("Only working in game");
			ImGui::Text("Accuracy Boost");
			const char* listbox_accuracy[] = { "None", "Recoil", "Recoil / Spread" };
			ComboBox("Accuracy Boost", &cvar.legit[CheckWeapon(cvar.menu_legit_global_section, cvar.menu_legit_sub_section)].trigger_accuracy, listbox_accuracy, IM_ARRAYSIZE(listbox_accuracy), 3);
			ImGui::Separator();
			ImGui::Text("Adjust Delay");
			SliderFloat("Shot Delay", &cvar.legit[CheckWeapon(cvar.menu_legit_global_section, cvar.menu_legit_sub_section)].trigger_delay_shot, 0.f, 1000.f, "Shot Delay: %.0fms");
			const char* listbox_count[] = { "Count", "Random" };
			ComboBox("Shot Type", &cvar.legit[CheckWeapon(cvar.menu_legit_global_section, cvar.menu_legit_sub_section)].trigger_shot_type, listbox_count, IM_ARRAYSIZE(listbox_count), 2);
			if (!cvar.legit[CheckWeapon(cvar.menu_legit_global_section, cvar.menu_legit_sub_section)].trigger_shot_type) SliderFloat("Shot Count", &cvar.legit[CheckWeapon(cvar.menu_legit_global_section, cvar.menu_legit_sub_section)].trigger_shot_count, 1.f, 5.f, "Shot Count: %.0f");
			if (cvar.legit[CheckWeapon(cvar.menu_legit_global_section, cvar.menu_legit_sub_section)].trigger_shot_type) SliderFloat("Random Max", &cvar.legit[CheckWeapon(cvar.menu_legit_global_section, cvar.menu_legit_sub_section)].trigger_random_max, 2.f, 5.f, "Random Max: %.0f");
			ImGui::Separator();
			ImGui::Text("Other Legit/Trigger");
			ImGui::Checkbox("Fast Zoom", &cvar.legit_fast_zoom);
			ImGui::Separator();
		}
		ImGui::EndChild();
	}
}

void MenuKnifebot()
{
	ImGui::BeginChild("Knifebot - General", ImVec2(225, 125), true);
	{
		ImGui::Text("Knifebot"), ImGui::Separator();

		ImGui::Checkbox("Activate", &cvar.knifebot_active);
		ImGui::Checkbox("Aim Team", &cvar.knifebot_team);
		ImGui::Checkbox("Silent", &cvar.knifebot_silent);
		ImGui::Checkbox("Perfect Silent", &cvar.knifebot_perfect_silent);
	}
	ImGui::EndChild();

	Spacing();

	ImGui::BeginChild("Empty", ImVec2(225, 125), true);
	{
		ImGui::Text("Knifebot2"), ImGui::Separator();

		SliderFloat("Fov", &cvar.knifebot_fov, 0.f, 180.f, "Fov: %.1f");
		ImGui::Text("Knife Attack Method");
		const char* listbox_attack[] = { "Slash", "Stab" };
		ComboBox("Knife Attack Method", &cvar.knifebot_attack, listbox_attack, IM_ARRAYSIZE(listbox_attack), 2);
		if (cvar.knifebot_attack == 0)SliderFloat("Slash Distance", &cvar.knifebot_attack_distance, 0.f, 100.f, "Slash Distance: %.0f");
		if (cvar.knifebot_attack == 1)SliderFloat("Stab Distance", &cvar.knifebot_attack2_distance, 0.f, 100.f, "Stab Distance: %.0f");
	}
	ImGui::EndChild();
}

void MenuMisc()
{
	ImGui::BeginChild("Misc - General", ImVec2(225, 215), true);
	{
		ImGui::Text("Misc1"), ImGui::Separator();

		ImGui::Checkbox("Debug Bypass Valid", &cvar.bypass_valid_blockers);
		ImGui::Checkbox("Debug Bypass Trace", &cvar.bypass_trace_blockers);
		ImGui::Checkbox("Display My Weapon Name", &cvar.misc_weapon_local);
		ImGui::Checkbox("Change STEAM ID", &cvar.misc_steamid);
		ImGui::Checkbox("Clean Snapshot", &cvar.misc_snapshot);
		SliderFloat("Snapshot Time", &cvar.misc_snapshot_time, 1.f, 60.f, "Snapshot Time: %.0f sec");
		ImGui::Separator();
	}
	ImGui::EndChild();

	Spacing();

	ImGui::BeginChild("Misc - General2", ImVec2(225, 215), true);
	{
		ImGui::Text("Misc2"), ImGui::Separator();

		ImGui::Checkbox("Gui Chat", &cvar.gui_chat);
		HudKeyBind(cvar.gui_chat_key,"Gui Chat Key", false); ImGui::SameLine(); ShowHelpMarker("Only working in game");
		HudKeyBind(cvar.gui_chat_key_team,"Chat Key Team", false); ImGui::SameLine(); ShowHelpMarker("Only working in game");
		ImGui::Checkbox("Quick Change", &cvar.misc_quick_change);
		HudKeyBind(cvar.misc_quick_change_key, "Quick Change Key", false); ImGui::SameLine(); ShowHelpMarker("Only working in game");
		ImGui::Separator();
		ImGui::Text("Radio");
		const char* listbox_radio[] = { "Radio Off","PulseEDM Dance Music","Hard Style","Big Fm","Big Fm Deutsch Rap","Radio Record","Record Dubstep","Record Hardstyle","Record Dancecore","Anison FM" };
		if (ComboBox("Radio Channel.", &cvar.gui_radio, listbox_radio, IM_ARRAYSIZE(listbox_radio), 10))channelchange = true;
		SliderFloat("Radio Volume", &cvar.gui_radio_volume, 0.f, 100.f, "Radio Volume: %.0f");
	}
	ImGui::EndChild();
}

void MenuChams()
{
	ImGui::BeginChild("Chams - General", ImVec2(225, 276), true);
	{
		ImGui::Text("Chams"), ImGui::Separator();

		ImGui::Text("Player");
		ImGui::Checkbox("Glow", &cvar.chams_player_glow);
		ImGui::Checkbox("Wall", &cvar.chams_player_wall);
		const char* listbox_player[] = { "None", "Material", "Texture", "Flat" };
		ComboBox("Player.", &cvar.chams_player, listbox_player, IM_ARRAYSIZE(listbox_player), 4);
		ImGui::Separator();
		ImGui::Text("View Model");
		ImGui::Checkbox("Glow##3", &cvar.chams_view_model_glow);
		const char* listbox_weapon[] = { "None", "Material", "Texture", "Flat" };
		ComboBox("View Model.", &cvar.chams_view_model, listbox_weapon, IM_ARRAYSIZE(listbox_weapon), 4);
		ImGui::Separator();
		ImGui::Text("World");
		ImGui::Checkbox("Glow##4", &cvar.chams_world_glow);
		ImGui::Checkbox("Wall##3", &cvar.chams_world_wall);
		const char* listbox_world[] = { "None", "Material", "Texture", "Flat" };
		ComboBox("World.", &cvar.chams_world, listbox_world, IM_ARRAYSIZE(listbox_world), 4);
	}
	ImGui::EndChild();

	Spacing();

	ImGui::BeginChild("Skeleton - General", ImVec2(225, 276), true);
	{
		ImGui::Text("Skeleton"), ImGui::Separator();
		
		ImGui::Text("Player");
		ImGui::Checkbox("Bone", &cvar.skeleton_player_bone);
		ImGui::Checkbox("HitBox", &cvar.skeleton_player_hitbox);
		const char* listbox_skeleton_model[] = { "None", "Draw Bones", "Draw Hulls" };
		ComboBox("Player.", &cvar.skeleton_models, listbox_skeleton_model, IM_ARRAYSIZE(listbox_skeleton_model), 3);
		ImGui::Separator();
		ImGui::Text("View Model");
		ImGui::Checkbox("Bone##1", &cvar.skeleton_view_model_bone);
		ImGui::Checkbox("HitBox##1", &cvar.skeleton_view_model_hitbox);
		ImGui::Checkbox("No Hands", &cvar.visual_no_hands);
		ImGui::Separator();
		ImGui::Text("World");
		ImGui::Checkbox("Bone##2", &cvar.skeleton_world_bone);
		ImGui::Checkbox("Hitbox##2", &cvar.skeleton_world_hitbox);
	}
	ImGui::EndChild();
}

void MenuEsp()
{
	ImGui::BeginChild("Visual - General", ImVec2(225, 529), true);
	{
		ImGui::Text("Visual"), ImGui::Separator();

		ImGui::Text("Player");
		ImGui::Checkbox("Box", &cvar.visual_box);
		ImGui::Checkbox("Headshot Mark", &cvar.visual_headshot_mark);
		ImGui::Checkbox("Health Bar", &cvar.visual_health);
		ImGui::Checkbox("Kill Lightning", &cvar.visual_kill_lightning);
		ImGui::Checkbox("Kill Sound", &cvar.visual_kill_sound);
		ImGui::Checkbox("Lambert", &cvar.visual_lambert);
		ImGui::Checkbox("Name", &cvar.visual_name);
		ImGui::Checkbox("Radar", &cvar.visual_radar);
		ImGui::Checkbox("Reload", &cvar.visual_reload_bar);
		ImGui::Checkbox("Sound", &cvar.visual_sound);
		ImGui::Checkbox("Vip", &cvar.visual_vip);
		ImGui::Checkbox("Visual Team", &cvar.visual_visual_team);
		ImGui::Checkbox("Wall", &cvar.visual_wall);
		ImGui::Checkbox("Weapon Name", &cvar.visual_weapon);
		ImGui::Checkbox("Weapon Model", &cvar.visual_weapon_model);
		ImGui::Separator();
		ImGui::Text("Third Person");
		ImGui::Checkbox("Chase", &cvar.visual_chase_cam);
		ImGui::Separator();
		ImGui::Text("Other");
		ImGui::Checkbox("Crosshair", &cvar.visual_crosshair);
		ImGui::Checkbox("Grenade Trajectory", &cvar.visual_grenade_trajectory);
		ImGui::Checkbox("Skin", &cvar.visual_skins);
	}
	ImGui::EndChild();

	Spacing();

	ImGui::BeginChild("Visual - Other", ImVec2(225, 529), true);
	{
		ImGui::Text("Visual Adjust"), ImGui::Separator();

		SliderFloat("Alpha Color All", &cvar.visual_alpha, 0.f, 1.f, "Alpha Color All: %.2f");
		ImGui::Separator();
		ImGui::Text("Player");
		SliderFloat("Box Height", &cvar.visual_box_height, 0.05f, 1.f, "Box Height: %.2f");
		SliderFloat("Box Width", &cvar.visual_box_width, 0.05f, 1.f, "Box Width: %.2f");
		SliderFloat("Box Top", &cvar.visual_box_size_top, 1.f, 100.f, "Box Top: %.0f");
		SliderFloat("Box Bot", &cvar.visual_box_size_bot, 1.f, 100.f, "Box Bot: %.0f");
		SliderFloat("Kill Volume", &cvar.visual_kill_volume, 0.f, 100.f, "Kill Volume: %.0f");
		SliderFloat("Radar Size", &cvar.visual_radar_size, 25.f, 100.f, "Radar Size: %.0f");
		SliderFloat("Rounding", &cvar.visual_rounding, 0.f, 5.f, "Rounding: %.0f");
		ImGui::Separator();
		ImGui::Text("Third Person");
		SliderFloat("Chase Back", &cvar.visual_chase_back, 100.f, 1000.f, "Chase Back: %.0f");
		SliderFloat("Chase Up", &cvar.visual_chase_up, 16.f, 500.f, "Chase Up: %.0f");
		ImGui::Separator();
		ImGui::Text("Other");
		SliderFloat("Crosshair Offset", &cvar.visual_crosshair_offset, 0.f, 25.f, "Crosshair Offset: %.0f");
		SliderFloat("Crosshair Spin Speed", &cvar.visual_crosshair_spin, -10.f, 10.f, "Crosshair Spin: %.1f");
		SliderFloat("Custom FOV", &cvar.visual_custom_fov, 45.f, 150.f, "FOV: %.0f");
		SliderFloat("No Flash", &cvar.visual_rem_flash, 0.f, 255.f, "No Flash: %.0f%%");
		ImGui::Separator();
	}
	ImGui::EndChild();
}

void MenuKz()
{
	ImGui::BeginChild("Kz - General", ImVec2(225, 171), true);
	{
		ImGui::Text("Kz1"), ImGui::Separator();
		
		ImGui::Checkbox("Bhop", &cvar.kz_bhop);
		ImGui::Checkbox("Fast Run", &cvar.kz_fast_run);
		ImGui::Checkbox("GStrafe", &cvar.kz_gstrafe);
		ImGui::Checkbox("Jump Bug", &cvar.kz_jump_bug);
		ImGui::Checkbox("Jump Bug Auto", &cvar.kz_jump_bug_auto);
		ImGui::Checkbox("Strafe", &cvar.kz_strafe);
	}
	ImGui::EndChild();

	Spacing();

	ImGui::BeginChild("Kz2 - General", ImVec2(225, 171), true);
	{
		ImGui::Text("Kz2"), ImGui::Separator();
		
		ImGui::Checkbox("Kz Window", &cvar.kz_show_kz);
		HudKeyBind(cvar.kz_bhop_key, "Bhop Key", false); ImGui::SameLine(); ShowHelpMarker("Only working in game");
		HudKeyBind(cvar.kz_fastrun_key, "Fastrun Key", false); ImGui::SameLine(); ShowHelpMarker("Only working in game");
		HudKeyBind(cvar.kz_gstrafe_key, "Gstrafe Key", false); ImGui::SameLine(); ShowHelpMarker("Only working in game");
		HudKeyBind(cvar.kz_jumpbug_key, "Jumpbug Key", false); ImGui::SameLine(); ShowHelpMarker("Only working in game");
		HudKeyBind(cvar.kz_strafe_key, "Strafe Key", false); ImGui::SameLine(); ShowHelpMarker("Only working in game");
	}
	ImGui::EndChild();
}

void MenuRoute()
{
	ImGui::BeginChild("Route - General", ImVec2(225, 194), true);
	{
		ImGui::Text("Route"), ImGui::Separator();
		
		ImGui::Checkbox("Auto Direction", &cvar.route_direction);
		SliderFloat("Direction Steps", &cvar.route_direction_step, 1.f, 10.f, "Direction Steps: %.0f");
		ImGui::Checkbox("Auto Jump", &cvar.route_jump);
		SliderFloat("Jump Steps", &cvar.route_jump_step, 1.f, 10.f, "Jump Steps: %.0f");
		ImGui::Checkbox("Draw Route", &cvar.route_draw);
		const char* listbox_route[] = { "Stay In Way", "Go After Enemy" };
		ComboBox("Route Mode.", &cvar.route_mode, listbox_route, IM_ARRAYSIZE(listbox_route), 2);
		ImGui::Checkbox("Visual While Rushing", &cvar.route_draw_visual);
	}
	ImGui::EndChild();

	Spacing();

	ImGui::BeginChild("Route Record - General", ImVec2(225, 194), true);
	{
		ImGui::Text("Route Record"), ImGui::Separator();
		
		ImVec4 prevColor = ImGui::GetStyle().Colors[ImGuiCol_Text];
		ImGui::GetStyle().Colors[ImGuiCol_Text] = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
		if (ImGui::Button("Start"))cmd.exec("route_record_start");
		ImGui::GetStyle().Colors[ImGuiCol_Text] = prevColor;
		if (autoroute.Record)ImGui::SameLine(), ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Recording!");
		prevColor = ImGui::GetStyle().Colors[ImGuiCol_Text];
		ImGui::GetStyle().Colors[ImGuiCol_Text] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
		if (ImGui::Button("Stop"))cmd.exec("route_record_stop");
		ImGui::GetStyle().Colors[ImGuiCol_Text] = prevColor;
		if (!autoroute.Record)ImGui::SameLine(), ImGui::Text("Recording Stopped!");
		if (ImGui::Button("Add Point"))cmd.exec("route_record_add");
		if (ImGui::Button("Clear"))cmd.exec("route_clear");
		if (ImGui::Button("Save"))cmd.exec("route_save");
		if (ImGui::Button("Load"))cmd.exec("route_load");
		HudKeyBind(cvar.route_rush_key, "Rush Key", false); ImGui::SameLine(); ShowHelpMarker("Only working in game");
	}
	ImGui::EndChild();
}

void MenuColor()
{
	ImGui::BeginChild("Color - General", ImVec2(225, 252), true);
	{
		ImGui::Text("Color"), ImGui::Separator();
		
		ImVec4 prevColor = ImGui::GetStyle().Colors[ImGuiCol_FrameBg];
		ImVec4 prevColor2 = ImGui::GetStyle().Colors[ImGuiCol_Text];
		ImGui::GetStyle().Colors[ImGuiCol_Text] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
		ImGui::GetStyle().Colors[ImGuiCol_FrameBg] = ImVec4(1.0f, 0.0f, 0.0f, 0.5f);
		SliderFloat("Color Red", &cvar.color_red, 0.f, 1.f, "Color Red: %.3f");
		ImGui::GetStyle().Colors[ImGuiCol_FrameBg] = prevColor;
		ImGui::GetStyle().Colors[ImGuiCol_Text] = prevColor2;

		prevColor = ImGui::GetStyle().Colors[ImGuiCol_FrameBg];
		prevColor2 = ImGui::GetStyle().Colors[ImGuiCol_Text];
		ImGui::GetStyle().Colors[ImGuiCol_Text] = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
		ImGui::GetStyle().Colors[ImGuiCol_FrameBg] = ImVec4(0.0f, 1.0f, 0.0f, 0.5f);
		SliderFloat("Color Green", &cvar.color_green, 0.f, 1.f, "Color Green: %.3f");
		ImGui::GetStyle().Colors[ImGuiCol_FrameBg] = prevColor;
		ImGui::GetStyle().Colors[ImGuiCol_Text] = prevColor2;

		prevColor = ImGui::GetStyle().Colors[ImGuiCol_FrameBg];
		prevColor2 = ImGui::GetStyle().Colors[ImGuiCol_Text];
		ImGui::GetStyle().Colors[ImGuiCol_Text] = ImVec4(0.0f, 0.0f, 1.0f, 1.0f);
		ImGui::GetStyle().Colors[ImGuiCol_FrameBg] = ImVec4(0.0f, 0.0f, 1.0f, 0.5f);
		SliderFloat("Color Blue", &cvar.color_blue, 0.f, 1.f, "Color Blue: %.3f");
		ImGui::GetStyle().Colors[ImGuiCol_FrameBg] = prevColor;
		ImGui::GetStyle().Colors[ImGuiCol_Text] = prevColor2;

		float crl[3];
		crl[0] = cvar.color_red, crl[1] = cvar.color_green, crl[2] = cvar.color_blue;
		ImGui::ColorPicker4("##1", (float*)&crl, ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_RGB | ImGuiColorEditFlags_PickerHueWheel, NULL);
		cvar.color_red = crl[0], cvar.color_green = crl[1], cvar.color_blue = crl[2];
	}
	ImGui::EndChild();

	Spacing();

	ImGui::BeginChild("Color2", ImVec2(225, 252), true);
	{
		ImGui::Text("Color2"), ImGui::Separator();
		
		ImGui::Checkbox("Random Color", &cvar.color_random);
		SliderFloat("Speed", &cvar.color_speed, -10.f, 10.f, "Color Change Speed: %.0f");
	}
	ImGui::EndChild();
}

void DrawMenuWindow()
{
	if (!bShowMenu)
		return;

	static bool freegui = false;

	ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x / 2, 0), freegui? ImGuiCond_Once:ImGuiCond_Always, ImVec2(0.5f, 0.5f));
	ImGui::Begin("Menu", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_AlwaysAutoResize);
	{
		ImageBorder(1, 120);
		ImGui::BeginMenuBar();
		{
			if (ImGui::BeginMenu("Gui"))
			{
				ImGui::MenuItem("Image Mouse", NULL, &cvar.gui_mouse_image);
				ImGui::MenuItem("Image Menu", NULL, &cvar.gui_menu_image);
				ImGui::MenuItem("Free Gui", NULL, &freegui);
				if (!loadtexture)ImGui::MenuItem("Reload Menu Texture", NULL, &loadtexture);
				ImGui::MenuItem("ShowDemo", NULL, &show_demo);
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Key"))
			{
				KeyBind(cvar.gui_key, "WND PROC");
				HudKeyBind(cvar.gui_key_hud, "HUD Key Event", true); ImGui::SameLine(); ShowHelpMarker("Only working in game");
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Image"))
			{
				ImGui::SliderFloat("##1", &cvar.gui_menu_button_spin, -10.f, 10.f, "Image Button Speed: %.1f");
				ImGui::SliderFloat("##2", &cvar.gui_menu_image_spin, -10.f, 10.f, "Image Menu Speed: %.1f");
				ImGui::SliderFloat("##3", &cvar.gui_mouse_image_spin, -10.f, 10.f, "Image Mouse Speed: %.1f");
				ImGui::SliderFloat("##4", &cvar.gui_mouse_image_amount, 1.f, 10.f, "Image Mouse Amount: %.0f");
				ImGui::EndMenu();
			}
			ImGui::SameLine(); 
			SYSTEMTIME SysTime;
			GetLocalTime(&SysTime);
			ImGui::SameLine(350); ImGui::Text("Time: %02d:%02d:%02d", SysTime.wHour, SysTime.wMinute, SysTime.wSecond);
			ImGui::EndMenuBar();
		}
	
		ImGui::Separator();

		if (ImGui::Button("ID Hook")) func_player_toggle();
		ImGui::SameLine(); if (ImGui::Button("Console")) show_app_console = !show_app_console;

		ImGui::Separator();
		ImGui::Spacing();

		ImVec2 LastSapcing = ImGui::GetStyle().ItemSpacing;
		ImGui::GetStyle().ItemSpacing = ImVec2(0, 0);
		Menutabs(1, RAGE, false);
		Menutabs(2, LEGIT, true);
		Menutabs(3, KNIFE, true);
		Menutabs(4, SETTINGS, true);
		Menutabs(5, MODEL, true);
		Menutabs(6, VISUAL, true);
		Menutabs(7, BUNNY, true);
		Menutabs(8, WAYPOINT, true);
		Menutabs(9, COLOR, true);
		ImGui::GetStyle().ItemSpacing = LastSapcing;

		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Separator();

		ImVec4 prevColor = ImGui::GetStyle().Colors[ImGuiCol_Text];
		ImGui::GetStyle().Colors[ImGuiCol_Text] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
		if (ImGui::Button("Rage"))cvar.rage_active = 1, ModeChangeDelay = GetTickCount();
		ImGui::GetStyle().Colors[ImGuiCol_Text] = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
		ImGui::SameLine();
		if (ImGui::Button("Legit"))cvar.rage_active = 0, ModeChangeDelay = GetTickCount();
		ImGui::GetStyle().Colors[ImGuiCol_Text] = prevColor;
		
		ImGui::SameLine();
		if (cvar.rage_active)
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Rage Active");
		else
			ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Legit Active");
		
		ImGui::Separator();
		if (MenuTab == 1)
			MenuRage();
		else if (MenuTab == 2)
			MenuLegit();
		else if (MenuTab == 3)
			MenuKnifebot();
		else if (MenuTab == 4)
			MenuMisc();
		else if (MenuTab == 5)
			MenuChams();
		else if (MenuTab == 6)
			MenuEsp();
		else if (MenuTab == 7)
			MenuKz();
		else if (MenuTab == 8)
			MenuRoute();
		else if (MenuTab == 9)
			MenuColor();

		ImGui::End();
	}
}