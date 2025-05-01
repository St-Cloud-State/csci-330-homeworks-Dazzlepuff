// Record.h
#ifndef RECORD_H
#define RECORD_H

#include <string>

struct Record {
    std::string zipCode;
    std::string state;
    std::string place;

    std::string serialize() const;
    static Record deserialize(const std::string& data);
};

#endif // RECORD_H