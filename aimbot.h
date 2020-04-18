#ifndef _AIMBOT_
#define _AIMBOT_

extern DWORD dwReactionTime;
extern bool TriggerKeyStatus;
extern bool RageKeyStatus;
extern int iTargetTrigger;
extern int iTargetLegit;
extern int iTargetRage;
extern int iTargetKnife;

void AimBot(struct usercmd_s* cmd);
void DrawAimBotFOV();
void AntiAim(struct usercmd_s* cmd);
void FakeLag(struct usercmd_s* cmd);
void FastZoom(struct usercmd_s* cmd);
void UpdateAimbot(float frametime);
void ContinueFire(usercmd_s* cmd);
void DrawAimbot();
bool FakeEdge(float& angle);

#endif