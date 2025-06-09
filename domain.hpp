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
	RectDomain(double X0, double Y0, double x0, double y0, double x1, double y1, size_t N, size_t M)
		: n(N), m(M)
		, x0(x0), y0(y0)
		, x1(x1), y1(y1)
		, X0(X0), Y0(Y0)
	{
		hx = (x1 - x0) / (n - 1);
		hy = (y1 - y0) / (m - 1);
		u_grid.resize(n, vect_t(m, 0.0));
	}

	explicit RectDomain(double X0, double Y0, double x0, double y0, double x1, double y1, double hx, double hy)
		: hx(hx), hy(hy)
		, x0(x0), y0(y0)
		, x1(x1), y1(y1)
		, X0(X0), Y0(Y0)
	{
		n = (x1 - x0) / hx + 1;
		m = (y1 - y0) / hy + 1;
		u_grid.resize(n, vect_t(m, 0.0));
	}

	void importFromTXT(const std::string& path);
	void exportToTXT(const std::string& path);
	void exportToCSV(const std::string& path);

	vect_t& operator[](int i) { return u_grid[i]; }
	const vect_t& operator[](int i) const { return u_grid[i]; }

	Vec2 toGlobal(Vec2i indices) const;
	Vec2i toIndices(Vec2 global) const;

	Vec2 step() const { return { hx, hy }; }
	Vec2i size() const { return Vec2i(n, m); }

	double X0{ 0.0 }, Y0{ 0.0 }; // global coords
	double x0{ 0.0 }, y0{ 0.0 }; // local begin
	double x1{ 1.0 }, y1{ 1.0 }; // local end

private:
	size_t n, m;   // grid size
	double hx, hy; // grid step
	matr_t u_grid; // grid
};

inline void RectDomain::importFromTXT(const std::string& path)
{
	std::string locked = path + ".lock";
	std::ifstream fin(locked, std::ios_base::trunc);
	if (!fin.is_open()) {
		throw std::exception(std::string("Cannot open the file " + path).c_str());
	}

	fin >> X0 >> Y0 >> x0 >> y0 >> x1 >> y1 >> hx >> hy;
	fin.close();
	std::rename(locked.c_str(), path.c_str());

	n = (x1 - x0) / hx + 1;
	m = (y1 - y0) / hy + 1;
	u_grid.resize(n, vect_t(m, 0.0));
}

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

inline Vec2 RectDomain::toGlobal(Vec2i indices) const
{
	return Vec2(
		X0 + indices.i * hx,
		Y0 + indices.i * hy
	);
}

inline Vec2i RectDomain::toIndices(Vec2 global) const
{
	return Vec2i(
		(global.x - X0) / hx,
		(global.y - Y0) / hy
	);
}
