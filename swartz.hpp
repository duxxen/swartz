#pragma once
#include "domain.hpp"
#include "SSOR.hpp"
class Swartz
{
public:
	static double EPS;
	static size_t solve(RectDomain& g1, RectDomain& g2, const func_t& f, size_t max_it = 10000);
	static size_t solve(std::vector<RectDomain>& domains, const func_t& f, size_t max_it = 10000);
};

inline double Swartz::EPS{ 1e-6 };

inline size_t Swartz::solve(RectDomain& g1, RectDomain& g2, const func_t& f, size_t max_it)
{
	auto [intersects, pos1, pos2] = g1.findIntersection(g2);
	if (!intersects) {
		std::cout << "No intersection!\n";
		SSOR::solve(g1, f);
		SSOR::solve(g2, f);
		return 0;
	}

	auto g1_beg = g1.toIndices(pos1);
	auto g1_end = g1.toIndices(pos2);
	auto g2_beg = g2.toIndices(pos1);
	auto g2_end = g2.toIndices(pos2);

	auto [n1, m1] = g1.size() - Vec2i(1, 1);
	auto [n2, m2] = g2.size() - Vec2i(1, 1);

	double prev_diff = 0.0;
	for (size_t iter = 0; iter != max_it; iter++)
	{
		for (int i = g1_beg.i; i <= g1_end.i; i++) {
			auto g2_ind1 = g2.toIndices(g1.toGlobal(Vec2i(i, 0)));
			auto g2_ind2 = g2.toIndices(g1.toGlobal(Vec2i(i, m1)));

			g1[i][0] = g2[g2_ind1.i][g2_ind1.j];
			g1[i][m1] = g2[g2_ind2.i][g2_ind2.j];
		}

		for (int j = g1_beg.j; j <= g1_end.j; j++) {
			auto g2_ind1 = g2.toIndices(g1.toGlobal(Vec2i(0, j)));
			auto g2_ind2 = g2.toIndices(g1.toGlobal(Vec2i(n1, j)));

			//g1[0][j] = g2[g2_ind1.i][g2_ind1.j];
			g1[n1][j] = g2[g2_ind2.i][g2_ind2.j];
		}

		SSOR::solve(g1, f);

		for (int i = g2_beg.i; i <= g2_end.i; i++) {
			auto g1_ind1 = g1.toIndices(g2.toGlobal(Vec2i(i, 0)));
			auto g1_ind2 = g1.toIndices(g2.toGlobal(Vec2i(i, m2)));

			//g2[i][0] = g1[g1_ind1.i][g1_ind1.j];
			//g2[i][m2] = g1[g1_ind2.i][g1_ind2.j];
		}

		for (int j = g2_beg.j; j <= g2_end.j; j++) {
			auto g1_ind1 = g1.toIndices(g2.toGlobal(Vec2i(0, j)));
			auto g1_ind2 = g1.toIndices(g2.toGlobal(Vec2i(n2, j)));

			g2[0][j] = g1[g1_ind1.i][g1_ind1.j];
			//g2[n2][j] = g1[g1_ind2.i][g1_ind2.j];
		}

		SSOR::solve(g2, f);

		double max_diff = 0.0;
		for (int i = g1_beg.i; i <= g1_end.i; i++) {
			for (int j = g1_beg.j; j <= g1_end.j; j++) {
				auto global_pos = g1.toGlobal(Vec2i(i, j));
				auto g2_ind = g2.toIndices(global_pos);

				double diff = std::abs(g1[i][j] - g2[g2_ind.i][g2_ind.j]);
				if (diff > max_diff)
					max_diff = diff;
			}
		}

		if (max_diff < EPS || prev_diff == max_diff)
			return iter;
		prev_diff = max_diff;
	}

	return max_it;
}

inline size_t Swartz::solve(std::vector<RectDomain>& domains, const func_t& f, size_t max_it)
{
	return 0.0;
}
