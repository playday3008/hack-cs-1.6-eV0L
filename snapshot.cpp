#include "client.h"

glReadPixels_t glReadPixels_s;
bool ScreenFirst = true, bAntiSSTemp = true, DrawVisuals, FirstFrame = true;
PBYTE BufferScreen;
int temp;
DWORD dwSize, time_scr;

void __stdcall m_glReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid* pixels)
{
	if (ScreenFirst || !cvar.misc_snapshot)
	{
		dwSize = (width * height) * 3;
		BufferScreen = (PBYTE)malloc(dwSize);
		glReadPixels_s(x, y, width, height, format, type, pixels);
		__try
		{
			memcpy(BufferScreen, pixels, dwSize);
		}
		__except (EXCEPTION_EXECUTE_HANDLER) {};
		DrawVisuals = true;
		ScreenFirst = false;
		return;
	}
	__try
	{
		memcpy(pixels, BufferScreen, dwSize);
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {};
}

void Snapshot()
{
	if (cvar.misc_snapshot)
	{
		if (bAntiSSTemp)
		{
			time_scr = GetTickCount();
			temp = 0;

			bAntiSSTemp = false;
		}

		if (GetTickCount() - time_scr > cvar.misc_snapshot_time * 1000)
		{
			DrawVisuals = false;
			temp++;

			if (temp > 10)
			{
				bAntiSSTemp = true;
				ScreenFirst = true;
				DWORD sz = ImGui::GetIO().DisplaySize.x * ImGui::GetIO().DisplaySize.y * 3;
				free((PBYTE)BufferScreen);
				PBYTE buf = (PBYTE)malloc(sz);
				glReadPixels(0, 0, ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y, GL_RGB, GL_UNSIGNED_BYTE, buf);
				free((PBYTE)buf);
			}

		}
	}
	if (FirstFrame)
	{
		DWORD sz = ImGui::GetIO().DisplaySize.x * ImGui::GetIO().DisplaySize.y * 3;
		PBYTE buf = (PBYTE)malloc(sz);
		glReadPixels(0, 0, ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y, GL_RGB, GL_UNSIGNED_BYTE, buf);
		free((PBYTE)buf);

		FirstFrame = false;
	}
}