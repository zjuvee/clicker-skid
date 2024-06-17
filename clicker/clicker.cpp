/*

The logic of the exhaust clicker was completely coded by tosted.

github: https://github.com/zjuvee

discord: whenyourestrange.

*/

#include "pch.hpp"
#include "clicker.hpp"

#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

//=========== legit mode

void c_clicker::tosted2()
{
	// initialize random number generators and some clicker shit
	std::random_device rd{};
	std::mt19937 gen{ rd() };
	std::uniform_int_distribution<> chance_dist(1, 100);
	std::normal_distribution<float> movement_dist(0.0f, 1.0f);
	std::uniform_real_distribution<float> intensity_dist(0.0f, 1.0f);

	// shit timings variables
	static std::chrono::steady_clock::time_point lastCpsChangeTime = std::chrono::steady_clock::now();
	static int nextCps = config.clicker.min_cps;
	static std::uniform_int_distribution<> distrib(config.clicker.min_cps, config.clicker.max_cps);
	static bool cpsReduced = false;
	static std::chrono::steady_clock::time_point cpsReductionTime;
	static bool delayStarted = false;
	static std::chrono::steady_clock::time_point delayStartTime;
	static auto last_time = std::chrono::high_resolution_clock::now();
	auto now = std::chrono::high_resolution_clock::now();
	auto milli = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

	// left button pressed check
	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000 || GetAsyncKeyState(VK_LBUTTON)) {
		if (!delayStarted) {
			delayStarted = true;
			delayStartTime = std::chrono::steady_clock::now();
		}
	}
	else {
		delayStarted = false;
		return;
	}

	// delay before start
	if (delayStarted) {
		auto currentTime = std::chrono::steady_clock::now();
		auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - delayStartTime).count();
		if (elapsedTime < 100) {
			return;
		}
	}

	// adjust CPS
	if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastCpsChangeTime).count() >= 180) {

		if (cpsReduced && std::chrono::duration_cast<std::chrono::seconds>(now - cpsReductionTime).count() >= 1) {
			nextCps += 3;
			cpsReduced = false;
		}

		int cpsChange = distrib(gen) % 4 == 0 ? -4 : 2;
		nextCps += cpsChange;
		if (nextCps < config.clicker.min_cps) {
			nextCps = config.clicker.min_cps;
		}
		else if (nextCps > config.clicker.max_cps) {
			nextCps = config.clicker.max_cps;
		}

		if (nextCps >= config.clicker.max_cps && !cpsReduced && nextCps >= 16) {
			nextCps -= 3;
			cpsReductionTime = std::chrono::steady_clock::now();
			cpsReduced = true;
		}

		lastCpsChangeTime = std::chrono::steady_clock::now();
	}

	// random interval, kurtosis
	std::uniform_int_distribution<> intervalDist(911, 1202);
	int interval_ms = intervalDist(gen);
	if (chance_dist(gen) <= 120) {
		interval_ms = intervalDist(gen);
	}

	std::chrono::milliseconds interval(interval_ms / nextCps);

	if (std::chrono::duration_cast<std::chrono::milliseconds>(now - last_time) < interval) {
		return;
	}

	// simulate the clicks
	HWND minecraft = FindWindowA("LWJGL", NULL);
	POINT pos_cursor;
	GetCursorPos(&pos_cursor);
	PostMessageA(minecraft, WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(pos_cursor.x, pos_cursor.y));
	PostMessageA(minecraft, WM_LBUTTONUP, 0, MAKELPARAM(pos_cursor.x, pos_cursor.y));
	last_time = now;
}

// load a sound file into memory
std::vector<char> LoadSoundFile(const char* filename) {
	std::ifstream file(filename, std::ios::binary);
	if (!file) {
		return std::vector<char>();
	}

	file.seekg(0, std::ios::end);
	std::streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);

	std::vector<char> buffer(size);
	if (file.read(buffer.data(), size)) {
		return buffer;
	}
	else {
		return std::vector<char>();
	}
}

// play the loaded sound file <- made by chatgpt
void PlaySoundInBackground(const std::vector<char>& soundData) {
	if (!soundData.empty()) {
		PlaySoundW(reinterpret_cast<const wchar_t*>(soundData.data()), NULL, SND_MEMORY | SND_ASYNC);
	}
}

// load sounds
std::vector<char> soundData = LoadSoundFile("D:\\repos\\Default.wav");
std::vector<char> soundData2 = LoadSoundFile("D:\\repos\\Regular.wav");
std::vector<char> soundData3 = LoadSoundFile("D:\\repos\\HP.wav");

//=========== legitplus mode
void c_clicker::tosted()
{
	// initialize random number generators and some shit
	static std::random_device rd{};
	static std::mt19937 gen{ rd() };
	static std::uniform_int_distribution<> chance_dist(1, 100);
	static std::uniform_int_distribution<> intervalDist(911, 1202);

	// timings variables
	static bool delayStarted = false;
	static std::chrono::steady_clock::time_point delayStartTime;
	static std::chrono::steady_clock::time_point lastCpsChangeTime = std::chrono::steady_clock::now();
	static int nextCps = config.clicker.min_cps;
	static bool cpsReduced = false;
	static std::chrono::steady_clock::time_point cpsReductionTime;

	// left button mouse check
	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000 || GetAsyncKeyState(VK_LBUTTON)) {
		if (!delayStarted) {
			delayStarted = true;
			delayStartTime = std::chrono::steady_clock::now();
		}
	}
	else {
		delayStarted = false;
		return;
	}

	// delay before start, for inventory refill
	if (delayStarted) {
		auto currentTime = std::chrono::steady_clock::now();
		auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - delayStartTime).count();
		if (elapsedTime < 120) {
			return;
		}
	}

	// adjust CPS
	auto now = std::chrono::steady_clock::now();
	if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastCpsChangeTime).count() >= 180) {

		std::uniform_int_distribution<> distrib(config.clicker.min_cps, config.clicker.max_cps);
		int cpsChange = distrib(gen) % 4 == 0 ? -4 : 2;
		nextCps += cpsChange;

		if (nextCps < config.clicker.min_cps) {
			nextCps = config.clicker.min_cps;
		}
		else if (nextCps > config.clicker.max_cps) {
			nextCps = config.clicker.max_cps;
		}

		if (nextCps >= config.clicker.max_cps && !cpsReduced && nextCps >= 16) {
			nextCps -= 3;
			cpsReductionTime = std::chrono::steady_clock::now();
			cpsReduced = true;
		}

		lastCpsChangeTime = std::chrono::steady_clock::now();
	}

	if (nextCps == 0) {
		nextCps = 1;
	}
	std::chrono::milliseconds interval(intervalDist(gen) / nextCps);

	if (std::chrono::duration_cast<std::chrono::milliseconds>(now - last_time) < interval) {
		return;
	}

	// simulate clicks
	HWND minecraft = FindWindowA("LWJGL", NULL);
	if (minecraft != NULL) {
		POINT pos_cursor;
		GetCursorPos(&pos_cursor);
		PostMessageA(minecraft, WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(pos_cursor.x, pos_cursor.y));
		PostMessageA(minecraft, WM_LBUTTONUP, 0, MAKELPARAM(pos_cursor.x, pos_cursor.y));
	}

	// play clicksound
	if (config.clicker.b_enable_sound) {
		PlaySoundInBackground(soundData);
	}

	last_time = now;
}

//============ jitter func
void c_clicker::jitter() {
	// initialize random bumbers generator and some lclicker shit
	static std::random_device rd{};
	static std::mt19937 gen{ rd() };
	std::normal_distribution<float> movement_dist(0.0f, config.clicker.jitter_intensity);
	HWND minecraft = FindWindowA("LWJGL", NULL);
	static bool jitterRunning = false;
	if (jitterRunning) return;
	jitterRunning = true;

	std::thread([&, minecraft]() {
		bool delayStarted = false;
		std::chrono::steady_clock::time_point delayStartTime;

		// shitty key detect
		while (GetAsyncKeyState(VK_LBUTTON) & 0x8000 || GetAsyncKeyState(VK_LBUTTON)) {
			if (!delayStarted) {
				delayStarted = true;
				delayStartTime = std::chrono::steady_clock::now();
			}

			// delay before starting
			if (delayStarted) {
				auto currentTime = std::chrono::steady_clock::now();
				auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - delayStartTime).count();
				if (elapsedTime < 450) {
					continue;
				}
			}

			// cursor movement function for jitter, styx skidded
			if (config.clicker.b_enable_jitter) {
				if (minecraft) {
					POINT p;
					GetCursorPos(&p);
					float xOffset = movement_dist(gen);
					float yOffset = movement_dist(gen);

					if (xOffset > 10.0f) xOffset = 10.0f;
					if (xOffset < -10.0f) xOffset = -10.0f;
					if (yOffset > 10.0f) yOffset = 10.0f;
					if (yOffset < -10.0f) yOffset = -10.0f;

					SetCursorPos(static_cast<int>(p.x + xOffset), static_cast<int>(p.y + yOffset));

					int sleepDuration = std::rand() % 45 + 15;
					sleepDuration = (sleepDuration * config.clicker.jitter_chance) / 100;
					std::this_thread::sleep_for(std::chrono::milliseconds(sleepDuration));
				}
				else {
					std::this_thread::sleep_for(std::chrono::milliseconds(50));
				}
			}
		}

		jitterRunning = false;
		}).detach();
}

//========== blockhit func
void c_clicker::blockhit() {
	// initialize random number generators and some clicker shit
	static std::random_device rd{};
	static std::mt19937 gen{ rd() };
	static bool delayStarted = false;
	static std::chrono::steady_clock::time_point delayStartTime;
	std::uniform_int_distribution<> chances_dist(1, 300);

	HWND minecraft = FindWindowA("LWJGL", NULL);

	// shitty key detect
	if (GetAsyncKeyState(VK_LBUTTON) & 0x8000 || GetAsyncKeyState(VK_LBUTTON)) {
		if (!delayStarted) {
			delayStarted = true;
			delayStartTime = std::chrono::steady_clock::now();
		}
	}
	else {
		delayStarted = false;
		return;
	}

	// delay before starting
	if (delayStarted) {
		auto currentTime = std::chrono::steady_clock::now();
		auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - delayStartTime).count();
		if (elapsedTime < 400) {
			return;
		}
	}

	static auto lastClick = std::chrono::steady_clock::now();
	static int interval = 1000 / config.clicker.bps;

	if (config.clicker.b_enable_blockhit && config.clicker.b_enable_left_clicker) {
		if (chances_dist(gen) <= config.clicker.blockhit_chance) {

			if (config.clicker.b_hold_rmb) {
				if (GetAsyncKeyState(VK_RBUTTON) & 0x8000) {
					auto now = std::chrono::steady_clock::now();
					auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastClick).count();

					if (elapsedTime >= interval) {
						POINT pos_cursor;
						GetCursorPos(&pos_cursor);
						PostMessageA(minecraft, WM_RBUTTONDOWN, MK_RBUTTON, MAKELPARAM(pos_cursor.x, pos_cursor.y));
						PostMessageA(minecraft, WM_RBUTTONUP, 0, MAKELPARAM(pos_cursor.x, pos_cursor.y));

						lastClick = now;
					}
				}
			}
			else {
				auto now = std::chrono::steady_clock::now();
				auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastClick).count();

				if (elapsedTime >= interval) {
					POINT pos_cursor;
					GetCursorPos(&pos_cursor);
					PostMessageA(minecraft, WM_RBUTTONDOWN, MK_RBUTTON, MAKELPARAM(pos_cursor.x, pos_cursor.y));
					PostMessageA(minecraft, WM_RBUTTONUP, 0, MAKELPARAM(pos_cursor.x, pos_cursor.y));

					lastClick = now;
				}
			}
		}
	}
}

//=========== thread func = clicker main
void c_clicker::thread()
{
	SetThreadPriority(std::this_thread::get_id, THREAD_PRIORITY_TIME_CRITICAL);

	while (vars::b_is_running)
	{
		if (!this->b_is_right_clicking || !this->b_is_left_clicking)
			std::this_thread::sleep_for(1ms);

		if (vars::key::clicker_enabled.get())
		{
			if (focus::window_think()) {

				if (!config.clicker.b_enable_left_clicker) return;

				if (config.clicker.b_enable_left_clicker) {
					if (config.clicker.mode_legitplus) {
						tosted();
					}
					else if (config.clicker.mode_legit) {
						tosted2();
					}

					blockhit();
					jitter();
				}

			}
		}
	}
}
