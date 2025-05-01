// BPlusTree.h
#ifndef BPLUSTREE_H
#define BPLUSTREE_H

#include "Header.h"
#include "SequenceSetBlock.h"
#include "IndexBlock.h"
#include "Key.h"
#include <string>
#include <fstream>

class BPlusTree {
public:
    BPlusTree(const std::string& filename, int blkSize = 4096);
    void buildFromCSV(const std::string& csvFile);
    void dumpLeaf(int rbn);
    std::string searchState(const std::string& zip);
    std::string searchPlace(const std::string& zip);

    Header header;

private:
    std::string treeFileName;
    std::fstream treeFile;
    int blockSize;

    void writeHeader();
    void readHeader();

    void insertIntoLeaf(SequenceSetBlock& leaf, const Record& rec);
    void writeBlock(int rbn, const SequenceSetBlock& block);
    void readBlock(int rbn, SequenceSetBlock& block);

    void writeIndexBlock(int rbn, const IndexBlock& ib);
    void readIndexBlock(int rbn, IndexBlock& ib);

    void promoteToIndex(Key& promo, int leftRbn, int rightRbn);
    void splitIndexBlock(IndexBlock& iblock, int parentRbn);
};

#endif // BPLUSTREE_H