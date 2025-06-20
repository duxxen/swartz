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
	RectDomain() = default;
	RectDomain(double x0, double y0, double x1, double y1, size_t N, size_t M)
		: n(N), m(M)
		, x0(x0), y0(y0)
		, x1(x1), y1(y1)
	{
		hx = (x1 - x0) / (n - 1);
		hy = (y1 - y0) / (m - 1);
		u_grid.resize(n, vect_t(m, 0.0));
	}

	explicit RectDomain(double x0, double y0, double x1, double y1, double hx, double hy)
		: hx(hx), hy(hy)
		, x0(x0), y0(y0)
		, x1(x1), y1(y1)
	{
		n = (x1 - x0) / hx + 1;
		m = (y1 - y0) / hy + 1;
		u_grid.resize(n, vect_t(m, 0.0));
	}

	void importFromTXT(const std::string& path);
	void exportToTXT(const std::string& path);
	void exportToCSV(const std::string& path);

	std::tuple<bool, Vec2, Vec2> findIntersection(const RectDomain& other) const;

	vect_t& operator[](int i) { return u_grid[i]; }
	const vect_t& operator[](int i) const { return u_grid[i]; }

	Vec2 toGlobal(Vec2i indices) const;
	Vec2i toIndices(Vec2 global) const;

	Vec2 step() const { return { hx, hy }; }
	Vec2i size() const { return Vec2i(n, m); }

	double x0{ 0.0 }, y0{ 0.0 }; 
	double x1{ 1.0 }, y1{ 1.0 }; 

private:
	size_t n, m;   // grid size
	double hx, hy; // grid step
	matr_t u_grid; // grid
};

inline void RectDomain::importFromTXT(const std::string& path)
{
	std::ifstream fin(path);
	if (!fin.is_open()) {
		throw std::exception(std::string("Cannot open the file " + path).c_str());
	}

	fin >> x0 >> y0 >> x1 >> y1 >> hx >> hy;
	fin.close();

	n = (x1 - x0) / hx + 1;
	m = (y1 - y0) / hy + 1;
	u_grid.resize(n, vect_t(m, 0.0));
}

inline void RectDomain::exportToTXT(const std::string& path)
{
	std::ofstream fout(path);
	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < m; ++j) {
			auto [x, y] = toGlobal(Vec2i(i, j));
			fout << x << " " << y << " " << u_grid[i][j] << "\n";
		}
		fout << "\n";
	}
	fout.close();
}

inline void RectDomain::exportToCSV(const std::string& path)
{
}

inline std::tuple<bool, Vec2, Vec2> RectDomain::findIntersection(const RectDomain& other) const
{
	auto ix0 = std::max(x0, other.x0);
	auto ix1 = std::min(x1, other.x1);
	auto iy0 = std::max(y0, other.y0);
	auto iy1 = std::min(y1, other.y1);

	if (ix0 < ix1 && iy0 < iy1)
		return std::make_tuple(true, Vec2{ ix0, iy0 }, Vec2{ ix1, iy1 });
	return { false, {}, {} };
}

inline Vec2 RectDomain::toGlobal(Vec2i indices) const
{
	return Vec2(
		x0 + indices.i * hx,
		y0 + indices.j * hy
	);
}

inline Vec2i RectDomain::toIndices(Vec2 global) const
{
	return Vec2i(
		(global.x - x0) / hx,
		(global.y - y0) / hy
	);
}
