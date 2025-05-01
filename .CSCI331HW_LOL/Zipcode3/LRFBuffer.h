#ifndef LRFBUFFER_H
#define LRFBUFFER_H

#include <fstream>
#include <string>
#include <vector>
#include <stdexcept>

/**
 * @brief LRFBuffer reads records from a structured file
 *        that begins with a header record and has length-indicated records.
 *
 * The file format is assumed to be:
 *   1. A binary header size (stored as size_t).
 *   2. A header string (in ASCII) of that many bytes.
 *      The header string contains:
 *         - Line 1: fileStructureType
 *         - Line 2: version
 *         - Line 3: recordLengthFieldSize
 *         - Line 4: sizeFormatType
 *         - Line 5: primaryKeyIndexFileName
 *         - Line 6: recordCount
 *         - Line 7: fieldCount
 *         - Lines 8+: for each field: fieldName,typeSchema,isPrimaryKey
 *   3. For each record:
 *         - A binary record length (e.g., a 4-byte unsigned int)
 *         - The record text (a comma-separated string) of that length.
 */
class LRFBuffer {
public:
    /// @brief Constructor that opens the structured file and reads the header.
    /// @param filePath Path to the structured file.
    LRFBuffer(const std::string& filePath);

    /// @brief Destructor that closes the file stream.
    ~LRFBuffer();

    /// @brief Returns the column headers (extracted from the header record).
    std::vector<std::string> getHeaders() const;

    /// @brief Reads the next record from the file.
    /// @param record A vector that will contain the fields of the record.
    /// @return True if a record was successfully read; false if end-of-file is reached.
    bool getNextRecord(std::vector<std::string>& record);

    std::streampos getCurrentFilePosition();

    void seekTo(std::streampos pos); 

    /// @brief Resets the file pointer to the beginning and re-reads the header.
    void reset();

private:
    /// @brief Reads and parses the header record, extracting the column names.
    void readHeader();

    std::ifstream inputFile;
    std::vector<std::string> headers;   // Extracted column headers from the header record.
    std::streampos headerEndPosition;   // File position immediately after the header.
};

#endif // LRFBUFFER_H
