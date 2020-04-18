#include "client.h"

typedef void (APIENTRY* glBegin_t)(GLenum);
typedef void (APIENTRY* glColor4f_t)(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
typedef BOOL(APIENTRY* wglSwapBuffers_t)(HDC  hdc);
typedef void (APIENTRY* glViewport_t)(GLint x, GLint y, GLsizei width, GLsizei height);
typedef void (APIENTRY* glClear_t)(GLbitfield mask);
typedef void (APIENTRY* glVertex3f_t)(GLfloat x, GLfloat y, GLfloat z);

glBegin_t pglBegin = NULL;
glColor4f_t pglColor4f = NULL;
wglSwapBuffers_t pwglSwapBuffers = NULL;
glViewport_t pglViewport = NULL;
glClear_t pglClear = NULL;
glVertex3f_t pglVertex3f = NULL;

int TexHandsIndex[3];

void APIENTRY Hooked_glBegin(GLenum mode)
{
	cl_entity_s* ent = g_Studio.GetCurrentEntity();
	bool Player = ent && ent->player;
	bool Weapon = ent && ent->model && (strstr(ent->model->name, "p_") || strstr(ent->model->name, "w_"));
	bool View_Model = ent && ent->model && strstr(ent->model->name, "v_");
	
	if (cvar.visual_wall && CheckDrawEngine())
	{
		if (Player || Weapon)
			glDepthRange(0, 0.5);
		else
			glDepthRange(0.5, 1);
	}

	if (cvar.visual_lambert && (Player && !cvar.chams_player_glow || Weapon && !cvar.chams_world_glow || View_Model && !cvar.chams_view_model_glow) && CheckDrawEngine())
	{
		if (mode == GL_TRIANGLE_STRIP)
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	}
	if (cvar.visual_no_hands && g_Local.bAlive && CheckDrawEngine())
	{
		GLint last_texture;
		glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
		
		for (int i = 0; i < 3; i++)
			if (last_texture == TexHandsIndex[i]) return;
	}
	(*pglBegin)(mode);
}

void APIENTRY Hooked_glColor4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
	if (chams_world)
	{
		if (cvar.chams_world == 1 || cvar.chams_world == 3)
			red = chams_world_r, green = chams_world_g, blue = chams_world_b;
		if (cvar.chams_world == 2)
			red = chams_world_r* red, green = chams_world_g* green, blue = chams_world_b* blue;
	}
	if (chams_viewmodel)
	{
		if (cvar.chams_view_model == 1 || cvar.chams_view_model == 3)
			red = chams_viewmodel_r, green = chams_viewmodel_g, blue = chams_viewmodel_b;
		if (cvar.chams_view_model == 2)
			red = chams_viewmodel_r* red, green = chams_viewmodel_g* green, blue = chams_viewmodel_b* blue;
	}
	if (chams_player)
	{
		if (cvar.chams_player == 1 || cvar.chams_player == 3)
			red = chams_player_r, green = chams_player_g, blue = chams_player_b;
		if (cvar.chams_player == 2)
			red = chams_player_r * red, green = chams_player_g * green, blue = chams_player_b * blue;
	}
	(*pglColor4f)(red, green, blue, alpha);
}

BOOL APIENTRY Hooked_wglSwapBuffers(HDC hdc)
{
	HookImGui(hdc);
	return(*pwglSwapBuffers)(hdc);
}

void APIENTRY Hooked_glViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
	static bool FirstFrame = true;
	if (FirstFrame)
	{
		g_Engine.pfnClientCmd("toggleconsole");

		g_Engine.Con_Printf("\n\n\t\t\t\tHello, %s ;)\n", g_Engine.pfnGetCvarString("name"));
		g_Engine.Con_Printf("\t\t\t\tYou are injected!\n\n");

		g_Engine.Con_Printf("\t\t\t\tMultihack by:\n");
		g_Engine.Con_Printf("\t\t\t\t            [..         [..    [....     [..      \n");
		g_Engine.Con_Printf("\t\t\t\t             [..       [..   [..    [..  [..      \n");
		g_Engine.Con_Printf("\t\t\t\t     [..      [..     [..  [..        [..[..      \n");
		g_Engine.Con_Printf("\t\t\t\t   [.   [..    [..   [..   [..        [..[..      \n");
		g_Engine.Con_Printf("\t\t\t\t  [..... [..    [.. [..    [..        [..[..      \n");
		g_Engine.Con_Printf("\t\t\t\t  [.             [....       [..     [.. [..      \n");
		g_Engine.Con_Printf("\t\t\t\t    [....         [..          [....     [........\n\n");

		g_Engine.Con_Printf("\t\t\t\tSpecial thanks to my friend BloodSharp and oxiKKK <3\n\n");
		g_Engine.Con_Printf("\t\t\t\tATTENTION! Only WND PROC menu key can be set outside game!\n");
		if (cvar.gui_key == -1)
			g_Engine.Con_Printf("\t\t\t\tMenu key is [No key]!\n");
		else if (cvar.gui_key == -2)
			g_Engine.Con_Printf("\t\t\t\tMenu key is [Press key]!\n");
		else
			g_Engine.Con_Printf("\t\t\t\tMenu key is [%s]!\n", GetKeyName((int)cvar.gui_key).c_str());

		FirstFrame = false;
	}
	static float ChangeKey = cvar.gui_key;
	if (ChangeKey != cvar.gui_key)
	{
		if (cvar.gui_key == -1)
			g_Engine.Con_Printf("\t\t\t\tMenu key is [No key]!\n");
		else if (cvar.gui_key == -2)
			g_Engine.Con_Printf("\t\t\t\tMenu key is [Press key]!\n");
		else
			g_Engine.Con_Printf("\t\t\t\tMenu key is [%s]!\n", GetKeyName((int)cvar.gui_key).c_str());

		ChangeKey = cvar.gui_key;
	}
	(*pglViewport)(x, y, width, height);
}

void APIENTRY Hooked_glClear(GLbitfield mask)
{
	if (cvar.visual_chase_cam && g_pEngine->GetMaxClients() && g_Local.bAlive)
	{
		if (mask == GL_DEPTH_BUFFER_BIT)
			(*pglClear)(GL_COLOR_BUFFER_BIT), glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	}
	(*pglClear)(mask);
}

void APIENTRY Hooked_glVertex3f(GLfloat x, GLfloat y, GLfloat z)
{
	cl_entity_s* ent = g_Studio.GetCurrentEntity();
	bool Player = ent && ent->player && (g_Player[ent->index].iTeam != g_Local.iTeam || cvar.visual_visual_team);
	if (cvar.skeleton_models && Player && CheckDrawEngine())
		return;
	(*pglVertex3f)(x, y, z);
}

void HookOpenGL()
{
	HMODULE hmOpenGL = GetModuleHandle("opengl32.dll");
	if (hmOpenGL)
	{
		pglBegin = (glBegin_t)DetourFunction((LPBYTE)GetProcAddress(hmOpenGL, "glBegin"), (LPBYTE)& Hooked_glBegin);
		pglColor4f = (glColor4f_t)DetourFunction((LPBYTE)GetProcAddress(hmOpenGL, "glColor4f"), (LPBYTE)& Hooked_glColor4f);
		pwglSwapBuffers = (wglSwapBuffers_t)DetourFunction((LPBYTE)GetProcAddress(hmOpenGL, "wglSwapBuffers"), (LPBYTE)& Hooked_wglSwapBuffers);
		pglViewport = (glViewport_t)DetourFunction((LPBYTE)GetProcAddress(hmOpenGL, "glViewport"), (LPBYTE)& Hooked_glViewport);
		pglClear = (glClear_t)DetourFunction((LPBYTE)GetProcAddress(hmOpenGL, "glClear"), (LPBYTE)& Hooked_glClear);
		pglVertex3f = (glVertex3f_t)DetourFunction((LPBYTE)GetProcAddress(hmOpenGL, "glVertex3f"), (LPBYTE)& Hooked_glVertex3f);
	}
}
