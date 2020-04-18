#ifndef _IMGUIHOOK_
#define _IMGUIHOOK_

extern bool bShowMenu;
extern bool show_app_console;
extern bool loadtexture;
extern bool keys[256];
extern bool triggerkeys[256];
extern bool keysmenu[256];
extern bool show_demo;
extern bool channelchange;

extern int MenuTab;

extern GLuint texture_id[1024];

extern DWORD ModeChangeDelay;
extern DWORD stream;

bool WorldToScreen(float* pflOrigin, float* pflVecScreen);

ImVec2 ImRotationCenter();

string vkToString(int vk);
string GetKeyName(unsigned int virtualKey);
char* KeyEventToChar(int Key);

void HookImGui(HDC hdc);
void ImRotateStart();
void ImRotateEnd(float rad, ImVec2 center = ImRotationCenter());
void InitRadio();
void PlayRadio();
void GetTextureModel();

#endif