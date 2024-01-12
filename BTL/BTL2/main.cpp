#include "main.h"
#include "restaurant.cpp"

int main(int argc, char *argv[]) {
    if(argc < 2) {
        std::cerr << "Please provide an input file name.\n";
        return 1;
    }
    string fileName = argv[1];
    simulate(fileName);
    return 0;
}
