#pragma once
#include<string>

class Huffman
{
public:
	Huffman();
	~Huffman();
	void intializeFromFile(std::string fileName);
	void encodeFile(std::string inFile, std::string outFile);
	void decodeFile(std::string inFile, std::string outFile);
private:
	struct HuffmanNode
	{
		int weight = 0;
		unsigned char value;
		HuffmanNode* leftChild = nullptr;
		HuffmanNode* rightChild = nullptr;
	};
	HuffmanNode* root;
	bool nodesNeedMerging(HuffmanNode* nodes[256]);
	HuffmanNode* getMinNode(HuffmanNode* nodes[256]);
	std::string binaryPaths[256];
	void setEncodingStrings(HuffmanNode* node);
	void traverse(HuffmanNode* node);
	std::string currentPath;
	std::string getPaddingBits(int numberOfBitsNeeded);
};

