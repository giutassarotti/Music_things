#include <iostream>
#include <string>

#include "music_sheet.hpp"

int main(int argc, char** argv)
{
    const std::string filename = argc >= 2 ? argv[1] : "test.jpg";

    music::music_sheet sheet(filename);

    char c;
    std::cin >> c; 
    return 0;
}