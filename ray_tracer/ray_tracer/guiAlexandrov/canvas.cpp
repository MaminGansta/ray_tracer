
struct Color
{
	union
	{
		struct { uint8_t b, g, r, a;};
		uint8_t raw[4];
		uint32_t whole;
	};

	inline Color() = default;
	inline Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) : r(r), g(g), b(b), a(a) {}
	inline Color(uint8_t color) : r(color), g(color), b(color), a(255) {}


	Color operator *(float f)
	{
		return Color(r * f, g * f, b * f);
	}

	Color operator *=(float f)
	{
		r *= f;
		g *= f;
		b *= f;
	}
};


struct Canvas
{
	int height, width;
	int whole_size;
	Color* memory = nullptr;

	BITMAPINFO bitmap_info;

	~Canvas() { delete[] memory; }

	void resize(HWND hwnd)
	{
		RECT rect;
		GetClientRect(hwnd, &rect);

		width = rect.right - rect.left;
		height = rect.bottom - rect.top;

		whole_size = width * height;
		int size = whole_size * sizeof(unsigned int);

		delete[] memory;
		memory = new Color[size];

		bitmap_info.bmiHeader.biSize = sizeof(bitmap_info.bmiHeader);
		bitmap_info.bmiHeader.biWidth = width;
		bitmap_info.bmiHeader.biHeight = height;
		bitmap_info.bmiHeader.biPlanes = 1;
		bitmap_info.bmiHeader.biBitCount = 32;
		bitmap_info.bmiHeader.biCompression = BI_RGB;
	}

	Color& operator [] (int inx)
	{
		assert((uint32_t)inx < whole_size);
		return memory[inx];
	}

};