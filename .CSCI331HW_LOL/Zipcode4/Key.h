// Key.h
#ifndef KEY_H
#define KEY_H

#include <string>
#include <istream>
#include <ostream>
#include <cstdint>

struct Key {
    std::string zip;

    bool operator<(Key const& o) const {
        return zip < o.zip;
    }

    std::string serialize() const {
        uint32_t n = static_cast<uint32_t>(zip.size());
        std::string out;
        out.append(reinterpret_cast<char const*>(&n), sizeof(n));
        out += zip;
        return out;
    }

    static Key deserialize(std::istream& in) {
        uint32_t n;
        in.read(reinterpret_cast<char*>(&n), sizeof(n));
        Key k;
        k.zip.resize(n);
        in.read(&k.zip[0], n);
        return k;
    }
};

#endif // KEY_H