#pragma once
#include "domain.hpp"
#include "SSOR.hpp"

inline std::tuple<bool, Vec2, Vec2> findIntersection(const RectDomain& g1, const RectDomain& g2)
{
	auto x1 = std::max(g1.X0, g2.X0);
	auto x2 = std::min(g1.X0 + (g1.x1 - g1.x0), g2.X0 + (g2.x1 - g2.x0));
	auto y1 = std::max(g1.Y0, g2.Y0);
	auto y2 = std::min(g1.Y0 + (g1.y1 - g1.y0), g2.Y0 + (g2.y1 - g2.y0));

	if (x1 < x2 && y1 < y2) {
		return std::make_tuple<bool, Vec2, Vec2>(true, { x1, y1 }, { x2 - x1, y2 - y1 });
	}
	return std::make_tuple<bool, Vec2, Vec2>(false, {}, {});
}

class SwartzMethod
{
public:
	static double EPS;

	static int solve(RectDomain& g1, RectDomain& g2, const func_t& f, size_t max_iter = 10000);
	static int solve(std::vector<RectDomain>& gs, const func_t& f, size_t max_iter = 10000);
};

inline double SwartzMethod::EPS{ 1e-6 };

inline int SwartzMethod::solve(RectDomain& g1, RectDomain& g2, const func_t& f, size_t max_iter)
{
	auto [intersects, pos, size] = findIntersection(g1, g2);
	if (!intersects)
	{
		std::cout << "No intersection\n";
		SSOR::solve(g1, f);
		SSOR::solve(g2, f);
		return 0;
	}

	auto g1_beg = g1.toIndices(g1.toLocal(pos));
	auto g1_end = g1.toIndices(g1.toLocal(pos + size));
	auto g2_beg = g2.toIndices(g2.toLocal(pos));
	auto g2_end = g2.toIndices(g2.toLocal(pos + size));

	std::cout << "[G1] [BEG]: " << pos << " ; " << g1.toLocal(pos) << " ; " << g1_beg << std::endl;
	std::cout << "[G1] [END]: " << pos + size << " ; " << g1.toLocal(pos + size) << " ; " << g1_end << std::endl;
	std::cout << "[G2] [BEG]: " << pos << " ; " << g2.toLocal(pos) << " ; " << g2_beg << std::endl;
	std::cout << "[G2] [END]: " << pos + size << " ; " << g2.toLocal(pos + size) << " ; " << g2_end << std::endl;

	double max_diff = 0.0;
	double sum_sq = 0.0;
	
	auto len = g1_end - g1_beg;
	size_t n_points = (len.i + 1) * (len.j + 1);

	auto [n1, m1] = g1.size() - Vec2i(1, 1);
	auto [n2, m2] = g2.size() - Vec2i(1, 1);


	int iter, it;
	for (iter = 0; iter != max_iter; iter++)
	{
		//for (int i = g2_beg.i; i <= g2_end.i; i++) {
		//	for (int j = g2_beg.j; j <= g2_end.j; j++) {
		//		auto g1_ind = g1.toIndices(g1.toLocal(g2.toGlobal(Vec2i(i, j))));
		//		g2[i][j] = g1[g1_ind.i][g1_ind.j];
		//	}
		//}

		//it = SSOR::solve(g2, f);
		////std::cout << "[SWARTZ #" << iter << "]: Inner solve G1 for " << it << " iterations\n";

		//for (int i = g1_beg.i; i <= g1_end.i; i++) {
		//	for (int j = g1_beg.j; j <= g1_end.j; j++) {
		//		auto g2_ind = g2.toIndices(g2.toLocal(g1.toGlobal(Vec2i(i, j))));

		//		auto diff = std::abs(g1[i][j] - g2[g2_ind.i][g2_ind.j]);
		//		max_diff = std::max(max_diff, diff);
		//		sum_sq += diff * diff;

		//		g1[i][j] = g2[g2_ind.i][g2_ind.j];
		//	}
		//}

		//it = SSOR::solve(g1, f);
		////std::cout << "[SWARTZ #" << iter << "]: Inner solve G2 for " << it << " iterations\n";

		for (int i = g1_beg.i; i <= g1_end.i; i++) {
			auto g2_ind1 = g2.toIndices(g2.toLocal(g1.toGlobal(Vec2i(i, 0))));
			auto g2_ind2 = g2.toIndices(g2.toLocal(g1.toGlobal(Vec2i(i, m1))));

			g1[i][0] = g2[g2_ind1.i][g2_ind1.j];
			g1[i][m1] = g2[g2_ind2.i][g2_ind2.j];
		}

		for (int j = g1_beg.j; j <= g1_end.j; j++) {
			auto g2_ind1 = g2.toIndices(g2.toLocal(g1.toGlobal(Vec2i(0, j))));
			auto g2_ind2 = g2.toIndices(g2.toLocal(g1.toGlobal(Vec2i(n1, j))));

			//g1[0][j] = g2[g2_ind1.i][g2_ind1.j];
			g1[n1][j] = g2[g2_ind2.i][g2_ind2.j];
		}
		it = SSOR::solve(g1, f);

		for (int j = g2_beg.j; j <= g2_end.j; j++) {
			auto g1_ind1 = g1.toIndices(g1.toLocal(g2.toGlobal(Vec2i(0, j))));
			auto g1_ind2 = g1.toIndices(g1.toLocal(g2.toGlobal(Vec2i(n2, j))));

			g2[0][j] = g1[g1_ind1.i][g1_ind1.j];
			//g2[n2][j] = g1[g1_ind2.i][g1_ind2.j];
		}
		it = SSOR::solve(g2, f);

		for (int i = g1_beg.i; i <= g1_end.i; i++) {
			for (int j = g1_beg.j; j <= g1_end.j; j++) {
				auto g2_ind = g2.toIndices(g2.toLocal(g1.toGlobal(Vec2i(i, j))));

				auto diff = std::abs(g1[i][j] - g2[g2_ind.i][g2_ind.j]);
				max_diff = std::max(max_diff, diff);
				sum_sq += diff * diff;
			}
		}
		double l2_norm = std::sqrt(sum_sq / n_points);

		// 60, 30 ; 20, 50
		// 65, 35 ; 25, 55
		// 70, 40 ; 30, 60

		if (iter % 10000 == 0) {
			std::cout << "[SWARTZ #" << iter << "]: Max norm = " << max_diff << ", L2 norm = " << l2_norm << std::endl;
		}
		if (max_diff <= EPS)
			return iter;
	}
}

inline int SwartzMethod::solve(std::vector<RectDomain>& gs, const func_t& f, size_t max_iter)
{
	int iter = 0;

	std::cout << "Solving for " << gs.size() << " domains:\n";
	for (int i = 0; i < gs.size(); i++) {
		for (int j = 0; j < gs.size(); j++) {
			if (i != j) {
				std::cout << "Solving G[" << i << "] : G[" << j << "]: ";;
				auto it = solve(gs[i], gs[j], f, max_iter);
				if (it)
					std::cout << it << " iterations\n";
				
				iter += it;
			}
		}
	}
	return iter;
}
