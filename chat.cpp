#include "client.h"

int Active, Mode;
HWND hEdit;
WNDPROC pfnOrigProc;
HWND mainhwnd;

void EnterInput()
{
	char Text[255];
	char SayText[255];
	GetWindowText(hEdit, Text, 255);
	if (Mode == 1)
		sprintf(SayText, "say %s", Text);
	else if (Mode == 2)
		sprintf(SayText, "say_team %s", Text);

	g_Engine.pfnClientCmd(SayText);
}

LRESULT CALLBACK EditWindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (wparam)
	{
	case VK_RETURN:
	{
		if (msg == WM_KEYDOWN)
		{
			EnterInput();
			SetWindowText(hEdit, "");
			SetFocus(mainhwnd);
			Active = false;
		}
		return(0);
	}
	break;

	case VK_ESCAPE:
	{
		if (msg == WM_KEYDOWN)
		{
			SetWindowText(hEdit, "");
			SetFocus(mainhwnd);
			Active = false;
		}
		return(0);
	}
	break;

	default:break;
	}

	return (pfnOrigProc(hwnd, msg, wparam, lparam));

}

void OpenInput()
{
	hEdit = CreateWindowEx(WS_EX_CLIENTEDGE | WS_EX_NOPARENTNOTIFY, "EDIT", NULL, WS_CHILDWINDOW | ES_AUTOHSCROLL | WS_VISIBLE | WS_TABSTOP, 0, 0, 0, 0, mainhwnd, 0, NULL, 0);
	HDC dc = GetDC(hEdit);
	SetBkMode(dc, TRANSPARENT);
	SendMessage(hEdit, EM_LIMITTEXT, 72, 0L);
	pfnOrigProc = (WNDPROC)GetWindowLong(hEdit, GWL_WNDPROC);
	SetWindowLong(hEdit, GWL_WNDPROC, (LONG)(WNDPROC)EditWindowProc);
}

void gChatInputInit()
{
	Mode = 1;
	mainhwnd = GetFocus();
	static bool firsttime = true;
	if (firsttime)
	{
		OpenInput();
		firsttime = false;
	}
	SetFocus(hEdit);
}

void ActivateChat()
{
	if (!Active)
	{
		gChatInputInit();
		Mode = 1;
		Active = 1;
	}
}

void ActivateChatTeam()
{
	if (!Active)
	{
		gChatInputInit();
		Mode = 2;
		Active = 1;
	}
}

void DrawChatWindow()
{
	if (!Active) return;

	SetFocus(hEdit);

	if (Mode == 1)
	{
		ImGui::SetNextWindowPos(ImVec2(20, ((float)ImGui::GetIO().DisplaySize.y / 2) + ImGui::GetIO().DisplaySize.y / 20 * 5), ImGuiCond_FirstUseEver);
		ImGui::Begin("ChatInput", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);
		{
			char Text[255];
			GetWindowText(hEdit, Text, 255);
			int len = GetWindowText(hEdit, Text, 255);

			if (len)
			{
				ImGui::Text(Text);
			}
			else
			{
				sprintf(Text, "%s", "Input text");
				ImGui::Text(Text);
			}
			ImGui::End();
		}
	}
	else
	{
		ImGui::SetNextWindowPos(ImVec2(20, (ImGui::GetIO().DisplaySize.y / 2) + ImGui::GetIO().DisplaySize.y / 20 * 5), ImGuiCond_FirstUseEver);
		ImGui::Begin("ChatInput Team", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize);
		{
			char Text[255];
			GetWindowText(hEdit, Text, 255);
			int len = GetWindowText(hEdit, Text, 255);

			if (len)
			{
				ImGui::Text(Text);
			}
			else
			{
				sprintf(Text, "%s", "Input text");
				ImGui::Text(Text);
			}
			ImGui::End();
		}
	}
}