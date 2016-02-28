// EECS 2510
// David Carek

#include "stdafx.h"
#include <string>
#include "Huffman.h"
#include <iostream>

using namespace std;

int main()
{
	char waitChar;
	string inputFilePath = "C:\\Users\\DMCar\\Desktop\\Shakespeare";
	Huffman huffman;

	huffman.intializeFromFile(inputFilePath + ".txt");
	huffman.encodeFile(inputFilePath + ".txt", inputFilePath + ".enc");
	//huffman.decodeFile(inputFilePath + ".enc", inputFilePath + ".dec");

	cout << "Processing finished. Press ENTER to exit" << endl;
	cin.get(waitChar);

    return 0;
}

