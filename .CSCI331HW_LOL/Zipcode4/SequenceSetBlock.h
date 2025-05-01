// SequenceSetBlock.h
#ifndef SEQUENCESETBLOCK_H
#define SEQUENCESETBLOCK_H

#include "Record.h"
#include <vector>

class SequenceSetBlock {
public:
    static const int MAX_RECORDS = 100;

    SequenceSetBlock();
    void insert(const Record& rec);
    bool isFull() const;

    void pack(std::ostream& out) const;
    void unpack(std::istream& in);

    std::vector<Record> records;
    int prevBlock;
    int nextBlock;
};

#endif // SEQUENCESETBLOCK_H