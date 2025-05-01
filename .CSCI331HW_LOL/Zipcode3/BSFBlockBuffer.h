#ifndef BSFBLOCKBUFFER_H
#define BSFBLOCKBUFFER_H

#include <vector>
#include <fstream>
#include <string>

class BSFBlockBuffer {
public:
    BSFBlockBuffer(size_t blockSize = 512);

    bool readBlock(std::ifstream& file, int rbn);
    int  getRecordCount() const;
    int  getNextRBN() const;
    int  getPrevRBN() const;
    std::vector<std::string> getAllRawRecords() const;

private:
    size_t blockSize;
    int recordCount;
    int prevRBN;
    int nextRBN;
    std::vector<std::string> rawRecords;
};

#endif
