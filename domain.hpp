#pragma once
#include "coords.hpp"

#include <vector>
#include <fstream>
#include <iostream>
#include <functional>

using vect_t = std::vector<double>;
using matr_t = std::vector<vect_t>;
using func_t = std::function<double(double, double)>;

class RectDomain
{
public:
	RectDomain(size_t N, size_t M, double x0, double y0, double x1, double y1, double X0 = 0.0, double Y0 = 0.0)
		: n(N), m(M)
		, x0(x0), y0(y0)
		, x1(x1), y1(y1)
		, X0(X0), Y0(Y0)
	{
		hx = (x1 - x0) / (n - 1);
		hy = (y1 - y0) / (m - 1);
		u_grid.resize(n, vect_t(m, 0.0));
	}

	explicit RectDomain(double hx, double hy, double x0, double y0, double x1, double y1, double X0 = 0.0, double Y0 = 0.0)
		: hx(hx), hy(hy)
		, x0(x0), y0(y0)
		, x1(x1), y1(y1)
		, X0(X0), Y0(Y0)
	{
		n = (x1 - x0) / hx + 1;
		m = (y1 - y0) / hy + 1;
		u_grid.resize(n, vect_t(m, 0.0));

		//std::cout << u_grid[0][m + 1];
	}

	void exportToTXT(const std::string& path);
	void exportToCSV(const std::string& path);

	vect_t& operator[](int i) { return u_grid[i]; }
	const vect_t& operator[](int i) const { return u_grid[i]; }

	Vec2 toGlobal(const Vec2& coords) const;
	Vec2 toGlobal(const Vec2i& indices) const;

	Vec2 toLocal(const Vec2& coords) const;
	Vec2i toIndices(const Vec2& coords) const;

	Vec2 h() const { return { hx, hy }; }
	Vec2i size() const { return Vec2i(n, m); }

	double X0{ 0.0 }, Y0{ 0.0 }; // global coords
	double x0{ 0.0 }, y0{ 0.0 }; // local start 
	double x1{ 1.0 }, y1{ 1.0 }; // local end

private:
	size_t n, m;   // grid size
	double hx, hy; // grid step
	matr_t u_grid; // grid
};

inline void RectDomain::exportToTXT(const std::string& path)
{
	std::ofstream fout(path);
	for (int i = 0; i < n; ++i) {
		double x_local = x0 + i * hx;
		double x_global = X0 + x_local;
		for (int j = 0; j < m; ++j) {
			double y_local = y0 + j * hy;
			double y_global = Y0 + y_local;
			fout << x_global << " " << y_global << " " << u_grid[i][j] << "\n";
		}
		fout << "\n";
	}
	fout.close();
}

inline void RectDomain::exportToCSV(const std::string& path)
{
}

inline Vec2 RectDomain::toGlobal(const Vec2& coords) const
{
	return Vec2(
		X0 + coords.x - x0,
		Y0 + coords.y - y0
	);
}

inline Vec2 RectDomain::toGlobal(const Vec2i& indices) const
{
	return toGlobal(Vec2(
		indices.i * hx,
		indices.j * hy
	));
}

inline Vec2 RectDomain::toLocal(const Vec2& coords) const
{
	return Vec2(
		x0 + coords.x - X0,
		y0 + coords.y - Y0
	);
}

inline Vec2i RectDomain::toIndices(const Vec2& coords) const
{
	return Vec2i(
		std::floor(coords.x / hx),
		std::floor(coords.y / hy)
	);
}
