
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
	Sphere sphere(vec3f(-3, 0, -16), 2);
	render(screen, sphere);

	Window::wait_msg_proc();
	return 0;
}