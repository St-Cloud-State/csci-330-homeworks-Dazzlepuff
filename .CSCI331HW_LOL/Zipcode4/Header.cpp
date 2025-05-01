// Header.cpp
#include "Header.h"
#include <vector>

void Header::pack(std::ostream& out, int blkSize) const {
    out.write(reinterpret_cast<const char*>(&blockSize), sizeof(blockSize));
    out.write(reinterpret_cast<const char*>(&rootRBN), sizeof(rootRBN));
    out.write(reinterpret_cast<const char*>(&nextFreeRBN), sizeof(nextFreeRBN));
    int written = sizeof(blockSize) + sizeof(rootRBN) + sizeof(nextFreeRBN);
    std::vector<char> pad(blkSize - written, 0);
    out.write(pad.data(), pad.size());
}

void Header::unpack(std::istream& in) {
    in.read(reinterpret_cast<char*>(&blockSize), sizeof(blockSize));
    in.read(reinterpret_cast<char*>(&rootRBN), sizeof(rootRBN));
    in.read(reinterpret_cast<char*>(&nextFreeRBN), sizeof(nextFreeRBN));
}