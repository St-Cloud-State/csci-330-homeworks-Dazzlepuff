#include "CSVBuffer.h"
#include "LRFBuffer.h"
#include "CSVToLengthIndicated.h"
#include "HeaderRecordBuffer.h"
#include "StateExtremesReport.h"
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <unordered_map>

// Function to print CSV records using CSVBuffer.
void printCSVRecords(CSVBuffer& buffer) {
    std::vector<std::string> headerNames = buffer.getHeaders();
    std::cout << "Column Headers: ";
    for (const auto& header : headerNames) {
        std::cout << header << " | ";
    }
    std::cout << "\n\n";

    std::vector<std::string> record;
    while (buffer.getNextRecord(record)) {
        for (size_t i = 0; i < record.size() && i < headerNames.size(); ++i) {
            std::cout << headerNames[i] << " = " << record[i] << " | ";
        }
        std::cout << "\n";
    }
}

// Function to print LRF records using LRFBuffer.
void printLRFRecords(LRFBuffer& buffer) {
    std::vector<std::string> record;
    while (buffer.getNextRecord(record)) {
        for (const auto& field : record) {
            std::cout << field << " | ";
        }
        std::cout << "\n";
    }
}

// Function to read and display the header record.
void readHeaderRecord(const std::string& filename) {
    try {
        HeaderRecordBuffer headerBuffer(filename);
        headerBuffer.readHeader();
        std::cout << "\n--- Header Record ---\n";
        std::cout << "File Type: " << headerBuffer.getFileType() << "\n";
        std::cout << "Version: " << headerBuffer.getVersion() << "\n";
        std::cout << "Record Length Field Size: " << headerBuffer.getRecordLengthFieldSize() << "\n";
        std::cout << "Size Format Type: " << headerBuffer.getSizeFormatType() << "\n";
        std::cout << "Primary Key Index File Name: " << headerBuffer.getPrimaryKeyIndexFileName() << "\n";
        std::cout << "Record Count: " << headerBuffer.getRecordCount() << "\n";
        std::cout << "Field Count: " << headerBuffer.getFieldCount() << "\n";
        std::cout << "Field Definitions:\n";
        for (const auto& def : headerBuffer.getFieldDefinitions()) {
            std::cout << "  " << def << "\n";
        }
        std::cout << "----------------------\n";
    }
    catch (const std::exception& e) {
        std::cerr << "Error reading header: " << e.what() << "\n";
    }
}

// Function to update (write) the header record.
void writeHeaderRecord(const std::string& filename) {
    try {
        HeaderRecordBuffer headerBuffer(filename);
        headerBuffer.readHeader();

        std::cout << "\nCurrent File Type: " << headerBuffer.getFileType() << "\n";
        std::cout << "Current Version: " << headerBuffer.getVersion() << "\n";

        std::cin.ignore();
        std::string newFileType, newVersion;
        std::cout << "Enter new File Type: ";
        std::getline(std::cin, newFileType);
        std::cout << "Enter new Version: ";
        std::getline(std::cin, newVersion);

        headerBuffer.setFileType(newFileType);
        headerBuffer.setVersion(newVersion);

        headerBuffer.writeHeader();
        std::cout << "Header record updated successfully.\n";
    }
    catch (const std::exception& e) {
        std::cerr << "Error writing header: " << e.what() << "\n";
    }
}

// Function for searching by Zip Code primary key
void searchZipCodeRecords(const std::string& lrfFilename) {
    // Derive the index file name from the LRF file name.
    std::string idxFilename = lrfFilename.substr(0, lrfFilename.find_last_of('.')) + ".idx";

    // Load the primary key index from the index file into an unordered_map.
    std::unordered_map<std::string, std::streampos> primaryKeyIndex;
    std::ifstream idxIn(idxFilename, std::ios::binary);
    if (!idxIn.is_open()) {
        std::cerr << "Error: Could not open index file: " << idxFilename << "\n";
        return;
    }
    std::string zipKey;
    std::streampos pos;
    long long pos_ll;
    while (idxIn >> zipKey >> pos_ll) {
        primaryKeyIndex[zipKey] = static_cast<std::streampos>(pos_ll);
    }

    idxIn.close();

    // Prompt the user to enter one or more Zip Code flags.
    std::cin.ignore();
    std::cout << "Enter Zip Code flags (e.g., -Z56301 -Z12345), separated by spaces: ";
    std::string input;
    std::getline(std::cin, input);
    std::istringstream iss(input);
    std::vector<std::string> zipFlags;
    std::string token;
    while (iss >> token) {
        zipFlags.push_back(token);
    }

    // Open the LRF file using LRFBuffer.
    LRFBuffer buffer(lrfFilename);
    // Get header information
    std::vector<std::string> headerNames = buffer.getHeaders();

    // For each Zip Code flag, search the index.
    for (const auto& flag : zipFlags) {
        // Expect flag format to be "-Z<zipCode>", so strip off "-Z". Probably couldve just had user type Zips but thought I'd follow assignment example closer
        std::string zipSearch;
        if (flag.substr(0, 2) == "-Z") {
            zipSearch = flag.substr(2);
        }
        else {
            zipSearch = flag;
        }

        auto it = primaryKeyIndex.find(zipSearch);
        if (it == primaryKeyIndex.end()) {
            std::cout << "Zip Code " << zipSearch << " not found in index.\n";
        }
        else {
            // Use the index to seek to the record in the LRF file.
            buffer.seekTo(it->second);
            std::vector<std::string> record;
            if (buffer.getNextRecord(record)) {
                std::cout << "Record for Zip Code " << zipSearch << ":\n";
                // Display each field with its header label.
                for (size_t i = 0; i < record.size() && i < headerNames.size(); ++i) {
                    std::cout << headerNames[i] << ": " << record[i] << " | ";
                }
                std::cout << "\n";
            }
            else {
                std::cout << "Error reading record for Zip Code " << zipSearch << ".\n";
            }
        }
    }
}

int main() {
    try {
        int choice;
        std::cout << "1. Print CSV Records\n";
        std::cout << "2. Print LRF Records\n";
        std::cout << "3. Convert CSV to LRF\n";
        std::cout << "4. Read Header Record\n";
        std::cout << "5. Write (Update) Header Record\n";
        std::cout << "6. Run State Extremes Report\n";
        std::cout << "7. Search for Zip Code Records by Index\n";
        std::cout << "Enter your choice: ";
        std::cin >> choice;

        std::string filename;
        std::string lrfFilename;

        switch (choice) {
        case 1:
            std::cout << "Enter the CSV file name to print records from: ";
            std::cin >> filename;
            if (filename.substr(filename.size() - 4) != ".csv")
                filename += ".csv";
            {
                CSVBuffer csvBuffer(filename);
                printCSVRecords(csvBuffer);
            }
            break;
        case 2:
            std::cout << "Enter the LRF file name to print records from: ";
            std::cin >> filename;
            if (filename.substr(filename.size() - 4) != ".lrf")
                filename += ".lrf";
            {
                LRFBuffer lrfBuffer(filename);
                printLRFRecords(lrfBuffer);
            }
            break;
        case 3:
            std::cout << "Enter the CSV file name to convert to LRF: ";
            std::cin >> filename;
            if (filename.substr(filename.size() - 4) != ".csv")
                filename += ".csv";
            std::cout << "Enter the LRF file name (without extension) to save as .lrf (Leave empty to use the same name as the CSV): ";
            std::cin.ignore();
            std::getline(std::cin, lrfFilename);
            if (lrfFilename.empty())
                lrfFilename = filename.substr(0, filename.size() - 4) + ".lrf";
            else
                lrfFilename += ".lrf";
            writeStructuredFile(filename, lrfFilename);
            std::cout << "Conversion complete. Structured file created as '" << lrfFilename << "'.\n";
            break;
        case 4:
            std::cout << "Enter the LRF file name to read the header from: ";
            std::cin >> filename;
            if (filename.substr(filename.size() - 4) != ".lrf")
                filename += ".lrf";
            readHeaderRecord(filename);
            break;
        case 5:
            std::cout << "Enter the LRF file name to update the header: ";
            std::cin >> filename;
            if (filename.substr(filename.size() - 4) != ".lrf")
                filename += ".lrf";
            writeHeaderRecord(filename);
            break;
        case 6:
            std::cout << "Enter the LRF file name to run state extremes report from: ";
            std::cin >> filename;
            if (filename.substr(filename.size() - 4) != ".lrf")
                filename += ".lrf";
            runStateExtremesReport(filename);
            break;
        case 7:
            std::cout << "Enter the LRF file name to search in: ";
            std::cin >> filename;
            if (filename.substr(filename.size() - 4) != ".lrf")
                filename += ".lrf";
            searchZipCodeRecords(filename);
            break;
        default:
            std::cout << "Invalid choice!\n";
            break;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }
    return 0;
}