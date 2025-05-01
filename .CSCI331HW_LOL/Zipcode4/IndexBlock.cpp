// IndexBlock.cpp
#include "IndexBlock.h"

void IndexBlock::pack(std::ostream& out, int blockSize) const {
    uint32_t cnt = static_cast<uint32_t>(entries.size());
    out.write(reinterpret_cast<char const*>(&cnt), sizeof(cnt));
    int used = sizeof(cnt);
    for (auto const& pr : entries) {
        std::string blob = pr.first.serialize();
        out.write(blob.data(), blob.size());
        out.write(reinterpret_cast<char const*>(&pr.second), sizeof(pr.second));
        used += sizeof(uint32_t) + pr.first.zip.size() + sizeof(pr.second);
    }
    if (int pad = blockSize - used; pad > 0) {
        std::string zeros(pad, 0);
        out.write(zeros.data(), pad);
    }
}

void IndexBlock::unpack(std::istream& in) {
    entries.clear();
    uint32_t cnt;
    in.read(reinterpret_cast<char*>(&cnt), sizeof(cnt));
    for (uint32_t i = 0; i < cnt; ++i) {
        Key k = Key::deserialize(in);
        int rbn;
        in.read(reinterpret_cast<char*>(&rbn), sizeof(rbn));
        entries.emplace_back(k, rbn);
    }
}
