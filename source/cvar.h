#ifndef _CVARS_
#define _CVARS_

class CVARlist
{
public:
	void init();

	float rage_active;
	float rage_wall;
	float rage_fov;
	float rage_hitbox;
	float rage_multipoint;
	float rage_target_selection;
	float rage_perfect_silent;
	float rage_silent;
	float rage_knife_attack;
	float rage_shield_attack;
	float rage_nospread_method;
	float rage_auto_fire;
	float rage_auto_fire_key;
	float rage_team;

	float aim_id_mode;

	struct legit_weapons
	{
		float active;
		float speed;
		float speed_scale_fov;
		float reaction_time;
		float humanize;
		float recoil_compensation_pitch;
		float recoil_compensation_yaw;
		float recoil_compensation_after_shots_fired;
		float block_attack_after_kill;
		float accuracy;
		float perfect_silent;
		float fov;
		float speed_in_attack;
		float waist;
		float chest;
		float head;
		float all;

		float trigger_active;
		float trigger_wall;
		float trigger_accuracy;
		float trigger_head;
		float trigger_chest;
		float trigger_waist;
		float trigger_all;
		float trigger_delay_shot;
		float trigger_shot_count;
		float trigger_shot_type;
		float trigger_random_max;
	} legit[31];

	struct rage_weapons
	{
		float rage_recoil;
		float rage_delay_shot;
		float rage_shot_count;
		float rage_shot_type;
		float rage_random_max;
	} rage[31];

	float legit_trigger_only_zoom;
	float legit_trigger_team;
	float legit_trigger_key;

	float legit_fast_zoom;
	float legit_team;

	float menu_legit_global_section;
	float menu_legit_sub_section;
	float menu_rage_global_section;
	float menu_rage_sub_section;

	float knifebot_active;
	float knifebot_attack;
	float knifebot_attack_distance;
	float knifebot_attack2_distance;
	float knifebot_silent;
	float knifebot_perfect_silent;
	float knifebot_fov;
	float knifebot_team;

	float bypass_trace_blockers;
	float bypass_valid_blockers;

	float aa_pitch;
	float aa_edge;
	float aa_yaw;
	float aa_yaw_static;
	float aa_yaw_while_running;
	float aa_roll;
	float aa_roll_static;
	float aa_roll_while_running;

	float fakelag_active;
	float fakelag_while_shooting;
	float fakelag_move;
	float fakelag_type;
	float fakelag_limit;
	float fakelag_variance;

	float misc_snapshot;
	float misc_snapshot_time;
	float misc_quick_change;
	float misc_quick_change_key;
	float misc_weapon_local;
	float misc_steamid;
	float misc_wav_speed;

	float chams_view_model;
	float chams_view_model_glow;
	float chams_player;
	float chams_player_glow;
	float chams_player_wall;
	float chams_world;
	float chams_world_wall;
	float chams_world_glow;

	float kz_strafe;
	float kz_fast_run;
	float kz_gstrafe;
	float kz_bhop;
	float kz_jump_bug;
	float kz_jump_bug_auto;
	float kz_show_kz;
	float kz_strafe_key;
	float kz_fastrun_key;
	float kz_gstrafe_key;
	float kz_bhop_key;
	float kz_jumpbug_key;

	float route_mode;
	float route_auto;
	float route_jump;
	float route_jump_step;
	float route_direction;
	float route_direction_step;
	float route_draw_visual;
	float route_draw;
	float route_rush_key;

	float color_red;
	float color_green;
	float color_blue;
	float color_random;
	float color_speed;

	float visual_name;
	float visual_weapon;
	float visual_weapon_model;
	float visual_reload_bar;
	float visual_box;
	float visual_health;
	float visual_alpha;
	float visual_visual_team;
	float visual_box_width;
	float visual_box_height;
	float visual_box_size_bot;
	float visual_box_size_top;
	float visual_sound;
	float visual_idhook_only;
	float visual_rem_flash;
	float visual_chase_cam;
	float visual_chase_back;
	float visual_chase_up;
	float visual_headshot_mark;
	float visual_kill_lightning;
	float visual_rounding;
	float visual_radar;
	float visual_radar_size;
	float visual_kill_sound;
	float visual_kill_volume;
	float visual_grenade_trajectory;
	float visual_crosshair;
	float visual_crosshair_offset;
	float visual_crosshair_spin;
	float visual_vip;
	float visual_custom_fov;
	float visual_skins;
	float visual_wall;
	float visual_lambert;
	float visual_no_hands;

	float skeleton_player_bone;
	float skeleton_player_hitbox;
	float skeleton_view_model_bone;
	float skeleton_view_model_hitbox;
	float skeleton_models;
	float skeleton_world_hitbox;
	float skeleton_world_bone;

	float gui_key;
	float gui_key_hud;
	float gui_chat;
	float gui_chat_key;
	float gui_chat_key_team;
	float gui_radio;
	float gui_radio_volume;
	float gui_menu_button_spin;
	float gui_menu_image;
	float gui_menu_image_spin;
	float gui_mouse_image;
	float gui_mouse_image_spin;
	float gui_mouse_image_amount;

	float test1;
	float test2;
	float test3;
	float test4;
	float test5;
	float test6;
};

extern CVARlist cvar;

#endif