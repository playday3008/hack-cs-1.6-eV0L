#ifndef _ESP_
#define _ESP_

static constexpr float rad_2_deg(float radian) { return radian * (180.f / M_PI); }
static constexpr float deg_2_rad(float degree) { return degree * (M_PI / 180.f); }

void DrawPlayerEsp();
void DrawNoFlash();
void DrawHitMark();
void DrawSoundEsp();
void DrawTraceGrenade();
void DrawCrossHair(); 
void CustomFOV();

#endif