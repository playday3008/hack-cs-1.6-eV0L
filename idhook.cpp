#include "client.h"

IdHook idhook;
typedef IdHook::Player Player;
typedef IdHook::PlayerEntry PlayerEntry;
static IdHook::Player* curPlayer = idhook.basePlayer;

bool player_active = false;
int selection=0, seekselection=0;

void func_relistplayer()
{
	idhook.RelistPlayer();
}

void func_clearallplayer()
{
	idhook.ClearPlayer();
}

void func_addplayer()
{
	if (!cmd.argS(1).empty())
		idhook.AddPlayer(cmd.argI(1));
}

void player_describe_current()
{
	if (!player_active || !curPlayer) return;
	IdHook::PlayerEntry& entry = curPlayer->items[curPlayer->selection];
}

void func_player_select()
{
	if (!player_active)
	{
		idhook.init();
	}

	curPlayer = idhook.basePlayer;
	curPlayer->boundSelection();
	player_describe_current();

	if (player_active)
	{
		IdHook::PlayerEntry& entry = curPlayer->items[curPlayer->selection];
		if (entry.player)
		{
			curPlayer = entry.player;
			curPlayer->boundSelection();
			player_describe_current();
		}
		else
		{
			int i = curPlayer->selection;
			int j = curPlayer->seekselection;
			cmd.exec(const_cast<char*>(entry.content));
			player_active = false;
			func_player_select();
			curPlayer->selection = i;
			curPlayer->seekselection = j;
			curPlayer->boundSelection1();
			player_describe_current();
		}
	}
	else
	{
		curPlayer->selection = selection;
		curPlayer->seekselection = seekselection;
		curPlayer->boundSelection1();
		player_describe_current();
	}
	player_active = true;
}

void func_player_toggle()
{
	if (!player_active)
	{
		func_player_select();
	}
	else
	{
		selection = curPlayer->selection;
		seekselection = curPlayer->seekselection;
		player_active = false;
		SaveCvar();
	}
}

void ListIdHook()
{
	if (player_active)
	{
		int i = curPlayer->selection;
		int j = curPlayer->seekselection;
		idhook.RelistPlayer();
		player_active = false;
		func_player_select();
		curPlayer->selection = i;
		curPlayer->seekselection = j;
		curPlayer->boundSelection1();
		player_describe_current();
	}
}

void func_first_kill_mode()
{
	if (cvar.aim_id_mode == 0.0)
		cvar.aim_id_mode = 1.0;
	else if (cvar.aim_id_mode == 1.0)
		cvar.aim_id_mode = 2.0;
	else
		cvar.aim_id_mode = 0.0;
}

void listPlayer(Player* pPlayer)
{
	PlayerEntry newEntry;

	sprintf(newEntry.content, "relistplayer");

	if (cvar.aim_id_mode == 0)
		sprintf(newEntry.name, "Attack All");
	if (cvar.aim_id_mode == 1)
		sprintf(newEntry.name, "Attack On First, Dont Attack Off");
	if (cvar.aim_id_mode == 2)
		sprintf(newEntry.name, "Attack Only On");
	sprintf(newEntry.content, "first_kill_mode");
	newEntry.player = 0;
	pPlayer->items.push_back(newEntry);

	sprintf(newEntry.name, "Clear ID");
	sprintf(newEntry.content, "clearallplayer");
	newEntry.player = 0;
	pPlayer->items.push_back(newEntry);

	char* teamname;
	for (int ax = 0; ax < 33; ax++)
	{
		if (ax == g_Local.iIndex)
			continue;

		if (strcmp(g_Player[ax].entinfo.name, "\\missing-name\\") && g_Player[ax].entinfo.name[0] != '\0' && g_Player[ax].iTeam > 0 && g_Player[ax].iTeam == 1)
		{
			teamname = "[T]";//T
			if (idhook.FirstKillPlayer[ax] == 0)
				sprintf(newEntry.name, "%s %s", teamname, g_Player[ax].entinfo.name);
			else if (idhook.FirstKillPlayer[ax] == 1)
				sprintf(newEntry.name, "%s %s [On]", teamname, g_Player[ax].entinfo.name);
			else
				sprintf(newEntry.name, "%s %s [Off]", teamname, g_Player[ax].entinfo.name);
			sprintf(newEntry.content, "addplayer %d", ax);
			newEntry.player = 0;
			pPlayer->items.push_back(newEntry);
		}
	}
	for (int ax = 0; ax < 33; ax++)
	{
		if (ax == g_Local.iIndex)
			continue;

		if (strcmp(g_Player[ax].entinfo.name, "\\missing-name\\") && g_Player[ax].entinfo.name[0] != '\0' && g_Player[ax].iTeam > 0 && g_Player[ax].iTeam == 2)
		{
			teamname = "[CT]";//CT
			if (idhook.FirstKillPlayer[ax] == 0)
				sprintf(newEntry.name, "%s %s", teamname, g_Player[ax].entinfo.name);
			else if (idhook.FirstKillPlayer[ax] == 1)
				sprintf(newEntry.name, "%s %s [On]", teamname, g_Player[ax].entinfo.name);
			else
				sprintf(newEntry.name, "%s %s [Off]", teamname, g_Player[ax].entinfo.name);
			sprintf(newEntry.content, "addplayer %d", ax);
			newEntry.player = 0;
			pPlayer->items.push_back(newEntry);
		}
	}
}

void IdHook::init()
{
	if (basePlayer) delete basePlayer;
	basePlayer = new Player;
	basePlayer->name = " ";
	listPlayer(basePlayer);
}

void IdHook::AddPlayer(int ax)
{
	if (FirstKillPlayer[ax] == 0)
		FirstKillPlayer[ax] = 1;
	else if (FirstKillPlayer[ax] == 1)
		FirstKillPlayer[ax] = 2;
	else
		FirstKillPlayer[ax] = 0;
}

void IdHook::ClearPlayer()
{
	for (int i = 0; i < 33; i++)
	{
		FirstKillPlayer[i] = 0;
	}
}

void IdHook::RelistPlayer()
{
	for (int i = 0; i < 33; i++)
	{
		if (i >= 33)
			return;

		if (g_Player[i].entinfo.name[0] == '\0')
			FirstKillPlayer[i] = 0;
	}
}

void ID_HookCommands()
{
	cmd.AddCommand("addplayer", func_addplayer);
	cmd.AddCommand("relistplayer", func_relistplayer);
	cmd.AddCommand("clearallplayer", func_clearallplayer);
	cmd.AddCommand("player_toggle", func_player_toggle);
	cmd.AddCommand("player_select", func_player_select);
	cmd.AddCommand("first_kill_mode", func_first_kill_mode);
}

void DrawIDHookWindow()
{
	if (!player_active)
		return;

	if (!bShowMenu)
		return;

	if (!curPlayer)
		curPlayer = idhook.basePlayer;

	ImGui::SetNextWindowPos(ImVec2(100, 50), ImGuiCond_Once);
	ImGui::Begin("ID Hook", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize);
	{
		ImGui::Checkbox("Esp Only ID Hook", &cvar.visual_idhook_only);

		vector<IdHook::PlayerEntry>& items = curPlayer->items;
		ImVec4 col_default_text = ImGui::GetStyleColorVec4(ImGuiCol_Text);
		for (unsigned int i = 0; i < items.size(); i++)
		{
			IdHook::PlayerEntry& item = items[i];
			ImVec4 col = col_default_text;
			if (strstr(item.name, "[T]")) col = ImColor(1.f, 0.f, 0.f, 1.0f);
			if (strstr(item.name, "[CT]")) col = ImColor(0.f, 0.f, 1.f, 1.0f);
			if (strstr(item.name, "[On]")) col = ImColor(0.f, 1.f, 0.f, 1.0f);
			if (strstr(item.name, "[Off]")) col = ImColor(1.f, 1.f, 0.f, 1.0f);

			ImGui::PushStyleColor(ImGuiCol_Text, col);

			if (i <= 1)
			{
				if (ImGui::Button(item.name))
					curPlayer->selection = i, func_player_select();
				ImGui::Separator();
			}

			if (i >= 2)
			{
				if (ImGui::Button(item.name))
					curPlayer->selection = i, func_player_select();
			}
			ImGui::PopStyleColor();
		}
		ImGui::End();
	}
}