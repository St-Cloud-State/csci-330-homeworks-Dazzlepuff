/**
 * @file FileHeader.h
 * @brief Defines structures for file header metadata.
 *
 * This header defines the data structures that store metadata about the
 * file structure used for processing Zip Code records.
 */

#ifndef FILEHEADER_H
#define FILEHEADER_H

#include <string>
#include <vector>

 /**
  * @struct FieldInfo
  * @brief Stores information about a single field in a record.
  *
  * This structure holds the field name, its data type schema, and a flag
  * indicating if this field is the primary key.
  */
struct FieldInfo {
    std::string fieldName;   /**< Name or ID of the field */
    std::string typeSchema;  /**< Data type */
    bool isPrimaryKey;       /**< True if this field is the primary key */
};

/**
 * @struct FileHeader
 * @brief Contains metadata for the file structure.
 *
 * This structure is used to store header information for the structured
 * file format, including file type, version, size parameters, and field definitions.
 */
struct FileHeader {
    std::string fileStructureType;   /**< Type identifier (e.g., "ZIPCODE_DATA") */
    std::string version;             /**< Version of the file structure (e.g., "1.0") */
    size_t headerSize;               /**< Total size in bytes of the header record */
    size_t recordLengthFieldSize;    /**< Number of bytes used for each record length indicator */
    std::string sizeFormatType;      /**< Format type for record length (only "binary" is implemented for now) */
    std::string primaryKeyIndexFileName;  /**< Name of the primary key index file */
    size_t recordCount;              /**< Number of data records in the file */
    size_t fieldCount;               /**< Number of fields per record */
    std::vector<FieldInfo> fields;   /**< Vector containing definitions for each field */
};

#endif // FILEHEADER_H
