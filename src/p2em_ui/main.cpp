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

using namespace ultralight;

class P2EMonUI : public WindowListener {
	RefPtr<App> app_;
	RefPtr<Window> window_;
	RefPtr<Overlay> overlay_;

public:
	P2EMonUI() {
		///
		/// Create our main App instance.
		///
		app_ = App::Create();

		///
		/// Configure our FileSystem and root directory
		///
		file_system = CreatePlatformFileSystem("E:\\David\\Personal Documents\\git\\p2e_mons\\res\\");
		Platform::instance().set_file_system(file_system);

		///
		/// Create our Window with the Resizable window flag.
		///
		window_ = Window::Create(app_->main_monitor(), 300, 300, false,
			kWindowFlags_Titled | kWindowFlags_Resizable);

		///
		/// Set our window title.
		///
		window_->SetTitle("Tutorial 3 - Resize Me!");

		///
		/// Bind our App's main window.
		///
		/// @note This MUST be done before creating any overlays or calling App::Run
		///
		app_->set_window(*window_.get());

		///
		/// Create our Overlay, use the same dimensions as our Window.
		///
		overlay_ = Overlay::Create(*window_.get(), window_->width(),
			window_->height(), 0, 0);

		///
		/// Load a string of HTML into our overlay's View
		///
		overlay_->view()->LoadURL("file:///p2em_ui/html/main.html");

		///
		/// Register our MyApp instance as a WindowListener so we can handle the
		/// Window's OnResize event below.
		///
		window_->set_listener(this);
	}

	virtual ~P2EMonUI() {}

	///
	/// Inherited from WindowListener, not used.
	///
	virtual void OnClose() {}

	///
	/// Inherited from WindowListener, called when the Window is resized.
	///
	virtual void OnResize(int width, int height) {
		///
		/// Resize our Overlay to match the new Window dimensions.
		///
		overlay_->Resize(width, height);
	}

	void Run() {
		app_->Run();
	}
};

int main() {
	P2EMonUI app;
	app.Run();

	return 0;
}
