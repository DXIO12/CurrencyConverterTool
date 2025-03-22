// Dear ImGui: standalone example application for GLFW + OpenGL 3, using programmable pipeline
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)

// Learn about Dear ImGui:
// - FAQ                  https://dearimgui.com/faq
// - Getting Started      https://dearimgui.com/getting-started
// - Documentation        https://dearimgui.com/docs (same as your local docs/ folder).
// - Introduction, links and more at the top of imgui.cpp
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include <CurrencyConverter.h>
#include <ToolStyleEditor.h>

#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <glfw3.h> // Will drag system OpenGL headers

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

// This example can also compile and run with Emscripten! See 'Makefile.emscripten' for details.
#ifdef __EMSCRIPTEN__
#include "../libs/emscripten/emscripten_mainloop_stub.h"
#endif

static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

// Main code
int main(int, char**)
{
	float height = 600;
	float width = 500;

	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit())
		return 1;

	// Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
	// GL ES 2.0 + GLSL 100 (WebGL 1.0)
	const char* glsl_version = "#version 100";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(IMGUI_IMPL_OPENGL_ES3)
	// GL ES 3.0 + GLSL 300 es (WebGL 2.0)
	const char* glsl_version = "#version 300 es";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
	// GL 3.2 + GLSL 150
	const char* glsl_version = "#version 150";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
	// GL 3.0 + GLSL 130
	const char* glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

	// Create window with graphics context
	GLFWwindow* window = glfwCreateWindow(height, width, "Currency Converter", nullptr, nullptr);
	if (window == nullptr)
		return 1;
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1); // Enable vsync

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dark style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
#ifdef __EMSCRIPTEN__
	ImGui_ImplGlfw_InstallEmscriptenCallbacks(window, "#canvas");
#endif
	ImGui_ImplOpenGL3_Init(glsl_version);

	//The Currencies are fetch and they are listed
	CurrencyConverter::AvailableCurrencies fetchCurrencies;
	fetchCurrencies.fetchAvailableCurrencies();
	CurrencyConverter::Calculator converterCurrencies;

	//Declare the variable money that the user wants to convert
	static double moneyToConvert = 0.00000000f;

	// Stored data
	static MenuEditorWindow::ImGuiMainData main_data;
	
	/*This code is needed for transform std::vector<std::string> currenciesAvailable that is used
	to store the data extracted from the API into currenciesAvailable and then transform it into
	std::vector<const char*> currencyItems*/

	std::vector < const char*> currencyItems;
	for (const std::string& currency : fetchCurrencies.currenciesAvailable) {
		currencyItems.push_back(currency.c_str());  // Safe reference
	}

	// Main loop
#ifdef __EMSCRIPTEN__
	// For an Emscripten build we are disabling file-system access, so let's not attempt to do a fopen() of the imgui.ini file.
	// You may manually call LoadIniSettingsFromMemory() to load settings from your own storage.
	io.IniFilename = nullptr;
	EMSCRIPTEN_MAINLOOP_BEGIN
#else
	while (!glfwWindowShouldClose(window))
#endif
	{
		// Poll and handle events (inputs, window resize, etc.)
		// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
		// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
		// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
		// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
		glfwPollEvents();
		if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0)
		{
			ImGui_ImplGlfw_Sleep(10);
			continue;
		}

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// 1. Show the Currency Converter Window
		static float f = 0.0f;

		ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
		ImGui::SetNextWindowSize(ImVec2(height, width));

		ImGui::Begin("Currency Tool", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoBringToFrontOnFocus); // Create a window called "Hello, world!" and append into it.

		// Menu Bar
		MenuEditorWindow::ShowMainWindowMenuBar(&main_data);

		//Text for the Currency Converter
		ImGui::Text("Tool use for Currency Converter.");        // Display some text (you can use a format strings too)
		ImGui::NewLine();

		// Declare and inizialized a counter for doing the conversion
		unsigned short counterBase = 0;
		unsigned short counterTarget = 0;

		// Declare and inizialized the index of the vector of strings for the Base Currencies and the Target Currencies
		static int BaseCurrenciesSelectedIdx = 0;
		static int TargetCurrenciesSelectedIdx = 0;

		// Call the method that fetch all the currencies that are available in the API

		if (fetchCurrencies.currenciesAvailable.empty())
		{
			// When Currencies are Fetch correctly
			if (fmodf((float)ImGui::GetTime(), 0.40f) < 0.25f)
			{
				ImGui::Text("<<NO CURRENCIES AVAILABLE. FETCHING...>>");
			}
			else
			{
				ImGui::NewLine();
			}
			//fetchCurrencies.fetchAvailableCurrencies(); // Fetch if empty

		}
		else
		{

			// When Currencies are Fetch correctly
			if (fmodf((float)ImGui::GetTime(), 0.40f) < 0.25f)
			{
				ImGui::Text("<<CURRENCIES FETCH>>");
			}
			else
			{
				ImGui::NewLine();
			}

			const char* comboBaseCurrencies = currencyItems[BaseCurrenciesSelectedIdx];
			static ImGuiComboFlags flagsCombo = 0;

			ImGui::Text("Select a currency to be converted:");

			if (ImGui::BeginCombo("Base Currency Selector with filter", comboBaseCurrencies, ImGuiComboFlags_WidthFitPreview))
			{
				static ImGuiTextFilter filter;
				if (ImGui::IsWindowAppearing())
				{
					ImGui::SetKeyboardFocusHere();
					filter.Clear();
				}
				ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_F);
				filter.Draw("##Filter", -FLT_MIN);

				for (int n = 0; n < currencyItems.size(); n++)
				{
					const bool is_selected = (BaseCurrenciesSelectedIdx == n);
					if (filter.PassFilter(currencyItems[n]))
						if (ImGui::Selectable(currencyItems[n], is_selected)) 
						{
							BaseCurrenciesSelectedIdx = n;
							if (BaseCurrenciesSelectedIdx != 0)
							{
								CurrencyConverter::RT_ExchangeCurrency::getExchangeRate(currencyItems[BaseCurrenciesSelectedIdx], currencyItems[TargetCurrenciesSelectedIdx]);
								converterCurrencies.convert(currencyItems[BaseCurrenciesSelectedIdx], currencyItems[TargetCurrenciesSelectedIdx], moneyToConvert);
							}
						}						
				}



				ImGui::EndCombo();
			}
			
			const char* comboTargetCurrencies = currencyItems[TargetCurrenciesSelectedIdx];

			ImGui::Text("Select a currency to be converted into:");

			if (ImGui::BeginCombo("Target Currency Selector with filter", comboTargetCurrencies, ImGuiComboFlags_WidthFitPreview))
			{

				static ImGuiTextFilter filter;
				if (ImGui::IsWindowAppearing())
				{
					ImGui::SetKeyboardFocusHere();
					filter.Clear();
				}
				ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_F);
				filter.Draw("##Filter", -FLT_MIN);

				for (int n = 0; n < currencyItems.size(); n++)
				{
					const bool is_selected = (TargetCurrenciesSelectedIdx == n);
					if (filter.PassFilter(currencyItems[n]))
						if (ImGui::Selectable(currencyItems[n], is_selected)) 
						{
							TargetCurrenciesSelectedIdx = n;
							if (TargetCurrenciesSelectedIdx != 0)
							{
								CurrencyConverter::RT_ExchangeCurrency::getExchangeRate(currencyItems[BaseCurrenciesSelectedIdx], currencyItems[TargetCurrenciesSelectedIdx]);
								converterCurrencies.convert(currencyItems[BaseCurrenciesSelectedIdx], currencyItems[TargetCurrenciesSelectedIdx], moneyToConvert);
							}
						}
				}

				ImGui::EndCombo();

			}

			//
			ImGui::NewLine();

			ImGui::Text("How much you want to convert");

			//Change the size of the input 
			ImGui::PushItemWidth(260);
			ImGui::InputDouble(currencyItems[BaseCurrenciesSelectedIdx], &moneyToConvert, 0.01f, 1.0f, "%.2f", ImGuiInputTextFlags_CurrencyConverterTool);
			ImGui::PopItemWidth();

			if (ImGui::Button("Convert", ImVec2(200, 30)))
			{

				//
				CurrencyConverter::RT_ExchangeCurrency::getExchangeRate(currencyItems[BaseCurrenciesSelectedIdx], currencyItems[TargetCurrenciesSelectedIdx]);

				//
				converterCurrencies.convert(currencyItems[BaseCurrenciesSelectedIdx], currencyItems[TargetCurrenciesSelectedIdx], moneyToConvert);

				
			}

		}
	
		ImGui::NewLine();
		ImGui::Text("%f %s equals %f %s ", moneyToConvert, currencyItems[BaseCurrenciesSelectedIdx], converterCurrencies.resultConversion, currencyItems[TargetCurrenciesSelectedIdx]);
		ImGui::NewLine();
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
		ImGui::End();

		// Rendering
		ImGui::Render();
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClear(GL_COLOR_BUFFER_BIT);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	}
#ifdef __EMSCRIPTEN__
	EMSCRIPTEN_MAINLOOP_END;
#endif

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
