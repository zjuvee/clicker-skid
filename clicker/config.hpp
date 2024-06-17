#pragma once

#include "archivex.hpp"

#include <shlobj.h>
#include <fstream>
#include <filesystem>

class c_config
{
private:
	std::filesystem::path path;
	std::vector<std::string> configs;

public:

	constexpr auto& get_configs() { return configs; };
	constexpr auto& get_path() { return path; };

	struct
	{
		int i_clicker_key { 0 };
		int i_key_type { 0 };
		int i_version_type { 0 };

		bool b_enable_left_clicker { false };
		bool b_enable_right_clicker { false };
		bool b_enable_blockhit { false };

		bool mode_legitplus { true };
		bool mode_legit { false };

		float min_cps = 10.0f;
		float max_cps = 14.0f;

		float bps = 4.5f;
		float blockhit_chance = 50;
		bool b_hold_rmb	{ true };

		bool b_enable_jitter { false };
		float jitter_intensity = 3;
		float jitter_chance = 100;

		bool b_enable_sound { false };

		bool b_only_in_game { true };
		bool b_work_in_inventory { true };

		float f_color_accent[4] { 0.41f, 0.41f, 0.86f, 1.00f };
		float f_color_accent_hovered[4] { 0.53f, 0.53f, 0.91f, 1.00f };
		float f_color_accent_active[4] { 0.40f, 0.39f, 0.90f, 1.00f };
		float f_color_accent_text[4] { 0.94f, 0.94f, 0.94f, 1.00f };

		std::string str_window_title;
	} clicker;
};

inline auto config = c_config();