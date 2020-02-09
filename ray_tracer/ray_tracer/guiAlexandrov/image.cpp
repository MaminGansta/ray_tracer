
// ============= standart image ==================

struct Image
{
	int height, width;
	Color* data = NULL;
	bool invalid = false;

	Image() = default;
	Image(const wchar_t* filename_utf8)
	{
		int chanels;
		char filename[256];
		stbi_convert_wchar_to_utf8(filename, sizeof(filename), filename_utf8);

		uint8_t* raw = stbi_load(filename, &width, &height, &chanels, 0);

		if (raw == NULL)
		{
			invalid = true;
			return;
		}
		
		data = new Color[width * height];
		
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				int pos = (y * width + x) * chanels;
				data[(height - y - 1) * width + x] = Color(raw[pos], raw[pos + 1], raw[pos +2]);
			}
		}

		stbi_image_free(raw);
	}

	Image(int width, int height) : width(width), height(height)
	{
		data = new Color[width * height];
		invalid = false;
	}

	Image(Image& copy)
	{
		height = copy.height;
		width = copy.width;
		data = new Color[height * width];
		memmove(data, copy.data, sizeof(Color) * height * width);
	}

	Image(Image&& other)
	{
		data = other.data;
		width = other.width;
		height = other.height;
		other.data = NULL;
		other.invalid = true;
	}

	Image& operator = (Image&& other)
	{
		delete[] data;
		data = other.data;
		width = other.width;
		height = other.height;
		other.data = NULL;
		other.invalid = true;
		return *this;
	}


	Color& get_pixel(int x, int y)
	{
		assert(((uint32_t)y < height) | ((uint32_t)x < width));
		return data[y * width + x];
	}
	
	Color& operator [] (int idx)
	{
		assert((uint32_t)idx < width * height);
		return data[idx];
	}

	Color& get_pixel_scaled(int x, int y, int screen_w, int screen_h)
	{
		y = y * height / screen_h;
		x = x * width / screen_w;
	
		assert(((uint32_t)y < height) | ((uint32_t)x < width));
		return data[y * width + x];
	}

	~Image() { delete[] data; }
};


void draw_image(Canvas& surface, Image& image,
				float fpos_x, float fpos_y, float fwidth, float fheight)
{
	if (fpos_x > 1.0f || fpos_y > 1.0f || fpos_x < 0.0f || fpos_y < 0.0f) return;

	int pos_x = surface.width * fpos_x;
	int pos_y = surface.height *fpos_y;

	fwidth = min(fwidth, 1.0f - fpos_x);
	fheight= min(fheight, 1.0f - fpos_y);

	int width  = surface.width * fwidth;
	int height = surface.height * fheight;

	std::future<void> res[MAX_THREADS];

	for (int i = 0; i < workers.size; i++)
	{
		int from_x = i * width / workers.size;
		int to_x   = (i + 1) * width / workers.size;

		res[i] = workers.add_task([from_x, to_x, pos_y, pos_x, height, width, &surface, &image]()
		{
			for (int y = 0; y < height; y++)
				for (int x = from_x; x < to_x; x++)
				{
					assert(x < surface.width);
					Color color = image.get_pixel_scaled(x, y, width, height);
					surface.memory[(y + pos_y) * surface.width + (x + pos_x)] = color;
				}
		});
	}
	
	for (int i = 0; i < workers.size; i++)
		res[i].get();
}


// =============== float Image  ==================


struct fColor
{
	union
	{
		struct { float b, g, r, a; };
		float raw[4];
	};

	inline fColor() = default;
	inline fColor(float r, float g, float b, float a = 1.0f) : r(r), g(g), b(b), a(a) {}
	inline fColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) : r(float(r) / 255.0f), g(float(g) / 255.0f), b(float(b) / 255.0f), a(float(a) / 255.0f) {}
	inline fColor(float color) : r(color), g(color), b(color), a(1.0f) {}


	fColor operator *(float f)
	{
		return fColor(r * f, g * f, b * f);
	}

	fColor operator *= (float f)
	{
		r *= f;
		g *= f;
		b *= f;
	}

	Color get_uint()
	{
		return Color(255.0f * r, 255.0f * g, 255.0f * b, 255.0f * a);
	}
};


struct fImage
{
	int height, width;
	fColor* data = NULL;
	bool invalid = false;

	fImage() = default;
	fImage(const wchar_t* filename_utf8)
	{
		int chanels;
		char filename[256];
		stbi_convert_wchar_to_utf8(filename, sizeof(filename), filename_utf8);

		uint8_t* raw = stbi_load(filename, &width, &height, &chanels, 0);

		if (raw == NULL)
		{
			invalid = true;
			return;
		}

		data = new fColor[width * height];

		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				int pos = (y * width + x) * chanels;
				data[(height - y - 1) * width + x] = fColor(raw[pos], raw[pos + 1], raw[pos + 2]);
			}
		}

		stbi_image_free(raw);
	}

	fImage(int width, int height) : width(width), height(height)
	{
		data = new fColor[width * height];
		invalid = false;
	}

	fImage(fImage & copy)
	{
		height = copy.height;
		width = copy.width;
		data = new fColor[height * width];
		memmove(data, copy.data, sizeof(fColor) * height * width);
	}

	fImage(fImage && other)
	{
		data = other.data;
		width = other.width;
		height = other.height;
		other.data = NULL;
		other.invalid = true;
	}
	
	fImage& operator = (fImage&& other)
	{
		delete[] data;
		data = other.data;
		width = other.width;
		height = other.height;
		other.data = NULL;
		other.invalid = true;
		return *this;
	}

	~fImage() { delete[] data; }


	void resize(int width, int height)
	{
		this->width = width;
		this->height = height;
		delete[] data;
		data = new fColor[width * height];
		invalid = false;
	}

	fColor& get_pixel(int x, int y)
	{
		assert(((uint32_t)y < height) | ((uint32_t)x < width));
		return data[y * width + x];
	}

	fColor& operator [] (int idx)
	{
		assert((uint32_t)idx < width * height);
		return data[idx];
	}

	fColor& get_pixel_scaled(int x, int y, int screen_w, int screen_h)
	{
		y = y * height / screen_h;
		x = x * width / screen_w;

		assert(((uint32_t)y < height) | ((uint32_t)x < width));
		return data[y * width + x];
	}

};


void draw_image(Canvas& surface, fImage& image,
	float fpos_x, float fpos_y, float fwidth, float fheight)
{
	if (fpos_x > 1.0f || fpos_y > 1.0f || fpos_x < 0.0f || fpos_y < 0.0f) return;

	int pos_x = surface.width * fpos_x;
	int pos_y = surface.height * fpos_y;

	fwidth = min(fwidth, 1.0f - fpos_x);
	fheight = min(fheight, 1.0f - fpos_y);

	int width = surface.width * fwidth;
	int height = surface.height * fheight;

	std::future<void> res[MAX_THREADS];

	for (int i = 0; i < workers.size; i++)
	{
		int from_x = i * width / workers.size;
		int to_x = (i + 1) * width / workers.size;

		res[i] = workers.add_task([from_x, to_x, pos_y, pos_x, height, width, &surface, &image]()
		{
			for (int y = 0; y < height; y++)
				for (int x = from_x; x < to_x; x++)
				{
					assert(x < surface.width);
					Color color = image.get_pixel_scaled(x, y, width, height).get_uint();
					surface.memory[(y + pos_y) * surface.width + (x + pos_x)] = color;
				}
		});
	}

	for (int i = 0; i < workers.size; i++)
		res[i].get();
}