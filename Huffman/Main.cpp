// EECS 2510
// David Carek
// Lab 2 Huffman Tree Encoder/Decoder

#include "stdafx.h"
#include <string>
#include "Huffman.h"
#include <iostream>

using namespace std;

int main()
{
	string inputFilePath = "C:\\Users\\DMCar\\Desktop\\Shakespeare";
	Huffman huffman;

	// The Huffman tree and encoding strings will be generated
	huffman.intializeFromFile(inputFilePath + ".txt");
	// Then, the original text file is encoded 
	huffman.encodeFile(inputFilePath + ".txt", inputFilePath + ".enc");
	// Finally, the encoded file gets decoded
	huffman.decodeFile(inputFilePath + ".enc", inputFilePath + ".dec");

	// then we wait for the user to finish viewing the data
	cout << "Processing finished. Press ENTER to exit" << endl;
	char waitChar;
	cin.get(waitChar);

    return 0;
}
