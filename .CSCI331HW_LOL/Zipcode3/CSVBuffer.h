#ifndef CSVBUFFER_H
#define CSVBUFFER_H

#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <stdexcept>

/// @brief CSVBuffer reads records from a CSV file in order including header
class CSVBuffer {
public:
    /// @brief Constructor that opens the CSV file and reads the header.
    /// @param filePath Path to the CSV file.
    CSVBuffer(const std::string& filePath);

    /// @brief Destructor that closes the file stream.
    ~CSVBuffer();

    /// @brief Gets the column headers.
    /// @return A vector of strings containing the column header names.
    std::vector<std::string> getHeaders() const;

    /// @brief Reads the next CSV record into a vector of strings.
    /// @param record A vector that will contain the fields of the CSV record.
    /// @return True if a record was successfully read, false if EOF
    bool getNextRecord(std::vector<std::string>& record);

    /// @brief Resets the file pointer to the beginning of the file and re-reads the header.
    void reset();

private:
    std::ifstream inputFile;
    std::vector<std::string> headers;
};

#endif // CSVBUFFER_H
