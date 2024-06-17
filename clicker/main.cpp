
/*

WARNING! this part has been skidded from https://github.com/b1scoito/clicker
anyway, most of the things are not used by the exhaustclicker

credits: b1scoito

*/

#include "pch.hpp"
#include "burguer.hpp"
#include "clicker.hpp"
#include "menu.hpp"
#include <xorstr.hpp>

// random name generator
std::string generateRandomName() {
	std::string name;
	srand(time(0));

	for (int i = 0; i < 10; ++i) {
		if (i % 2 == 0) {
			char c = 'a' + rand() % 26;
			name += c;
		}
		else {
			char c = 'A' + rand() % 26;
			name += c;
		}
	}
	return name;
}


// change console color
void setcolor(unsigned short color)
{
	HANDLE hcon = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hcon, color);
}

// animate message printer
void animateMessage(const std::string& message, int delay) {
	for (char c : message) {
		std::cout << c << std::flush;
		Sleep(delay);
	}
	std::cout << std::endl;
}


INT WINAPI WinMain( _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd )
{
	// show the console
	if (AllocConsole()) {
		FILE* fDummy;
		freopen_s(&fDummy, "CONOUT$", "w", stdout);
		freopen_s(&fDummy, "CONOUT$", "w", stderr);
		freopen_s(&fDummy, "CONIN$", "r", stdin);
	}

	// console size
	HWND hWnd = GetConsoleWindow();
	SetWindowPos(hWnd, NULL, 0, 0, 524, 298, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);

	// change console title
	std::string consoleTitle = generateRandomName();
	SetConsoleTitleA(consoleTitle.c_str());

	// console position
	RECT r;
	GetWindowRect(hWnd, &r);
	MoveWindow(hWnd, r.left, r.top, 524, 298, TRUE);

	setcolor(4);
	//============= debuggers checks(poor bypass) =//
	std::string initmsg = " [+] Checking for sandbox/debuggers!";

	animateMessage(initmsg, 5);

	SecurityChecks::burguer();

	//============= debuggers checks(poor bypass) =//

	// if the debugger check is 0, close the consola and open the clicker
	FreeConsole();
	if (hWnd != NULL) {
		PostMessage(hWnd, WM_CLOSE, 0, 0);
	}


	std::atexit( [] { 
		// dispose
		vars::b_is_running = false;
	} );

	// set random seed
	std::srand( (std::uint32_t) std::time( NULL ) );

	SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);

	// initialize clicker threads
	std::thread(&c_clicker::thread, g_clicker.get()).detach();

	// setup menu	
	if ( !g_menu->setup() )
	{
		g_menu->destroy();

		return EXIT_FAILURE;
	}
	
	// Menu renderer loop
	g_menu->render();

	return EXIT_SUCCESS;
}