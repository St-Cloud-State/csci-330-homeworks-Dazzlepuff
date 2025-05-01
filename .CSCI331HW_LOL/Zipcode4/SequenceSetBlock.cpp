// SequenceSetBlock.cpp
#include "SequenceSetBlock.h"
#include <ostream>
#include <istream>

SequenceSetBlock::SequenceSetBlock()
    : prevBlock(-1), nextBlock(-1) {}

void SequenceSetBlock::insert(const Record& rec) {
    records.push_back(rec);
}

bool SequenceSetBlock::isFull() const {
    return records.size() >= MAX_RECORDS;
}

void SequenceSetBlock::pack(std::ostream& out) const {
    int count = records.size();
    out.write(reinterpret_cast<const char*>(&count), sizeof(count));
    for (auto& rec : records) {
        std::string s = rec.serialize();
        int len = s.size();
        out.write(reinterpret_cast<const char*>(&len), sizeof(len));
        out.write(s.data(), len);
    }
    out.write(reinterpret_cast<const char*>(&prevBlock), sizeof(prevBlock));
    out.write(reinterpret_cast<const char*>(&nextBlock), sizeof(nextBlock));
}

void SequenceSetBlock::unpack(std::istream& in) {
    records.clear();
    int count;
    in.read(reinterpret_cast<char*>(&count), sizeof(count));
    for (int i = 0; i < count; ++i) {
        int len;
        in.read(reinterpret_cast<char*>(&len), sizeof(len));
        std::string s(len, '\0');
        in.read(&s[0], len);
        records.push_back(Record::deserialize(s));
    }
    in.read(reinterpret_cast<char*>(&prevBlock), sizeof(prevBlock));
    in.read(reinterpret_cast<char*>(&nextBlock), sizeof(nextBlock));
}