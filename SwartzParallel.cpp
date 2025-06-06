#include "swartz.hpp"
#include <string>

double f(double x, double y) {
	return 1.0;
}

int main()
{
	double hx = 0.1;
	double hy = 0.1;

	std::vector<RectDomain> domains;
	domains.reserve(2);
	domains.push_back(RectDomain(hx, hy, 0, 0, 4, 2, 0, 2));
	domains.push_back(RectDomain(hx, hy, 0, 0, 4, 4, 2, 1));

	std::cout << "[G1] [SIZE]: " << domains[0].size() << std::endl;
	std::cout << "[G2] [SIZE]: " << domains[1].size() << std::endl;

	//auto iter = SwartzMethod::solve(domains, f, 1000);
	//std::cout << "Solved [" << domains.size() << "] domains for " << iter << " iterations\n";

	auto iter = SwartzMethod::solve(domains[0], domains[1], f, 1'000'000);
	std::cout << "Swartz solved for " << iter << " iterations\n";

	for (int i = 0; i < domains.size(); i++) {
		domains[i].exportToTXT("solution_" + std::to_string(i) + ".txt");
	}

	return 0;
}