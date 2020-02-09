
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
    Material(const float& r, const vec4f& a, const vec3f& color, const float& spec) : refractive_index(r), albedo(a), diffuse_color(color), specular_exponent(spec) {}
    Material() : refractive_index(1), albedo(1, 0, 0, 0), diffuse_color(), specular_exponent() {}
    float refractive_index;
    vec4f albedo;
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

    float checkerboard_dist = (std::numeric_limits<float>::max)();
    if (fabs(dir.y) > 1e-3) {
        float d = -(orig.y + 4) / dir.y; // the checkerboard plane has equation y = -4
        vec3f pt = orig + dir * d;
        if (d > 0 && fabs(pt.x) < 10 && pt.z<-10 && pt.z>-30 && d < spheres_dist) {
            checkerboard_dist = d;
            hit = pt;
            N = vec3f(0, 1, 0);
            material.diffuse_color = (int(.5 * hit.x + 1000) + int(.5 * hit.z)) & 1 ? vec3f(1, 1, 1) : vec3f(1, .7, .3);
            material.diffuse_color = material.diffuse_color * .3;
        }
    }
    return min(spheres_dist, checkerboard_dist) < 1000;
}

vec3f reflect(vec3f I, vec3f& N) {
    return I - N * 2.f * (I * N);
}


vec3f refract(const vec3f& I, const vec3f& N, const float& refractive_index) { // Snell's law
    float cosi = -max(-1.f, min(1.f, I * N));
    float etai = 1, etat = refractive_index;
    vec3f n = N;
    if (cosi < 0) { // if the ray is inside the object, swap the indices and invert the normal to get the correct result
        cosi = -cosi;
        std::swap(etai, etat); n = -N;
    }
    float eta = etai / etat;
    float k = 1 - eta * eta * (1 - cosi * cosi);
    return k < 0 ? vec3f(0, 0, 0) : I * eta + n * (eta * cosi - sqrtf(k));
}

vec3f cast_ray(const vec3f& orig, const vec3f& dir, std::vector<Sphere>& spheres, const std::vector<Light>& lights, size_t depth = 0) {
    vec3f point, N;
    Material material;
    
    if (depth > 4 || !scene_intersect(orig, dir, spheres, point, N, material)) {
        return vec3f(0.2, 0.7, 0.8); // background color
    }


    vec3f reflect_dir = reflect(dir, N).normalize();
    vec3f refract_dir = refract(dir, N, material.refractive_index).normalize();
    vec3f reflect_orig = reflect_dir * N < 0 ? point - N * 1e-3 : point + N * 1e-3; // offset the original point to avoid occlusion by the object itself
    vec3f refract_orig = refract_dir * N < 0 ? point - N * 1e-3 : point + N * 1e-3;
    vec3f reflect_color = cast_ray(reflect_orig, reflect_dir, spheres, lights, depth + 1);
    vec3f refract_color = cast_ray(refract_orig, refract_dir, spheres, lights, depth + 1);

    float diffuse_light_intensity = 0, specular_light_intensity = 0;
    for (size_t i = 0; i < lights.size(); i++) {
        vec3f light_dir = (lights[i].position - point).normalize();
        float light_distance = (lights[i].position - point).norm();

        vec3f shadow_orig = light_dir * N < 0 ? point - N * 1e-3 : point + N * 1e-3; // checking if the point lies in the shadow of the lights[i]
        vec3f shadow_pt, shadow_N;
        Material tmpmaterial;
        if (scene_intersect(shadow_orig, light_dir, spheres, shadow_pt, shadow_N, tmpmaterial) && (shadow_pt - shadow_orig).norm() < light_distance)
            continue;

        diffuse_light_intensity += lights[i].intensity * max(0.f, light_dir * N);
        specular_light_intensity += powf(max(0.f, -reflect(-light_dir, N) * dir), material.specular_exponent) * lights[i].intensity;
    }
    return material.diffuse_color * diffuse_light_intensity * material.albedo.raw[0] + vec3f(1., 1., 1.) * specular_light_intensity * material.albedo.raw[1] + reflect_color * material.albedo.raw[2] + refract_color * material.albedo.raw[3];
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
            surface[i + j * width] = vec_color(cast_ray(vec3f(0, 0, 0), dir, spheres, lights));
        }
    }
}