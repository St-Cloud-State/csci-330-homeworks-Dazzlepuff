#include "BSFBlockBuffer.h"
#include <cstring>

BSFBlockBuffer::BSFBlockBuffer(size_t blkSize)
    : blockSize(blkSize), recordCount(0), prevRBN(-1), nextRBN(-1) {}

bool BSFBlockBuffer::readBlock(std::ifstream& file, int rbn) {
    std::vector<char> buffer(blockSize);
    file.seekg(rbn * blockSize);
    file.read(buffer.data(), blockSize);
    if (file.gcount() < 6) return false;

    recordCount = *reinterpret_cast<uint16_t*>(&buffer[0]);
    prevRBN = *reinterpret_cast<uint16_t*>(&buffer[2]);
    nextRBN = *reinterpret_cast<uint16_t*>(&buffer[4]);

    rawRecords.clear();
    size_t offset = 6;

    while (offset + 4 <= blockSize && rawRecords.size() < recordCount) {
        int len = *reinterpret_cast<int32_t*>(&buffer[offset]);
        offset += 4;

        if (offset + len > blockSize) break;
        rawRecords.emplace_back(&buffer[offset], len);
        offset += len;
    }

    return true;
}

int BSFBlockBuffer::getRecordCount() const { return recordCount; }
int BSFBlockBuffer::getNextRBN() const { return nextRBN; }
int BSFBlockBuffer::getPrevRBN() const { return prevRBN; }

std::vector<std::string> BSFBlockBuffer::getAllRawRecords() const {
    return rawRecords;
}
