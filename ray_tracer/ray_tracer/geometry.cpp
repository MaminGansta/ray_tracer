
#include <math.h>

template <typename T>
struct vec3
{
	union
	{
		struct { T x, y, z; };
		T raw[3];
	};

	vec3() : x(0), y(0), z(0) {}
	vec3(T x, T y, T z) : x(x), y(y), z(z) {}

	vec3 operator - (vec3 other) const
	{
		return vec3(x - other.x, y - other.y, z - other.z);
	}

	float operator * (vec3 other)
	{
		return x * other.x + y * other.y + z * other.z;
	}

	float norm()
	{
		return sqrtf(x * x + y * y + z * z);
	}

	vec3 normalize()
	{
		float n = 1.0f / norm();
		return vec3(x * n, y * n, z * n);
	}

};

typedef vec3<float> vec3f;



struct Sphere
{
	vec3f center;
	float radius;

	Sphere(vec3f center, float radius) : center(center), radius(radius) {}

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
