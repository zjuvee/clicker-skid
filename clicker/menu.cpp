
/*

WARNING! this part has been skidded from https://github.com/b1scoito/clicker
anyway, most of the things are not used by the exhaustclicker

credits: b1scoito

*/

#include "pch.hpp"
#include "menu.hpp"

namespace directx9
{
	inline IDirect3D9* context = {};
	inline IDirect3DDevice9* device = {};
	inline D3DPRESENT_PARAMETERS param;
}

namespace ctx
{
	inline HWND hWnd = {};
	inline WNDCLASSEX wc = {};
	inline int menu_width = 450, menu_height = 400;

	inline ImGuiStyle* imgui_style = {};
	inline ImGuiIO* imgui_io = {};
}

void c_menu::on_paint()
{
	static int x = 0, y = 0;

	ImGui::SetNextWindowSize({ (float)ctx::menu_width, (float)ctx::menu_height }, ImGuiCond_Once);
	ImGui::SetNextWindowPos({ 0, 0 }, ImGuiCond_Once);

	if (!vars::b_is_running)
		std::exit(0);

	if (ImGui::Begin("                                                     exhaust clicker", &vars::b_is_running, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove))
	{
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			get_mouse_offset(x, y, ctx::hWnd);

		if (y >= 0 && y <= (ImGui::GetTextLineHeight() + ImGui::GetStyle().FramePadding.y * 4) && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
			set_position(x, y, ctx::menu_width, ctx::menu_height, ctx::hWnd);

		if (ImGui::BeginTabBar("##var::clicker::tabs"))
		{
			if (ImGui::BeginTabItem("clicker"))
			{
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip("Press Ctrl + Left click on the slider for custom values.\nValues between 9.5f - 12.5f are recommended for bypassing server-sided anti-cheats.");

				ImGui::Checkbox("LeftClicker", &config.clicker.b_enable_left_clicker);

				//ImGui::SameLine();

				//keybind_button(config.clicker.i_clicker_key, 150, 22);

				//ImGui::SameLine();

				//ImGui::PushItemWidth(100.f);
				//ImGui::Combo("##var::clicker::i_key_type", &config.clicker.i_key_type, "Always\0Hold\0Toggle\0\0");
				//ImGui::PopItemWidth();

				if (config.clicker.b_enable_left_clicker)
					ImGui::Text("Mode:");
				if (config.clicker.b_enable_left_clicker)
					ImGui::Checkbox("Legit+", &config.clicker.mode_legitplus);
					if (config.clicker.mode_legitplus) {
						config.clicker.mode_legit = false;
						config.clicker.mode_legitplus = true;
					}
				if (config.clicker.b_enable_left_clicker)
					ImGui::SameLine();
				if (config.clicker.b_enable_left_clicker)
					ImGui::Checkbox("Legit", &config.clicker.mode_legit);
					if (config.clicker.mode_legit) {
						config.clicker.mode_legitplus = false;
						config.clicker.mode_legit = true;
					}
				if (config.clicker.b_enable_left_clicker)
					ImGui::Spacing();

				if (config.clicker.b_enable_left_clicker)
					if (ImGui::SliderFloat("##var::clicker::min_cps", &config.clicker.min_cps, 1.f, 20.f, "%.2f min cps"))
					{
						if (config.clicker.min_cps > config.clicker.max_cps)
						{
							config.clicker.max_cps = config.clicker.min_cps;
						}
					}

				if (config.clicker.b_enable_left_clicker)
					if (ImGui::SliderFloat("##var::clicker::max_cps", &config.clicker.max_cps, 1.f, 20.f, "%.2f max cps"))
					{
						if (config.clicker.max_cps < config.clicker.min_cps)
						{
							config.clicker.min_cps = config.clicker.max_cps;
						}
					}

				if (config.clicker.b_enable_left_clicker)
					ImGui::Spacing();

				if (config.clicker.b_enable_left_clicker)
					ImGui::Separator();
				if (config.clicker.b_enable_left_clicker)
					ImGui::Spacing();

				ImGui::Checkbox("Block Hit", &config.clicker.b_enable_blockhit);
				if (config.clicker.b_enable_blockhit)
					ImGui::Checkbox("Hold RMB", &config.clicker.b_hold_rmb);

				if (config.clicker.b_enable_blockhit)
					ImGui::Spacing();
				if (config.clicker.b_enable_blockhit)
					ImGui::Separator();
				if (config.clicker.b_enable_blockhit)
					ImGui::Spacing();

				ImGui::Checkbox("Jitter", &config.clicker.b_enable_jitter);

				if (config.clicker.b_enable_jitter)
					ImGui::Spacing();

				if (config.clicker.b_enable_jitter)
					ImGui::SliderFloat("##var::clicker::jitter_intensity", &config.clicker.jitter_intensity, 1.f, 8.f, "%.2f intensity");
				if (config.clicker.b_enable_jitter)
					ImGui::SliderFloat("##var::clicker::jitter_chance", &config.clicker.jitter_chance, 1.f, 100.f, "%.2f chance");

				if (config.clicker.b_enable_jitter)
					ImGui::Spacing();

				if (config.clicker.b_enable_jitter)
					ImGui::Separator();
				if (config.clicker.b_enable_jitter)
					ImGui::Spacing();

				ImGui::Checkbox("Clicksounds", &config.clicker.b_enable_sound);

				if (config.clicker.b_enable_sound)
					ImGui::Spacing();
				if (config.clicker.b_enable_sound)
					ImGui::Separator();
				if (config.clicker.b_enable_sound)
					ImGui::Spacing();

				ImGui::EndTabItem();
			}
			

			if (ImGui::BeginTabItem("info")) {
				ImGui::Text("this autoclick was made by tosted & malbor0, inspired by the b1scoito clicker");	
				ImGui::Text("(https://github.com/b1scoito/clicker) <- thanks for the imgui and some features :)");
				ImGui::Text("for any issues or suggestions, you can contact me on discord: whenyourestrange.");
			}
			ImGui::EndTabBar();
		}
		ImGui::End();
	}
}

// from here on down, its all part of b1scoito clicker
void c_menu::keybind_button(int& i_key, int i_width, int i_height)
{
	static auto b_get = false;
	static std::string sz_text("Click to bind");

	if (ImGui::Button(sz_text.c_str(), ImVec2((float)(i_width), (float)(i_height))))
		b_get = true;

	if (b_get)
	{
		for (auto i = 1; i < 256; i++)
		{
			if (GetAsyncKeyState(i) & 0x8000)
			{
				if (i != 12)
				{
					i_key = i == VK_ESCAPE ? 0 : i;
					b_get = false;
				}
			}
		}
		sz_text = "Press a key";
	}
	else if (!b_get && i_key == 0)
		sz_text = "Click to bind";
	else if (!b_get && i_key != 0)
		sz_text = "Bound to " + get_key_name_by_id(i_key);
}

std::string c_menu::get_key_name_by_id(int id)
{
	static std::unordered_map<int, std::string> key_names = {
		{ 0, "None" },
		{ VK_LBUTTON, "Mouse 1" },
		{ VK_RBUTTON, "Mouse 2" },
		{ VK_MBUTTON, "Mouse 3" },
		{ VK_XBUTTON1, "Mouse 4" },
		{ VK_XBUTTON2, "Mouse 5" },
		{ VK_BACK, "Back" },
		{ VK_TAB, "Tab" },
		{ VK_CLEAR, "Clear" },
		{ VK_RETURN, "Enter" },
		{ VK_SHIFT, "Shift" },
		{ VK_CONTROL, "Ctrl" },
		{ VK_MENU, "Alt" },
		{ VK_PAUSE, "Pause" },
		{ VK_CAPITAL, "Caps Lock" },
		{ VK_ESCAPE, "Escape" },
		{ VK_SPACE, "Space" },
		{ VK_PRIOR, "Page Up" },
		{ VK_NEXT, "Page Down" },
		{ VK_END, "End" },
		{ VK_HOME, "Home" },
		{ VK_LEFT, "Left Key" },
		{ VK_UP, "Up Key" },
		{ VK_RIGHT, "Right Key" },
		{ VK_DOWN, "Down Key" },
		{ VK_SELECT, "Select" },
		{ VK_PRINT, "Print Screen" },
		{ VK_INSERT, "Insert" },
		{ VK_DELETE, "Delete" },
		{ VK_HELP, "Help" },
		{ VK_SLEEP, "Sleep" },
		{ VK_MULTIPLY, "*" },
		{ VK_ADD, "+" },
		{ VK_SUBTRACT, "-" },
		{ VK_DECIMAL, "." },
		{ VK_DIVIDE, "/" },
		{ VK_NUMLOCK, "Num Lock" },
		{ VK_SCROLL, "Scroll" },
		{ VK_LSHIFT, "Left Shift" },
		{ VK_RSHIFT, "Right Shift" },
		{ VK_LCONTROL, "Left Ctrl" },
		{ VK_RCONTROL, "Right Ctrl" },
		{ VK_LMENU, "Left Alt" },
		{ VK_RMENU, "Right Alt" },
	};

	if (id >= 0x30 && id <= 0x5A)
		return std::string(1, (char)id);

	if (id >= 0x60 && id <= 0x69)
		return "Num " + std::to_string(id - 0x60);

	if (id >= 0x70 && id <= 0x87)
		return "F" + std::to_string((id - 0x70) + 1);

	return key_names[id];
}

void c_menu::set_position(int x, int y, int cx, int cy, HWND hwnd)
{
	POINT point; GetCursorPos(&point);

	auto flags = SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE;
	if (x != 0 && y != 0)
	{
		x = point.x - x;
		y = point.y - y;
		flags &= ~SWP_NOMOVE;
	}

	if (cx != 0 && cy != 0)
		flags &= ~SWP_NOSIZE;

	SetWindowPos(hwnd, nullptr, x, y, cx, cy, flags);
}

void c_menu::get_mouse_offset(int& x, int& y, HWND hwnd)
{
	POINT point; RECT rect;

	GetCursorPos(&point);
	GetWindowRect(hwnd, &rect);

	x = point.x - rect.left;
	y = point.y - rect.top;
}

// Taken from: https://github.com/ocornut/imgui/blob/master/examples/example_win32_directx9/main.cpp
LRESULT WINAPI c_menu::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_SIZE:
		if (directx9::device != NULL && wParam != SIZE_MINIMIZED)
		{
			directx9::param.BackBufferWidth = LOWORD(lParam);
			directx9::param.BackBufferHeight = HIWORD(lParam);
			ResetDevice();
		}
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU)
			return 0;
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

bool c_menu::CreateDeviceD3D(HWND hWnd)
{
	if ((directx9::context = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
		return false;

	// Create the D3DDevice
	ZeroMemory(&directx9::param, sizeof(directx9::param));
	{
		directx9::param.Windowed = TRUE;
		directx9::param.SwapEffect = D3DSWAPEFFECT_DISCARD;
		directx9::param.BackBufferFormat = D3DFMT_UNKNOWN; // Need to use an explicit format with alpha if needing per-pixel alpha composition.
		directx9::param.EnableAutoDepthStencil = TRUE;
		directx9::param.AutoDepthStencilFormat = D3DFMT_D16;
		directx9::param.PresentationInterval = D3DPRESENT_INTERVAL_ONE; // Present with vsync
	}

	if (directx9::context->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &directx9::param, &directx9::device) < 0)
		return false;

	return true;
}

void c_menu::CleanupDeviceD3D()
{
	if (directx9::device) { directx9::device->Release(); directx9::device = NULL; }
	if (directx9::context) { directx9::context->Release(); directx9::context = NULL; }
}

void c_menu::ResetDevice()
{
	ImGui_ImplDX9_InvalidateDeviceObjects();

	HRESULT hr = directx9::device->Reset(&directx9::param);
	if (hr == D3DERR_INVALIDCALL)
		IM_ASSERT(0);

	ImGui_ImplDX9_CreateDeviceObjects();
}

void c_menu::destroy()
{
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	CleanupDeviceD3D();
	DestroyWindow(ctx::hWnd);
	UnregisterClass(ctx::wc.lpszClassName, ctx::wc.hInstance);
}

void c_menu::render()
{
	const auto clear_color = ImVec4(0.09f, 0.09f, 0.09f, 0.94f);

	auto done = false;
	while (!done)
	{
		MSG msg;
		while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
				done = true;
		}

		if (done)
			break;

		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		auto* colors = ctx::imgui_style->Colors;
		colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.11f, 0.11f, 0.11f, 0.94f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.11f, 0.11f, 0.11f, 0.94f);
		colors[ImGuiCol_Border] = ImVec4(0.21f, 0.21f, 0.21f, 1.0f);  // Borde oscuro
		colors[ImGuiCol_FrameBg] = ImVec4(0.15f, 0.15f, 0.15f, 0.54f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.25f, 0.25f, 0.25f, 0.50f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.26f, 0.26f, 0.54f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.11f, 0.11f, 0.11f, 0.94f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.6f, 0.6f, 0.6f, 0.8f);  // Ajusta seg�n sea necesario
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.5f, 0.5f, 0.5f, 0.4f);  // Ajusta seg�n sea necesario
		colors[ImGuiCol_CheckMark] = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);  // Ajusta seg�n sea necesario
		colors[ImGuiCol_SliderGrab] = ImVec4(0.8f, 0.8f, 0.8f, 0.3f);  // Ajusta seg�n sea necesario
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
		colors[ImGuiCol_Button] = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.0f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.35f, 0.35f, 0.35f, 1.0f);
		colors[ImGuiCol_Header] = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.0f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.35f, 0.35f, 0.35f, 1.0f);
		colors[ImGuiCol_Separator] = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
		colors[ImGuiCol_Tab] = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
		colors[ImGuiCol_TabHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.0f);
		colors[ImGuiCol_TabActive] = ImVec4(0.35f, 0.35f, 0.35f, 0.50f);

		on_paint();

		ImGui::EndFrame();

		directx9::device->SetRenderState(D3DRS_ZENABLE, FALSE);
		directx9::device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		directx9::device->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);

		D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x * clear_color.w * 255.0f), (int)(clear_color.y * clear_color.w * 255.0f), (int)(clear_color.z * clear_color.w * 255.0f), (int)(clear_color.w * 255.0f));
		directx9::device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);

		if (directx9::device->BeginScene() >= 0)
		{
			ImGui::Render();
			ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
			directx9::device->EndScene();
		}

		HRESULT result = directx9::device->Present(NULL, NULL, NULL, NULL);
		if (result == D3DERR_DEVICELOST && directx9::device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
			ResetDevice();

	}

	destroy();
}

bool c_menu::setup()
{
	ImGui_ImplWin32_EnableDpiAwareness();
	ctx::wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, L"Class", NULL };
	RegisterClassEx(&ctx::wc);

	ctx::hWnd = CreateWindow(ctx::wc.lpszClassName, L"", WS_POPUP, 100, 100, ctx::menu_width, ctx::menu_height, NULL, NULL, ctx::wc.hInstance, NULL);
	if (!CreateDeviceD3D(ctx::hWnd))
	{
		CleanupDeviceD3D();
		UnregisterClass(ctx::wc.lpszClassName, ctx::wc.hInstance);

		return false;
	}

	ShowWindow(ctx::hWnd, SW_SHOWDEFAULT);
	UpdateWindow(ctx::hWnd);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ctx::imgui_io = &ImGui::GetIO();
	ctx::imgui_style = &ImGui::GetStyle();

	ctx::imgui_io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	if (PWSTR fonts_path; SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Fonts, 0, nullptr, &fonts_path)))
	{
		const std::filesystem::path path{ fonts_path };
		CoTaskMemFree(fonts_path);

		ctx::imgui_io->Fonts->AddFontFromFileTTF((path / "SegoeUI.ttf").string().data(), 16.f, NULL, ctx::imgui_io->Fonts->GetGlyphRangesDefault());
	}

	static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
	ImFontConfig icons_config; icons_config.MergeMode = true; icons_config.PixelSnapH = true;

	ctx::imgui_io->Fonts->AddFontFromMemoryCompressedTTF(fontawesome_compressed_data, fontawesome_compressed_size, 10.f, &icons_config, icons_ranges);
	
	ctx::imgui_style->WindowRounding = 5.0f;
	ctx::imgui_style->ScrollbarSize = 10.0f;
	ctx::imgui_style->GrabRounding = 5.0f;
	ctx::imgui_style->GrabMinSize = 10.0f;
	ctx::imgui_style->FrameRounding = 3.0f;
	ctx::imgui_style->TabRounding = 3.0f;

	ctx::imgui_io->IniFilename = nullptr;

	ImGui_ImplWin32_Init(ctx::hWnd);
	ImGui_ImplDX9_Init(directx9::device);

	return true;
}
