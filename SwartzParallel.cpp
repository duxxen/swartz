#include "swartz.hpp"
#include <string>

double f(double x, double y) {
	return 1.0;
}


int main(int argc, char* argv[])
{
    size_t n_domains = 2;
    std::vector<std::string> input_files = {
        "domain_0.txt", "domain_1.txt"
    };
    std::vector<std::string> output_files = {
        "solution_0.txt", "solution_1.txt"
    };

    std::vector<RectDomain> domains;

    domains.resize(n_domains);
    for (int i = 0; i < domains.size(); i++)
        domains[i].importFromTXT(input_files[i]);

    Swartz::solve(domains[0], domains[1], f);

    for (int i = 0; i < domains.size(); i++)
        domains[i].exportToTXT(output_files[i]);
    

	return 0;
}