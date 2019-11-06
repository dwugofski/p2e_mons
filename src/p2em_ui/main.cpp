#include <AppCore/App.h>
#include <AppCore/Window.h>
#include <AppCore/Overlay.h>
#include <string>
#include <Windows.h>
#include "filesystem.h"
#include <Ultralight/String.h>
#include <shlwapi.h>
#include <Ultralight/platform/Platform.h>

#pragma comment(lib, "Shlwapi.lib")

ultralight::FileSystem* file_system = nullptr;

namespace framework {

	ultralight::FileSystem* CreatePlatformFileSystem(const char* baseDir) {
		std::string baseDirStr(baseDir);
		std::wstring baseDirWStr(baseDirStr.begin(), baseDirStr.end());

		WCHAR cur_dir[_MAX_PATH];
		GetCurrentDirectoryW(_MAX_PATH, cur_dir);
		WCHAR absolute_dir[_MAX_PATH];
		PathCombineW(absolute_dir, cur_dir, baseDirWStr.c_str());
		//PathCchCombine(absolute_dir, _MAX_PATH, cur_dir, baseDirWStr.c_str());

		return new ultralight::FileSystemWin(absolute_dir);
	}

}

using namespace ultralight;

int main() {

	///
	/// Create our main App instance.
	///
	auto app = App::Create();

	file_system = framework::CreatePlatformFileSystem("E:\\David\\Personal Documents\\git\\p2e_mons\\res\\p2em_ui\\");
	auto& platform = Platform::instance();
	platform.set_file_system(file_system);
	file_system = Platform::instance().file_system();
	ultralight::String16 path("html\\main.html");
	bool doesexist = file_system->FileExists(path);

	///
	/// Create our Window using default window flags.
	///
	auto window = Window::Create(app->main_monitor(), 300, 300, false, kWindowFlags_Titled);

	///
	/// Set our window title.
	///
	window->SetTitle("Tutorial 2 - Basic App");

	///
	/// Bind our App's main window.
	///
	/// @note This MUST be done before creating any overlays or calling App::Run
	///
	app->set_window(window);

	///
	/// Create our Overlay, use the same dimensions as our Window.
	///
	auto overlay = Overlay::Create(window, window->width(), window->height(), 0, 0);

	///
	/// Load a string of HTML into our overlay's View
	///
	ultralight::String url = "file:///html/main.html";
	ultralight::Ref<ultralight::View> view = overlay->view();
	view->LoadURL(url);

	///
	/// Run our main loop.
	///
	app->Run();

	return 0;
}
