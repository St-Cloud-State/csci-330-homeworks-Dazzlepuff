#ifndef BSF_HEADER_RECORD_BUFFER_H
#define BSF_HEADER_RECORD_BUFFER_H

#include "BSFFileHeader.h"

#include <string>
#include <vector>
#include <stdexcept>



class BSFHeaderRecordBuffer {
public:
    BSFHeaderRecordBuffer(const std::string& filePath);

    const BSFFileHeader& getHeader() const;
    void setHeader(const BSFFileHeader& hdr);

    void readHeader();
    void writeHeader();

    // Getter functions for the required header information
    std::string getFileType() const;
    std::string getVersion() const;
    size_t getRecordLengthFieldSize() const;
    std::string getSizeFormatType() const;
    std::string getPrimaryKeyIndexFileName() const;
    size_t getRecordCount() const;
    size_t getFieldCount() const;
    std::vector<std::string> getFieldDefinitions() const;

    // Setter functions for file type and version
    void setFileType(const std::string& newFileType);
    void setVersion(const std::string& newVersion);

private:
    std::string filePath;
    BSFFileHeader header;

    std::vector<std::string> split(const std::string& s, char delim);
};


#endif // BSF_HEADER_RECORD_BUFFER_H
