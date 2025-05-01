#include "LRFToBSF.h"
#include "LRFHeaderRecordBuffer.h"
#include "LRFFileHeader.h"
#include "BSFFileHeader.h"
#include "BSFHeaderRecordBuffer.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cstdint>

const size_t BLOCK_SIZE = 5120;

struct BSFRecord {
    std::string rawData;
    int primaryKey;
};

// Extracts the primary key (Zip_Code) from the raw CSV line
int extractPrimaryKey(const std::string& line) {
    std::stringstream ss(line);
    std::string token;
    std::getline(ss, token, ','); // assumes Zip_Code is first field
    return std::stoi(token);
}

void LRFToBSF(const std::string& lrfFile, const std::string& bsfFile) {
    // 1) Read the LRF header
    LRFHeaderRecordBuffer lrfBuf(lrfFile);
    try {
        lrfBuf.readHeader();
    }
    catch (const std::exception& e) {
        std::cerr << "Error reading LRF header: " << e.what() << "\n";
        return;
    }

    // 2) Populate LRF header struct
    LRFFileHeader lrfHdr;
    lrfHdr.fileStructureType = lrfBuf.getFileType();
    lrfHdr.version = lrfBuf.getVersion();
    lrfHdr.recordLengthFieldSize = std::stoul(lrfBuf.getRecordLengthFieldSize());
    lrfHdr.sizeFormatType = lrfBuf.getSizeFormatType();
    lrfHdr.primaryKeyIndexFileName = lrfBuf.getPrimaryKeyIndexFileName();
    lrfHdr.recordCount = std::stoul(lrfBuf.getRecordCount());
    lrfHdr.fieldCount = std::stoul(lrfBuf.getFieldCount());

    for (auto& def : lrfBuf.getFieldDefinitions()) {
        std::stringstream ss(def);
        LRFFieldInfo fi;
        std::string pkFlag;
        std::getline(ss, fi.fieldName, ',');
        std::getline(ss, fi.typeSchema, ',');
        std::getline(ss, pkFlag, ',');
        fi.isPrimaryKey = (pkFlag == "1");
        lrfHdr.fields.push_back(fi);
    }

    // 3) Build BSF header
    BSFFileHeader bsfHdr;
    bsfHdr.recordLengthFieldSize = lrfHdr.recordLengthFieldSize;
    bsfHdr.indexFileName = lrfHdr.primaryKeyIndexFileName;
    bsfHdr.indexFileSchemaInfo = "Zip_Code→RBN (4‑byte binary)";
    bsfHdr.recordCount = lrfHdr.recordCount;
    bsfHdr.fieldCount = lrfHdr.fieldCount;
    bsfHdr.availListRBN = 2;
    bsfHdr.activeListRBN = 3;

    for (const auto& lf : lrfHdr.fields) {
        BSFFieldInfo bf;
        bf.fieldName = lf.fieldName;
        bf.typeSchema = lf.typeSchema;
        bf.isPrimaryKey = lf.isPrimaryKey;
        bsfHdr.fields.push_back(bf);
    }

    for (size_t i = 0; i < bsfHdr.fields.size(); ++i) {
        if (bsfHdr.fields[i].isPrimaryKey) {
            bsfHdr.primaryKey.name = bsfHdr.fields[i].fieldName;
            bsfHdr.primaryKey.ordinal = static_cast<int>(i);
            break;
        }
    }

    // 4) Read LRF records
    std::ifstream in(lrfFile, std::ios::binary);
    if (!in.is_open()) {
        std::cerr << "Failed to reopen LRF file for record reading.\n";
        return;
    }

    // Skip header lines (header size is unknown, so skip 7 + fieldCount)
    std::string skipLine;
    size_t skipLines = 7 + lrfHdr.fieldCount;
    for (size_t i = 0; i < skipLines; ++i) {
        std::getline(in, skipLine);
    }

    std::vector<BSFRecord> records;
    while (true) {
        char lenBytes[4];
        in.read(lenBytes, lrfHdr.recordLengthFieldSize);
        if (in.eof()) break;

        int32_t len = *reinterpret_cast<int32_t*>(lenBytes);
        if (len <= 0 || len > 4096) break;

        std::string data(len, '\0');
        in.read(&data[0], len);

        BSFRecord rec;
        rec.rawData = data;
        rec.primaryKey = extractPrimaryKey(data);
        records.push_back(rec);
    }
    in.close();

    std::sort(records.begin(), records.end(), [](const BSFRecord& a, const BSFRecord& b) {
        return a.primaryKey < b.primaryKey;
        });

    // 5) Write BSF file
    std::ofstream out(bsfFile, std::ios::binary | std::ios::in | std::ios::out);
    if (!out.is_open()) {
        out.open(bsfFile, std::ios::binary | std::ios::trunc);
    }

    // Write header
    BSFHeaderRecordBuffer bsfBuf(bsfFile);
    bsfBuf.setHeader(bsfHdr);
    bsfBuf.writeHeader();

    // Write blocks
    int blockCount = 0;
    int prevRBN = -1;
    size_t i = 0;

    while (i < records.size()) {
        std::ostringstream block;
        std::streampos blockStart = out.tellp();

        int recordCount = 0;
        int blockSizeUsed = 6; // 2B count, 2B prevRBN, 2B nextRBN

        // Reserve header space
        block.write("\0\0\0\0\0\0", 6);

        while (i < records.size()) {
            const std::string& rec = records[i].rawData;
            int len = static_cast<int>(rec.size());
            if (blockSizeUsed + 4 + len > BLOCK_SIZE) break;

            block.write(reinterpret_cast<const char*>(&len), 4);
            block.write(rec.data(), len);
            blockSizeUsed += 4 + len;
            ++i;
            ++recordCount;
        }

        std::string blockData = block.str();
        blockData.resize(BLOCK_SIZE, ' ');

        // Fill in header
        *reinterpret_cast<uint16_t*>(&blockData[0]) = static_cast<uint16_t>(recordCount);
        *reinterpret_cast<uint16_t*>(&blockData[2]) = static_cast<uint16_t>(prevRBN);
        *reinterpret_cast<uint16_t*>(&blockData[4]) = static_cast<uint16_t>(blockCount + 1);

        out.write(blockData.data(), BLOCK_SIZE);
        prevRBN = blockCount;
        ++blockCount;
    }

    bsfHdr.blockCount = blockCount;
    bsfBuf.setHeader(bsfHdr);
    bsfBuf.writeHeader(); // rewrite header with updated block count

    std::cout << "BSF data conversion complete. Wrote " << blockCount << " blocks to " << bsfFile << "\n";
}
