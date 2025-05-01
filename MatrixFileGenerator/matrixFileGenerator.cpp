#include <cstdlib>
#include <fstream>
#include <iostream>
#include <ostream>
#include <string>
#include <random>
#include <iomanip>
#include <cmath>

#include "args.hxx"

using namespace std;

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
    args::ArgumentParser parser("Generates text file(s) with n x n matrix data in standard format.");
    args::HelpFlag helpFlag(parser, "help", "Display this help menu.", { 'h', "help" });
    args::Group requiredGroup(parser, "Required arguments:", args::Group::Validators::All);
    args::ValueFlag<std::string> fileNameFlag(requiredGroup, "Output file(s) name", "Name for output file(s) with matrix data in standard format. Can be given either with or without '.txt' extension.", { "fileName" });
    args::ValueFlag<unsigned int> mSizeFlag(requiredGroup, "Matrix size", "Matrix size (dimension). If lesser than 1, empty file(s) will be generated.", { "mSize" });
    args::Group optionalGroup(parser, "Optional arguments:");
    args::ValueFlag<float> minValueFlag(optionalGroup, "Min. value", "Min. value for each matrix element. Defaults to -10.", { "minValue" }, -10.0f);
    args::ValueFlag<float> maxValueFlag(optionalGroup, "Max. value", "Max. value for each matrix element. Defaults to 10.", { "maxValue" }, 10.0f);
    args::ValueFlag<unsigned int> mCountFlag(optionalGroup, "Number of files", "Number of files to generate. Defaults to 1. If greater than 1, file number appended to each file name.", { "mCount" }, 1);
    args::ValueFlag<unsigned int> precisionFlag(optionalGroup, "Decimal precison", "Decimal precision. Each matrix element rounded to 'precision' decimal points.", { "precison" }, 0);

    try
    {
        parser.ParseCLI(argc, argv);
    }
    catch (args::Help)
    {
        std::cout << parser;
        return EXIT_SUCCESS;
    }
    catch (args::ParseError e)
    {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return EXIT_FAILURE;
    }
    catch (args::ValidationError e)
    {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return EXIT_FAILURE;
    }

    int mSize = args::get(mSizeFlag);
    int mCount = args::get(mCountFlag);
    std::string fileName = args::get(fileNameFlag);
    int precision = args::get(precisionFlag);
    float start = args::get(minValueFlag);
    float end = args::get(maxValueFlag);

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
            return EXIT_FAILURE;
        }

        // generate file
        for (int row = 0; row < mSize; row++) {
            for (int col = 0; col < mSize; col++) {
                float number = generateRandomFloat(start, end, precision);
                if (precision == 0 && number == -0.0f) number = 0.0f;
                File << std::fixed << std::setprecision(precision) << number << " ";
            }
            File << "\n";
        }
        File.close();
    }

    return EXIT_SUCCESS;
}