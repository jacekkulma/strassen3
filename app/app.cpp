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
    std::cerr << std::setw(4) << "" << std::left << std::setw(9) << "A, B" << 
        std::setw(14) << "(required)" << "Input files with matrix data in standard format" << std::endl;
    std::cerr << std::setw(4) << "" << std::left << std::setw(9) << "C" << 
        std::setw(14) << "(required)" << "Output file with matrix data in standard format" << std::endl;
    std::cerr << std::setw(4) << "" << std::left << std::setw(9) << "--help" << 
        std::setw(14) << "(optional)" << "Print this help message" << std::endl;
    std::cerr << std::setw(4) << "" << std::left << std::setw(9) << "--triv" << 
        std::setw(14) << "(optional)" << "Use trivial matrix multiplication algorithm" << std::endl;
    std::cerr << std::setw(4) << "" << std::left << std::setw(9) << "--thres" << 
        std::setw(14) << "(optional)" << "Positive integer value (default: 1) for a threshold between Strassen and trivial algorithms. " <<
        "Submatrices of sizes not greater than the value of the threshold are multiplied using trivial algorithm." << std::endl;
    std::cerr << std::setw(4) << "" << std::left << std::setw(9) << "--double" << 
        std::setw(14) << "(optional)" << "Use double precision floating point numbers" << std::endl;
}

struct arguments {
    std::string programName;
    std::string aPath;
    std::string bPath;
    std::string cPath;
    bool useStrassen;
    bool useDouble;
    int threshold;
};

arguments processArguments(int argc, char* argv[]) {
    arguments args;

    if (argc == 0) exit(EXIT_FAILURE);
    std::filesystem::path path(argv[0]);
    args.programName = path.filename().string();

    args.useStrassen = true;
    args.useDouble = false;
    args.threshold = 1;

    int pathCount = 0;
    std::string paths[3];
    for (int i = 1; i < argc; i++) {
        if (strncmp(argv[i], "--help", 7) == 0) {
			printHelpMessage(args.programName.c_str());
			exit(EXIT_FAILURE);
        }

        if (strncmp(argv[i], "--triv", 7) == 0) {
            args.useStrassen = false;
            continue;
        }

        if (strncmp(argv[i], "--thres", 8) == 0) {
            if (i + 1 >= argc) {
                std::cerr << "Expected a positive integer threshold value." << std::endl;
				printHelpMessage(args.programName.c_str());
			    exit(EXIT_FAILURE);
            }

            if ((args.threshold = std::atoi(argv[i + 1])) < 1) {
                std::cerr << "Expected a positive integer threshold value. Got: " << argv[i + 1] << std::endl;
				printHelpMessage(args.programName.c_str());
			    exit(EXIT_FAILURE);
            }
            i++;
            continue;
        }

        if (strncmp(argv[i], "--double", 9) == 0) {
            args.useDouble = true;
            continue;
        }

        if (pathCount < 3) {
            paths[pathCount++] = argv[i];
        } else {
            std::cerr << "Unknown option: " << argv[i] << std::endl;
            printHelpMessage(args.programName.c_str());
			exit(EXIT_FAILURE);
        }
    }

    if (pathCount != 3) {
        std::cerr << "Input or output file(s) not specified" << std::endl;
        printHelpMessage(args.programName.c_str());
	    exit(EXIT_FAILURE);
    } 

    args.aPath = paths[0];
    args.bPath = paths[1];
    args.cPath = paths[2];

    return args;
}

template<typename T>
int run(const arguments& args) {
    Matrix<T> A, B;

    std::ifstream aFile(args.aPath);
    if (!aFile.is_open()) {
        std::cerr << "Could not open file " << args.aPath << std::endl;
        printHelpMessage(args.programName.c_str());
        return EXIT_FAILURE;
    }
	aFile >> A;
	aFile.close();

    std::ifstream bFile(args.bPath);
    if (!bFile.is_open()) {
        std::cerr << "Could not open file " << args.bPath << std::endl;
        printHelpMessage(args.programName.c_str());
        return EXIT_FAILURE;
    }
	bFile >> B;
	bFile.close();

	std::ofstream cFile(args.cPath);
	if (!cFile.is_open()) {
		std::cerr << "Could not open file " << args.cPath << std::endl;
		printHelpMessage(args.programName.c_str());
        return EXIT_FAILURE;
	}
	try {
		cFile << (args.useStrassen ? strassen3(A, B, args.threshold) : (A * B));
	}
	catch (const std::runtime_error& error) {
		std::cerr << error.what() << std::endl;
        return EXIT_FAILURE;
	}
	cFile.close();

    return EXIT_SUCCESS;
}

int main(int argc, char* argv[])
{
    auto args = processArguments(argc, argv);

    return args.useDouble ? run<double>(args) : run<float>(args);
}
