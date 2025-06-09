#include "SSOR.hpp"
#include <string>

double f(double x, double y) {
	return 1.0;
}

bool stats = false;
size_t n_domains = 0;
std::vector<std::string> input_files;
std::vector<std::string> output_files;

std::vector<RectDomain> domains;

void run_iteratively(int steps)
{
    for (int k = 0; k != steps; k++) {
        for (int i = 0; i < domains.size(); i++) {
            auto norm = SSOR::step(domains[i], f);
            domains[i].exportToTXT(output_files[i]);
            if (stats) {
                std::cout << "[G" << i << "] [SSOR #" << k << "] Norm: " << norm << std::endl;
            }
            if (norm <= SSOR::EPS) {
                return;
            }
        }
    }
}

void run_classic()
{
    for (int i = 0; i < domains.size(); i++) {
        auto iter = SSOR::solve(domains[i], f);
    }
}

int main(int argc, char* argv[])
{
    setlocale(LC_ALL, "ru");
    int steps = 0;

    // --- Чтение флагов ---
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "-h" || arg == "--help") {
            std::cout << "Usage:\n"
                << "  solver -n <count> -i <input1> <input2> ... -o <output1> <output2> ... [options]\n\n"
                << "Required flags:\n"
                << "  -n <count>          Количество прямоугольных областей\n"
                << "  -i <file1> ...      Входные файлы с параметрами для каждой области\n"
                << "  -o <file1> ...      Файлы, куда сохранить результаты после SSOR\n\n"
                << "Optional flags:\n"
                << "  --stats             Вывести статистику (размеры, шаги, время)\n"
                << "  --steps <count>     Запустить итеративно\n"
                << "  -h, --help          Показать эту справку\n";
            return 0;
        }
        else if (arg == "-n" && i + 1 < argc) {
            n_domains = std::stoi(argv[++i]);
            input_files.reserve(n_domains);
            output_files.reserve(n_domains);
        }
        else if (arg == "-i") {
            for (int j = 0; j < n_domains && i + 1 < argc; ++j) {
                input_files.push_back(argv[++i]);
            }
        }
        else if (arg == "-o") {
            for (int j = 0; j < n_domains && i + 1 < argc; ++j) {
                output_files.push_back(argv[++i]);
            }
        }
        else if (arg == "--stats") {
            stats = true;
        }
        else if (arg == "--steps") {
            steps = std::stoi(argv[++i]);
        }
    }

    if (n_domains == 0 || input_files.size() != n_domains || output_files.size() != n_domains) {
        std::cerr << "Ошибка: некорректные аргументы.\n";
        std::cerr << "Пример: ./solver -n 2 -i g1.txt g2.txt -o g1_out.txt g2_out.txt\n";
        return 1;
    }

    domains.resize(n_domains);
    for (int i = 0; i < domains.size(); i++)
        domains[i].importFromTXT(input_files[i]);

    for (int i = 0; i < domains.size(); i++) {
        if (stats) {
            std::cout << "[G" << i << "] [GRID] Size: " << domains[i].size() << std::endl;
            std::cout << "[G" << i << "] [GRID] Step: " << domains[i].step() << std::endl;
            std::cout << "[G" << i << "] [GLOBAL] Coords: " << domains[i].X0 << " " << domains[i].Y0 << std::endl;
            std::cout << "[G" << i << "] [GLOBAL] Begin: " << domains[i].x0 << " " << domains[i].y0 << std::endl;
            std::cout << "[G" << i << "] [GLOBAL] End: " << domains[i].x1 << " " << domains[i].y1 << std::endl;
        }
    }

    if (steps) {
        run_iteratively(steps);
    }
    else {
        run_classic();
    }

	return 0;
}