#ifndef HEADERRECORDBUFFER_H
#define HEADERRECORDBUFFER_H

#include <string>
#include <vector>

/**
 * @brief The LRFHeaderRecordBuffer class handles reading and writing
 *        the header record of the structured (LRF) file.
 *
 * The header record is stored as plain text with each item on a separate line:
 *   Line 1: file structure type (e.g., ZIPCODE_DATA)
 *   Line 2: version (e.g., 1.0)
 *   Line 3: record length field size (e.g., 4)
 *   Line 4: size format type (e.g., binary)
 *   Line 5: primary key index file name (e.g., primary_index.txt)
 *   Line 6: record count (e.g., 40933)
 *   Line 7: field count (e.g., 6)
 *   Lines 8+: field definitions (each line formatted as: FieldName,TypeSchema,PrimaryKeyIndicator)
 */
class LRFHeaderRecordBuffer {
public:
    /// @brief Constructor that takes the file path of the LRF file.
    LRFHeaderRecordBuffer(const std::string& filePath);

    /// @brief Reads the header record from the file.
    void readHeader();

    /// @brief Writes the header record to the file.
    void writeHeader();

    // Getters for header fields.
    std::string getFileType() const;
    std::string getVersion() const;
    std::string getRecordLengthFieldSize() const;
    std::string getSizeFormatType() const;
    std::string getPrimaryKeyIndexFileName() const;
    std::string getRecordCount() const;
    std::string getFieldCount() const;
    std::vector<std::string> getFieldDefinitions() const;

    // Setters for header fields.
    void setFileType(const std::string& fileType);
    void setVersion(const std::string& version);

private:
    std::string filePath;
    // Header fields.
    std::string fileType;              // Line 1
    std::string version;               // Line 2
    std::string recordLengthFieldSize; // Line 3
    std::string sizeFormatType;        // Line 4
    std::string primaryKeyIndexFileName; // Line 5
    std::string recordCount;           // Line 6
    std::string fieldCount;            // Line 7
    std::vector<std::string> fieldDefinitions; // Lines 8+
};

#endif // HEADERRECORDBUFFER_H
