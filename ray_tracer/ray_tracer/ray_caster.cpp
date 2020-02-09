
#include <limits>

Color cast_ray(const vec3f& orig, const vec3f& dir, const Sphere& sphere) {
    float sphere_dist = (std::numeric_limits<float>::max)();
    if (!sphere.ray_intersect(orig, dir, sphere_dist)) {
        return Color(50, 180, 240); // background color
    }

    return Color(100, 100, 80);
}


void render(Image& surface, const Sphere& sphere) {
    const int width = surface.width;
    const int height = surface.height;
    const int fov = PI / 2.0f;

#pragma omp parallel for
    for (size_t j = 0; j < height; j++) {
        for (size_t i = 0; i < width; i++) {
            float x = (2 * (i + 0.5) / (float)width - 1.0f) * tan(fov / 2.0f) * width / (float)height;
            float y = -(2 * (j + 0.5) / (float)height - 1.0f) * tan(fov / 2.0f);
            vec3f dir = vec3f(x, y, -1).normalize();
            surface[i + j * width] = cast_ray(vec3f(0, 0, 0), dir, sphere);
        }
    }
}