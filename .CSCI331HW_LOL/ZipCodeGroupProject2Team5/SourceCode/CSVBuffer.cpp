#include "CSVBuffer.h"

CSVBuffer::CSVBuffer(const std::string& filePath) {
    inputFile.open(filePath);
    if (!inputFile.is_open()) {
        throw std::runtime_error("Could not open file: " + filePath);
    }

    // Read the header line and store column names.
    std::string headerLine;
    if (std::getline(inputFile, headerLine)) {
        std::istringstream ss(headerLine);
        std::string token;
        while (std::getline(ss, token, ',')) {
            headers.push_back(token);
        }
    }
}

CSVBuffer::~CSVBuffer() {
    if (inputFile.is_open()) {
        inputFile.close();
    }
}

std::vector<std::string> CSVBuffer::getHeaders() const {
    return headers;
}

bool CSVBuffer::getNextRecord(std::vector<std::string>& record) {
    std::string line;
    if (std::getline(inputFile, line)) {
        record.clear();
        std::istringstream ss(line);
        std::string token;
        // Split the line on commas.
        while (std::getline(ss, token, ',')) {
            record.push_back(token);
        }
        return true;
    }
    return false;
}

void CSVBuffer::reset() {
    inputFile.clear();
    inputFile.seekg(0);
    // Re-read header
    headers.clear();
    std::string headerLine;
    if (std::getline(inputFile, headerLine)) {
        std::istringstream ss(headerLine);
        std::string token;
        while (std::getline(ss, token, ',')) {
            headers.push_back(token);
        }
    }
}
