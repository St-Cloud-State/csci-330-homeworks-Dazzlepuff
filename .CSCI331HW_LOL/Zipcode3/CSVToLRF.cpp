#include "CSVToLRF.h"
#include "LRFFileHeader.h"
#include "LRFBuffer.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <string>
#include <cstdint>
#include <unordered_map>

void convertCSVToLRF(const std::string& csvFile, const std::string& outputFile) {
    // Open CSV file for reading.
    std::ifstream in(csvFile);
    if (!in.is_open()) {
        std::cerr << "Error opening CSV file: " << csvFile << std::endl;
        return;
    }

    // Read header line from CSV.
    std::string headerLine;
    std::getline(in, headerLine);
    std::istringstream headerStream(headerLine);
    std::string token;
    std::vector<std::string> headers;
    while (std::getline(headerStream, token, ',')) {
        headers.push_back(token);
    }

    // Build the LRFLRFFileHeader structure.
    LRFFileHeader fh;
    fh.fileStructureType = "ZIPCODE_DATA";
    fh.version = "1.0";
    fh.recordLengthFieldSize = sizeof(uint32_t);  // 4-byte binary length indicator.
    fh.sizeFormatType = "binary";
    // Set primaryKeyIndexFileName to match output file name but with .idx extension.
    std::string idxFileName = outputFile.substr(0, outputFile.find_last_of('.')) + ".idx";
    fh.primaryKeyIndexFileName = idxFileName;
    fh.fieldCount = headers.size();
    for (size_t i = 0; i < headers.size(); i++) {
        LRFFieldInfo fi;
        fi.fieldName = headers[i];
        fi.typeSchema = "string";
        fi.isPrimaryKey = (i == 0);
        fh.fields.push_back(fi);
    }

    // Read remaining CSV records into a vector.
    std::vector<std::string> records;
    std::string line;
    while (std::getline(in, line)) {
        records.push_back(line);
    }
    in.close();
    fh.recordCount = records.size();
    fh.headerSize = 0;

    // Open the output LRF file in binary mode.
    std::ofstream out(outputFile, std::ios::binary);
    if (!out.is_open()) {
        std::cerr << "Error opening output file: " << outputFile << std::endl;
        return;
    }

    // Create a header string
    std::ostringstream headerOut;
    headerOut << fh.fileStructureType << "\n"
        << fh.version << "\n"
        << fh.recordLengthFieldSize << "\n"
        << fh.sizeFormatType << "\n"
        << fh.primaryKeyIndexFileName << "\n"
        << fh.recordCount << "\n"
        << fh.fieldCount << "\n";
    for (const auto& field : fh.fields) {
        headerOut << field.fieldName << "," << field.typeSchema << "," << field.isPrimaryKey << "\n";
    }
    std::string headerStr = headerOut.str();
    fh.headerSize = headerStr.size();

    // Write the header size, then the header string.
    out.write(reinterpret_cast<const char*>(&fh.headerSize), sizeof(fh.headerSize));
    out.write(headerStr.c_str(), headerStr.size());

    // Write each CSV record with a preceding length indicator.
    for (const auto& record : records) {
        uint32_t recordLength = record.size();
        out.write(reinterpret_cast<const char*>(&recordLength), sizeof(recordLength));
        out.write(record.c_str(), record.size());
    }
    out.close();

    // Build the Primary Key Index --------------------------------------------
    // Open the newly created LRF file for reading via LRFBuffer.
    std::unordered_map<std::string, std::streampos> primaryKeyIndex;
    LRFBuffer buffer(outputFile);
    std::vector<std::string> rec;
    // LRFBuffer::readHeader() will have positioned the stream pointer just after the header.
    while (true) {
        // Get the current file position for current record.
        std::streampos pos = buffer.getCurrentFilePosition();
        if (!buffer.getNextRecord(rec))
            break;
        if (!rec.empty()) {
            std::string zip = rec[0];  // Assuming primary key is the first field.
            primaryKeyIndex[zip] = pos;
        }
    }

    // Write the index to a file (name based on LRF file but with .idx extension).
    std::ofstream idxOut(idxFileName, std::ios::binary);
    if (!idxOut.is_open()) {
        std::cerr << "Error opening index file: " << idxFileName << std::endl;
        return;
    }
    // Write the index as plain text: each line contains "zipCode fileOffset".
    for (const auto& entry : primaryKeyIndex) {
        idxOut << entry.first << " " << entry.second << "\n";
    }
    idxOut.close();
}
