
#define DEF_STYLE (WS_OVERLAPPEDWINDOW | WS_VISIBLE)


// =========================================== CALLBACK ARGUMENTS =============================================================

struct args 
{
	void* vals[2];
	void* operator [] (int i) { return vals[i]; }
};

struct Arguments
{
	std::vector<std::pair<HWND, args>> buffer;

	void add(HWND hwnd, void* pwindow, void* parg)
	{
		buffer.push_back(std::make_pair(hwnd, args{ pwindow, parg }));
	}

	args get(HWND hwnd)
	{
		auto it = std::find_if(buffer.begin(), buffer.end(), [hwnd](std::pair<HWND, args> in) { return hwnd == in.first; });
		return it == buffer.end() ? args{NULL,NULL} : it->second;
	}

	void remove(HWND hwnd)
	{
		auto it = std::find_if(buffer.begin(), buffer.end(), [hwnd](std::pair<HWND, args> in) { return hwnd == in.first; });
		if (it != buffer.end())
			buffer.erase(it);
	}
};
Arguments arguments;


struct HWND_constainer
{
	int gen_id = 0;
	std::vector<std::pair<int, HWND>> handles;

	int add(HWND handle)
	{
		handles.push_back(std::make_pair(gen_id, handle));
		return gen_id++;
	}

	void remove(int id)
	{
		auto handle = std::find_if(handles.begin(), handles.end(), [id](std::pair<int, HWND> in) {return in.first == id; });
		arguments.remove(handle->second);
		DestroyWindow(handle->second);
		handles.erase(handle);
	}

	HWND operator [](int id)
	{
		auto res = std::find_if(handles.begin(), handles.end(), [id](std::pair<int, HWND> in) {return in.first == id; });

		if (res == handles.end())
			return NULL;

		return res->second;
	}

	int size() { return handles.size(); };
};

HWND_constainer handles;

// ========================================= WINDOW ========================================================

struct Window
{	
	int class_id;
	static int name_id;
	HDC hdc;
	Canvas canvas;

	Window() {}
	
	Window(
		std::wstring window_name,
		int width,
		int height,
		UINT style,
		HWND parent,
		void* arg_ptr,
		LRESULT(CALLBACK* callback)(HWND, UINT, WPARAM, LPARAM),
		int id = 0
	)
	{
		init(window_name, width, height, style, parent, arg_ptr, callback , id);
	}

	void init(
		std::wstring window_name,
		int width,
		int height,
		UINT style,
		HWND parent,
		void* arg_ptr,
		LRESULT(CALLBACK* callback)(HWND, UINT, WPARAM, LPARAM),
		int id = 0
	)
	{
		wchar_t class_name[16];
		swprintf_s(class_name, L"class_%d", name_id++);
		std::wstring name(class_name);

		WNDCLASSEX wc;
		wc.cbSize = sizeof(wc);
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = callback;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = hInst;
		wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
		wc.lpszMenuName = NULL;
		wc.lpszClassName = name.c_str();
		//  wc.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);
		wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

		if (!RegisterClassEx(&wc))
		{
			MessageBox(NULL, L"Cannot register class", L"Error", MB_OK);
			assert(false);
		}

		HWND handle = CreateWindow(wc.lpszClassName, window_name.c_str(), style, CW_USEDEFAULT, CW_USEDEFAULT, width, height, parent, (HMENU)id, (HINSTANCE)hInst, NULL);

		arguments.add(handle, this, arg_ptr);

		SendMessage(handle, WM_CREATE, 0, 0);
		SendMessage(handle, WM_SIZE, 0, 0);

		class_id = handles.add(handle);
		hdc = GetDC(handle);
	}

	virtual ~Window() { handles.remove(class_id); }

	void render_canvas()
	{
		StretchDIBits(hdc, 0, 0, canvas.width, canvas.height, 0, 0, canvas.width, canvas.height, canvas.memory, &canvas.bitmap_info, DIB_RGB_COLORS, SRCCOPY);
	}

	HWND getHWND() { return handles[class_id]; }

	static void default_msg_proc() {
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	static void wait_msg_proc() {
		MSG msg;
		while (GetMessage(&msg, NULL, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
};

int Window::name_id = 0;



// ================================== WINDOW COMPONENTS ====================================================

struct Component
{
	int x, y, winth, height;
	int old_parent_w, old_parent_h;
	HWND handle;
};

struct Button
{
	HWND handle;

	Button() = default;
	Button(
		const WCHAR* button_name,
		HWND parent,
		int id,
		int x = 10,
		int y = 10,
		int width = 100,
		int height = 20,
		UINT style = WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON
	)
	{
		init(button_name, parent, id, x, y, width, height, style);
	}

	void init(
		const WCHAR* button_name,
		HWND parent,
		int id,
		int x = 10,
		int y = 10,
		int width = 100,
		int height = 20,
		UINT style = WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON
		)
	{
		handle = CreateWindow(
			L"BUTTON",  // Predefined class; Unicode assumed 
			button_name, // Button text 
			style,     // Styles 
			x,         // x position 
			y,         // y position 
			width,     // Button width
			height,    // Button height
			parent,    // Parent window
			(HMENU)id, // menu.
			hInst,
			NULL);      // Pointer not needed.
	}

	~Button() { DestroyWindow(handle); };
};


struct RadioButton
{
	HWND handle;

	RadioButton() = default;
	RadioButton(HWND parent, std::wstring text, int id, int x = 100, int y = 100, int width = 100, int height = 20)
	{
		init(parent, text, id, x, y, width, height);
	}

	void init(HWND parent, std::wstring text, int id, int x = 100, int y = 100, int width = 100, int height = 20)
	{
		handle = CreateWindow(L"Button", text.c_str(),
			BS_AUTORADIOBUTTON | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE ,
			x, y, width, height, parent, (HMENU)id, hInst, NULL);
	}
};

struct CheckBox
{
	HWND handle;

	CheckBox() = default;
	CheckBox(HWND parent, std::wstring text, int id, int x = 100, int y = 100, int width = 100, int height = 20)
	{
		init(parent, text, id, x, y, width, height);
	}

	void init(HWND parent, std::wstring text, int id, int x = 100, int y = 100, int width = 100, int height = 10)
	{
		handle = CreateWindow(L"Button", text.c_str(),
			BS_AUTOCHECKBOX | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE ,
			x, y, width, height, parent, (HMENU)id, hInst, NULL);
	}

};

struct ComboBox
{
	HWND handle;

	ComboBox() = default;
	ComboBox(HWND parent, int id, int x = 100, int y = 100, int width = 200, int height = 200)
	{
		init(parent, id, x, y, width, height);
	}

	void init(HWND parent, int id, int x = 100, int y = 100, int width = 200, int height = 200)
	{
		handle = CreateWindow(L"ComboBox", TEXT("combo"),
			CBS_DROPDOWN | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
			x, y, width, height, parent, (HMENU)id, hInst, NULL);
	}

	void add(std::wstring element)
	{
		// Add string to combobox.
		SendMessage(handle, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)element.c_str());

		// Send the CB_SETCURSEL message to display an initial item 
		//  in the selection field  
		SendMessage(handle, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);

	}

	void add(std::vector<std::wstring> elements)
	{
		// Add strings to combobox.
		for (auto& element : elements)
			SendMessage(handle, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)element.c_str());

		SendMessage(handle, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);

	}

	void remove(int idx)
	{
		SendMessage(handle, CB_DELETESTRING, (WPARAM)0, (LPARAM)0);
	}

	void remove(std::wstring name)
	{
		int idx = SendMessage(handle, CB_FINDSTRING, (WPARAM)0, (LPARAM)0);
		SendMessage(handle, CB_DELETESTRING, (WPARAM)idx, (LPARAM)0);
	}

	void clear()
	{
		int count = SendMessage(handle, CB_GETCOUNT, (WPARAM)0, (LPARAM)0);
		for (int i = 0; i < count; i++)
			SendMessage(handle, CB_DELETESTRING, (WPARAM)0, (LPARAM)0);
	}


	// -----------------  STATIC ELEMENTS  --------------------

	static std::wstring choosed(LPARAM lParam)
	{
		int ItemIndex = SendMessage((HWND)lParam, (UINT)CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
		TCHAR  ListItem[256];
		(TCHAR)SendMessage((HWND)lParam, (UINT)CB_GETLBTEXT, (WPARAM)ItemIndex, (LPARAM)ListItem);
		return std::wstring(ListItem);
	}

	static void add(HWND handle, std::wstring element)
	{
		// Add string to combobox.
		SendMessage(handle, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)element.c_str());

		// Send the CB_SETCURSEL message to display an initial item 
		//  in the selection field  
		SendMessage(handle, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
	}

	static void add(LPARAM  handle, std::vector<std::wstring> elements)
	{
		// Add strings to combobox.
		for (auto& element : elements)
			SendMessage((HWND)handle, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)element.c_str());

		SendMessage((HWND)handle, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);

	}

	static void remove(LPARAM  handle, int idx)
	{
		SendMessage((HWND)handle, CB_DELETESTRING, (WPARAM)0, (LPARAM)0);
	}

	static void remove(LPARAM  handle, std::wstring name)
	{
		int idx = SendMessage((HWND)handle, CB_FINDSTRING, (WPARAM)0, (LPARAM)name.c_str());
		SendMessage((HWND)handle, CB_DELETESTRING, (WPARAM)idx, (LPARAM)0);
	}

	static void clear(LPARAM  handle)
	{
		int count = SendMessage((HWND)handle, CB_GETCOUNT, (WPARAM)0, (LPARAM)0);
		for (int i = 0; i < count; i++)
			SendMessage((HWND)handle, CB_DELETESTRING, (WPARAM)0, (LPARAM)0);
	}
};


struct Label
{
	HWND handle;

	Label() = default;
	Label(HWND parent, int id, std::wstring text, int x = 100, int y = 100, int width = 200, int height = 20)
	{
		init(parent, id, text, x, y, width, height);
	}

	void init(HWND parent, int id, std::wstring text, int x = 100, int y = 100, int width = 200, int height = 20)
	{
		handle = CreateWindow(L"static", L"label", WS_CHILD | WS_VISIBLE | SS_SUNKEN | SS_WORDELLIPSIS | SS_CENTER , x, y, width, height, parent, (HMENU)id, hInst, NULL);
		SetWindowText(handle, text.c_str());
	}

	void set_text(std::wstring text)
	{
		SetWindowText(handle, text.c_str());
	}
};


struct Text
{
	HWND handle;
	
	Text() = default;
	Text(HWND parent, int id, int x, int y, int width, int height)
	{
		init(parent, id, x, y, width, height);
	}

	void init(HWND parent, int id, int x, int y, int width, int height)
	{
		handle = CreateWindow(L"edit", L"", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL, x, y, width, height, parent, (HMENU)id , hInst, NULL);
	}
		
	~Text() { DestroyWindow(handle); }
};

