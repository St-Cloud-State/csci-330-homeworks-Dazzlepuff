#include "BSFRecordBuffer.h"
#include <sstream>

BSFRecordBuffer::BSFRecordBuffer(const std::string& rawRecord)
    : rawCsv(rawRecord) {}

ZipCodeRecord BSFRecordBuffer::unpack() const {
    ZipCodeRecord rec;
    std::stringstream ss(rawCsv);
    std::getline(ss, rec.zip, ',');
    std::getline(ss, rec.place, ',');
    std::getline(ss, rec.state, ',');
    std::getline(ss, rec.county, ',');
    std::getline(ss, rec.lat, ',');
    std::getline(ss, rec.lon, ',');
    return rec;
}
