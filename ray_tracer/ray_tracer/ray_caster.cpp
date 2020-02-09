
#include <limits>

struct Material {
    Color diffuse_color;
    Material() = default;
    Material(const Color& color) : diffuse_color(color) {}
};

struct Sphere
{
    vec3f center;
    float radius;
    Material material;

    Sphere(vec3f center, float radius, Material material) : center(center), radius(radius), material(material) {}

    bool ray_intersect(const vec3f& orig, const vec3f& dir, float& t0) const
    {
        vec3f L = center - orig;
        float tca = L * dir;
        float d2 = L * L - tca * tca;
        if (d2 > radius* radius) return false;
        float thc = sqrtf(radius * radius - d2);
        t0 = tca - thc;
        float t1 = tca + thc;
        if (t0 < 0) t0 = t1;
        if (t0 < 0) return false;
        return true;
    }

};


bool scene_intersect(const vec3f& orig, const vec3f& dir, const std::vector<Sphere>& spheres, vec3f& hit, vec3f& N, Material& material) {
    float spheres_dist = (std::numeric_limits<float>::max)();

    for (size_t i = 0; i < spheres.size(); i++)
    {
        float dist_i;
        if (spheres[i].ray_intersect(orig, dir, dist_i) && dist_i < spheres_dist) {
            spheres_dist = dist_i;
            hit = orig + dir * dist_i;
            N = (hit - spheres[i].center).normalize();
            material = spheres[i].material;
        }
    }
    return spheres_dist < 1000;
}


Color cast_ray(const vec3f& orig, const vec3f& dir, std::vector<Sphere>& spheres) {
    vec3f point, N;
    Material material;
    
    if (!scene_intersect(orig, dir, spheres, point, N, material))
        return Color(60, 180, 210); // background color

    return material.diffuse_color;
}


void render(Image& surface, std::vector<Sphere>& spheres) {
    const int width = surface.width;
    const int height = surface.height;
    const int fov = PI / 2.0f;

#pragma omp parallel for
    for (size_t j = 0; j < height; j++) {
        for (size_t i = 0; i < width; i++) {
            float x = (2 * (i + 0.5f) / (float)width - 1.0f) * tan(fov / 2.0f) * width / (float)height;
            float y = -(2 * (j + 0.5f) / (float)height - 1.0f) * tan(fov / 2.0f);
            vec3f dir = vec3f(x, y, -1).normalize();
            surface[i + j * width] = cast_ray(vec3f(0, 0, 0), dir, spheres);
        }
    }
}