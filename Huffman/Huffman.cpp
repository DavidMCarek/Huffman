#include "stdafx.h"
#include "Huffman.h"
#include <fstream>
#include <string>
#include <iostream>
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
	bytesRead = 0;
	HuffmanNode* nodes[256] = {};

	// initialize all nodes of the array
	for (int i = 0; i < 256; i++)
	{
		nodes[i] = new HuffmanNode();
		nodes[i]->value = i;
	}

	char nextChar;
	std::fstream inputStream(fileName, std::fstream::in);
	// get all characters from file not skipping white space
	// since every character is one byte increment bytes read for every char read
	while (inputStream >> std::noskipws >> nextChar) 
	{
		nodes[nextChar]->weight++;
		bytesRead++;
	}

	// while there are still nodes to be grouped get the two smallest and create a root for the 
	// subtree and set the pointers for that roots children
	// then set that root to an empty slot in the array
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

void Huffman::encodeFile(std::string inputFile, std::string outputFile)
{
	time_t startTime = time(0);
	std::ofstream outputStream;
	outputStream.open(outputFile);
	char nextChar;
	std::fstream inputStream(inputFile, std::fstream::in);
	std::string bitsToWrite = "";
	std::string nextByte = "";
	char byte;

	bytesEncoded = 0;

	while (!inputStream.eof())
	{
		if (bitsToWrite.length() < 8)
		{
			inputStream >> std::noskipws >> nextChar;
			bitsToWrite.append(binaryPaths[nextChar]);
		}
		else
		{
			byte = 0;
			for (int i = 8; i > 1; i--)
			{
				byte = byte << 1;
				if (bitsToWrite.front() == '1')
					byte++;

				bitsToWrite.erase(bitsToWrite.begin());
			}
			outputStream.put(byte);
			bytesEncoded++;
		}
	}

	if (bitsToWrite.length() != 0)
	{
		bitsToWrite.append(getPaddingBits(8 - bitsToWrite.length()));
		byte = 0;
		for (int i = 8; i > 1; i--)
		{
			byte = byte << 1;
			if (bitsToWrite.front() == '1')
				byte++;

			bitsToWrite.erase(bitsToWrite.begin());
		}
		outputStream.put(byte);
		bytesEncoded++;
	}

	outputStream.close();

	time_t endTime = time(0);

	std::cout << "Bytes Encoded: " << bytesEncoded << std::endl;
	int compression = (double)bytesEncoded / bytesRead * 100;

	std::cout << "Compression: " << compression << "%" << std::endl;
	std::cout << "Elapsed Encoding Time: " << endTime - startTime << " s" << std::endl;
}

std::string Huffman::getPaddingBits(int numberOfBitsNeeded)
{
	for each (std::string path in binaryPaths)
	{
		if (path.length() > numberOfBitsNeeded)
			return path.substr(0, numberOfBitsNeeded);
	}

	return "";
}