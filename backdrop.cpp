#include "client.h"

class BackDrop
{
public:
	BackDrop(Vector2D p, Vector2D v)
	{
		RelativePosition = p;
		Velocity = v;
	}

	float r = g_Engine.pfnRandomFloat(4, 4 + 10);

	Vector2D RelativePosition = Vector2D(0, 0);
	Vector2D Velocity;
};

std::vector<BackDrop*> Dots;

void DrawBackDrop()
{
	float WindowBorderSize = ImGui::GetStyle().WindowBorderSize;
	ImGui::GetStyle().WindowBorderSize = 0.0f;
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
	ImGui::Begin("Backdrop", reinterpret_cast<bool*>(true), ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs);
	{
		ImGui::SetWindowPos(ImVec2(0, 0), ImGuiSetCond_Always);
		ImGui::SetWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y), ImGuiSetCond_Always);
		
		if (bShowMenu && cvar.gui_mouse_image)
		{
			static DWORD Tickcount = 0;
			static DWORD Tickcheck = 0;
			if (GetTickCount() - Tickcount >= 1)
			{
				if (Tickcheck != Tickcount)
				{
					static float radius = 0;
					radius++;
					if (radius > 3)
						radius = 1;
					static int angle = 0;
					angle += cvar.gui_mouse_image_spin;
					if (angle >= 360 || angle <= -360)
						angle = 0;
					float x = radius * sin(M_PI * 2 * angle / 360);
					float y = radius * cos(M_PI * 2 * angle / 360);
					static float s = 0;
					s++;
					if (s > 10-(int)cvar.gui_mouse_image_amount)
						s = 0;

					if (cvar.gui_mouse_image_spin && s == 0)Dots.push_back(new BackDrop(Vector2D(ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y), Vector2D(x, y)));

					for (auto i = Dots.begin(); i < Dots.end();)
					{
						if ((*i)->RelativePosition.y < -20 || (*i)->RelativePosition.y > ImGui::GetIO().DisplaySize.y + 20 || (*i)->RelativePosition.x < -20 || (*i)->RelativePosition.x > ImGui::GetIO().DisplaySize.x + 20)
						{
							delete (*i);
							i = Dots.erase(i);
						}
						else
						{
							(*i)->RelativePosition.x = (*i)->RelativePosition.x + ((*i)->Velocity.x * (*i)->r) * 0.1;

							(*i)->RelativePosition.y = (*i)->RelativePosition.y + ((*i)->Velocity.y * (*i)->r) * 0.1;
							i++;
						}
					}
					Tickcheck = Tickcount;
				}
				Tickcount = GetTickCount();
			}
			for (auto i = Dots.begin(); i < Dots.end(); i++) 
			{
				ImGui::GetWindowDrawList()->AddImageQuad((GLuint*)texture_id[BACKDROP], ImVec2((*i)->RelativePosition.x + (*i)->r+10, (*i)->RelativePosition.y - (*i)->r-10), ImVec2((*i)->RelativePosition.x - (*i)->r-10, (*i)->RelativePosition.y - (*i)->r-10), ImVec2((*i)->RelativePosition.x - (*i)->r-10, (*i)->RelativePosition.y + (*i)->r+10), ImVec2((*i)->RelativePosition.x + (*i)->r+10, (*i)->RelativePosition.y + (*i)->r+10));
			}
		}
		ImGui::End();
		ImGui::PopStyleColor();
		ImGui::GetStyle().WindowBorderSize = WindowBorderSize;
	}
}