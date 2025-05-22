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
    std::cerr << std::setw(4) << "" << std::left << std::setw(8) << "--thres" << 
        std::setw(14) << "(optional)" << "Positive integer value (default: 1) for a threshold between Strassen and trivial algorithms. " <<
        "Submatrices of sizes not greater than the value of the threshold are multiplied using trivial algorithm." << std::endl;
}

int main(int argc, char* argv[])
{
    if (argc == 0) return EXIT_FAILURE;
    std::filesystem::path path(argv[0]);
    auto programName = path.filename().string();

    auto useStrassen = true;
    int threshold = 1, pathCount = 0;
    std::string paths[3];
    for (int i = 1; i < argc; i++) {
        if (strncmp(argv[i], "--help", 7) == 0) {
			printHelpMessage(programName.c_str());
			return EXIT_SUCCESS;
        }

        if (strncmp(argv[i], "--triv", 7) == 0) {
            useStrassen = false;
            continue;
        }

        if (strncmp(argv[i], "--thres", 8) == 0) {
            if (i + 1 >= argc) {
                std::cerr << "Expected a positive integer threshold value." << std::endl;
				printHelpMessage(programName.c_str());
				return EXIT_FAILURE;
            }

            if ((threshold = std::atoi(argv[i + 1])) < 1) {
                std::cerr << "Expected a positive integer threshold value. Got: " << argv[i + 1] << std::endl;
				printHelpMessage(programName.c_str());
				return EXIT_FAILURE;
            }
            i++;
            continue;
        }

        if (pathCount < 3) {
            paths[pathCount++] = argv[i];
        } else {
            std::cerr << "Unknown option: " << argv[i] << std::endl;
            printHelpMessage(programName.c_str());
            return EXIT_FAILURE;
        }
    }

    if (pathCount != 3) {
        std::cerr << "Input or output file(s) not specified" << std::endl;
        printHelpMessage(programName.c_str());
        return EXIT_FAILURE;
    } 

    std::ifstream aFile(paths[0]);
    std::ifstream bFile(paths[1]);
    if (!aFile.is_open()) {
        std::cerr << "Could not open file " << paths[0] << std::endl;
        printHelpMessage(programName.c_str());
        return EXIT_FAILURE;
    }

    if (!bFile.is_open()) {
        std::cerr << "Could not open file " << paths[1] << std::endl;
        printHelpMessage(programName.c_str());
        return EXIT_FAILURE;
    }

	Matrix<float> A, B;
	aFile >> A;
    bFile >> B;
    
    aFile.close();
    bFile.close();

    std::ofstream cFile(paths[2]);
    if (!cFile.is_open()) {
        std::cerr << "Could not open file " << paths[2] << std::endl;
        printHelpMessage(programName.c_str());
        return EXIT_FAILURE;
    }

    try {
        cFile << (useStrassen ? strassen3(A, B, threshold) : (A * B));
    }
    catch (const std::runtime_error& error) {
        std::cerr << error.what() << std::endl;
        return EXIT_FAILURE;
    }

    cFile.close();

	return EXIT_SUCCESS;
}
