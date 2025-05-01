#include "LRFBuffer.h"
#include <sstream>
#include <cstdint>
#include <iostream>

LRFBuffer::LRFBuffer(const std::string& filePath) {
    inputFile.open(filePath, std::ios::binary);
    if (!inputFile.is_open()) {
        throw std::runtime_error("Could not open file: " + filePath);
    }
    readHeader();
}

LRFBuffer::~LRFBuffer() {
    if (inputFile.is_open()) {
        inputFile.close();
    }
}

void LRFBuffer::readHeader() {
    headers.clear();
    size_t headerSize = 0;
    inputFile.read(reinterpret_cast<char*>(&headerSize), sizeof(headerSize));
    if (!inputFile) {
        throw std::runtime_error("Failed to read header size.");
    }
    std::string headerStr(headerSize, '\0');
    inputFile.read(&headerStr[0], headerSize);
    if (!inputFile) {
        throw std::runtime_error("Failed to read header string.");
    }
    std::istringstream headerStream(headerStr);
    std::string line;
    for (int i = 0; i < 7 && std::getline(headerStream, line); ++i) {
        // Skip first 7 header lines.
    }
    while (std::getline(headerStream, line)) {
        if (line.empty()) {
            continue;
        }
        std::istringstream lineStream(line);
        std::string fieldName;
        if (std::getline(lineStream, fieldName, ',')) {
            headers.push_back(fieldName);
        }
    }
    headerEndPosition = inputFile.tellg();
}

std::vector<std::string> LRFBuffer::getHeaders() const {
    return headers;
}

bool LRFBuffer::getNextRecord(std::vector<std::string>& record) {
    if (inputFile.peek() == EOF) {
        return false;
    }
    uint32_t recordLength = 0;
    inputFile.read(reinterpret_cast<char*>(&recordLength), sizeof(recordLength));
    if (!inputFile) {
        return false;
    }
    std::string recordStr(recordLength, '\0');
    inputFile.read(&recordStr[0], recordLength);
    if (!inputFile) {
        return false;
    }
    record.clear();
    std::istringstream ss(recordStr);
    std::string token;
    while (std::getline(ss, token, ',')) {
        record.push_back(token);
    }
    return true;
}

std::streampos LRFBuffer::getCurrentFilePosition() {
    return inputFile.tellg();
}

void LRFBuffer::seekTo(std::streampos pos) {
    inputFile.clear();
    inputFile.seekg(pos);
}

void LRFBuffer::reset() {
    inputFile.clear();
    inputFile.seekg(0);
    readHeader();
}
