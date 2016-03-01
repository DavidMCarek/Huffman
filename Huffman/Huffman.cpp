#include "stdafx.h"
#include "Huffman.h"
#include <fstream>
#include <string>
#include <iostream>
#include <iomanip>
#include <xtgmath.h>
#include <ctime>

Huffman::Huffman()
{
}


Huffman::~Huffman()
{
}

// set up the tree and encoding strings
void Huffman::intializeFromFile(std::string fileName)
{
	time_t startTime = time(0);
	int bytesRead = 0;
	HuffmanNode* nodes[256] = {};

	// initialize all nodes of the array
	for (int i = 0; i < 256; i++)
	{
		nodes[i] = new HuffmanNode();
		nodes[i]->value = i;
	}

	char nextChar;
	std::ifstream inputStream;
	inputStream.open(fileName, std::ios::binary);

	if (inputStream.fail())
	{
		std::cout << "Could not open initalization file" << std::endl;
		return;
	}

	// get all characters from file and since every character is one byte increment bytes 
	// read for every char read
	while (inputStream.get(nextChar)) 
	{
		if (inputStream.eof())
			break;
		nodes[nextChar]->weight++;
		bytesRead++;
	}

	inputStream.close();

	// while there are still nodes to be grouped get the two smallest and create a root for the 
	// subtree and set the pointers for that roots children
	// then set that root to an empty slot in the array (the getMinNode method will free up slots in the array)
	while (nodesNeedMerging(nodes))
	{
		HuffmanNode* min1 = getMinNode(nodes);
		HuffmanNode* min2 = getMinNode(nodes);

		HuffmanNode* subtreeRoot = new HuffmanNode();
		subtreeRoot->weight = min1->weight + min2->weight;
		subtreeRoot->leftChild = min1;
		subtreeRoot->rightChild = min2;

		for (int i = 0; i < 256; i++)
		{
			if (nodes[i] == nullptr)
			{
				nodes[i] = subtreeRoot;
				break;
			}
		}
	}

	root = nodes[0];

	// tree is now built, create encoding strings
	// we only need the first node of the array to be passed since it is the root
	setEncodingStrings(root);

	time_t finishTime = time(0);

	std::cout << "Bytes read: " << bytesRead << std::endl;
	std::cout << "Elapsed Initialization Time: " << finishTime - startTime << " s" << std::endl;
}

// create the paths to the leaves
void Huffman::setEncodingStrings(HuffmanNode* node)
{
	// clear the current path and initialize the array to hold encoding strings
	currentPath = "";
	binaryPaths[256] = {};
	// traverse till leaf and store the path in the binary paths string array
	traverse(node);
}

// recurse through the tree to create encoding strings
void Huffman::traverse(HuffmanNode* node)
{
	// if for some reason we pass a null node in we cant proceed
	if (node == nullptr)
		return;

	// if the left child is not null go left and add a 0 to current path to indicate left traversal
	if (node->leftChild != nullptr)
	{
		currentPath.push_back('0');
		traverse(node->leftChild);

		// once this is reached a full path has been stored so we need to undo the go left
		// on our current path
		currentPath.pop_back();
	}
	// if the right child is not null go right and add a 1 to current path to indicate right traversal
	if (node->leftChild != nullptr)
	{
		currentPath.push_back('1');
		traverse(node->rightChild);

		// same a first if but now we are undoing the go right of current path
		currentPath.pop_back();
	}

	// now that we are at a leaf we need to store the encoding string for that leaf
	// had to do a null check here because of some funky business with overwriting the null character's path
	if (node->leftChild == nullptr && node->rightChild == nullptr)
		binaryPaths[node->value] = currentPath;

}

// lets us know when there is only one root left
bool Huffman::nodesNeedMerging(HuffmanNode* nodes[256])
{
	// if more than 1 node is left in the array then we need to continue merging
	int nodeCount = 0;
	for each (HuffmanNode* node in nodes)
	{
		if (node != nullptr)
		{
			nodeCount++;
			if (nodeCount > 1)
				return true;
		}
	}

	return false;
}

// returns the node in the array with the lowest weight
Huffman::HuffmanNode* Huffman::getMinNode(HuffmanNode* nodes[256])
{
	HuffmanNode* smallestWeightNode = nullptr;
	int locationOfSmallestNode;

	// iterate through the arrray comparing the weights of the nodes to get the 
	// one with the lowest weight
	for (int i = 0; i < 256; i++)
	{
		if (nodes[i] == nullptr)
			continue;
		if (smallestWeightNode == nullptr)
		{
			smallestWeightNode = nodes[i];
			locationOfSmallestNode = i;
		}
		else if (smallestWeightNode->weight > nodes[i]->weight)
		{
			smallestWeightNode = nodes[i];
			locationOfSmallestNode = i;
		}
	}

	// remove the node being returned from the array
	nodes[locationOfSmallestNode] = nullptr;

	return smallestWeightNode;
}

// encodes the input file based on the table of string paths
void Huffman::encodeFile(std::string inputFile, std::string outputFile)
{
	time_t startTime = time(0);
	std::ofstream outputStream;
	outputStream.open(outputFile, std::ios::binary);
	char nextChar;
	std::ifstream inputStream;
	inputStream.open(inputFile, std::ios::binary);

	if (inputStream.fail())
		return;
	

	std::string bitsToWrite = "";
	std::string nextByte = "";
	unsigned char byte;

	int bytesEncoded = 0;
	int bytesRead = 0;

	// while there are still characters left in the input file to get continue getting them
	// and converting them to binary to add to the output file
	while (true)
	{
		// if the number of bits to write is less than 8 we need to get the next character so that we can 
		// write at least one byte in the upcoming loop
		
		inputStream.get(nextChar);

		if (inputStream.eof())
		{
			// we need to handle the case where we are at the end of the file and we need padding bits for the last byte
			if (bitsToWrite.length() > 0)
				bitsToWrite.append(getPaddingBits(8 - bitsToWrite.length()));
			else break;
		}
		else
		{
			bitsToWrite.append(binaryPaths[nextChar]);
			bytesRead++;
		}
		

		// if we have 8 or more bits we can write a char to the file so we need to iterate through the bits to 
		// create the byte that needs to be written
		while (bitsToWrite.length() > 7)
		{
			byte = 0;

			// iterate 8 times over the bits to generate the byte that will be written
			for (int i = 8; i > 0; i--)
			{
				// will shift any previous bit out of the way for the new one coming in
				byte = byte << 1;

				// if the next bit is a one we add one to the byte
				// this bit will be shifted to the correct position once the loop finishes
				if (bitsToWrite.front() == '1')
					byte++;

				// when the bit has been added to the byte remove it from the bits to write string as it
				// will be written when the loop finishes
				bitsToWrite.erase(bitsToWrite.begin());
			}

			// write the generated byte to the output file and increment bytes written
			outputStream.put(byte);
			bytesEncoded++;
		}
	}

	// now we are done getting input and writing to the output file so we need to close 
	// the input and output streams
	inputStream.close();
	outputStream.close();

	time_t endTime = time(0);

	// then calculate elapsed time and file compression
	std::cout << "Bytes Read: " << bytesRead << std::endl;
	std::cout << "Bytes Encoded: " << bytesEncoded << std::endl;
	int compression = (double)bytesEncoded / bytesRead * 100;

	std::cout << "Compression: " << compression << "%" << std::endl;
	std::cout << "Elapsed Encoding Time: " << endTime - startTime << " s" << std::endl;
}

// gets the prefix bits of a node so that the end bits of a file are padded
std::string Huffman::getPaddingBits(int numberOfBitsNeeded)
{
	// for each bit string path return the first sub string from first charcter to number of bits needed
	// this creates a bit string that points to no character in the tree
	for each (std::string path in binaryPaths)
	{
		if (path.length() > numberOfBitsNeeded)
			return path.substr(0, numberOfBitsNeeded);
	}

	return "";
}

// takes a huffman encoded file and turns it into the original input text
void Huffman::decodeFile(std::string inFile, std::string outFile)
{
	std::ifstream inputStream;
	inputStream.open(inFile, std::ios::binary);

	if (inputStream.fail())
	{
		std::cout << "Could not open encoded file" << std::endl;
		return;
	}

	std::ofstream outputStream;
	outputStream.open(outFile);
	unsigned char nextUChar;
	char nextChar;
	HuffmanNode* node = root;
	std::string bitString = "";
	time_t startTime = time(0);
	int bytesDecoded = 0;
	int bytesRead = 0;

	// while there are still characters to read in
	// if the bit string is empty get the next char from the input file and turn it into a
	// bit string to be added to the bit string
	// then use the bit string to traverse the tree until a node is reached
	// output that nodes value to the output file
	while (true)
	{
		if (bitString.length() == 0)
		{
			inputStream.get(nextChar);
			if (inputStream.eof())
				break;

			nextUChar = (unsigned char)nextChar;

			bytesRead++;
		
			for (int i = 8; i > 0; i--)
			{
				if ((nextUChar >> (i - 1)) == 1)
					bitString.append("1");
				else
					bitString.append("0");

				nextUChar = nextUChar << (9 - i);
				nextUChar = nextUChar >> (9 - i);
			}

		}

		while (bitString.length() != 0)
		{
			if (node->leftChild == nullptr && node->rightChild == nullptr)
			{
				outputStream.put(node->value);
				node = root;
				bytesDecoded++;
			}

			if (bitString.front() == '0')
				node = node->leftChild;
			else
				node = node->rightChild;

			bitString.erase(bitString.begin());
		}
	}

	inputStream.close();
	outputStream.close();

	time_t endTime = time(0);
	std::cout << "Elapsed Decoding Time: " << endTime - startTime << " s" << std::endl;
	std::cout << "Bytes Read: " << bytesRead << std::endl;
	std::cout << "Bytes Decoded: " << bytesDecoded << std::endl;

}

