#ifndef BUFFER_H
#define BUFFER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

// Structure to hold individual Zip Code records
struct ZipCodeRecord {
    int zipCode;
    std::string placeName;
    std::string stateID;
    std::string county;
    double latitude;
    double longitude;
};

class Buffer {
private:
    std::vector<ZipCodeRecord> records; // Stores all records
    std::string filename;

public:
    // Constructor
    Buffer(const std::string& file) : filename(file) {}

    // Method to read and parse CSV file
    void loadCSV();

    // Method to get all records
    const std::vector<ZipCodeRecord>& getRecords() const {
        return records;
    }
};

#endif 
