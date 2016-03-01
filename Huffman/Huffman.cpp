#include "stdafx.h"
#include "Huffman.h"
#include <fstream>
#include <string>
#include <iostream>
#include <iomanip>
#include <xtgmath.h>
#include <chrono>
#include <stdio.h>

Huffman::Huffman()
{
}


Huffman::~Huffman()
{
}

// set up the tree and encoding strings given the path to a file
void Huffman::intializeFromFile(std::string fileName)
{
	initFinished = false;

	// we need to make sure that the file that the user entered is valid
	std::ifstream inputStream;
	inputStream.open(fileName, std::ios::binary);

	// if we couldn't open the file, let the user know and return
	if (inputStream.fail())
	{
		std::cout << "Could not open file for initalization" << std::endl;
		return;
	}
	
	std::cout << "Initializing..." << std::endl;

	// start and end time are needed to keep track of elapsed time
	std::chrono::time_point<std::chrono::system_clock> start, end;
	start = std::chrono::system_clock::now();

	HuffmanNode* nodes[256] = {};

	// initialize all nodes of the array
	for (int i = 0; i < 256; i++)
	{
		nodes[i] = new HuffmanNode();
		nodes[i]->value = i;
	}

	int bytesRead = 0;
	char nextChar;

	// get all characters from file and since every character is one byte increment bytes 
	// read for every char read
	while (inputStream.get(nextChar)) 
	{
		// if we reached the end of the file stop trying to get more characters
		if (inputStream.eof())
			break;

		nodes[nextChar]->weight++;
		bytesRead++;
	}

	inputStream.close();

	// while there are still nodes to be grouped get the two smallest and create a root for the subtree and set
	// the pointers for that roots children
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

	// now the only node left in the array will be in the 0th position and it is the root
	// of the huffman tree
	root = nodes[0];

	// tree is now built, create encoding strings
	// we only need the first node of the array to be passed since it is the root
	setEncodingStrings(root);

	end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = end - start;

	std::cout << "Bytes read: " << bytesRead << std::endl;
	printf("Elapsed Initialization Time: %.3f s\n", elapsed_seconds.count());
	std::cout << "Finished Initializing" << std::endl << std::endl;
	initFinished = true;
}

// create the bit string paths to the leaves
void Huffman::setEncodingStrings(HuffmanNode* node)
{
	// clear the current path and initialize the array to hold encoding strings
	currentPath = "";
	binaryPaths[256] = {};
	// traverse till leaf and store the path in the binary paths string array
	traverse(node);
}

// recurse through the tree until every leaf is reached to create the encoding strings
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

// lets us know when there is only one node left in the array which is the root of the tree
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
		// if the current slot in the array is null skip it
		if (nodes[i] == nullptr)
			continue;
		// if no smallest node has been declared the current node will become the smallest
		if (smallestWeightNode == nullptr)
		{
			smallestWeightNode = nodes[i];
			locationOfSmallestNode = i;
		}
		// else if the current nodes weight is less than the weight of the smallest weight node
		// it then becomes the new smallest weight node
		else if (smallestWeightNode->weight > nodes[i]->weight)
		{
			smallestWeightNode = nodes[i];
			locationOfSmallestNode = i;
		}
	}

	// remove the node being returned from the array to free up space for the resulting root
	// of the subtree that will be created from 2 min weight nodes
	nodes[locationOfSmallestNode] = nullptr;

	return smallestWeightNode;
}

// encodes the input file based on the table of string paths
void Huffman::encodeFile(std::string inputFile, std::string outputFile)
{
	// we need to make sure that the init method has run all the way through before encoding
	if (!initFinished)
	{
		std::cout << "Cannot run encode, initialization failed" << std::endl;
		return;
	}

	std::ifstream inputStream;
	inputStream.open(inputFile, std::ios::binary);

	// we can't open the text file we can't continue
	if (inputStream.fail())
	{
		std::cout << "Could not open file for encoding" << std::endl;
		return;
	}

	std::cout << "Encoding..." << std::endl;

	// create start and end times for elapsed time
	std::chrono::time_point<std::chrono::system_clock> start, end;
	start = std::chrono::system_clock::now();

	std::ofstream outputStream;
	outputStream.open(outputFile, std::ios::binary);

	std::string bitsToWrite = "";
	unsigned char byte;

	int bytesEncoded = 0;
	int bytesRead = 0;

	char nextChar;

	// while there are still characters left in the input file continue getting them
	// and converting them to encoded bytes to add to the output file
	while (true)
	{	
		inputStream.get(nextChar);

		// if the end of file has been reached we have 2 cases:
		// 1 : the number of bits left in bitsToWrite is 0 so we do not have to add padding bits
		// 2 : the number of bits left in bitsToWrite is some positive integer so we have to get padding bits to write
		//     out a full byte to the output file
		if (inputStream.eof())
		{
			if (bitsToWrite.length() > 0)
				bitsToWrite.append(getPaddingBits(8 - bitsToWrite.length()));
			else break;
		}
		// if we are not at the end of the file we need to append the bit string for the character from the input
		// to bitsToWrite
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
				if (bitsToWrite.front() == '1')
					byte++;

				// when the bit has been added to the byte remove it from the bits to write string as it
				// will be written to the file when the loop finishes
				bitsToWrite.erase(bitsToWrite.begin());
			}

			// write the generated byte to the output file
			outputStream.put(byte);
			bytesEncoded++;
		}
	}

	// now we are done getting input and writing to the output file so we need to close 
	// the input and output streams
	inputStream.close();
	outputStream.close();

	end = std::chrono::system_clock::now();

	std::chrono::duration<double> elapsed_seconds = end - start;

	// then calculate elapsed time and file compression etc.
	std::cout << "Bytes Read: " << bytesRead << std::endl;
	std::cout << "Bytes Encoded: " << bytesEncoded << std::endl;
	double compression = (double)bytesEncoded / bytesRead;

	printf("Compression Level: %.3f\n", compression);
	printf("Elapsed Encoding Time: %.3f s\n", elapsed_seconds.count());
	std::cout << "Finished Encoding" << std::endl << std::endl;
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
	// we can't continue if the initialize has not been run
	if (!initFinished)
	{
		std::cout << "Cannot run decode, initialization failed" << std::endl;
		return;
	}

	std::ifstream inputStream;
	inputStream.open(inFile, std::ios::binary);

	// we can't continue if the encoded file can't be opened
	if (inputStream.fail())
	{
		std::cout << "Could not open encoded file" << std::endl;
		return;
	}

	std::cout << "Decoding..." << std::endl;

	std::chrono::time_point<std::chrono::system_clock> start, end;
	start = std::chrono::system_clock::now();

	std::ofstream outputStream;
	outputStream.open(outFile, std::ios::binary);
	unsigned char nextUChar;
	char nextChar;
	HuffmanNode* node = root;
	std::string bitString = "";
	int bytesDecoded = 0;
	int bytesRead = 0;

	// while there are still characters to read get the next char from the input file and 
	// turn it into a bit string to be added to bitString
	// then use the bit string to traverse the tree until a node is reached
	// output that nodes value to the output file
	while (true)
	{
		inputStream.get(nextChar);
		// if we reached the end of the file we have no more characters to decode
		if (inputStream.eof())
			break;

		// convert the signed char into an unsigned one 
		// this is required for the shifting to work correctly
		nextUChar = (unsigned char)nextChar;

		bytesRead++;
		
		// iterate 8 times to convert and append the byte to a bit string
		for (int i = 8; i > 0; i--)
		{
			// shift the unsigned char to the right i - 1 times to isolate a single bit
			// this works because after every check the bit gets set to a 0 from the shifting 5 lines below
			if ((nextUChar >> (i - 1)) == 1)
				bitString.append("1");
			else
				bitString.append("0");

			// these shifts clear the most recently checked bit of the char
			nextUChar = nextUChar << (9 - i);
			nextUChar = nextUChar >> (9 - i);
		}

		// while there are bits left in the bitString continue traversing down the tree until a leaf is reached
		// when the leaf is reached write its value to the output file and reset the node to the root
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

			// remove the first char in the string since it has been checked
			bitString.erase(bitString.begin());
		}
	}

	inputStream.close();
	outputStream.close();

	end = std::chrono::system_clock::now();

	std::chrono::duration<double> elapsed_seconds = end - start;

	std::cout << "Bytes Read: " << bytesRead << std::endl;
	std::cout << "Bytes Decoded: " << bytesDecoded << std::endl;

	printf("Elapsed Encoding Time: %.3f s\n", elapsed_seconds.count());
	std::cout << "Finished Decoding" << std::endl << std::endl;
}

