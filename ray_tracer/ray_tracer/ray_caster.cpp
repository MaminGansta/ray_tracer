
#include <limits>


Color vec_color(vec3f vec)
{
    return Color(min(255.0f * vec.x, 255.0f), min(255.0f, 255.0f * vec.y), min(255.0f, 255.0f * vec.y));
}

struct Light
{
    vec3f position;
    float intensity;
    Light(const vec3f& p, const float& i) : position(p), intensity(i) {}
};

struct Material {
    Material(const vec2f& a, const vec3f& color, const float& spec) : albedo(a), diffuse_color(color), specular_exponent(spec) {}
    Material() : albedo(1, 0), diffuse_color(), specular_exponent() {}
    vec2f albedo;
    vec3f diffuse_color;
    float specular_exponent;
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

vec3f reflect(vec3f I, vec3f& N) {
    return I - N * 2.f * (I * N);
}

Color cast_ray(const vec3f& orig, const vec3f& dir, std::vector<Sphere>& spheres, const std::vector<Light>& lights) {
    vec3f point, N;
    Material material;
    
    if (!scene_intersect(orig, dir, spheres, point, N, material))
        return Color(60, 180, 210); // background color

    float diffuse_light_intensity = 0, specular_light_intensity = 0;
    for (size_t i = 0; i < lights.size(); i++) {
        vec3f light_dir = (lights[i].position - point).normalize();
        float light_distance = (lights[i].position - point).norm();

        vec3f shadow_orig = light_dir * N < 0 ? point - N * 1e-3 : point + N * 1e-3; // checking if the point lies in the shadow of the lights[i]
        vec3f shadow_pt, shadow_N;
        Material tmpmaterial;
        if (scene_intersect(shadow_orig, light_dir, spheres, shadow_pt, shadow_N, tmpmaterial) && (shadow_pt - shadow_orig).norm() < light_distance)
            continue;

        diffuse_light_intensity += lights[i].intensity * (std::max)(0.f, light_dir * N);
        specular_light_intensity += powf((std::max)(0.0f, -reflect(-light_dir, N) * dir), material.specular_exponent) * lights[i].intensity;
    }
    return vec_color(material.diffuse_color * diffuse_light_intensity * material.albedo.raw[0] + vec3f(1.0f, 1.0f, 1.0f) * specular_light_intensity * material.albedo.raw[1]);
}


void render(Image& surface, std::vector<Sphere>& spheres, const std::vector<Light>& lights) {
    const int width = surface.width;
    const int height = surface.height;
    const int fov = PI / 2.0f;

#pragma omp parallel for
    for (size_t j = 0; j < height; j++) {
        for (size_t i = 0; i < width; i++) {
            float x = (2 * (i + 0.5f) / (float)width - 1.0f) * tan(fov / 2.0f) * width / (float)height;
            float y = -(2 * (j + 0.5f) / (float)height - 1.0f) * tan(fov / 2.0f);
            vec3f dir = vec3f(x, y, -1).normalize();
            surface[i + j * width] = cast_ray(vec3f(0, 0, 0), dir, spheres, lights);
        }
    }
}