#include "client.h"

CVARlist cvar;

void CVARlist::init()
{
	#define INITCVAR(name,value) cmd.AddCvarFloat(#name, &##name );##name=##value;

	INITCVAR(rage_active, 0.0);
	INITCVAR(rage_wall, 0.0);
	INITCVAR(rage_fov, 360.0);
	INITCVAR(rage_hitbox, 0.0);
	INITCVAR(rage_multipoint, 0.0);
	INITCVAR(rage_target_selection, 0.0);
	INITCVAR(rage_perfect_silent, 0.0);
	INITCVAR(rage_silent, 0.0); 
	INITCVAR(rage_knife_attack, 1.0);
	INITCVAR(rage_shield_attack, 0.0);
	INITCVAR(rage_nospread_method, 0.0);
	INITCVAR(rage_auto_fire, 1.0);
	INITCVAR(rage_auto_fire_key, -1.0);
	INITCVAR(rage_team, 0.0);

	INITCVAR(aim_id_mode, 1.0);

	for (int i = 1; i <= 30; i++)
	{
		if (i == 2 || i == 4 || i == 6 || i == 9 || i == 25 || i == 29) continue;

		char str[256];
		sprintf(str, "legit[%d].active", i);
		cmd.AddCvarFloat(str, &cvar.legit[i].active); legit[i].active = 0.0;
		sprintf(str, "legit[%d].speed", i);
		cmd.AddCvarFloat(str, &cvar.legit[i].speed); legit[i].speed = 0.0;
		sprintf(str, "legit[%d].speed_scale_fov", i);
		cmd.AddCvarFloat(str, &cvar.legit[i].speed_scale_fov); legit[i].speed_scale_fov = 0.0;
		sprintf(str, "legit[%d].reaction_time", i);
		cmd.AddCvarFloat(str, &cvar.legit[i].reaction_time); legit[i].reaction_time = 0.0;
		sprintf(str, "legit[%d].humanize", i);
		cmd.AddCvarFloat(str, &cvar.legit[i].humanize); legit[i].humanize = 0.0;
		sprintf(str, "legit[%d].recoil_compensation_pitch", i);
		cmd.AddCvarFloat(str, &cvar.legit[i].recoil_compensation_pitch); legit[i].recoil_compensation_pitch = 0.0;
		sprintf(str, "legit[%d].recoil_compensation_yaw", i);
		cmd.AddCvarFloat(str, &cvar.legit[i].recoil_compensation_yaw); legit[i].recoil_compensation_yaw = 0.0;
		sprintf(str, "legit[%d].recoil_compensation_after_shots_fired", i);
		cmd.AddCvarFloat(str, &cvar.legit[i].recoil_compensation_after_shots_fired); legit[i].recoil_compensation_after_shots_fired = 0.0;
		sprintf(str, "legit[%d].block_attack_after_kill", i);
		cmd.AddCvarFloat(str, &cvar.legit[i].block_attack_after_kill); legit[i].block_attack_after_kill = 0.0;
		sprintf(str, "legit[%d].accuracy", i);
		cmd.AddCvarFloat(str, &cvar.legit[i].accuracy); legit[i].accuracy = 0.0;
		sprintf(str, "legit[%d].perfect_silent", i);
		cmd.AddCvarFloat(str, &cvar.legit[i].perfect_silent); legit[i].perfect_silent = 0.0;
		sprintf(str, "legit[%d].fov", i);
		cmd.AddCvarFloat(str, &cvar.legit[i].fov); legit[i].fov = 5.0;
		sprintf(str, "legit[%d].speed_in_attack", i);
		cmd.AddCvarFloat(str, &cvar.legit[i].speed_in_attack); legit[i].speed_in_attack = 100;
		sprintf(str, "legit[%d].waist", i);
		cmd.AddCvarFloat(str, &cvar.legit[i].waist); legit[i].waist = 0.0;
		sprintf(str, "legit[%d].chest", i);
		cmd.AddCvarFloat(str, &cvar.legit[i].chest); legit[i].chest = 0.0;
		sprintf(str, "legit[%d].head", i);
		cmd.AddCvarFloat(str, &cvar.legit[i].head); legit[i].head = 1.0;
		sprintf(str, "legit[%d].all", i);
		cmd.AddCvarFloat(str, &cvar.legit[i].all); legit[i].all = 0.0;
		sprintf(str, "legit[%d].trigger_active", i);
		cmd.AddCvarFloat(str, &cvar.legit[i].trigger_active); legit[i].trigger_active = 0.0;
		sprintf(str, "legit[%d].trigger_wall", i);
		cmd.AddCvarFloat(str, &cvar.legit[i].trigger_wall); legit[i].trigger_wall = 0.0;
		sprintf(str, "legit[%d].trigger_accuracy", i);
		cmd.AddCvarFloat(str, &cvar.legit[i].trigger_accuracy); legit[i].trigger_accuracy = 0.0;
		sprintf(str, "legit[%d].trigger_head", i);
		cmd.AddCvarFloat(str, &cvar.legit[i].trigger_head); legit[i].trigger_head = 1.0;
		sprintf(str, "legit[%d].trigger_chest", i);
		cmd.AddCvarFloat(str, &cvar.legit[i].trigger_chest); legit[i].trigger_chest = 0.0;
		sprintf(str, "legit[%d].trigger_waist", i);
		cmd.AddCvarFloat(str, &cvar.legit[i].trigger_waist); legit[i].trigger_waist = 0.0;
		sprintf(str, "legit[%d].trigger_all", i);
		cmd.AddCvarFloat(str, &cvar.legit[i].trigger_all); legit[i].trigger_all = 0.0;
		sprintf(str, "legit[%d].trigger_delay_shot", i);
		cmd.AddCvarFloat(str, &cvar.legit[i].trigger_delay_shot); legit[i].trigger_delay_shot = 0.0;
		sprintf(str, "legit[%d].trigger_shot_count", i);
		cmd.AddCvarFloat(str, &cvar.legit[i].trigger_shot_count); legit[i].trigger_shot_count = 1.0;
		sprintf(str, "legit[%d].trigger_shot_type", i);
		cmd.AddCvarFloat(str, &cvar.legit[i].trigger_shot_type); legit[i].trigger_shot_type = 0.0;
		sprintf(str, "legit[%d].trigger_random_max", i);
		cmd.AddCvarFloat(str, &cvar.legit[i].trigger_random_max); legit[i].trigger_random_max = 3.0;
	}

	for (int i = 1; i <= 30; i++)
	{
		if (i == 2 || i == 4 || i == 6 || i == 9 || i == 25 || i == 29) continue;

		char str[256];
		sprintf(str, "rage[%d].rage_recoil", i);
		cmd.AddCvarFloat(str, &cvar.rage[i].rage_recoil); rage[i].rage_recoil = 2.0;
		sprintf(str, "rage[%d].rage_delay_shot", i);
		cmd.AddCvarFloat(str, &cvar.rage[i].rage_delay_shot); rage[i].rage_delay_shot = 0.0;
		sprintf(str, "rage[%d].rage_shot_count", i);
		cmd.AddCvarFloat(str, &cvar.rage[i].rage_shot_count); rage[i].rage_shot_count = 1.0;
		sprintf(str, "rage[%d].rage_shot_type", i);
		cmd.AddCvarFloat(str, &cvar.rage[i].rage_shot_type); rage[i].rage_shot_type = 0.0;
		sprintf(str, "rage[%d].rage_random_max", i);
		cmd.AddCvarFloat(str, &cvar.rage[i].rage_random_max); rage[i].rage_random_max = 3.0;
	}

	INITCVAR(legit_trigger_only_zoom, 0.0);
	INITCVAR(legit_trigger_team, 0.0);
	INITCVAR(legit_trigger_key, -1.0);

	INITCVAR(legit_fast_zoom, 0.0);
	INITCVAR(legit_team, 0.0);

	INITCVAR(menu_legit_global_section, 0.0);
	INITCVAR(menu_legit_sub_section, 0.0);
	INITCVAR(menu_rage_global_section, 0.0);
	INITCVAR(menu_rage_sub_section, 0.0);

	INITCVAR(knifebot_active, 0.0);
	INITCVAR(knifebot_attack, 1.0);
	INITCVAR(knifebot_attack_distance, 72.0);
	INITCVAR(knifebot_attack2_distance, 64.0);
	INITCVAR(knifebot_silent, 0.0);
	INITCVAR(knifebot_perfect_silent, 0.0);
	INITCVAR(knifebot_fov, 45.0);
	INITCVAR(knifebot_team, 0.0);

	INITCVAR(bypass_trace_blockers, 1.0);
	INITCVAR(bypass_valid_blockers, 1.0);

	INITCVAR(aa_pitch, 0.0);
	INITCVAR(aa_edge, 0.0);
	INITCVAR(aa_yaw, 0.0);
	INITCVAR(aa_yaw_static, 0.0);
	INITCVAR(aa_yaw_while_running, 0.0);
	INITCVAR(aa_roll, 0.0);
	INITCVAR(aa_roll_static, 0.0);
	INITCVAR(aa_roll_while_running, 0.0);

	INITCVAR(fakelag_active, 0.0);
	INITCVAR(fakelag_while_shooting, 0.0);
	INITCVAR(fakelag_move, 2.0);
	INITCVAR(fakelag_type, 2.0);
	INITCVAR(fakelag_limit, 0.0);
	INITCVAR(fakelag_variance, 0.0);

	INITCVAR(misc_snapshot, 0.0);
	INITCVAR(misc_snapshot_time, 10.0);
	INITCVAR(misc_quick_change, 0.0);
	INITCVAR(misc_quick_change_key, -1.0);
	INITCVAR(misc_weapon_local, 0.0);
	INITCVAR(misc_steamid, 0.0);
	INITCVAR(misc_wav_speed, 1.0);

	INITCVAR(chams_view_model, 0.0);
	INITCVAR(chams_view_model_glow, 0.0);
	INITCVAR(chams_player, 0.0);
	INITCVAR(chams_player_glow, 0.0);
	INITCVAR(chams_player_wall, 0.0);
	INITCVAR(chams_world, 0.0);
	INITCVAR(chams_world_wall, 0.0);
	INITCVAR(chams_world_glow, 0.0);

	INITCVAR(kz_strafe, 0.0);
	INITCVAR(kz_fast_run, 0.0);
	INITCVAR(kz_gstrafe, 0.0);
	INITCVAR(kz_bhop, 0.0);
	INITCVAR(kz_jump_bug, 0.0);
	INITCVAR(kz_jump_bug_auto, 0.0);
	INITCVAR(kz_show_kz, 0.0);
	INITCVAR(kz_strafe_key, -1.0);
	INITCVAR(kz_fastrun_key, -1.0);
	INITCVAR(kz_gstrafe_key, -1.0);
	INITCVAR(kz_bhop_key, -1.0);
	INITCVAR(kz_jumpbug_key, -1.0);

	INITCVAR(route_mode, 1.0);
	INITCVAR(route_auto, 0.0);
	INITCVAR(route_jump, 1.0);
	INITCVAR(route_jump_step, 2.0);
	INITCVAR(route_direction, 1.0);
	INITCVAR(route_direction_step, 4.0);
	INITCVAR(route_draw_visual, 0.0);
	INITCVAR(route_draw, 1.0);
	INITCVAR(route_rush_key, -1.0);

	INITCVAR(color_red, 0.5);
	INITCVAR(color_green, 0.5);
	INITCVAR(color_blue, 0.5);
	INITCVAR(color_random, 0.0);
	INITCVAR(color_speed, 0.0);

	INITCVAR(visual_name, 1.0);
	INITCVAR(visual_weapon, 0.0);
	INITCVAR(visual_weapon_model, 0.0);
	INITCVAR(visual_reload_bar, 0.0);
	INITCVAR(visual_box, 0.0);
	INITCVAR(visual_health, 0.0);
	INITCVAR(visual_alpha, 1.0);
	INITCVAR(visual_visual_team, 1.0);
	INITCVAR(visual_box_width, 1.0);
	INITCVAR(visual_box_height, 1.0);
	INITCVAR(visual_box_size_bot, 80.0);
	INITCVAR(visual_box_size_top, 80.0);
	INITCVAR(visual_sound, 1.0);
	INITCVAR(visual_idhook_only, 0.0);
	INITCVAR(visual_rem_flash, 0.0);
	INITCVAR(visual_chase_cam, 0.0);
	INITCVAR(visual_chase_back, 100.0);
	INITCVAR(visual_chase_up, 16.0);
	INITCVAR(visual_headshot_mark, 0.0);
	INITCVAR(visual_kill_lightning, 0.0);
	INITCVAR(visual_rounding, 0.0);
	INITCVAR(visual_radar, 0.0);
	INITCVAR(visual_radar_size, 100.0); 
	INITCVAR(visual_kill_sound, 0.0);
	INITCVAR(visual_kill_volume, 15.0);
	INITCVAR(visual_grenade_trajectory, 0.0);
	INITCVAR(visual_crosshair, 0.0);
	INITCVAR(visual_crosshair_offset, 0.0);
	INITCVAR(visual_crosshair_spin, 0.0);
	INITCVAR(visual_vip, 0.0);
	INITCVAR(visual_custom_fov, 90.0);
	INITCVAR(visual_skins, 0.0);
	INITCVAR(visual_wall, 0.0);
	INITCVAR(visual_lambert, 0.0);
	INITCVAR(visual_no_hands, 0.0);

	INITCVAR(skeleton_player_bone, 0.0);
	INITCVAR(skeleton_player_hitbox, 0.0);
	INITCVAR(skeleton_view_model_bone, 0.0);
	INITCVAR(skeleton_view_model_hitbox, 0.0);
	INITCVAR(skeleton_models, 0.0);
	INITCVAR(skeleton_world_hitbox, 0.0);
	INITCVAR(skeleton_world_bone, 0.0);

	INITCVAR(gui_key, 45.0);
	INITCVAR(gui_key_hud, -1.0);
	INITCVAR(gui_chat, 0.0);
	INITCVAR(gui_chat_key, -1.0);
	INITCVAR(gui_chat_key_team, -1.0);
	INITCVAR(gui_radio, 0.0);
	INITCVAR(gui_radio_volume, 0.0);
	INITCVAR(gui_menu_button_spin, 0.0);
	INITCVAR(gui_menu_image, 0.0);
	INITCVAR(gui_menu_image_spin, 0.0);
	INITCVAR(gui_mouse_image, 0.0);
	INITCVAR(gui_mouse_image_spin, 0.0);
	INITCVAR(gui_mouse_image_amount, 0.0);

	INITCVAR(test1, 0.0);
	INITCVAR(test2, 0.0);
	INITCVAR(test3, 0.0);
	INITCVAR(test4, 0.0);
	INITCVAR(test5, 0.0);
	INITCVAR(test6, 0.0);
}