#include <iostream>
#include <fstream>
#include <vector>
#include <windows.h>
#include "BMPReader.h"


int main(int argc, char* argv[])
{
    // Tehnical task
    if (argc == 2) {
        BMPReader bmp(argv[1]);
        bmp.openBMP();
        bmp.displayBMP(std::cout);
        bmp.closeBMP();

        std::cout << "Press Enter for continue..." << std::endl;
        std::cin.get();

        return 0;
    }

    // Logger to file (output bmp)
    if (argc > 2 && argv[2][0] == '1') {
        std::ofstream logFile("log.txt", std::ofstream::app);

        BMPReader bmp(argv[1]);
        bmp.openBMP();
        bmp.displayBMP(logFile);
        bmp.closeBMP();
        logFile.close();

        return 0;
    }

    // Logger to file (output bmp & output info)
    if (argc > 2 && argv[2][0] == '2') {
        std::ofstream logFile("log.txt", std::ofstream::app);

        BMPReader bmp(argv[1]);
        bmp.openBMP();
        bmp.displayInfo(logFile, argv[1]);
        bmp.displayBMP(logFile);
        bmp.closeBMP();
        logFile.close();

        return 0;
    }

    return 0;
}
