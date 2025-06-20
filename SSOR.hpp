#pragma once
#include "domain.hpp"

class SSOR
{
public:
	static double OMEGA;
	static double EPS;

	static double step(RectDomain& dom, const func_t& f);
	static int solve(RectDomain& dom, const func_t& f, int max_it = 10000);
};

inline double SSOR::OMEGA{ 1.8 };
inline double SSOR::EPS{ 1e-12 };

inline double SSOR::step(RectDomain& dom, const func_t& f)
{
	auto [n, m] = dom.size();
	double hx2 = dom.step().x * dom.step().x;
	double hy2 = dom.step().y * dom.step().y;
	double denom = 2.0 * (1.0 / hx2 + 1.0 / hy2);

	double max_diff = 0.0;
	for (int i = 1; i < n - 1; i++) {
		for (int j = 1; j < m - 1; j++) {
			auto [x, y] = dom.toGlobal(Vec2i(i, j));

			auto sum = (
				(dom[i + 1][j] + dom[i - 1][j]) / hx2 +
				(dom[i][j + 1] + dom[i][j - 1]) / hy2 +
				f(x, y)
			) / denom;
			auto unew = (1 - OMEGA) * dom[i][j] + OMEGA * sum;
			max_diff = std::max(max_diff, std::abs(dom[i][j] - unew));
			dom[i][j] = unew;
		}
	}
	for (int i = n - 2; i > 0; i--) {
		for (int j = m - 2; j > 0; j--) {
			auto [x, y] = dom.toGlobal(Vec2i(i, j));

			auto sum = (
				(dom[i + 1][j] + dom[i - 1][j]) / hx2 +
				(dom[i][j + 1] + dom[i][j - 1]) / hy2 +
				f(x, y)
				) / denom;
			auto unew = (1 - OMEGA) * dom[i][j] + OMEGA * sum;
			max_diff = std::max(max_diff, std::abs(dom[i][j] - unew));
			dom[i][j] = unew;
		}
	}
	return max_diff;
}

inline int SSOR::solve(RectDomain& dom, const func_t& f, int max_it)
{
	for (int iter = 0; iter != max_it; iter++)
	{
		auto norm = step(dom, f);
		if (norm <= EPS)
			return iter;
	}
	return max_it;
}
