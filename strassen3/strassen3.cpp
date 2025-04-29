#include "matrix.h"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <ostream>
#include <string>

using namespace std;

void printHelpMessage(const char* programName) {
    std::cerr << "*** Strassen3 ***" << std::endl; 
    std::cerr << "Performs matrix multiplication using Strassen method with 3x3 partitioning." << std::endl;
    std::cerr << std::endl << "\tC = A * B" << std::endl << std::endl;
    std::cerr << "USAGE: " << programName << " A B C" << std::endl;
    std::cerr << "\tA, B\tInput files with matrix data in standard format" << std::endl;
    std::cerr << "\tC\tOutput file with matrix data in standard format" << std::endl;
}

int main(int argc, char* argv[])
{
    if (argc != 4) {
        printHelpMessage(argv[0]);
        return EXIT_FAILURE;
    }
    std::string aFileName(argv[1]);
    std::string bFileName(argv[2]);
    std::string cFileName(argv[3]);

    std::ifstream aFile(aFileName);
    std::ifstream bFile(bFileName);
    if (!aFile.is_open()) {
        std::cerr << "Could not open file " << aFileName << std::endl;
        printHelpMessage(argv[0]);
        return EXIT_FAILURE;
    }

    if (!bFile.is_open()) {
        std::cerr << "Could not open file " << bFileName << std::endl;
        printHelpMessage(argv[0]);
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
        printHelpMessage(argv[0]);
        return EXIT_FAILURE;
    }
    cFile << strassen3(A, B);

    cFile.close();

	return EXIT_SUCCESS;
}
