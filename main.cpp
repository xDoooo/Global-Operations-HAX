#include "Menu_ImGui.h"
#include "game_logic.h"

#include <thread>

int __stdcall wWinMain(
	HINSTANCE instance,
	HINSTANCE previousInstance,
	PWSTR arguments,
	int commandShow)
{
	// create gui
	gui::CreateHWindow("Global Operations-HAX", "Global Operations-HAX_Class");
	gui::CreateDevice();
	gui::CreateImGui();

	//run cheat
	std::thread the_cheat(game_logic::TheCheat);

	while (gui::isRunning)
	{
		gui::BeginRender();
		gui::Render();
		gui::EndRender();

		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}

	// Destroy gui
	gui::DestroyImGui();
	gui::DestroyDevice();
	gui::DestroyHWindow();

	// Close process handle
	CloseHandle(game_logic::Global_Operations_Process);

	// Close threads
	game_logic::stopThread0 = false;
	if (the_cheat.joinable()) {
		the_cheat.join();

	}


	return EXIT_SUCCESS;
}
