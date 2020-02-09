
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

	vec3 operator - () const
	{
		return vec3(-x, -y, -z);
	}

	float operator * (vec3 other) const
	{
		return x * other.x + y * other.y + z * other.z;
	}

	vec3 operator * (float mul) const
	{
		return vec3(x * mul, y * mul, z * mul);
	}

	vec3 operator + (vec3 other) const
	{
		return vec3(x + other.x, y + other.y, z + other.z);
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


template <typename T>
struct vec2
{
	union {
		struct { T x, y; };
		T raw[2];
	};

	vec2(T x, T y) : x(x), y(y) {}
};

typedef vec2<float> vec2f;


template <typename T>
struct vec4
{
	union {
		struct { T x, y, z, w; };
		T raw[4];
	};

	vec4(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {}
};

typedef vec4<float> vec4f;