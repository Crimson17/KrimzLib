#pragma once


namespace kl {
	class window {
	public:
		// Input
		kl::keys keys;
		kl::mouse mouse;

		// User functions
		std::function<void(void)> start = []() {};
		std::function<void(void)> update = []() {};
		std::function<void(void)> end = []() {};

		// Window creation
		void startNew(kl::ivec2 size, std::string name, bool resizeable = true, bool continuous = false, bool opengl = false) {
			// Converting window name to a wstring
			std::wstring wName = kl::convert::toWString(name);

			// Registering winapi window class
			registerWindowClass(wName);

			// Creating a window
			createWindow(size, wName, resizeable);

			// Setting up bitmap info
			setupBitmapInfo();

			// Setting up OpenGL context
			if (opengl) {
				setupOpenGL();
			}

			// Binding the mouse
			this->mouse.bind(hwnd);

			// Starting the update loops
			if (continuous) {
				start();
				while (IsWindow(hwnd)) {
					while (PeekMessageW(&wndMsg, hwnd, 0, 0, PM_REMOVE)) {
						handleMessage();
					}
					update();
				}
				end();
			}
			else {
				start();
				while (IsWindow(hwnd)) {
					GetMessageW(&wndMsg, hwnd, 0, 0);
					handleMessage();
					update();
				}
				end();
			}

			// Cleanup
			cleanup(wName, opengl);
		}
		~window() {
			stop();
		}
		void stop() {
			PostMessageW(hwnd, WM_CLOSE, 0, 0);
		}

		// Returns a handle to the window
		HWND getHWND() {
			return hwnd;
		}

		// Returns the window size
		kl::ivec2 getSize() {
			RECT clientArea;
			GetClientRect(hwnd, &clientArea);
			return kl::ivec2(clientArea.right - clientArea.left, clientArea.bottom - clientArea.top);
		}

		// Returns the center point of the frame
		kl::ivec2 getCenter() {
			return getSize() / 2;
		}

		// Sets the window title
		void setTitle(std::string data) {
			SetWindowTextA(hwnd, data.c_str());
		}

		// Sets the pixels of the window
		void drawImage(kl::image& toDraw, kl::ivec2 position = { 0, 0 }) {
			bmpInfo.bmiHeader.biWidth = toDraw.getWidth();
			bmpInfo.bmiHeader.biHeight = toDraw.getHeight();
			StretchDIBits(hdc, position.x, (toDraw.getHeight() - 1) + position.y, toDraw.getWidth(), -(int)toDraw.getHeight(), 0, 0, toDraw.getWidth(), toDraw.getHeight(), toDraw.pointer(), &bmpInfo, DIB_RGB_COLORS, SRCCOPY);
		}
		void drawImage(kl::image&& toDraw, kl::ivec2 position = { 0, 0 }) {
			drawImage(toDraw, position);
		}

		// Swaps the front and back buffers
		void swapFrames() {
			SwapBuffers(hdc);
		}

	private:
		// Winapi variables
		HINSTANCE hInstance = GetModuleHandleW(nullptr);
		HWND hwnd = nullptr;
		HDC hdc = nullptr;
		HGLRC hglrc = nullptr;
		BITMAPINFO bmpInfo = {};
		MSG wndMsg = {};

		// Registers a new window class
		void registerWindowClass(std::wstring name) {
			WNDCLASSEXW windowClass = {};
			windowClass.cbSize = sizeof(WNDCLASSEX);
			windowClass.style = CS_OWNDC;
			windowClass.lpfnWndProc = DefWindowProc;
			windowClass.cbClsExtra = 0;
			windowClass.cbWndExtra = 0;
			windowClass.hInstance = hInstance;
			windowClass.hIcon = nullptr;
			windowClass.hCursor = nullptr;
			windowClass.hbrBackground = nullptr;
			windowClass.lpszMenuName = nullptr;
			windowClass.lpszClassName = name.c_str();
			windowClass.hIconSm = nullptr;
			if (!RegisterClassExW(&windowClass)) {
				printf("Could not register a window class! Error code: %d\n", GetLastError());
				exit(69);
			}
		}

		// Creates a new window
		void createWindow(kl::ivec2 size, std::wstring name, bool resizeable) {
			DWORD windowStyle = resizeable ? WS_OVERLAPPEDWINDOW : (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX);
			RECT adjustedWindowSize = { 0, 0, (LONG)size.x, (LONG)size.y };
			AdjustWindowRect(&adjustedWindowSize, windowStyle, FALSE);
			size.x = (adjustedWindowSize.right - adjustedWindowSize.left);
			size.y = (adjustedWindowSize.bottom - adjustedWindowSize.top);
			hwnd = CreateWindowExW(0, name.c_str(), name.c_str(), windowStyle, (kl::constant::ints::screenWidth / 2 - size.x / 2), (kl::constant::ints::screenHeight / 2 - size.y / 2), size.x, size.y, nullptr, nullptr, hInstance, nullptr);
			if (!hwnd) {
				printf("Could not create a window! Error code: %d\n", GetLastError());
				exit(69);
			}
			ShowWindow(hwnd, SW_SHOW);
			hdc = GetDC(hwnd);
		}

		// Sets up the bitmap properties
		void setupBitmapInfo() {
			bmpInfo.bmiHeader.biSize = sizeof(bmpInfo.bmiHeader);
			bmpInfo.bmiHeader.biPlanes = 1;
			bmpInfo.bmiHeader.biBitCount = 24;
			bmpInfo.bmiHeader.biCompression = BI_RGB;
		}

		// Sets up OpenGL context
		void setupOpenGL() {
			PIXELFORMATDESCRIPTOR pfd = {
				sizeof(PIXELFORMATDESCRIPTOR),
				1,
				PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    // Flags
				PFD_TYPE_RGBA,        // The kind of framebuffer. RGBA or palette.
				32,                   // Colordepth of the framebuffer.
				0, 0, 0, 0, 0, 0,
				0,
				0,
				0,
				0, 0, 0, 0,
				24,                   // Number of bits for the depthbuffer
				8,                    // Number of bits for the stencilbuffer
				0,                    // Number of Aux buffers in the framebuffer.
				PFD_MAIN_PLANE,
				0,
				0, 0, 0
			};
			int pixelFormat = ChoosePixelFormat(hdc, &pfd);
			if (!pixelFormat) {
				printf("Error, could not choose the pixel format %d\n", pixelFormat);
				exit(69);
			}
			SetPixelFormat(hdc, pixelFormat, &pfd);
			hglrc = wglCreateContext(hdc);
			wglMakeCurrent(hdc, hglrc);
			gladLoadGL(); /* Loading modern opengl functions */
			RECT clientArea = {};
			GetClientRect(hwnd, &clientArea);
			glViewport(clientArea.left, clientArea.top, clientArea.right, clientArea.bottom);
		}

		// Handles the windows message
		void handleMessage() {
			switch (wndMsg.message) {
			case WM_KEYDOWN:
				this->keys.setKey(wndMsg.wParam, true);
				break;

			case WM_KEYUP:
				this->keys.setKey(wndMsg.wParam, false);
				break;

			case WM_LBUTTONDOWN:
				this->mouse.lmb = true;
				break;

			case WM_LBUTTONUP:
				this->mouse.lmb = false;
				break;

			case WM_MBUTTONDOWN:
				this->mouse.mmb = true;
				break;

			case WM_MBUTTONUP:
				this->mouse.mmb = false;
				break;

			case WM_RBUTTONDOWN:
				this->mouse.rmb = true;
				break;

			case WM_RBUTTONUP:
				this->mouse.rmb = false;
				break;

			case WM_MOUSEMOVE:
				this->mouse.position = kl::ivec2(GET_X_LPARAM(wndMsg.lParam), GET_Y_LPARAM(wndMsg.lParam));
				break;

			default:
				DispatchMessageW(&wndMsg);
				break;
			}
		}

		// Destroys the contexts
		void cleanup(std::wstring name, bool opengl) {
			// Destroying the opengl contexts
			if (opengl) {
				wglMakeCurrent(nullptr, nullptr);
				ReleaseDC(hwnd, hdc);
				wglDeleteContext(hglrc);
				hglrc = nullptr;
			}

			// Unregistering the window class
			UnregisterClassW(name.c_str(), hInstance);
			hdc = nullptr;
			hwnd = nullptr;
		}
	};
}