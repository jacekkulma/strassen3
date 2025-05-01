#include <cstdlib>
#include <fstream>
#include <iostream>
#include <ostream>
#include <string>
#include <random>
#include <iomanip>
#include <cmath>

using namespace std;

void printHelpMessage(const char* programName) {
    std::cerr << "*** MatrixFileGenerator ***" << std::endl;
    std::cerr << "Generates text file containing n x n matrix." << std::endl;
    std::cerr << "USAGE: " << programName << " n fileName minValue maxValue filesToGenerate precision" << std::endl;
    std::cerr << "\tn\t\tMatrix size." << std::endl;
    std::cerr << "\tfileName\tOutput file with matrix data in standard format." << std::endl;
    std::cerr << "\tminValue\tMin. value for each matrix element. Defaults to -10." << std::endl;
    std::cerr << "\tmaxValue\tMax. value for each matrix element. Defaults to 10." << std::endl;
    std::cerr << "\tfilesToGenerate\tNumber of matrix files to generate. Defaults to 1. If greater than 1, file number appended to file name." << std::endl;
    std::cerr << "\tprecision\tDecimal precision. Each matrix element rounded to 'precision' decimal points. Defaults to 0." << std::endl;
}

float generateRandomFloat(float start, float end, int precision) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(start, end);

    float raw = dis(gen);

    float factor = std::pow(10.0f, precision);
    return std::round(raw * factor) / factor;
}

std::string addIndexToFilename(const std::string& filename, int index) {
    // last dot position
    size_t dotPos = filename.rfind('.');

    // if dot occurs, put '_i' before dot
    if (dotPos != std::string::npos) {
        return filename.substr(0, dotPos) + "_" + std::to_string(index) + filename.substr(dotPos);
    }
    // if not put '_i' at the end
    else {
        return filename + "_" + std::to_string(index);
    }
}

std::string ensureTxtExtension(const std::string& filename) {
    size_t dotPos = filename.rfind('.');

    // if no extension add '.txt'
    if (dotPos == std::string::npos || dotPos == 0 || dotPos == filename.length() - 1) {
        return filename + ".txt";
    }

    return filename;
}


int main(int argc, char* argv[])
{
    // default values
    int mCount = 1, precision = 0;
    float start = -10, end = 10;

    // get values if specified
    if (argc == 4) {
        start = std::stoi(argv[3]);
    }
    else if (argc == 5) {
        start = std::stoi(argv[3]);
        end = std::stoi(argv[4]);
    }
    else if (argc == 6) {
        start = std::stoi(argv[3]);
        end = std::stoi(argv[4]);
        mCount = std::stoi(argv[5]);
    }
    else if (argc == 7) {
        start = std::stoi(argv[3]);
        end = std::stoi(argv[4]);
        mCount = std::stoi(argv[5]);
        precision = std::stoi(argv[6]);
    }
    else if (argc != 3) {
        printHelpMessage(argv[0]);
        return EXIT_FAILURE;
    }

    // matrix size and file name need to be always given
    int mSize = std::stoi(argv[1]);
    std::string fileName(argv[2]);

    // loop for each file
    for (int i = 0; i < mCount; i++) {
        std::string currentFileName = fileName;
        if (mCount > 1) {
            currentFileName = addIndexToFilename(currentFileName, i + 1);
        }
        currentFileName = ensureTxtExtension(currentFileName);

        std::ofstream File(currentFileName);
        if (!File.is_open()) {
            std::cerr << "Could not open file " << currentFileName << std::endl;
            printHelpMessage(argv[0]);
            return EXIT_FAILURE;
        }

        // generate file
        for (int row = 0; row < mSize; row++) {
            for (int col = 0; col < mSize; col++) {
                float number = generateRandomFloat(start, end, precision);
                File << std::fixed << std::setprecision(precision) << number << " ";
            }
            File << "\n";
        }
        File.close();
    }

    return EXIT_SUCCESS;
}