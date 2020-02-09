
#define MAX_THREADS 2
#include "guiAlexandrov/include.h"


#define PI 3.14159265359f
#include "geometry.cpp"
#include "ray_caster.cpp"


void up_side_dawn(Image& img)
{
	for (int y = 0; y < img.height / 2; y++)
		for (int x = 0; x < img.width; x++)
			std::swap(img.get_pixel(x, y), img.get_pixel(x, img.height - y - 1));
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE lool, LPSTR cmdLine, int show)
{
	al_init(hInst);

	Image screen(800, 600);

	Window window(L"ray tracer", 800, 600, DEF_STYLE, NULL, &screen, [](HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)->LRESULT
	{
		Window* window = (Window*)arguments.get(hwnd)[0];
		if (!window) return DefWindowProc(hwnd, msg, wParam, lParam);

		Image* buffer = (Image*)arguments.get(hwnd)[1];

		switch (msg)
		{
			case WM_SIZE:
			{
				window->canvas.resize(hwnd);
			}break;
			case WM_PAINT:
			{
				PAINTSTRUCT plug;
				BeginPaint(hwnd, &plug);
				
				draw_image(window->canvas, *buffer, 0.0f, 0.0f, 1.0f, 1.0f);
				window->render_canvas();

				EndPaint(hwnd, &plug);
			}break;
			case WM_CLOSE:
			{
				PostQuitMessage(0);
			}break;
		}


		return DefWindowProc(hwnd, msg, wParam, lParam);
	});



	// ray tracer
	Material      ivory(1.0, vec4f(0.6, 0.3, 0.1, 0.0), vec3f(0.4, 0.4, 0.3), 50.);
	Material      glass(1.5, vec4f(0.0, 0.5, 0.1, 0.8), vec3f(0.6, 0.7, 0.8), 125.);
	Material red_rubber(1.0, vec4f(0.9, 0.1, 0.0, 0.0), vec3f(0.3, 0.1, 0.1), 10.);
	Material     mirror(1.0, vec4f(0.0, 10.0, 0.8, 0.0), vec3f(1.0, 1.0, 1.0), 1425.);

	std::vector<Sphere> spheres;
	spheres.push_back(Sphere(vec3f(-3, 0, -16), 2, ivory));
	spheres.push_back(Sphere(vec3f(-1.0, -1.5, -12), 2, glass));
	spheres.push_back(Sphere(vec3f(1.5, -0.5, -18), 3, red_rubber));
	spheres.push_back(Sphere(vec3f(7, 5, -18), 4, mirror));

	std::vector<Light>  lights;
	lights.push_back(Light(vec3f(-20, 20, 20), 1.5));
	lights.push_back(Light(vec3f(30, 50, -25), 1.8));
	lights.push_back(Light(vec3f(30, 20, 30), 1.7));

	render(screen, spheres, lights);
	up_side_dawn(screen);

	Window::wait_msg_proc();
	return 0;
}