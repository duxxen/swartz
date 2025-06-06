#pragma once
#include "domain.hpp"

class SSOR
{
public:
	static double OMEGA;
	static double EPS;

	static int solve(RectDomain& dom, const func_t& f, size_t max_iter = 10000);
};

inline double SSOR::OMEGA{ 1.8 };
inline double SSOR::EPS{ 1e-12 };

inline int SSOR::solve(RectDomain& dom, const func_t& f, size_t max_iter)
{
	size_t N = dom.size().i;
	size_t M = dom.size().j;
	double hx = dom.h().x;
	double hy = dom.h().y;

	double hx2 = hx * hx;
	double hy2 = hy * hy;
	double denom = 2.0 * (1.0 / hx2 + 1.0 / hy2);

	int iter;
	for (iter = 0; iter != max_iter; iter++)
	{
		double max_diff = 0.0;
		for (int i = 1; i < N - 1; i++) 
		{
			double xi = dom.x0 + i * hx;
			for (int j = 1; j < M - 1; j++) 
			{
				double yi = dom.y0 + j * hy;
				double sum = ((dom[i + 1][j] + dom[i - 1][j]) / hx2 +
							  (dom[i][j + 1] + dom[i][j - 1]) / hy2 +
							  f(xi, yi)) / denom;
				double unew = (1 - OMEGA) * dom[i][j] + OMEGA * sum;
				max_diff = std::max(max_diff, std::abs(unew - dom[i][j]));
				dom[i][j] = unew;
			}
		}
		for (int i = N - 2; i > 0; i--) 
		{
			double xi = dom.x0 + i * hx;
			for (int j = M - 2; j > 0; j--) 
			{
				double yi = dom.y0 + j * hy;
				double sum = ((dom[i + 1][j] + dom[i - 1][j]) / hx2 +
							  (dom[i][j + 1] + dom[i][j - 1]) / hy2 +
							  f(xi, yi)) / denom;
				double unew = (1 - OMEGA) * dom[i][j] + OMEGA * sum;
				max_diff = std::max(max_diff, std::abs(unew - dom[i][j]));
				dom[i][j] = unew;
			}
		}
		if (max_diff <= EPS)
			break;
	}
	return iter;
}
