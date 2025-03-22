#include "imgui.h"

// For creating the button on top of the tool that open a menu

class MenuEditorWindow
{
public:

	struct ImGuiMainData
	{
		bool ShowMainMenuBar = false;
		bool ShowStyleEditor = false;
	};

	static void ShowMainWindowMenuBar(ImGuiMainData* main_data);

private:

};
