#ifndef BSFRECORDBUFFER_H
#define BSFRECORDBUFFER_H

#include "BSFRecord.h"
#include <string>

class BSFRecordBuffer {
public:
    BSFRecordBuffer(const std::string& rawRecord);

    ZipCodeRecord unpack() const;

private:
    std::string rawCsv;
};

#endif
