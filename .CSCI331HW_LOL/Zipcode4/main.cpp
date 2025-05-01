// main.cpp

#include "BPlusTree.h"
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc < 5) {
        std::cerr << "Usage: " << argv[0] << " <csv-file> <btree-file> <leaf-rbn> <zip-code>\n";
        return 1;
    }

    BPlusTree tree(argv[2], 4096);
    tree.buildFromCSV(argv[1]);
    std::cout << "B+ Tree file generated.\n" << argv[2] << std::endl;


    int leafRbn = std::stoi(argv[3]);
    std::string targetZip = argv[4];

    tree.dumpLeaf(leafRbn);

    std::string state = tree.searchState(targetZip);
    if (!state.empty()) {
        std::cout << "ZIP " << targetZip
            << " is in state: " << state << "\n";
    }
    else {
        std::cout << "ZIP " << targetZip
            << " not found.\n";
    }

    std::string place = tree.searchPlace(targetZip);
    if (!place.empty()) {
        std::cout << "ZIP " << targetZip
            << " is in the place: " << place << "\n";
    }
    else {
        std::cout << "ZIP " << targetZip
            << " not found..\n";
    }

    return 0;
}
