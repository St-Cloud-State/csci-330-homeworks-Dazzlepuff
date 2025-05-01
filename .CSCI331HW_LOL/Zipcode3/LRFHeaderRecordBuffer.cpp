#include "LRFHeaderRecordBuffer.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>

LRFHeaderRecordBuffer::LRFHeaderRecordBuffer(const std::string& filePath)
    : filePath(filePath)
{
}

void LRFHeaderRecordBuffer::readHeader() {
    std::ifstream in(filePath);
    if (!in.is_open()) {
        throw std::runtime_error("Could not open file for reading header: " + filePath);
    }

    std::string line;
    // Read the fixed header lines.
    if (std::getline(in, line))
        fileType = line;
    if (std::getline(in, line))
        version = line;
    if (std::getline(in, line))
        recordLengthFieldSize = line;
    if (std::getline(in, line))
        sizeFormatType = line;
    if (std::getline(in, line))
        primaryKeyIndexFileName = line;
    if (std::getline(in, line))
        recordCount = line;
    if (std::getline(in, line))
        fieldCount = line;

    // Convert fieldCount to an integer.
    int numFields = std::stoi(fieldCount);

    // Read exactly 'numFields' lines for field definitions.
    fieldDefinitions.clear();
    for (int i = 0; i < numFields; ++i) {
        if (std::getline(in, line)) {
            fieldDefinitions.push_back(line);
        }
        else {
            throw std::runtime_error("Expected " + std::to_string(numFields) +
                " field definitions, but file ended early.");
        }
    }
    in.close();
}


void LRFHeaderRecordBuffer::writeHeader() {
    std::ofstream out(filePath, std::ios::trunc);
    if (!out.is_open()) {
        throw std::runtime_error("Could not open file for writing header: " + filePath);
    }

    // Write each header item on its own line.
    out << fileType << "\n";
    out << version << "\n";
    out << recordLengthFieldSize << "\n";
    out << sizeFormatType << "\n";
    out << primaryKeyIndexFileName << "\n";
    out << recordCount << "\n";
    out << fieldCount << "\n";
    // Write each field definition.
    for (const auto& fieldDef : fieldDefinitions) {
        out << fieldDef << "\n";
    }
    out.close();
}

// The wall of Getters. >:D
std::string LRFHeaderRecordBuffer::getFileType() const {
    return fileType;
}

std::string LRFHeaderRecordBuffer::getVersion() const {
    return version;
}

std::string LRFHeaderRecordBuffer::getRecordLengthFieldSize() const {
    return recordLengthFieldSize;
}

std::string LRFHeaderRecordBuffer::getSizeFormatType() const {
    return sizeFormatType;
}

std::string LRFHeaderRecordBuffer::getPrimaryKeyIndexFileName() const {
    return primaryKeyIndexFileName;
}

std::string LRFHeaderRecordBuffer::getRecordCount() const {
    return recordCount;
}

std::string LRFHeaderRecordBuffer::getFieldCount() const {
    return fieldCount;
}

std::vector<std::string> LRFHeaderRecordBuffer::getFieldDefinitions() const {
    return fieldDefinitions;
}

// Little Setters.
void LRFHeaderRecordBuffer::setFileType(const std::string& newFileType) {
    fileType = newFileType;
}

void LRFHeaderRecordBuffer::setVersion(const std::string& newVersion) {
    version = newVersion;
}
