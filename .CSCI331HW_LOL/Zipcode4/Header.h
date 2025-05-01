// Header.h
#ifndef HEADER_H
#define HEADER_H

#include <ostream>
#include <istream>

struct Header {
    int blockSize;
    int rootRBN;
    int nextFreeRBN;

    void pack(std::ostream& out, int blkSize) const;
    void unpack(std::istream& in);
};

#endif // HEADER_H