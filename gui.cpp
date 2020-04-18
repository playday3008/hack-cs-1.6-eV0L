#include "client.h"

HWND hGameWnd;
WNDPROC hGameWndProc;
LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

bool bShowMenu = false;
bool show_app_console = false;
bool loadtexture = true;
bool keys[256];
bool keysmenu[256];
bool bOldOpenGL = true;
bool show_demo = false;
bool channelchange = false;

int MenuTab;
int rotation_start_index;

GLuint texture_id[1024];
GLint iMajor, iMinor;

DWORD ModeChangeDelay = 0;
DWORD stream;

void InitRadio()
{
	BASS::bass_lib_handle = BASS::bass_lib.LoadFromMemory(bass_dll_image, sizeof(bass_dll_image));
	static bool once;
	if (!once)
	{
		if (BASS_Init(-1, 44100, 0, nullptr, nullptr))
		{
			BASS_SetConfig(BASS_CONFIG_NET_PLAYLIST, 1);
			BASS_SetConfig(BASS_CONFIG_NET_PREBUF, 0);
			BASS::stream_sounds.headshot = BASS_StreamCreateFile(TRUE, headshot, 0, sizeof(headshot), 0);
			BASS::stream_sounds.doublekill = BASS_StreamCreateFile(TRUE, doublekill, 0, sizeof(doublekill), 0);
			BASS::stream_sounds.triplekill = BASS_StreamCreateFile(TRUE, triplekill, 0, sizeof(triplekill), 0);
			BASS::stream_sounds.monsterkill = BASS_StreamCreateFile(TRUE, monsterkill, 0, sizeof(monsterkill), 0);
			once = true;
		}
	}
}

std::string zstations[] = {
			"Not Used",
			"http://pulseedm.cdnstream1.com:8124/1373_128",
			"http://uk5.internet-radio.com:8270/",
			"http://streams.bigfm.de/bigfm-deutschland-128-mp3",
			"https://streams.bigfm.de/bigfm-deutschrap-128-mp3",
			"http://air2.radiorecord.ru:805/rr_320",
			"http://air.radiorecord.ru:805/dub_320",
			"http://air.radiorecord.ru:805/teo_320",
			"http://air.radiorecord.ru:805/dc_320",
			"http://pool.anison.fm:9000/AniSonFM(320)?nocache=0.752104723294601"
};

void PlayRadio()
{
	if (channelchange)
	{
		BASS_ChannelStop(stream);
		stream = NULL;
		stream = BASS_StreamCreateURL(zstations[(int)cvar.gui_radio].c_str(), 0, 0, NULL, 0);
		channelchange = false;
	}
	if (cvar.gui_radio)
	{
		static bool radioInit = false;
		if (!radioInit)
		{
			BASS_Init(-1, 44100, BASS_DEVICE_3D, 0, NULL);
			stream = BASS_StreamCreateURL(zstations[(int)cvar.gui_radio].c_str(), 0, 0, NULL, 0);
			radioInit = true;
		}
		BASS_ChannelSetAttribute(stream, BASS_ATTRIB_VOL, cvar.gui_radio_volume / 100.0f);
		BASS_ChannelPlay(stream, false);
	}
}

bool WorldToScreen(float* pflOrigin, float* pflVecScreen)
{
	int iResult = g_Engine.pTriAPI->WorldToScreen(pflOrigin, pflVecScreen);
	if (pflVecScreen[0] < 1 && pflVecScreen[1] < 1 && pflVecScreen[0] > -1 && pflVecScreen[1] > -1 && !iResult)
	{
		pflVecScreen[0] = pflVecScreen[0] * (ImGui::GetWindowSize().x / 2) + (ImGui::GetWindowSize().x / 2);
		pflVecScreen[1] = -pflVecScreen[1] * (ImGui::GetWindowSize().y / 2) + (ImGui::GetWindowSize().y / 2);
		return true;
	}
	return false;
}

void ImRotateStart()
{
	rotation_start_index = ImGui::GetWindowDrawList()->VtxBuffer.Size;
}

ImVec2 ImRotationCenter()
{
	ImVec2 l(FLT_MAX, FLT_MAX), u(-FLT_MAX, -FLT_MAX);

	const auto& buf = ImGui::GetWindowDrawList()->VtxBuffer;
	for (int i = rotation_start_index; i < buf.Size; i++)
		l = ImMin(l, buf[i].pos), u = ImMax(u, buf[i].pos);

	return ImVec2((l.x + u.x) / 2, (l.y + u.y) / 2);
}

ImVec2 operator-(const ImVec2& l, const ImVec2& r)
{
	return{ l.x - r.x, l.y - r.y };
}

void ImRotateEnd(float rad, ImVec2 center)
{
	float s = sin(rad), c = cos(rad);
	center = ImRotate(center, s, c) - center;

	auto& buf = ImGui::GetWindowDrawList()->VtxBuffer;
	for (int i = rotation_start_index; i < buf.Size; i++)
		buf[i].pos = ImRotate(buf[i].pos, s, c) - center;
}

void LoadTextureMemory(const unsigned char* image, int index, int size)
{
	int width, height;

	GLint last_texture;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
	glGenTextures(1, &texture_id[index]);
	glBindTexture(GL_TEXTURE_2D, texture_id[index]);
	unsigned char* soilimage = SOIL_load_image_from_memory(image, size ,&width, &height, 0, SOIL_LOAD_RGBA);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, soilimage);
	SOIL_free_image_data(soilimage);
	glBindTexture(GL_TEXTURE_2D, last_texture);
}

void GetTexture()
{
	if (loadtexture)
	{
		LoadTextureMemory(menuback, MENUBACK, 924980);
		LoadTextureMemory(target, TARGET, 6080);
		LoadTextureMemory(rage, RAGE, 5494);
		LoadTextureMemory(legit, LEGIT, 3447);
		LoadTextureMemory(knife, KNIFE, 2079);
		LoadTextureMemory(settings, SETTINGS, 5946);
		LoadTextureMemory(model, MODEL, 2407);
		LoadTextureMemory(visual, VISUAL, 2722);
		LoadTextureMemory(bunny, BUNNY, 4749);
		LoadTextureMemory(waypoint, WAYPOINT, 600);
		LoadTextureMemory(color, COLOR, 5411);
		LoadTextureMemory(backdrop, BACKDROP, 3805);

		loadtexture = false;
	}
}

void LoadTextureImage(char* image, int index)
{
	static char *images[1024];
	bool skipload = false;
	for (int i = 0; i < 1024; i++)
	{
		if (image == images[i])
		{
			texture_id[index] = texture_id[i];
			skipload = true;
		}
	}
	if (!skipload)
	{
		images[index] = image;

		char filename[256];
		int width, height;
		sprintf(filename, "%s%s", hackdir, image);
		GLint last_texture;
		glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
		glGenTextures(1, &texture_id[index]);
		glBindTexture(GL_TEXTURE_2D, texture_id[index]);
		unsigned char* soilimage = SOIL_load_image(filename, &width, &height, 0, SOIL_LOAD_RGBA);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, soilimage);
		SOIL_free_image_data(soilimage);
		glBindTexture(GL_TEXTURE_2D, last_texture);
	}
}

void GetTextureModel()
{
	static bool loadtexturemodel = true;
	if (loadtexturemodel)
	{
		LoadTextureImage("texture/player/arctic/ARTIC_Working1.bmp", PLAYER1);
		LoadTextureImage("texture/player/gign/GIGN_DMBASE2.bmp", PLAYER2);
		LoadTextureImage("texture/player/gsg9/GSG9_Working1.bmp", PLAYER3);
		LoadTextureImage("texture/player/guerilla/GUERILLA_DMBASE.bmp", PLAYER4);
		LoadTextureImage("texture/player/leet/Arab_dmbase1.bmp", PLAYER5);
		LoadTextureImage("texture/player/sas/SAS_DMBASE1.bmp", PLAYER6);
		LoadTextureImage("texture/player/terror/Terrorist_Working1.bmp", PLAYER7);
		LoadTextureImage("texture/player/urban/SEAL_Working1.bmp", PLAYER8);

		LoadTextureImage("texture/player/backpack/Backpack1.bmp", BACKPACK1);
		LoadTextureImage("texture/player/backpack/Backpack1.bmp", BACKPACK2);
		LoadTextureImage("texture/player/backpack/Backpack1.bmp", BACKPACK3);
		LoadTextureImage("texture/player/backpack/Backpack1.bmp", BACKPACK4);
		LoadTextureImage("texture/player/backpack/Backpack1.bmp", BACKPACK5);

		LoadTextureImage("texture/player/backpack/Backpack2.bmp", THIGHPACK1);
		LoadTextureImage("texture/player/backpack/Backpack2.bmp", THIGHPACK2);
		LoadTextureImage("texture/player/backpack/Backpack2.bmp", THIGHPACK3);
		LoadTextureImage("texture/player/backpack/Backpack2.bmp", THIGHPACK4);
		LoadTextureImage("texture/player/backpack/Backpack2.bmp", THIGHPACK5);

		LoadTextureImage("texture/weapon/ak47/barrel.bmp", AK471);
		LoadTextureImage("texture/weapon/ak47/forearm.bmp", AK472);
		LoadTextureImage("texture/weapon/ak47/handle.bmp", AK473);
		LoadTextureImage("texture/weapon/ak47/lower_body.bmp", AK474);
		LoadTextureImage("texture/weapon/ak47/magazine.bmp", AK475);
		LoadTextureImage("texture/weapon/ak47/reticle.bmp", AK476);
		LoadTextureImage("texture/weapon/ak47/upper_body.bmp", AK477);
		LoadTextureImage("texture/weapon/ak47/wood.bmp", AK478);
		LoadTextureImage("texture/weapon/ak47/ak47_skin.bmp", AK479);
		LoadTextureImage("texture/weapon/ak47/ak47_skin.bmp", AK4710);
		LoadTextureImage("texture/weapon/ak47/ak47_skin.bmp", AK4711);

		LoadTextureImage("texture/weapon/assault/kevlar_vest.bmp", KEVLAR);

		LoadTextureImage("texture/weapon/aug/barrel.bmp", AUG1);
		LoadTextureImage("texture/weapon/aug/body.bmp", AUG2);
		LoadTextureImage("texture/weapon/aug/magazine.bmp", AUG3);
		LoadTextureImage("texture/weapon/aug/w_aug.bmp", AUG4);
		LoadTextureImage("texture/weapon/aug/w_aug.bmp", AUG5);
		LoadTextureImage("texture/weapon/aug/w_aug.bmp", AUG6);

		LoadTextureImage("texture/weapon/awp/barrel.bmp", AWP1);
		LoadTextureImage("texture/weapon/awp/base.bmp", AWP2);
		LoadTextureImage("texture/weapon/awp/bolt_handle.bmp", AWP3);
		LoadTextureImage("texture/weapon/awp/buttstock.bmp", AWP4);
		LoadTextureImage("texture/weapon/awp/ejector_port.bmp", AWP5);
		LoadTextureImage("texture/weapon/awp/magazine.bmp", AWP6);
		LoadTextureImage("texture/weapon/awp/newparts.bmp", AWP7);
		LoadTextureImage("texture/weapon/awp/newparts2.bmp", AWP8);
		LoadTextureImage("texture/weapon/awp/scope.bmp", AWP9);
		LoadTextureImage("texture/weapon/awp/scope_clamps.bmp", AWP10);
		LoadTextureImage("texture/weapon/awp/w_awp.bmp", AWP11);
		LoadTextureImage("texture/weapon/awp/w_awp.bmp", AWP12);
		LoadTextureImage("texture/weapon/awp/w_awp.bmp", AWP13);

		LoadTextureImage("texture/weapon/c4/c4base.bmp", C41);
		LoadTextureImage("texture/weapon/c4/c4buttons.bmp", C42);
		LoadTextureImage("texture/weapon/c4/c4timer.bmp", C43);
		LoadTextureImage("texture/weapon/c4/c4wires.bmp", C44);
		LoadTextureImage("texture/weapon/c4/cbase_front.bmp", C45);
		LoadTextureImage("texture/weapon/c4/c4base.bmp", C46);
		LoadTextureImage("texture/weapon/c4/c4timer.bmp", C47);
		LoadTextureImage("texture/weapon/c4/c4wires.bmp", C48);
		LoadTextureImage("texture/weapon/c4/c4base.bmp", C49);
		LoadTextureImage("texture/weapon/c4/c4timer.bmp", C410);
		LoadTextureImage("texture/weapon/c4/c4wires.bmp", C411);

		LoadTextureImage("texture/weapon/deagle/DE_handle.bmp", DEAGLE1);
		LoadTextureImage("texture/weapon/deagle/DE_slide1.bmp", DEAGLE2);
		LoadTextureImage("texture/weapon/deagle/DE_slide2_eagle.bmp", DEAGLE3);
		LoadTextureImage("texture/weapon/deagle/deserteagle_skin.bmp", DEAGLE4);
		LoadTextureImage("texture/weapon/deagle/deserteagle_skin.bmp", DEAGLE5);
		LoadTextureImage("texture/weapon/deagle/deserteagle_skin.bmp", DEAGLE6);
		LoadTextureImage("texture/weapon/deagle/DE_handle.bmp", DEAGLE7);
		LoadTextureImage("texture/weapon/deagle/DE_slide1.bmp", DEAGLE8);
		LoadTextureImage("texture/weapon/deagle/DE_slide2_eagle.bmp", DEAGLE9);

		LoadTextureImage("texture/weapon/elite/barrel.bmp", ELITE1);
		LoadTextureImage("texture/weapon/elite/handle.bmp", ELITE2);
		LoadTextureImage("texture/weapon/elite/magazine.bmp", ELITE3);
		LoadTextureImage("texture/weapon/elite/slide.bmp", ELITE4);
		LoadTextureImage("texture/weapon/elite/w_elite.bmp", ELITE5);
		LoadTextureImage("texture/weapon/elite/w_elite.bmp", ELITE6);

		LoadTextureImage("texture/weapon/famas/v_famas.bmp", FAMAS1);
		LoadTextureImage("texture/weapon/famas/p_famas.bmp", FAMAS2);
		LoadTextureImage("texture/weapon/famas/p_famas.bmp", FAMAS3);
		LoadTextureImage("texture/weapon/famas/p_famas.bmp", FAMAS4);

		LoadTextureImage("texture/weapon/fiveseven/fs1.bmp", FIVESEVEN1);
		LoadTextureImage("texture/weapon/fiveseven/fs2.bmp", FIVESEVEN2);
		LoadTextureImage("texture/weapon/fiveseven/57_profile.bmp", FIVESEVEN3);
		LoadTextureImage("texture/weapon/fiveseven/57_profile.bmp", FIVESEVEN4);
		LoadTextureImage("texture/weapon/fiveseven/57_profile.bmp", FIVESEVEN5);
		LoadTextureImage("texture/weapon/fiveseven/fs1.bmp", FIVESEVEN6);
		LoadTextureImage("texture/weapon/fiveseven/fs2.bmp", FIVESEVEN7);

		LoadTextureImage("texture/weapon/flashbang/v_flash_body.bmp", FLASHBANG1);
		LoadTextureImage("texture/weapon/flashbang/flash_spoon.bmp", FLASHBANG2);
		LoadTextureImage("texture/weapon/flashbang/flash_top.bmp", FLASHBANG3);
		LoadTextureImage("texture/weapon/flashbang/f_body.bmp", FLASHBANG4);
		LoadTextureImage("texture/weapon/flashbang/f_top.bmp", FLASHBANG5);
		LoadTextureImage("texture/weapon/flashbang/f_body.bmp", FLASHBANG6);
		LoadTextureImage("texture/weapon/flashbang/f_top.bmp", FLASHBANG7);
		LoadTextureImage("texture/weapon/flashbang/f_body.bmp", FLASHBANG8);
		LoadTextureImage("texture/weapon/flashbang/f_top.bmp", FLASHBANG9);
		LoadTextureImage("texture/weapon/flashbang/flash_body.bmp", FLASHBANG10);
		LoadTextureImage("texture/weapon/flashbang/flash_spoon.bmp", FLASHBANG11);
		LoadTextureImage("texture/weapon/flashbang/flash_top.bmp", FLASHBANG12);

		LoadTextureImage("texture/weapon/g3sg1/barrel.bmp", G3SG11);
		LoadTextureImage("texture/weapon/g3sg1/body.bmp", G3SG12);
		LoadTextureImage("texture/weapon/g3sg1/buttstock.bmp", G3SG13);
		LoadTextureImage("texture/weapon/g3sg1/forearm.bmp", G3SG14);
		LoadTextureImage("texture/weapon/g3sg1/lowerbody.bmp", G3SG15);
		LoadTextureImage("texture/weapon/g3sg1/scope.bmp", G3SG16);
		LoadTextureImage("texture/weapon/g3sg1/scope_hold.bmp", G3SG17);
		LoadTextureImage("texture/weapon/g3sg1/scope_knob.bmp", G3SG18);
		LoadTextureImage("texture/weapon/g3sg1/w_g3sg1.bmp", G3SG19);
		LoadTextureImage("texture/weapon/g3sg1/w_g3sg1.bmp", G3SG110);
		LoadTextureImage("texture/weapon/g3sg1/w_g3sg1.bmp", G3SG111);

		LoadTextureImage("texture/weapon/galil/galil.bmp", GALIL1);
		LoadTextureImage("texture/weapon/galil/p_galil.bmp", GALIL2);
		LoadTextureImage("texture/weapon/galil/p_galil.bmp", GALIL3);
		LoadTextureImage("texture/weapon/galil/p_galil.bmp", GALIL4);

		LoadTextureImage("texture/weapon/glock18/glock_barrel.bmp", GLOCK181);
		LoadTextureImage("texture/weapon/glock18/glock_base.bmp", GLOCK182);
		LoadTextureImage("texture/weapon/glock18/glock_mag.bmp", GLOCK183);
		LoadTextureImage("texture/weapon/glock18/glock_slide.bmp", GLOCK184);
		LoadTextureImage("texture/weapon/glock18/w_glock18.bmp", GLOCK185);
		LoadTextureImage("texture/weapon/glock18/w_glock18.bmp", GLOCK186);
		LoadTextureImage("texture/weapon/glock18/w_glock18.bmp", GLOCK187);
		LoadTextureImage("texture/weapon/glock18/glock_barrel.bmp", GLOCK188);
		LoadTextureImage("texture/weapon/glock18/glock_base.bmp", GLOCK189);
		LoadTextureImage("texture/weapon/glock18/glock_mag.bmp", GLOCK1810);
		LoadTextureImage("texture/weapon/glock18/glock_slide.bmp", GLOCK1811);

		LoadTextureImage("texture/weapon/hegrenade/v_he_body.bmp", HEGRENADE1);
		LoadTextureImage("texture/weapon/hegrenade/he_spoon.bmp", HEGRENADE2);
		LoadTextureImage("texture/weapon/hegrenade/he_top.bmp", HEGRENADE3);
		LoadTextureImage("texture/weapon/hegrenade/f_body.bmp", HEGRENADE4);
		LoadTextureImage("texture/weapon/hegrenade/f_top.bmp", HEGRENADE5);
		LoadTextureImage("texture/weapon/hegrenade/f_body.bmp", HEGRENADE6);
		LoadTextureImage("texture/weapon/hegrenade/f_top.bmp", HEGRENADE7);
		LoadTextureImage("texture/weapon/hegrenade/f_body.bmp", HEGRENADE8);
		LoadTextureImage("texture/weapon/hegrenade/f_top.bmp", HEGRENADE9);
		LoadTextureImage("texture/weapon/hegrenade/he_body.bmp", HEGRENADE10);
		LoadTextureImage("texture/weapon/hegrenade/he_spoon.bmp", HEGRENADE11);
		LoadTextureImage("texture/weapon/hegrenade/he_top.bmp", HEGRENADE12);

		LoadTextureImage("texture/weapon/knife/knifeskin.bmp", KNIFE1);
		LoadTextureImage("texture/weapon/knife/pknifeskin.bmp", KNIFE2);
		LoadTextureImage("texture/weapon/knife/knifeskin.bmp", KNIFE3);

		LoadTextureImage("texture/weapon/m3/barrel.bmp", M31);
		LoadTextureImage("texture/weapon/m3/forearm.bmp", M32);
		LoadTextureImage("texture/weapon/m3/handle.bmp", M33);
		LoadTextureImage("texture/weapon/m3/sights.bmp", M34);
		LoadTextureImage("texture/weapon/m3/w_m3super90.bmp", M35);
		LoadTextureImage("texture/weapon/m3/w_m3super90.bmp", M36);
		LoadTextureImage("texture/weapon/m3/w_m3super90.bmp", M37);

		LoadTextureImage("texture/weapon/m4a1/barrel.bmp", M4A11);
		LoadTextureImage("texture/weapon/m4a1/buttstock.bmp", M4A12);
		LoadTextureImage("texture/weapon/m4a1/handle.bmp", M4A13);
		LoadTextureImage("texture/weapon/m4a1/magazine.bmp", M4A14);
		LoadTextureImage("texture/weapon/m4a1/receiver.bmp", M4A15);
		LoadTextureImage("texture/weapon/m4a1/silencer.bmp", M4A16);
		LoadTextureImage("texture/weapon/m4a1/m4a1_skin.bmp", M4A17);
		LoadTextureImage("texture/weapon/m4a1/m4a1_skin.bmp", M4A18);
		LoadTextureImage("texture/weapon/m4a1/m4a1_skin.bmp", M4A19);

		LoadTextureImage("texture/weapon/m249/ammobox.bmp", M2491);
		LoadTextureImage("texture/weapon/m249/barrel.bmp", M2492);
		LoadTextureImage("texture/weapon/m249/body.bmp", M2493);
		LoadTextureImage("texture/weapon/m249/bullet.bmp", M2494);
		LoadTextureImage("texture/weapon/m249/buttstock.bmp", M2495);
		LoadTextureImage("texture/weapon/m249/cover.bmp", M2496);
		LoadTextureImage("texture/weapon/m249/forearm.bmp", M2497);
		LoadTextureImage("texture/weapon/m249/handle.bmp", M2498);
		LoadTextureImage("texture/weapon/m249/sight.bmp", M2499);
		LoadTextureImage("texture/weapon/m249/w_m249.bmp", M24910);
		LoadTextureImage("texture/weapon/m249/w_m249.bmp", M24911);
		LoadTextureImage("texture/weapon/m249/w_m249.bmp", M24912);

		LoadTextureImage("texture/weapon/mac10/body.bmp", MAC101);
		LoadTextureImage("texture/weapon/mac10/buttstock.bmp", MAC102);
		LoadTextureImage("texture/weapon/mac10/handle.bmp", MAC103);
		LoadTextureImage("texture/weapon/mac10/w_mac10.bmp", MAC104);
		LoadTextureImage("texture/weapon/mac10/w_mac10.bmp", MAC105);

		LoadTextureImage("texture/weapon/mp5/barrel.bmp", MP51);
		LoadTextureImage("texture/weapon/mp5/buttstock.bmp", MP52);
		LoadTextureImage("texture/weapon/mp5/clip.bmp", MP53);
		LoadTextureImage("texture/weapon/mp5/forearm.bmp", MP54);
		LoadTextureImage("texture/weapon/mp5/handle.bmp", MP55);
		LoadTextureImage("texture/weapon/mp5/lowerrec.bmp", MP56);
		LoadTextureImage("texture/weapon/mp5/rearsight.bmp", MP57);
		LoadTextureImage("texture/weapon/mp5/w_mp5.bmp", MP58);
		LoadTextureImage("texture/weapon/mp5/w_mp5.bmp", MP59);
		LoadTextureImage("texture/weapon/mp5/w_mp5.bmp", MP510);

		LoadTextureImage("texture/weapon/p90/buttstock.bmp", P901);
		LoadTextureImage("texture/weapon/p90/handle-1.bmp", P902);
		LoadTextureImage("texture/weapon/p90/handles.bmp", P903);
		LoadTextureImage("texture/weapon/p90/magazine.bmp", P904);
		LoadTextureImage("texture/weapon/p90/sights-1.bmp", P905);
		LoadTextureImage("texture/weapon/p90/w_p90.bmp", P906);
		LoadTextureImage("texture/weapon/p90/w_p90.bmp", P907);
		LoadTextureImage("texture/weapon/p90/w_p90.bmp", P908);

		LoadTextureImage("texture/weapon/p228/p228_handle.bmp", P2281);
		LoadTextureImage("texture/weapon/p228/p228_mag.bmp", P2282);
		LoadTextureImage("texture/weapon/p228/p228_slide.bmp", P2283);
		LoadTextureImage("texture/weapon/p228/w_p228.bmp", P2284);
		LoadTextureImage("texture/weapon/p228/w_p228.bmp", P2285);
		LoadTextureImage("texture/weapon/p228/w_p228.bmp", P2286);
		LoadTextureImage("texture/weapon/p228/p228_handle.bmp", P2287);
		LoadTextureImage("texture/weapon/p228/p228_mag.bmp", P2288);
		LoadTextureImage("texture/weapon/p228/p228_slide.bmp", P2289);

		LoadTextureImage("texture/weapon/scout/base.bmp", SCOUT1);
		LoadTextureImage("texture/weapon/scout/magazine.bmp", SCOUT2);
		LoadTextureImage("texture/weapon/scout/rail.bmp", SCOUT3);
		LoadTextureImage("texture/weapon/scout/scope.bmp", SCOUT4);
		LoadTextureImage("texture/weapon/scout/scope_clamps.bmp", SCOUT5);
		LoadTextureImage("texture/weapon/scout/w_scout.bmp", SCOUT6);
		LoadTextureImage("texture/weapon/scout/w_scout.bmp", SCOUT7);
		LoadTextureImage("texture/weapon/scout/w_scout.bmp", SCOUT8);

		LoadTextureImage("texture/weapon/sg550/buttstock.bmp", SG5501);
		LoadTextureImage("texture/weapon/sg550/forearm.bmp", SG5502);
		LoadTextureImage("texture/weapon/sg550/handle.bmp", SG5503);
		LoadTextureImage("texture/weapon/sg550/magazine_transp.bmp", SG5504);
		LoadTextureImage("texture/weapon/sg550/receiver.bmp", SG5505);
		LoadTextureImage("texture/weapon/sg550/scope.bmp", SG5506);
		LoadTextureImage("texture/weapon/sg550/sg550_profile.bmp", SG5507);
		LoadTextureImage("texture/weapon/sg550/sg550_profile.bmp", SG5508);
		LoadTextureImage("texture/weapon/sg550/sg550_profile.bmp", SG5509);

		LoadTextureImage("texture/weapon/sg552/acog.bmp", SG5521);
		LoadTextureImage("texture/weapon/sg552/buttstock.bmp", SG5522);
		LoadTextureImage("texture/weapon/sg552/forearm.bmp", SG5523);
		LoadTextureImage("texture/weapon/sg552/handle.bmp", SG5524);
		LoadTextureImage("texture/weapon/sg552/magazine_transp.bmp", SG5525);
		LoadTextureImage("texture/weapon/sg552/sg552_skin.bmp", SG5526);
		LoadTextureImage("texture/weapon/sg552/sg552_skin.bmp", SG5527);
		LoadTextureImage("texture/weapon/sg552/sg552_skin.bmp", SG5528);

		LoadTextureImage("texture/weapon/shield/shield_back.bmp", SHIELD1);
		LoadTextureImage("texture/weapon/shield/shield_front.bmp", SHIELD2);
		LoadTextureImage("texture/weapon/shield/shield_glass.bmp", SHIELD3);
		LoadTextureImage("texture/weapon/shield/shield_back.bmp", SHIELD4);
		LoadTextureImage("texture/weapon/shield/shield_front.bmp", SHIELD5);
		LoadTextureImage("texture/weapon/shield/shield_glass.bmp", SHIELD6);
		LoadTextureImage("texture/weapon/shield/shield_back.bmp", SHIELD7);
		LoadTextureImage("texture/weapon/shield/shield_front.bmp", SHIELD8);
		LoadTextureImage("texture/weapon/shield/shield_glass.bmp", SHIELD9);
		LoadTextureImage("texture/weapon/shield/shield_back.bmp", SHIELD10);
		LoadTextureImage("texture/weapon/shield/shield_front.bmp", SHIELD11);
		LoadTextureImage("texture/weapon/shield/shield_glass.bmp", SHIELD12);
		LoadTextureImage("texture/weapon/shield/shield_back.bmp", SHIELD13);
		LoadTextureImage("texture/weapon/shield/shield_front.bmp", SHIELD14);
		LoadTextureImage("texture/weapon/shield/shield_glass.bmp", SHIELD15);
		LoadTextureImage("texture/weapon/shield/shield_back.bmp", SHIELD16);
		LoadTextureImage("texture/weapon/shield/shield_front.bmp", SHIELD17);
		LoadTextureImage("texture/weapon/shield/shield_glass.bmp", SHIELD18);
		LoadTextureImage("texture/weapon/shield/shield_back.bmp", SHIELD19);
		LoadTextureImage("texture/weapon/shield/shield_front.bmp", SHIELD20);
		LoadTextureImage("texture/weapon/shield/shield_glass.bmp", SHIELD21);
		LoadTextureImage("texture/weapon/shield/shield_back.bmp", SHIELD22);
		LoadTextureImage("texture/weapon/shield/shield_front.bmp", SHIELD23);
		LoadTextureImage("texture/weapon/shield/shield_glass.bmp", SHIELD24);
		LoadTextureImage("texture/weapon/shield/shield_back.bmp", SHIELD25);
		LoadTextureImage("texture/weapon/shield/shield_front.bmp", SHIELD26);
		LoadTextureImage("texture/weapon/shield/shield_glass.bmp", SHIELD27);
		LoadTextureImage("texture/weapon/shield/shield_back.bmp", SHIELD28);
		LoadTextureImage("texture/weapon/shield/shield_front.bmp", SHIELD29);
		LoadTextureImage("texture/weapon/shield/shield_glass.bmp", SHIELD30);
		LoadTextureImage("texture/weapon/shield/shield.bmp", SHIELD31);
		LoadTextureImage("texture/weapon/shield/shield_glass.bmp", SHIELD32);
		LoadTextureImage("texture/weapon/shield/shield.bmp", SHIELD33);
		LoadTextureImage("texture/weapon/shield/shield_glass.bmp", SHIELD34);
		LoadTextureImage("texture/weapon/shield/shield.bmp", SHIELD35);
		LoadTextureImage("texture/weapon/shield/shield_glass.bmp", SHIELD36);
		LoadTextureImage("texture/weapon/shield/shield.bmp", SHIELD37);
		LoadTextureImage("texture/weapon/shield/shield_glass.bmp", SHIELD38);
		LoadTextureImage("texture/weapon/shield/shield.bmp", SHIELD39);
		LoadTextureImage("texture/weapon/shield/shield_glass.bmp", SHIELD40);
		LoadTextureImage("texture/weapon/shield/shield.bmp", SHIELD41);
		LoadTextureImage("texture/weapon/shield/shield.bmp", SHIELD42);
		LoadTextureImage("texture/weapon/shield/shield_glass.bmp", SHIELD43);
		LoadTextureImage("texture/weapon/shield/shield.bmp", SHIELD44);
		LoadTextureImage("texture/weapon/shield/shield_glass.bmp", SHIELD45);
		LoadTextureImage("texture/weapon/shield/shield.bmp", SHIELD46);
		LoadTextureImage("texture/weapon/shield/shield_glass.bmp", SHIELD47);

		LoadTextureImage("texture/weapon/smokegrenade/v_smoke_body.bmp", SMOKEGRENADE1);
		LoadTextureImage("texture/weapon/smokegrenade/smoke_spoon.bmp", SMOKEGRENADE2);
		LoadTextureImage("texture/weapon/smokegrenade/smoke_top.bmp", SMOKEGRENADE3);
		LoadTextureImage("texture/weapon/smokegrenade/f_body.bmp", SMOKEGRENADE4);
		LoadTextureImage("texture/weapon/smokegrenade/f_top.bmp", SMOKEGRENADE5);
		LoadTextureImage("texture/weapon/smokegrenade/f_body.bmp", SMOKEGRENADE6);
		LoadTextureImage("texture/weapon/smokegrenade/f_top.bmp", SMOKEGRENADE7);
		LoadTextureImage("texture/weapon/smokegrenade/f_body.bmp", SMOKEGRENADE8);
		LoadTextureImage("texture/weapon/smokegrenade/f_top.bmp", SMOKEGRENADE9);
		LoadTextureImage("texture/weapon/smokegrenade/smoke_body.bmp", SMOKEGRENADE10);
		LoadTextureImage("texture/weapon/smokegrenade/smoke_spoon.bmp", SMOKEGRENADE11);
		LoadTextureImage("texture/weapon/smokegrenade/smoke_top.bmp", SMOKEGRENADE12);

		LoadTextureImage("texture/weapon/tmp/barrel.bmp", TMP1);
		LoadTextureImage("texture/weapon/tmp/body.bmp", TMP2);
		LoadTextureImage("texture/weapon/tmp/ejector.bmp", TMP3);
		LoadTextureImage("texture/weapon/tmp/forearm.bmp", TMP4);
		LoadTextureImage("texture/weapon/tmp/handle.bmp", TMP5);
		LoadTextureImage("texture/weapon/tmp/magazine.bmp", TMP6);
		LoadTextureImage("texture/weapon/tmp/misc.bmp", TMP7);
		LoadTextureImage("texture/weapon/tmp/silencer.bmp", TMP8);
		LoadTextureImage("texture/weapon/tmp/top.bmp", TMP9);
		LoadTextureImage("texture/weapon/tmp/w_tmp.bmp", TMP10);
		LoadTextureImage("texture/weapon/tmp/w_tmp.bmp", TMP11);

		LoadTextureImage("texture/weapon/ump45/buttstock.bmp", UMP451);
		LoadTextureImage("texture/weapon/ump45/handle.bmp", UMP452);
		LoadTextureImage("texture/weapon/ump45/receiver.bmp", UMP453);
		LoadTextureImage("texture/weapon/ump45/ump_profile.bmp", UMP454);
		LoadTextureImage("texture/weapon/ump45/ump_profile.bmp", UMP455);
		LoadTextureImage("texture/weapon/ump45/ump_profile.bmp", UMP456);

		LoadTextureImage("texture/weapon/usp/handle.bmp", USP1);
		LoadTextureImage("texture/weapon/usp/magazine.bmp", USP2);
		LoadTextureImage("texture/weapon/usp/silencer.bmp", USP3);
		LoadTextureImage("texture/weapon/usp/slide.bmp", USP4);
		LoadTextureImage("texture/weapon/usp/usp_skin.bmp", USP5);
		LoadTextureImage("texture/weapon/usp/usp_skin.bmp", USP6);
		LoadTextureImage("texture/weapon/usp/usp_skin.bmp", USP7);
		LoadTextureImage("texture/weapon/usp/handle.bmp", USP8);
		LoadTextureImage("texture/weapon/usp/magazine.bmp", USP9);
		LoadTextureImage("texture/weapon/usp/slide.bmp", USP10);

		LoadTextureImage("texture/weapon/xm1014/barrel.bmp", XM10141);
		LoadTextureImage("texture/weapon/xm1014/body.bmp", XM10142);
		LoadTextureImage("texture/weapon/xm1014/buttstock.bmp", XM10143);
		LoadTextureImage("texture/weapon/xm1014/forearm.bmp", XM10144);
		LoadTextureImage("texture/weapon/xm1014/rear_sight.bmp", XM10145);
		LoadTextureImage("texture/weapon/xm1014/shell.bmp", XM10146);
		LoadTextureImage("texture/weapon/xm1014/xm1014.bmp", XM10147);
		LoadTextureImage("texture/weapon/xm1014/xm1014.bmp", XM10148);
		LoadTextureImage("texture/weapon/xm1014/xm1014.bmp", XM10149);

		LoadTextureImage("texture/weapon/hands/view_glove.bmp", HANDS1);
		LoadTextureImage("texture/weapon/hands/view_glove.bmp", HANDS2);
		LoadTextureImage("texture/weapon/hands/view_glove.bmp", HANDS3);
		LoadTextureImage("texture/weapon/hands/view_glove.bmp", HANDS4);
		LoadTextureImage("texture/weapon/hands/view_glove.bmp", HANDS5);
		LoadTextureImage("texture/weapon/hands/view_glove.bmp", HANDS6);
		LoadTextureImage("texture/weapon/hands/view_glove.bmp", HANDS7);
		LoadTextureImage("texture/weapon/hands/view_glove.bmp", HANDS8);
		LoadTextureImage("texture/weapon/hands/view_glove.bmp", HANDS9);
		LoadTextureImage("texture/weapon/hands/view_glove.bmp", HANDS10);

		LoadTextureImage("texture/weapon/hands/view_glove.bmp", HANDS11);
		LoadTextureImage("texture/weapon/hands/view_glove.bmp", HANDS12);
		LoadTextureImage("texture/weapon/hands/view_glove.bmp", HANDS13);
		LoadTextureImage("texture/weapon/hands/view_glove.bmp", HANDS14);
		LoadTextureImage("texture/weapon/hands/view_glove.bmp", HANDS15);
		LoadTextureImage("texture/weapon/hands/view_glove.bmp", HANDS16);
		LoadTextureImage("texture/weapon/hands/view_glove.bmp", HANDS17);
		LoadTextureImage("texture/weapon/hands/view_glove.bmp", HANDS18);
		LoadTextureImage("texture/weapon/hands/view_glove.bmp", HANDS19);
		LoadTextureImage("texture/weapon/hands/view_glove.bmp", HANDS20);

		LoadTextureImage("texture/weapon/hands/view_glove.bmp", HANDS21);
		LoadTextureImage("texture/weapon/hands/view_glove.bmp", HANDS22);
		LoadTextureImage("texture/weapon/hands/view_glove.bmp", HANDS23);
		LoadTextureImage("texture/weapon/hands/view_glove.bmp", HANDS24);
		LoadTextureImage("texture/weapon/hands/view_glove.bmp", HANDS25);
		LoadTextureImage("texture/weapon/hands/view_glove.bmp", HANDS26);
		LoadTextureImage("texture/weapon/hands/view_glove.bmp", HANDS27);
		LoadTextureImage("texture/weapon/hands/view_glove.bmp", HANDS28);
		LoadTextureImage("texture/weapon/hands/view_glove.bmp", HANDS29);
		LoadTextureImage("texture/weapon/hands/view_glove.bmp", HANDS30);

		LoadTextureImage("texture/weapon/hands/view_glove.bmp", HANDS31);
		LoadTextureImage("texture/weapon/hands/view_glove.bmp", HANDS32);
		LoadTextureImage("texture/weapon/hands/view_glove.bmp", HANDS33);
		LoadTextureImage("texture/weapon/hands/view_glove.bmp", HANDS34);
		LoadTextureImage("texture/weapon/hands/view_glove.bmp", HANDS35);
		LoadTextureImage("texture/weapon/hands/view_glove.bmp", HANDS36);
		LoadTextureImage("texture/weapon/hands/view_glove.bmp", HANDS37);
		LoadTextureImage("texture/weapon/hands/view_glove.bmp", HANDS38);
		LoadTextureImage("texture/weapon/hands/view_finger.bmp", HANDS39);
		LoadTextureImage("texture/weapon/hands/view_finger.bmp", HANDS40);

		LoadTextureImage("texture/weapon/hands/view_finger.bmp", HANDS41);
		LoadTextureImage("texture/weapon/hands/view_finger.bmp", HANDS42);
		LoadTextureImage("texture/weapon/hands/view_finger.bmp", HANDS43);
		LoadTextureImage("texture/weapon/hands/view_finger.bmp", HANDS44);
		LoadTextureImage("texture/weapon/hands/view_finger.bmp", HANDS45);
		LoadTextureImage("texture/weapon/hands/view_finger.bmp", HANDS46);
		LoadTextureImage("texture/weapon/hands/view_finger.bmp", HANDS47);
		LoadTextureImage("texture/weapon/hands/view_finger.bmp", HANDS48);
		LoadTextureImage("texture/weapon/hands/view_finger.bmp", HANDS49);
		LoadTextureImage("texture/weapon/hands/view_finger.bmp", HANDS50);

		LoadTextureImage("texture/weapon/hands/view_finger.bmp", HANDS51);
		LoadTextureImage("texture/weapon/hands/view_finger.bmp", HANDS52);
		LoadTextureImage("texture/weapon/hands/view_finger.bmp", HANDS53);
		LoadTextureImage("texture/weapon/hands/view_finger.bmp", HANDS54);
		LoadTextureImage("texture/weapon/hands/view_finger.bmp", HANDS55);
		LoadTextureImage("texture/weapon/hands/view_finger.bmp", HANDS56);
		LoadTextureImage("texture/weapon/hands/view_finger.bmp", HANDS57);
		LoadTextureImage("texture/weapon/hands/view_finger.bmp", HANDS58);
		LoadTextureImage("texture/weapon/hands/view_finger.bmp", HANDS59);
		LoadTextureImage("texture/weapon/hands/view_finger.bmp", HANDS60);

		LoadTextureImage("texture/weapon/hands/view_finger.bmp", HANDS61);
		LoadTextureImage("texture/weapon/hands/view_finger.bmp", HANDS62);
		LoadTextureImage("texture/weapon/hands/view_finger.bmp", HANDS63);
		LoadTextureImage("texture/weapon/hands/view_finger.bmp", HANDS64);
		LoadTextureImage("texture/weapon/hands/view_finger.bmp", HANDS65);
		LoadTextureImage("texture/weapon/hands/view_finger.bmp", HANDS66);
		LoadTextureImage("texture/weapon/hands/view_finger.bmp", HANDS67);
		LoadTextureImage("texture/weapon/hands/view_finger.bmp", HANDS68);
		LoadTextureImage("texture/weapon/hands/view_finger.bmp", HANDS69);
		LoadTextureImage("texture/weapon/hands/view_finger.bmp", HANDS70);

		LoadTextureImage("texture/weapon/hands/view_finger.bmp", HANDS71);
		LoadTextureImage("texture/weapon/hands/view_finger.bmp", HANDS72);
		LoadTextureImage("texture/weapon/hands/view_finger.bmp", HANDS73);
		LoadTextureImage("texture/weapon/hands/view_finger.bmp", HANDS74);
		LoadTextureImage("texture/weapon/hands/view_finger.bmp", HANDS75);
		LoadTextureImage("texture/weapon/hands/view_finger.bmp", HANDS76);
		LoadTextureImage("texture/weapon/hands/view_skin.bmp", HANDS77);
		LoadTextureImage("texture/weapon/hands/view_skin.bmp", HANDS78);
		LoadTextureImage("texture/weapon/hands/view_skin.bmp", HANDS79);
		LoadTextureImage("texture/weapon/hands/view_skin.bmp", HANDS80);

		LoadTextureImage("texture/weapon/hands/view_skin.bmp", HANDS81);
		LoadTextureImage("texture/weapon/hands/view_skin.bmp", HANDS82);
		LoadTextureImage("texture/weapon/hands/view_skin.bmp", HANDS83);
		LoadTextureImage("texture/weapon/hands/view_skin.bmp", HANDS84);
		LoadTextureImage("texture/weapon/hands/view_skin.bmp", HANDS85);
		LoadTextureImage("texture/weapon/hands/view_skin.bmp", HANDS86);
		LoadTextureImage("texture/weapon/hands/view_skin.bmp", HANDS87);
		LoadTextureImage("texture/weapon/hands/view_skin.bmp", HANDS88);
		LoadTextureImage("texture/weapon/hands/view_skin.bmp", HANDS89);
		LoadTextureImage("texture/weapon/hands/view_skin.bmp", HANDS90);

		LoadTextureImage("texture/weapon/hands/view_skin.bmp", HANDS91);
		LoadTextureImage("texture/weapon/hands/view_skin.bmp", HANDS92);
		LoadTextureImage("texture/weapon/hands/view_skin.bmp", HANDS93);
		LoadTextureImage("texture/weapon/hands/view_skin.bmp", HANDS94);
		LoadTextureImage("texture/weapon/hands/view_skin.bmp", HANDS95);
		LoadTextureImage("texture/weapon/hands/view_skin.bmp", HANDS96);
		LoadTextureImage("texture/weapon/hands/view_skin.bmp", HANDS97);
		LoadTextureImage("texture/weapon/hands/view_skin.bmp", HANDS98);
		LoadTextureImage("texture/weapon/hands/view_skin.bmp", HANDS99);
		LoadTextureImage("texture/weapon/hands/view_skin.bmp", HANDS100);
		
		LoadTextureImage("texture/weapon/hands/view_skin.bmp", HANDS101);
		LoadTextureImage("texture/weapon/hands/view_skin.bmp", HANDS102);
		LoadTextureImage("texture/weapon/hands/view_skin.bmp", HANDS103);
		LoadTextureImage("texture/weapon/hands/view_skin.bmp", HANDS104);
		LoadTextureImage("texture/weapon/hands/view_skin.bmp", HANDS105);
		LoadTextureImage("texture/weapon/hands/view_skin.bmp", HANDS106);
		LoadTextureImage("texture/weapon/hands/view_skin.bmp", HANDS107);
		LoadTextureImage("texture/weapon/hands/view_skin.bmp", HANDS108);
		LoadTextureImage("texture/weapon/hands/view_skin.bmp", HANDS109);
		LoadTextureImage("texture/weapon/hands/view_skin.bmp", HANDS110);

		LoadTextureImage("texture/weapon/hands/view_skin.bmp", HANDS111);
		LoadTextureImage("texture/weapon/hands/view_skin.bmp", HANDS112);
		LoadTextureImage("texture/weapon/hands/view_skin.bmp", HANDS113);
		LoadTextureImage("texture/weapon/hands/view_skin.bmp", HANDS114);

		LoadTextureImage("texture/weapon/shell/rifle_goldshell.bmp", SHELL1);
		LoadTextureImage("texture/weapon/shell/pistol_goldshell.bmp", SHELL2);
		LoadTextureImage("texture/weapon/shell/shell.bmp", SHELL3);
		LoadTextureImage("texture/weapon/shell/rifle_goldshell.bmp", SHELL4);

		LoadTextureImage("texture/chicken/skin.bmp", CHICKEN);

		LoadTextureImage("texture/player/vip/newsvip.bmp", VIPBODY);
		LoadTextureImage("texture/player/vip/head2.bmp", VIPHEAD);
		LoadTextureImage("texture/player/vip/vip_militant_body.bmp", VIPBODY2);
		LoadTextureImage("texture/player/vip/vip_militant_head.bmp", VIPHEAD2);
		LoadTextureImage("texture/player/arctic/arctic_body.bmp", ARCTICBODY);
		LoadTextureImage("texture/player/arctic/arctic_head2.bmp", ARCTICHEAD);
		LoadTextureImage("texture/player/gign/body.bmp", GIGNBODY);
		LoadTextureImage("texture/player/gign/helmet.bmp", GIGNHEAD);
		LoadTextureImage("texture/player/gsg9/gsg9_body.bmp", GSG9BODY);
		LoadTextureImage("texture/player/gsg9/gsg9_head.bmp", GSG9HEAD);
		LoadTextureImage("texture/player/guerilla/guerilla_body.bmp", GUERILLABODY);
		LoadTextureImage("texture/player/guerilla/guerilla_head.bmp", GUERILLAHEAD);
		LoadTextureImage("texture/player/leet/leet_body5.bmp", LEETBODY);
		LoadTextureImage("texture/player/leet/guerilla_head3.bmp", LEETHEAD);
		LoadTextureImage("texture/player/sas/SAS_body.bmp", SASBODY);
		LoadTextureImage("texture/player/sas/SAS_head.bmp", SASHEAD);
		LoadTextureImage("texture/player/terror/terror_body.bmp", TERRORBODY);
		LoadTextureImage("texture/player/terror/terror_head3.bmp", TERRORHEAD);
		LoadTextureImage("texture/player/urban/seal_body.bmp", URBANBODY);
		LoadTextureImage("texture/player/urban/seal_head.bmp", URBANHEAD);
		LoadTextureImage("texture/player/urban/seal_chrome.bmp", URBANCHROME);

		loadtexturemodel = false;
	}
}

void DrawModeChange()
{
	float WindowBorderSize = ImGui::GetStyle().WindowBorderSize;
	ImGui::GetStyle().WindowBorderSize = 0.0f;
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
	ImGui::Begin("DrawModeChange", reinterpret_cast<bool*>(true), ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs);
	{
		ImGui::SetWindowPos(ImVec2(0, 0), ImGuiSetCond_Always);
		ImGui::SetWindowSize(ImVec2(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y), ImGuiSetCond_Always);

		if (CheckDraw() && g_pEngine->GetMaxClients())
		{
			int y = (ImGui::GetWindowSize().y / 2) + ImGui::GetWindowSize().y / 20 * 5;
			if (GetTickCount() - ModeChangeDelay < 5000)
			{
				ImVec2 label_size = ImGui::CalcTextSize(cvar.rage_active ? "Rage Mode":"Legit Mode", NULL, true);
				ImGui::GetCurrentWindow()->DrawList->AddRect({ 20 - 2, (float)y + 37 }, { label_size.x + 24, (float)y + 51 }, black(), cvar.visual_rounding);
				ImGui::GetCurrentWindow()->DrawList->AddRect({ 20 - 3, (float)y + 36 }, { label_size.x + 23, (float)y + 50 }, wheel1(), cvar.visual_rounding);
				ImGui::GetCurrentWindow()->DrawList->AddText({ 20, (float)y + 35 }, white(), cvar.rage_active ? "Rage Mode" : "Legit Mode");
			}
		}
		ImGui::End();
		ImGui::PopStyleColor();
		ImGui::GetStyle().WindowBorderSize = WindowBorderSize;
	}
}

void ColorChange()
{
	if (cvar.color_random)
	{
		static float Color[3];
		static DWORD Tickcount = 0;
		static DWORD Tickcheck = 0;
		ImGui::ColorConvertRGBtoHSV(cvar.color_red, cvar.color_green, cvar.color_blue, Color[0], Color[1], Color[2]);
		if (GetTickCount() - Tickcount >= 1)
		{
			if (Tickcheck != Tickcount)
			{
				Color[0] += 0.001f * cvar.color_speed;
				Tickcheck = Tickcount;
			}
			Tickcount = GetTickCount();
		}
		if (Color[0] < 0.0f) Color[0] += 1.0f;
		ImGui::ColorConvertHSVtoRGB(Color[0], Color[1], Color[2], cvar.color_red, cvar.color_green, cvar.color_blue);
	}
}

char* KeyEventToChar(int Key)
{
	switch (Key)
	{
	case K_TAB: return "Tab";
	case K_ENTER: return "Enter";
	case K_ESCAPE: return "Escape";
	case K_SPACE: return "Space";
	case K_0: return "0";
	case K_1: return "1";
	case K_2: return "2";
	case K_3: return "3";
	case K_4: return "4";
	case K_5: return "5";
	case K_6: return "6";
	case K_7: return "7";
	case K_8: return "8";
	case K_9: return "9";
	case K_A: return "A";
	case K_B: return "B";
	case K_C: return "C";
	case K_D: return "D";
	case K_E: return "E";
	case K_F: return "F";
	case K_G: return "G";
	case K_H: return "H";
	case K_I: return "I";
	case K_J: return "J";
	case K_K: return "K";
	case K_L: return "L";
	case K_M: return "M";
	case K_N: return "N";
	case K_O: return "O";
	case K_P: return "P";
	case K_Q: return "Q";
	case K_R: return "R";
	case K_S: return "S";
	case K_T: return "T";
	case K_U: return "U";
	case K_V: return "V";
	case K_W: return "W";
	case K_X: return "X";
	case K_Y: return "Y";
	case K_Z: return "Z";
	case K_BACKSPACE: return "Backspace";
	case K_UPARROW: return "Up Arrow";
	case K_DOWNARROW: return "Down Arrow";
	case K_LEFTARROW: return "Left Arrow";
	case K_RIGHTARROW: return "Right Arrow";
	case K_ALT: return "Alt";
	case K_CTRL: return "Ctrl";
	case K_SHIFT: return "Shift";
	case K_F1: return "F1";
	case K_F2: return "F2";
	case K_F3: return "F3";
	case K_F4: return "F4";
	case K_F5: return "F5";
	case K_F6: return "F6";
	case K_F7: return "F7";
	case K_F8: return "F8";
	case K_F9: return "F9";
	case K_F10: return "F10";
	case K_F11: return "F11";
	case K_F12: return "F12";
	case K_INS: return "Insert";
	case K_DEL: return "Delete";
	case K_PGDN: return "Page Down";
	case K_PGUP: return "Page Up";
	case K_HOME: return "Home";
	case K_END: return "End";
	case K_CAPSLOCK: return "Capslock";
	case K_MWHEELDOWN: return "Wheel Down";
	case K_MWHEELUP: return "Wheel Up";
	case K_MOUSE1: return "Mouse 1";
	case K_MOUSE2: return "Mouse 2";
	case K_MOUSE3: return "Mouse 3";
	case K_MOUSE4: return "Mouse 4";
	case K_MOUSE5: return "Mouse 5";
	}
	return "Unknown Ok";
}

string vkToString(int vk)
{
	char c[2] = { 0 };
	if (vk >= '0' && vk <= '9') { c[0] = (char)vk; return string(c); }
	if (vk >= 'A' && vk <= 'Z') { c[0] = (char)vk; return string(c); }
	switch (vk)
	{
		// VK_0 - VK_9 are the same as ASCII '0' - '9' (0x30 - 0x39)
		// 0x40 : unassigned
		// VK_A - VK_Z are the same as ASCII 'A' - 'Z' (0x41 - 0x5A)
		#define caseStringify(x) case x: return string(#x)
		caseStringify(VK_LBUTTON);
		caseStringify(VK_RBUTTON);
		caseStringify(VK_CANCEL);
		caseStringify(VK_MBUTTON);  // NOT contiguous with L & RBUTTON
		caseStringify(VK_XBUTTON1); // NOT contiguous with L & RBUTTON
		caseStringify(VK_XBUTTON2); // NOT contiguous with L & RBUTTON
		caseStringify(VK_BACK);
		caseStringify(VK_TAB);
		caseStringify(VK_CLEAR);
		caseStringify(VK_RETURN);
		caseStringify(VK_SHIFT);
		caseStringify(VK_CONTROL);
		caseStringify(VK_MENU);
		caseStringify(VK_PAUSE);
		caseStringify(VK_CAPITAL);
		caseStringify(VK_KANA);
		caseStringify(VK_JUNJA);
		caseStringify(VK_FINAL);
		caseStringify(VK_KANJI);
		caseStringify(VK_ESCAPE);
		caseStringify(VK_CONVERT);
		caseStringify(VK_NONCONVERT);
		caseStringify(VK_ACCEPT);
		caseStringify(VK_MODECHANGE);
		caseStringify(VK_SPACE);
		caseStringify(VK_PRIOR);
		caseStringify(VK_NEXT);
		caseStringify(VK_END);
		caseStringify(VK_HOME);
		caseStringify(VK_LEFT);
		caseStringify(VK_UP);
		caseStringify(VK_RIGHT);
		caseStringify(VK_DOWN);
		caseStringify(VK_SELECT);
		caseStringify(VK_PRINT);
		caseStringify(VK_EXECUTE);
		caseStringify(VK_SNAPSHOT);
		caseStringify(VK_INSERT);
		caseStringify(VK_DELETE);
		caseStringify(VK_HELP);
		caseStringify(VK_LWIN);
		caseStringify(VK_RWIN);
		caseStringify(VK_APPS);
		caseStringify(VK_SLEEP);
		caseStringify(VK_NUMPAD0);
		caseStringify(VK_NUMPAD1);
		caseStringify(VK_NUMPAD2);
		caseStringify(VK_NUMPAD3);
		caseStringify(VK_NUMPAD4);
		caseStringify(VK_NUMPAD5);
		caseStringify(VK_NUMPAD6);
		caseStringify(VK_NUMPAD7);
		caseStringify(VK_NUMPAD8);
		caseStringify(VK_NUMPAD9);
		caseStringify(VK_MULTIPLY);
		caseStringify(VK_ADD);
		caseStringify(VK_SEPARATOR);
		caseStringify(VK_SUBTRACT);
		caseStringify(VK_DECIMAL);
		caseStringify(VK_DIVIDE);
		caseStringify(VK_F1);
		caseStringify(VK_F2);
		caseStringify(VK_F3);
		caseStringify(VK_F4);
		caseStringify(VK_F5);
		caseStringify(VK_F6);
		caseStringify(VK_F7);
		caseStringify(VK_F8);
		caseStringify(VK_F9);
		caseStringify(VK_F10);
		caseStringify(VK_F11);
		caseStringify(VK_F12);
		caseStringify(VK_F13);
		caseStringify(VK_F14);
		caseStringify(VK_F15);
		caseStringify(VK_F16);
		caseStringify(VK_F17);
		caseStringify(VK_F18);
		caseStringify(VK_F19);
		caseStringify(VK_F20);
		caseStringify(VK_F21);
		caseStringify(VK_F22);
		caseStringify(VK_F23);
		caseStringify(VK_F24);
		caseStringify(VK_NUMLOCK);
		caseStringify(VK_SCROLL);
		caseStringify(VK_OEM_NEC_EQUAL);  // '=' key on numpad
		caseStringify(VK_OEM_FJ_MASSHOU); // 'Unregister word' key
		caseStringify(VK_OEM_FJ_TOUROKU); // 'Register word' key
		caseStringify(VK_OEM_FJ_LOYA);    // 'Left OYAYUBI' key
		caseStringify(VK_OEM_FJ_ROYA);    // 'Right OYAYUBI' key
		caseStringify(VK_LSHIFT);
		caseStringify(VK_RSHIFT);
		caseStringify(VK_LCONTROL);
		caseStringify(VK_RCONTROL);
		caseStringify(VK_LMENU);
		caseStringify(VK_RMENU);
		caseStringify(VK_BROWSER_BACK);
		caseStringify(VK_BROWSER_FORWARD);
		caseStringify(VK_BROWSER_REFRESH);
		caseStringify(VK_BROWSER_STOP);
		caseStringify(VK_BROWSER_SEARCH);
		caseStringify(VK_BROWSER_FAVORITES);
		caseStringify(VK_BROWSER_HOME);
		caseStringify(VK_VOLUME_MUTE);
		caseStringify(VK_VOLUME_DOWN);
		caseStringify(VK_VOLUME_UP);
		caseStringify(VK_MEDIA_NEXT_TRACK);
		caseStringify(VK_MEDIA_PREV_TRACK);
		caseStringify(VK_MEDIA_STOP);
		caseStringify(VK_MEDIA_PLAY_PAUSE);
		caseStringify(VK_LAUNCH_MAIL);
		caseStringify(VK_LAUNCH_MEDIA_SELECT);
		caseStringify(VK_LAUNCH_APP1);
		caseStringify(VK_LAUNCH_APP2);
		caseStringify(VK_OEM_1);      // ';:' for US
		caseStringify(VK_OEM_PLUS);   // '+' any country
		caseStringify(VK_OEM_COMMA);  // ',' any country
		caseStringify(VK_OEM_MINUS);  // '-' any country
		caseStringify(VK_OEM_PERIOD); // '.' any country
		caseStringify(VK_OEM_2);  // '/?' for US
		caseStringify(VK_OEM_3);  // '`~' for US
		caseStringify(VK_OEM_4);  //  '[{' for US
		caseStringify(VK_OEM_5);  //  '\|' for US
		caseStringify(VK_OEM_6);  //  ']}' for US
		caseStringify(VK_OEM_7);  //  ''"' for US
		caseStringify(VK_OEM_8);
		caseStringify(VK_OEM_AX);   //  'AX' key on Japanese AX kbd
		caseStringify(VK_OEM_102);  //  "<>" or "\|" on RT 102-key kbd.
		caseStringify(VK_ICO_HELP); //  Help key on ICO
		caseStringify(VK_ICO_00);   //  00 key on ICO
		caseStringify(VK_PROCESSKEY);
		caseStringify(VK_ICO_CLEAR);
		caseStringify(VK_PACKET);
	}
	c[0] = (char)vk;
	return string(c);
}

string GetKeyName(unsigned int virtualKey)
{
	unsigned int scanCode = MapVirtualKey(virtualKey, MAPVK_VK_TO_VSC);

	// because MapVirtualKey strips the extended bit for some keys
	switch (virtualKey)
	{
	case VK_LEFT: case VK_UP: case VK_RIGHT: case VK_DOWN: // arrow keys
	case VK_PRIOR: case VK_NEXT: // page up and page down
	case VK_END: case VK_HOME:
	case VK_INSERT: case VK_DELETE:
	case VK_DIVIDE: // numpad slash
	case VK_NUMLOCK:
	{
		scanCode |= 0x100; // set extended bit
		break;
	}
	}

	char keyName[50];
	if (GetKeyNameText(scanCode << 16, keyName, sizeof(keyName)) != 0)
		return keyName;
	else //error
		return vkToString(virtualKey);
}

LRESULT CALLBACK HOOK_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_LBUTTONDOWN:
		keys[VK_LBUTTON] = true;
		break;
	case WM_LBUTTONUP:
		keys[VK_LBUTTON] = false;
		break;
	case WM_RBUTTONDOWN:
		keys[VK_RBUTTON] = true;
		break;
	case WM_RBUTTONUP:
		keys[VK_RBUTTON] = false;
		break;
	case WM_KEYDOWN:
		keys[wParam] = true;
		break;
	case WM_KEYUP:
		keys[wParam] = false;
		break;
	case WM_SYSKEYDOWN:
		keys[wParam] = true;
		break;
	case WM_SYSKEYUP:
		keys[wParam] = false;
		break;
	default: break;
	}

	if (uMsg == WM_KEYDOWN && wParam == cvar.gui_key)
	{
		bShowMenu = !bShowMenu;
		SaveCvar();
		return false;
	}
	if (bShowMenu)
		ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);

	return CallWindowProc(hGameWndProc, hWnd, uMsg, wParam, lParam);
}

void SetCursor()
{
	static bool setcursor = bShowMenu;
	if (setcursor != bShowMenu)
	{
		if (!bShowMenu && g_pEngine->GetMaxClients())
			SetCursorPos(g_pEngine->GetWindowCenterX(), g_pEngine->GetWindowCenterY());
		setcursor = bShowMenu;
	}
}

void InistalizeImgui(HDC hdc)
{
	static bool bInitializeImGui = false;
	if (!bInitializeImGui)
	{
		hGameWnd = WindowFromDC(hdc);
		hGameWndProc = (WNDPROC)SetWindowLong(hGameWnd, GWL_WNDPROC, (LONG)HOOK_WndProc);
		glGetIntegerv(GL_MAJOR_VERSION, &iMajor);
		glGetIntegerv(GL_MINOR_VERSION, &iMinor);
		if ((iMajor * 10 + iMinor) >= 32)
			bOldOpenGL = false;
		ImGui::CreateContext();
		ImGui_ImplWin32_Init(hGameWnd);
		if (!bOldOpenGL)
		{
			ImplementGl3();
			ImGui_ImplOpenGL3_Init();
		}
		else
			ImGui_ImplOpenGL2_Init();
		ImGui::StyleColorsDark();
		ImGui::GetStyle().AntiAliasedFill = !bOldOpenGL ? true : false;
		ImGui::GetStyle().AntiAliasedLines = !bOldOpenGL ? true : false;
		ImGui::GetStyle().FrameRounding = 0.0f;
		ImGui::GetStyle().WindowRounding = 0.0f;
		ImGui::GetStyle().ChildRounding = 0.0f;
		ImGui::GetStyle().ScrollbarRounding = 0.0f;
		ImGui::GetStyle().GrabRounding = 0.0f;
		ImGui::GetStyle().FrameBorderSize = 1.0f;
		ImGui::GetStyle().WindowPadding = ImVec2(6, 8);
		ImGui::GetStyle().WindowTitleAlign = ImVec2(0.5f, 0.5f);
		ImGui::GetStyle().DisplaySafeAreaPadding = ImVec2(0.0f, 0.0f);
		ImGui::GetStyle().WindowBorderSize = 0.0f;
		ImGui::GetStyle().FrameBorderSize = 0.0f;
		ImGui::GetStyle().PopupBorderSize = 0.0f;
		ImGui::GetIO().IniFilename = NULL;
		ImGui::GetIO().LogFilename = NULL;
		if (cvar.gui_key < 0 || cvar.gui_key > 254)
			cvar.gui_key = VK_INSERT;

		bInitializeImGui = true;
	}
}

void GLNewFrame()
{
	if (!bOldOpenGL)
		ImGui_ImplOpenGL3_NewFrame();
	else
		ImGui_ImplOpenGL2_NewFrame();
}

void RenderData()
{
	if (!bOldOpenGL)
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	else
		ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
}

void MouseEvent()
{
	if (bShowMenu)
		g_pClient->IN_DeactivateMouse();
	else
		g_pClient->IN_ActivateMouse();
}

void Clearkeysmenu()
{
	for (int i = 0; i < 256; i++) keys[i] = false, keysmenu[i] = false;
}

void HookImGui(HDC hdc)
{
	//Credit to my kind friend BloodSharp for helping a noob <3
	SetCursor();
	InistalizeImgui(hdc);
	GLNewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	GetTexture();
	UpdateLocalPlayer();
	UpdatePlayer();
	ColorChange();

	DrawRouteLine();
	DrawHitMark();
	DrawLongJump(); 
	DrawSoundEsp();
	DrawPlayerEsp();
	DrawAimbot();
	DrawTraceGrenade();
	DrawCrossHair();
	DrawWeaponName();
	DrawModeChange();
	DrawNoFlash();
	DrawAimBotFOV();
	DrawKzWindow();
	DrawKzKeys();
	DrawChatWindow();
	DrawBackDrop();
	DrawIDHookWindow();
	DrawConsoleWindow();
	DrawMenuWindow();
	if (show_demo && bShowMenu) ImGui::ShowDemoWindow();

	PlayRadio();
	Clearkeysmenu();

	ImGui::Render();
	RenderData();
	ImGui::GetIO().MouseDrawCursor = bShowMenu;
	MouseEvent();
}
