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

	fileName = "C:\\Shakespeare.txt";

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

	std::cout << "done merging" << std::endl;
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