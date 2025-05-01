// Record.cpp
#include "Record.h"
#include <sstream>

std::string Record::serialize() const {
    return zipCode + "|" + state + "|" + place;
}

Record Record::deserialize(const std::string& data) {
    Record rec;
    std::istringstream ss(data);
    std::getline(ss, rec.zipCode, '|');
    std::getline(ss, rec.state, '|');
    std::getline(ss, rec.place, '|');
    return rec;
}