#pragma once
#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>

struct Vec2p;

struct Vec2
{
	double x{ 0.0 };
	double y{ 0.0 };

	Vec2 operator+(const Vec2& oth) { return { x + oth.x, y + oth.y }; }
	Vec2 operator-(const Vec2& oth) { return { x - oth.x, y - oth.y }; }
	operator Vec2p();

	double length() { return sqrt(x * x + y * y); }
	double dot(const Vec2& oth) { return x * oth.x + y * oth.y; }

	friend std::ostream& operator <<(std::ostream& out, const Vec2& coords);
};

struct Vec2i
{
	int32_t i{ 0 };
	int32_t j{ 0 };

	Vec2i operator+(const Vec2i& oth) { return { i + oth.i, j + oth.j }; }
	Vec2i operator-(const Vec2i& oth) { return { i - oth.i, j - oth.j }; }

	friend std::ostream& operator <<(std::ostream& out, const Vec2i& coords);
};

struct Vec2p
{
	double ro { 0.0 };
	double phi{ 0.0 };

	operator Vec2() { return Vec2(); }
};

inline Vec2::operator Vec2p()
{
	return Vec2p();
}

inline std::ostream& operator<<(std::ostream& out, const Vec2& coords)
{
	out << coords.x << " " << coords.y;
	return out;
}

inline std::ostream& operator<<(std::ostream& out, const Vec2i& coords)
{
	out << coords.i << " " << coords.j;
	return out;
}