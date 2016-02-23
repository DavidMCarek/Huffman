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
		char value;
		HuffmanNode* leftChild = nullptr;
		HuffmanNode* rightChild = nullptr;
	};
	bool nodesNeedMerging(HuffmanNode* nodes[]);
	HuffmanNode* getMinNode(HuffmanNode* nodes[]);
};

