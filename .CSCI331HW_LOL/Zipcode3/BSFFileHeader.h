#ifndef BSFFILEHEADER_H
#define BSFFILEHEADER_H

#include <string>
#include <vector>

struct BSFFieldInfo {
    std::string fieldName = "";
    std::string typeSchema = "";
    bool        isPrimaryKey = false;
};

struct BSFFileHeader {
    // 1. File structure type
    std::string fileStructureType = "BSF"; // e.g., "blocked sequence set with comma separated fields"

    // 2. Version of the file structure
    std::string version = "1.0";

    // 3. Header record size
    size_t headerSize = 256;

    // 4. Record length field size (in bytes)
    size_t recordLengthFieldSize;

    // 5. Size format type: "binary" or "ASCII"
    std::string sizeFormatType = "binary";

    // 6. Block size in bytes
    size_t blockSize = 512;

    // 7. Minimum block fill ratio (as a float)
    float minBlockFill = 0.5;

    // 8. Index file name (e.g., "us_postal_codes.idx")
    std::string indexFileName;

    // 9. Index file schema description (optional detail string)
    std::string indexFileSchemaInfo = "Primary key index (Zip_Code), with RBN and stale flags";

    // 10. Record count
    size_t recordCount = 0;

    // 11. Block count
    size_t blockCount = 0;

    // 12. Field count per record
    size_t fieldCount = 0;

    // 13. Field metadata
    std::vector<BSFFieldInfo> fields;

    // 14. Primary key configuration
    struct PrimaryKeyInfo {
        std::string name = "Zip_Code";  /**< Field name used as the primary key */
        int ordinal = 0;                /**< Index of the primary key field */
        size_t rbnOffset = 2;           /**< Offset to store the RBN (Relative Block Number) */
        size_t rbnLength = 5;           /**< Length of RBN link */
        int staleFlagPosition = 0;      /**< Position/index of the stale flag in the record */
    } primaryKey;

    // 15. Avail-list RBN pointer
    int availListRBN = -1;

    // 16. Active sequence set RBN pointer (e.g., RBN of the first block in active list)
    int activeListRBN = 1;
};

#endif