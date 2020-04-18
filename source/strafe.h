#ifndef _STRAFE_
#define _STRAFE_

extern bool Strafe;
extern bool Fastrun;
extern bool Gstrafe;
extern bool Bhop;
extern bool Jumpbug;
extern inline float EdgeDistance();

void Kz(float frametime, struct usercmd_s *cmd);
void DrawKzKeys();
void DrawKzWindow();
void DrawLongJump();
void KzFameCount();
float Damage();

#endif