#include "client.h"

GuiConsole console;

void GuiConsole::Draw()
{
	if (!show_app_console || !bShowMenu)
		return;

	ImGui::SetNextWindowPos(ImVec2(200, 200), ImGuiCond_Once);
	ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);
	ImGui::Begin("Console", NULL, ImGuiWindowFlags_NoTitleBar );
	{
		if (ImGui::SmallButton("Clear"))
		{
			ClearLog();
		}
		ImGui::SameLine();
		bool copy_to_clipboard = ImGui::SmallButton("Copy all to clipboard");
		ImGui::SameLine();
		if (ImGui::SmallButton("Scroll to bottom")) ScrollToBottom = true;

		ImGui::Separator();

		const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing(); // 1 separator, 1 input text
		ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar); // Leave room for 1 separator + 1 InputText
		if (ImGui::BeginPopupContextWindow())
		{
			if (ImGui::Selectable("Clear")) ClearLog();
			ImGui::EndPopup();
		}

		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1));
		if (copy_to_clipboard)
			ImGui::LogToClipboard();
		ImVec4 col_default_text = ImGui::GetStyleColorVec4(ImGuiCol_Text);
		for (unsigned int i = 0; i < Items.Size; i++)
		{
			const char* item = Items[i];
			ImVec4 col = col_default_text;
			if (strstr(item, "Unknown command")) col = ImColor(1.f, 0.f, 0.f, 1.0f);
			ImGui::PushStyleColor(ImGuiCol_Text, col);
			ImGui::TextUnformatted(item);
			ImGui::PopStyleColor();
		}
		if (copy_to_clipboard)
			ImGui::LogFinish();
		if (ScrollToBottom)
			ImGui::SetScrollHere(1.0f);
		ScrollToBottom = false;
		ImGui::PopStyleVar();
		ImGui::EndChild();
		ImGui::Separator();

		bool reclaim_focus = false;
		if (ImGui::InputText("Input Text", InputBuf, IM_ARRAYSIZE(InputBuf), ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory, &TextEditCallbackStub, (void*)this))
		{
			char* s = InputBuf;
			Strtrim(s);
			if (s[0])
			{
				AddLog("%s\n", s);
				// Insert into history. First find match and delete it so it can be pushed to the back. This isn't trying to be smart or optimal.
				HistoryPos = -1;
				for (int i = History.Size - 1; i >= 0; i--)
				{
					if (Stricmp(History[i], s) == 0)
					{
						free(History[i]);
						History.erase(History.begin() + i);
						break;
					}
				}
				History.push_back(Strdup(s));
				cmd.exec(s);
			}
			strcpy(s, "");
			reclaim_focus = true;
		}
		ImGui::SetItemDefaultFocus();
		if (reclaim_focus)
			ImGui::SetKeyboardFocusHere(-1);

		ImGui::End();
	}
}

void DrawConsoleWindow()
{
	console.Draw();
}

void LogToFile(const char * fmt, ...)
{
	va_list		va_alist;
	char		buf[256];
	char		logbuf[1024];
	char		cDirectory[600];
	FILE*		file;
	struct tm*	current_tm;
	time_t		current_time;

	time(&current_time);
	current_tm = localtime(&current_time);
	sprintf(logbuf, "%02d:%02d:%02d: ", current_tm->tm_hour, current_tm->tm_min, current_tm->tm_sec);
	va_start(va_alist, fmt);
	vsprintf(buf, fmt, va_alist);
	va_end(va_alist);

	strcat(logbuf, buf);
	strcat(logbuf, "\n");
	strcpy(cDirectory, hackdir);
	strcat(cDirectory, "console.log");

	if ((file = fopen(cDirectory, "a+")) != NULL)
	{
		fputs(logbuf, file);
		fclose(file);
	}
}

void ConsolePrint(const char* fmt, ...)
{
	va_list va_alist;
	char buf[256];
	va_start(va_alist, fmt);
	_vsnprintf(buf, sizeof(buf), fmt, va_alist);
	va_end(va_alist);
	console.AddLog(buf);
	LogToFile(buf);
}
