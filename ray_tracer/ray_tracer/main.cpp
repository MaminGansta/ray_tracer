
#define MAX_THREADS 2
#include "guiAlexandrov/include.h"


#define PI 3.14159265359f
#include "geometry.cpp"
#include "ray_caster.cpp"


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
	Material      ivory(vec2f(0.6, 0.3), vec3f(0.4, 0.4, 0.3), 50.);
	Material red_rubber(vec2f(0.9, 0.1), vec3f(0.3, 0.1, 0.1), 10.);

	std::vector<Sphere> spheres;
	spheres.push_back(Sphere(vec3f(-3, 0, -16), 2, ivory));
	spheres.push_back(Sphere(vec3f(-1.0, -1.5, -12), 2, red_rubber));
	spheres.push_back(Sphere(vec3f(1.5, -0.5, -18), 3, red_rubber));
	spheres.push_back(Sphere(vec3f(7, 5, -18), 4, ivory));

	std::vector<Light>  lights;
	lights.push_back(Light(vec3f(-20, 20, 20), 1.5));
	lights.push_back(Light(vec3f(30, 50, -25), 1.8));
	lights.push_back(Light(vec3f(30, 20, 30), 1.7));

	render(screen, spheres, lights);

	Window::wait_msg_proc();
	return 0;
}