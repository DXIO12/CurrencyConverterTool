#include <ToolStyleEditor.h>

// Helper to wire demo markers located in code to an interactive browser
typedef void (*ImGuiToolStyleEditorMarkerCallback)(const char* file, int line, const char* section, void* user_data);
extern ImGuiToolStyleEditorMarkerCallback      GImGuiToolStyleEditorMarkerCallback;
extern void* GImGuiToolStyleEditorMarkerCallbackUserData;
ImGuiToolStyleEditorMarkerCallback             GImGuiToolStyleEditorMarkerCallback = NULL;
void* GImGuiToolStyleEditorMarkerCallbackUserData = NULL;
#define IMGUI_ToolStyleEditor_MARKER(section)  do { if (GImGuiToolStyleEditorMarkerCallback != NULL) GImGuiToolStyleEditorMarkerCallback(__FILE__, __LINE__, section, GImGuiToolStyleEditorMarkerCallbackUserData); } while (0)


void MenuEditorWindow::ShowMainWindowMenuBar(MenuEditorWindow::ImGuiMainData* main_data)
{
    if (ImGui::BeginMenuBar())
    {      
        if (ImGui::BeginMenu("Style Menu", true))
        {
            IMGUI_ToolStyleEditor_MARKER("Menu/Tools");
            ImGuiIO& io = ImGui::GetIO();

            ImGui::MenuItem("Style Editor", NULL, &main_data->ShowStyleEditor);

            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
    //Create the Style Window Editor
    if (main_data->ShowStyleEditor)
    {
        ImGui::Begin("Currency Converter Tool Style Editor", &main_data->ShowStyleEditor);
        ImGui::ShowStyleEditor();
        ImGui::End();
    }
}
