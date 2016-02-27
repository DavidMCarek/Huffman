#include "stdafx.h"
#include "Huffman.h"
#include <fstream>
#include <string>
#include <iostream>

Huffman::Huffman()
{
}


Huffman::~Huffman()
{
}

void Huffman::intializeFromFile(std::string fileName)
{
	HuffmanNode* nodes[256] = {};

	// initialize all nodes of the array
	for (int i = 0; i < 256; i++)
	{
		nodes[i] = new HuffmanNode();
		nodes[i]->value = i;
	}

	char nextChar;
	std::fstream inputStream(fileName, std::fstream::in);
	while (inputStream >> std::noskipws >> nextChar) 
	{
		nodes[nextChar]->weight++;
	}

	for each (HuffmanNode* node in nodes)
	{
		if (node != nullptr)
			std::cout << node->value << " " << node->weight << std::endl;
	}

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

	// tree is now built, create encoding strings
	// we only need the first node of the array to be passed since it is the root
	setEncodingStrings(nodes[0]);
}

void Huffman::setEncodingStrings(HuffmanNode* node)
{
	// clear the current path and initialize the array to hold encoding strings
	currentPath = "";
	binaryPaths[256] = {};
	// traverse till leaf and store the path in the binary paths string array
	traverse(node);

	std::cout << "all encoding strings saved" << std::endl;
}

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

	// had to do a null check here because of some funky business with overwriting the null character's path
	if (node->leftChild == nullptr && node->rightChild == nullptr)
		binaryPaths[node->value] = currentPath;

	// now that we are at a leaf we need to store the encoding string for that leaf
	
}

bool Huffman::nodesNeedMerging(HuffmanNode* nodes[256])
{
	// more than 1 node is left in the array then we need to continue merging
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