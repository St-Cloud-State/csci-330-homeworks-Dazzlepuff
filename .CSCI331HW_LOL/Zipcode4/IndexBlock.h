// IndexBlock.h
#ifndef INDEXBLOCK_H
#define INDEXBLOCK_H

#include "Key.h"
#include <vector>
#include <utility>
#include <cstdint>
#include <istream>
#include <ostream>

class IndexBlock {
public:
    static constexpr int MAX_ENTRIES = 128;
    std::vector<std::pair<Key, int>> entries;

    bool isFull() const { return entries.size() >= MAX_ENTRIES; }
    void pack(std::ostream& out, int blockSize) const;
    void unpack(std::istream& in);
};

#endif // INDEXBLOCK_H
