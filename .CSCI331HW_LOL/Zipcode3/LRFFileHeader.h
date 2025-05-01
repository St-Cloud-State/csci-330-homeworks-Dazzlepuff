#ifndef LRFFILEHEADER_H
#define LRFFILEHEADER_H

#include <string>
#include <vector>

struct LRFFieldInfo {
    std::string fieldName = "";
    std::string typeSchema = "";
    bool        isPrimaryKey = false;
};

struct LRFFileHeader {
    std::string fileStructureType;   /**< Type identifier (e.g., "ZIPCODE_DATA") */
    std::string version;             /**< Version of the file structure (e.g., "1.0") */
    size_t headerSize;               /**< Total size in bytes of the header record */
    size_t recordLengthFieldSize;    /**< Number of bytes used for each record length indicator */
    std::string sizeFormatType;      /**< Format type for record length (only "binary" is implemented for now) */
    std::string primaryKeyIndexFileName;  /**< Name of the primary key index file */
    size_t recordCount;              /**< Number of data records in the file */
    size_t fieldCount;               /**< Number of fields per record */
    std::vector<LRFFieldInfo> fields;   /**< Vector containing definitions for each field */
};

#endif
