#include "BSFBlockBuffer.h"
#include "BSFRecordBuffer.h"
#include "BSFRecord.h"
#include "CSVBuffer.h"
#include "LRFBuffer.h"
#include "CSVToLRF.h"
#include "LRFToBSF.h"
#include "LRFHeaderRecordBuffer.h"
#include "BSFHeaderRecordBuffer.h"
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
void readLRFHeaderRecord(const std::string& filename) {
    try {
        LRFHeaderRecordBuffer LRFHeaderBuffer(filename);
        LRFHeaderBuffer.readHeader();
        std::cout << "\n--- Header Record ---\n";
        std::cout << "File Type: " << LRFHeaderBuffer.getFileType() << "\n";
        std::cout << "Version: " << LRFHeaderBuffer.getVersion() << "\n";
        std::cout << "Record Length Field Size: " << LRFHeaderBuffer.getRecordLengthFieldSize() << "\n";
        std::cout << "Size Format Type: " << LRFHeaderBuffer.getSizeFormatType() << "\n";
        std::cout << "Primary Key Index File Name: " << LRFHeaderBuffer.getPrimaryKeyIndexFileName() << "\n";
        std::cout << "Record Count: " << LRFHeaderBuffer.getRecordCount() << "\n";
        std::cout << "Field Count: " << LRFHeaderBuffer.getFieldCount() << "\n";
        std::cout << "Field Definitions:\n";
        for (const auto& def : LRFHeaderBuffer.getFieldDefinitions()) {
            std::cout << "  " << def << "\n";
        }
        std::cout << "----------------------\n";
    }
    catch (const std::exception& e) {
        std::cerr << "Error reading header: " << e.what() << "\n";
    }
}

// Function to update (write) the header record.
void writeLRFHeaderRecord(const std::string& filename) {
    try {
        LRFHeaderRecordBuffer LRFHeaderBuffer(filename);
        LRFHeaderBuffer.readHeader();

        std::cout << "\nCurrent File Type: " << LRFHeaderBuffer.getFileType() << "\n";
        std::cout << "Current Version: " << LRFHeaderBuffer.getVersion() << "\n";

        std::cin.ignore();
        std::string newFileType, newVersion;
        std::cout << "Enter new File Type: ";
        std::getline(std::cin, newFileType);
        std::cout << "Enter new Version: ";
        std::getline(std::cin, newVersion);

        LRFHeaderBuffer.setFileType(newFileType);
        LRFHeaderBuffer.setVersion(newVersion);

        LRFHeaderBuffer.writeHeader();
        std::cout << "Header record updated successfully.\n";
    }
    catch (const std::exception& e) {
        std::cerr << "Error writing header: " << e.what() << "\n";
    }
}

void readBSFHeaderRecord(const std::string& filename) {
    try {
        BSFHeaderRecordBuffer BSFHeaderBuffer(filename);
        BSFHeaderBuffer.readHeader();
        std::cout << "\n--- Header Record ---\n";
        std::cout << "File Type: " << BSFHeaderBuffer.getFileType() << "\n";
        std::cout << "Version: " << BSFHeaderBuffer.getVersion() << "\n";
        std::cout << "Record Length Field Size: " << BSFHeaderBuffer.getRecordLengthFieldSize() << "\n";
        std::cout << "Size Format Type: " << BSFHeaderBuffer.getSizeFormatType() << "\n";
        std::cout << "Primary Key Index File Name: " << BSFHeaderBuffer.getPrimaryKeyIndexFileName() << "\n";
        std::cout << "Record Count: " << BSFHeaderBuffer.getRecordCount() << "\n";
        std::cout << "Field Count: " << BSFHeaderBuffer.getFieldCount() << "\n";
        std::cout << "Field Definitions:\n";
        for (const auto& def : BSFHeaderBuffer.getFieldDefinitions()) {
            std::cout << "  " << def << "\n";
        }
        std::cout << "----------------------\n";
    }
    catch (const std::exception& e) {
        std::cerr << "Error reading BSF header: " << e.what() << "\n";
    }
}

// Function to update (write) the BSF header record.
void writeBSFHeaderRecord(const std::string& filename) {
    try {
        BSFHeaderRecordBuffer BSFHeaderBuffer(filename);
        BSFHeaderBuffer.readHeader();

        std::cout << "\nCurrent File Type: " << BSFHeaderBuffer.getFileType() << "\n";
        std::cout << "Current Version: " << BSFHeaderBuffer.getVersion() << "\n";

        std::cin.ignore();
        std::string newFileType, newVersion;
        std::cout << "Enter new File Type: ";
        std::getline(std::cin, newFileType);
        std::cout << "Enter new Version: ";
        std::getline(std::cin, newVersion);

        BSFHeaderBuffer.setFileType(newFileType);
        BSFHeaderBuffer.setVersion(newVersion);

        BSFHeaderBuffer.writeHeader();
        std::cout << "BSF header record updated successfully.\n";
    }
    catch (const std::exception& e) {
        std::cerr << "Error writing BSF header: " << e.what() << "\n";
    }
}

// Function for searching by Zip Code primary key
void searchLRFZipCodeRecords(const std::string& convertedFilename) {
    // Derive the index file name from the LRF file name.
    std::string idxFilename = convertedFilename.substr(0, convertedFilename.find_last_of('.')) + ".idx";

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
    LRFBuffer buffer(convertedFilename);
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

void displayBlockInfo(const BSFBlockBuffer& blockBuffer, int rbn) {
    // Display which block is being read
    std::cout << "Reading Block " << rbn << ":\n";

    // Display block metadata
    std::cout << "  Record Count: " << blockBuffer.getRecordCount() << "\n";
    std::cout << "  Previous RBN: " << blockBuffer.getPrevRBN() << "\n";
    std::cout << "  Next RBN: " << blockBuffer.getNextRBN() << "\n";

    // Check the number of records to verify if rawRecords is populated
    const auto& records = blockBuffer.getAllRawRecords();
    std::cout << "  Number of Raw Records: " << records.size() << "\n"; // Debugging line

    // Display all raw records in the block
    if (records.empty()) {
        std::cout << "  No raw records found in this block.\n";
    }
    else {
        for (size_t i = 0; i < records.size(); ++i) {
            std::cout << "  Raw Record " << i + 1 << ": " << records[i] << "\n";
        }
    }
}

void searchZipCodeInBSF(const std::string& filename, const std::string& zipCode) {
    std::ifstream file(filename, std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open BSF file: " << filename << std::endl;
        return;
    }

    size_t blockSize = 1024;  // Set block size for BSF format
    BSFBlockBuffer blockBuffer(blockSize);

    int rbn = 0;  // Start at the first block
    bool found = false;

    while (blockBuffer.readBlock(file, rbn)) {
        // Check the raw records for the zip code
        const auto& records = blockBuffer.getAllRawRecords();

        for (size_t i = 0; i < records.size(); ++i) {
            if (records[i].find(zipCode) != std::string::npos) {
                std::cout << "Found Zip Code " << zipCode << " in BSF Block " << rbn << ":\n";
                displayBlockInfo(blockBuffer, rbn);  // Display block details
                found = true;
                break;  // Stop after finding the first match
            }
        }

        if (found) {
            break;  // Exit the loop after finding a match
        }

        rbn = blockBuffer.getNextRBN();  // Move to the next block
        if (rbn == -1) {
            break;  // No more blocks to read
        }
    }

    if (!found) {
        std::cout << "Zip Code " << zipCode << " not found in the BSF file.\n";
    }

    file.close();  // Close the BSF file
}

void logEvent(const std::string& message) {
    std::cout << "[LOG] " << message << std::endl;
}

// Simulate index modification
void modifyIndexStub() {
    logEvent("Index modified.");
    // Optionally, dump index here
}

// Simulate dumping the index
void dumpIndexStub() {
    std::cout << "Dumping index (stubbed).\n";
}

// Simulate block dump
void dumpBSFBlock(std::ifstream& file, int rbn, size_t blockSize) {
    BSFBlockBuffer blockBuffer(blockSize);
    if (blockBuffer.readBlock(file, rbn)) {
        std::cout << "=== Dump of BSF Block " << rbn << " ===\n";
        std::cout << "Record Count: " << blockBuffer.getRecordCount() << "\n";
        for (const auto& record : blockBuffer.getAllRawRecords()) {
            std::cout << "  Record: " << record << "\n";
        }
    }
}

void addRecordsToBSF(const std::string& bsfFilename, const std::string& recordsFilename) {
    std::ifstream inputFile(recordsFilename);
    if (!inputFile.is_open()) {
        std::cerr << "Failed to open record input file: " << recordsFilename << std::endl;
        return;
    }

    std::string line;
    while (std::getline(inputFile, line)) {
        // Insert logic to add record to BSF
        // - If block is full → splitBlock() and log
        // - If index is updated → modifyIndexStub()

        std::cout << "Adding record: " << line << "\n";

        // EXAMPLE: simulate a block split
        if (line == "TRIGGER_SPLIT") {
            logEvent("Block split occurred.");
            modifyIndexStub();
            dumpIndexStub();
        }
    }
}

void deleteRecordsFromBSF(const std::string& bsfFilename, const std::string& keysFilename) {
    std::ifstream inputFile(keysFilename);
    if (!inputFile.is_open()) {
        std::cerr << "Failed to open deletion key file: " << keysFilename << std::endl;
        return;
    }

    std::string key;
    while (std::getline(inputFile, key)) {
        // Insert logic to delete record by key (e.g., zip code)
        // - If block is merged or redistributed → log
        // - If index is updated → modifyIndexStub()

        std::cout << "Deleting record with key: " << key << "\n";

        // EXAMPLE: simulate a merge
        if (key == "TRIGGER_MERGE") {
            logEvent("Block merge or redistribution occurred.");
            modifyIndexStub();
            dumpIndexStub();
        }
    }
}

int main() {
    try {
        int choice;
        std::cout << "1. Print CSV Records\n";
        std::cout << "2. Print LRF Records\n";
        std::cout << "3. Convert CSV to LRF\n";
        std::cout << "4. Read LRF Header Record\n";
        std::cout << "5. Write (Update) LRF Header Record\n";
        std::cout << "6. Run State Extremes Report\n";
        std::cout << "7. Search for LRF Zip Code Records by Index\n";
        std::cout << "8. Convert LRF to BSF\n";
        std::cout << "9. Print BSF Records\n";
        std::cout << "10. Read BSF Header Record\n"; // New option for BSF header reading
        std::cout << "11. Write (Update) BSF Header Record\n"; // New option for BSF header writing
        std::cout << "12. Dump File Contents (Block Dump)\n"; // New case for dumping file contents
        std::cout << "14. Search for BSF zipcode\n";
        std::cout << "14. Add Records to BSF\n";
        std::cout << "15. Delete Records from BSF\n";

        std::cout << "Enter your choice: ";
        std::cin >> choice;

        std::string filename;
        std::string convertedFilename;

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
            std::getline(std::cin, convertedFilename);
            if (convertedFilename.empty())
                convertedFilename = filename.substr(0, filename.size() - 4) + ".lrf";
            else
                convertedFilename += ".lrf";
            convertCSVToLRF(filename, convertedFilename);
            std::cout << "Conversion complete. Structured file created as '" << convertedFilename << "'.\n";
            break;
        case 4:
            std::cout << "Enter the LRF file name to read the header from: ";
            std::cin >> filename;
            if (filename.substr(filename.size() - 4) != ".lrf")
                filename += ".lrf";
            readLRFHeaderRecord(filename);
            break;
        case 5:
            std::cout << "Enter the LRF file name to update the header: ";
            std::cin >> filename;
            if (filename.substr(filename.size() - 4) != ".lrf")
                filename += ".lrf";
            writeLRFHeaderRecord(filename);
            break;
        case 6:
            std::cout << "Enter the LRF or BSF file name to run state extremes report from: ";
            std::cin >> filename;
            runStateExtremesReport(filename);
            break;
        case 7:
            std::cout << "Enter the LRF file name to search in: ";
            std::cin >> filename;
            if (filename.substr(filename.size() - 4) != ".lrf")
                filename += ".lrf";
            searchLRFZipCodeRecords(filename);
            break;
        case 8:
            std::cout << "Enter the LRF file name to convert to BSF: ";
            std::cin >> filename;
            if (filename.substr(filename.size() - 4) != ".lrf")
                filename += ".lrf";
            std::cout << "Enter the BSF file name (without extension) to save as .bsf (Leave empty to use the same name as the LRF): ";
            std::cin.ignore();
            std::getline(std::cin, convertedFilename);
            if (convertedFilename.empty())
                convertedFilename = filename.substr(0, filename.size() - 4) + ".bsf";
            else
                convertedFilename += ".bsf";
            LRFToBSF(filename, convertedFilename);
            std::cout << "Conversion complete. Structured file created as '" << convertedFilename << "'.\n";
            break;
        case 9:
            std::cout << "Enter the BSF file name to read records from: ";
            std::cin >> filename;
            if (filename.substr(filename.size() - 4) != ".bsf")
                filename += ".bsf";
            {
                std::ifstream bsfIn(filename, std::ios::binary);
                if (!bsfIn.is_open()) {
                    std::cerr << "Error opening BSF file.\n";
                    break;
                }

                int rbn = 3; // Start at active list root block
                BSFBlockBuffer block;
                int recordTotal = 0;

                while (block.readBlock(bsfIn, rbn)) {
                    std::vector<std::string> rawRecords = block.getAllRawRecords();

                    for (const auto& raw : rawRecords) {
                        BSFRecordBuffer recBuf(raw);
                        ZipCodeRecord rec = recBuf.unpack();
                        std::cout << "Zip: " << rec.zip
                            << ", Place: " << rec.place
                            << ", State: " << rec.state
                            << ", County: " << rec.county
                            << ", Lat: " << rec.lat
                            << ", Long: " << rec.lon
                            << "\n";
                        ++recordTotal;
                    }

                    rbn = block.getNextRBN();
                    if (rbn <= 0) break;
                }

                std::cout << "\nRead " << recordTotal << " total records from BSF file.\n";
            }
            break;

        case 10:
            std::cout << "Enter the BSF file name to read the header from: ";
            std::cin >> filename;
            if (filename.substr(filename.size() - 4) != ".bsf")
                filename += ".bsf";
            readBSFHeaderRecord(filename);
            break;
        case 11:
            std::cout << "Enter the BSF file name to update the header: ";
            std::cin >> filename;
            if (filename.substr(filename.size() - 4) != ".bsf")
                filename += ".bsf";
            writeBSFHeaderRecord(filename);
            break;
        case 12: {
            std::string filename;
            std::cout << "Enter the filename to dump: ";
            std::cin >> filename;

            std::ifstream file(filename, std::ios::binary);

            if (!file.is_open()) {
                std::cerr << "Error: Could not open file: " << filename << std::endl;
                break;
            }

            // Define block size (adjust based on your actual block size)
            size_t blockSize = 1024; // Adjust based on your block size
            BSFBlockBuffer blockBuffer(blockSize);

            int rbn = 0;  // Start from the first block
            while (blockBuffer.readBlock(file, rbn)) {
                // Call the function to display block info
                displayBlockInfo(blockBuffer, rbn);

                std::cout << "\n";  // Print a newline after each block
                rbn = blockBuffer.getNextRBN();  // Move to the next block based on the next RBN

                if (rbn == -1) {
                    break;  // Stop if there are no more blocks to read
                }
            }

            file.close();  // Close the file after reading
            break;
        }
        case 13: {
            std::string zipCode = "";

            std::cout << "Enter the BSF file name to search for zip codes: ";
            std::cin >> filename;

            std::cout << "Enter the zip code to search for: ";
            std::cin >> zipCode;

            // Perform the search in the BSF file
            searchZipCodeInBSF(filename, zipCode);
        }
        case 14: {  // Add records to BSF
            std::string bsfFile, recFile;
            std::cout << "Enter the BSF file name to add records to: ";
            std::cin >> bsfFile;
            if (bsfFile.size() < 4 || bsfFile.substr(bsfFile.size() - 4) != ".bsf")
                bsfFile += ".bsf";

            std::cout << "Enter the text file containing records to add: ";
            std::cin >> recFile;
            // optionally ensure .txt or .csv, etc.

            addRecordsToBSF(bsfFile, recFile);
            break;
        }

        case 15: {  // Delete records from BSF
            std::string bsfFile, keyFile;
            std::cout << "Enter the BSF file name to delete records from: ";
            std::cin >> bsfFile;
            if (bsfFile.size() < 4 || bsfFile.substr(bsfFile.size() - 4) != ".bsf")
                bsfFile += ".bsf";

            std::cout << "Enter the text file containing keys of records to delete: ";
            std::cin >> keyFile;
            // optionally ensure .txt

            deleteRecordsFromBSF(bsfFile, keyFile);
            break;
        }
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