#include "matrix.h"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <ostream>
#include <string>
#include <iomanip>
#include <filesystem>

void printHelpMessage(const char* programName) {
    std::cerr << "*** Strassen3 ***" << std::endl; 
    std::cerr << "Performs matrix multiplication using Strassen method with 3x3 partitioning." << std::endl;
    std::cerr << std::endl << std::setw(4) << "" << "C = A * B" << std::endl << std::endl;
    std::cerr << "USAGE: " << programName << " [options] A B C" << std::endl;
    std::cerr << std::setw(4) << "" << std::left << std::setw(8) << "A, B" << 
        std::setw(14) << "(required)" << "Input files with matrix data in standard format" << std::endl;
    std::cerr << std::setw(4) << "" << std::left << std::setw(8) << "C" << 
        std::setw(14) << "(required)" << "Output file with matrix data in standard format" << std::endl;
    std::cerr << std::setw(4) << "" << std::left << std::setw(8) << "--help" << 
        std::setw(14) << "(optional)" << "Print this help message" << std::endl;
    std::cerr << std::setw(4) << "" << std::left << std::setw(8) << "--triv" << 
        std::setw(14) << "(optional)" << "Use trivial matrix multiplication algorithm" << std::endl;
}

int main(int argc, char* argv[])
{
    if (argc == 0) return EXIT_FAILURE;
    std::filesystem::path path(argv[0]);
    auto programName = path.filename().string();

    if (argc < 4) {
        printHelpMessage(programName.c_str());
        return EXIT_FAILURE;
    }

    auto useStrassen = true;
    for (int i = 1; i < argc - 3; i++) {
        if (strncmp(argv[i], "--help", 7) == 0) {
			printHelpMessage(programName.c_str());
			return EXIT_SUCCESS;
        }

        if (strncmp(argv[i], "--triv", 7) == 0) {
            useStrassen = false;
            continue;
        }

		std::cerr << "Unknown option: " << argv[i] << std::endl;
		printHelpMessage(programName.c_str());
		return EXIT_FAILURE;
    }

    std::string aFileName(argv[argc - 3]);
    std::string bFileName(argv[argc - 2]);
    std::string cFileName(argv[argc - 1]);

    std::ifstream aFile(aFileName);
    std::ifstream bFile(bFileName);
    if (!aFile.is_open()) {
        std::cerr << "Could not open file " << aFileName << std::endl;
        printHelpMessage(programName.c_str());
        return EXIT_FAILURE;
    }

    if (!bFile.is_open()) {
        std::cerr << "Could not open file " << bFileName << std::endl;
        printHelpMessage(programName.c_str());
        return EXIT_FAILURE;
    }

	Matrix<float> A, B;
	aFile >> A;
    bFile >> B;
    
    aFile.close();
    bFile.close();

    std::ofstream cFile(cFileName);
    if (!cFile.is_open()) {
        std::cerr << "Could not open file " << cFileName << std::endl;
        printHelpMessage(programName.c_str());
        return EXIT_FAILURE;
    }

    try {
        cFile << (useStrassen ? strassen3(A, B) : (A * B));
    }
    catch (const std::runtime_error& error) {
        std::cerr << error.what() << std::endl;
        return EXIT_FAILURE;
    }

    cFile.close();

	return EXIT_SUCCESS;
}
