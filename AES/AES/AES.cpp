﻿#include "pch.h"
#include <fstream> 
#include <iostream>
#include <iomanip>
#include <windows.h>
#include <vector>
#include <iterator>
#include <sstream>
#include <string>
#include <conio.h>
#define Nb 4
#define BlockSize 16
#define xtime(x) ((x << 1) xor (((x >> 7) bitand 1) * 0x1b))
#define Multiply(x, y) (((y bitand 1) * x) xor ((y >> 1 bitand 1) * xtime(x)) xor ((y >> 2 bitand 1) * xtime(xtime(x))) xor ((y >> 3 bitand 1) * xtime(xtime(xtime(x)))) xor ((y >> 4 bitand 1) * xtime(xtime(xtime(xtime(x))))))
int Nr = 10, Nk = 4;
unsigned char state[4][4];
unsigned char RoundKey[240];
int getSBoxValue(int num) {
	int sbox[256] = {
		//0     1    2      3     4    5     6     7      8    9     A      B    C     D     E     F
		0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76, //0
		0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0, //1
		0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15, //2
		0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75, //3
		0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84, //4
		0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf, //5
		0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8, //6
		0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2, //7
		0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73, //8
		0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb, //9
		0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79, //A
		0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08, //B
		0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a, //C
		0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e, //D
		0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf, //E
		0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16 }; //F
	return sbox[num];
}
int getSBoxInvert(int num) {
	int rsbox[256] = {
		0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
		0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
		0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
		0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
		0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
		0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
		0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
		0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
		0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
		0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
		0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
		0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
		0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
		0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
		0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
		0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d };
	return rsbox[num];

}
int Rcon[255] = {
	0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a,
	0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39,
	0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a,
	0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8,
	0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef,
	0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc,
	0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b,
	0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3,
	0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94,
	0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20,
	0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35,
	0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f,
	0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04,
	0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63,
	0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd,
	0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb };
void KeyExpansion(std::vector<unsigned char> &Key)
{
	int i, j;
	unsigned char temp[4], k;
	for (i = 0; i < Nk; i++) {
		RoundKey[i * 4] = Key[i * 4];
		RoundKey[i * 4 + 1] = Key[i * 4 + 1];
		RoundKey[i * 4 + 2] = Key[i * 4 + 2];
		RoundKey[i * 4 + 3] = Key[i * 4 + 3];
	}
	while (i < (Nb * (Nr + 1))) {
		for (j = 0; j < 4; j++) {
			temp[j] = RoundKey[(i - 1) * 4 + j];
		}
		if (i % Nk == 0) {
			{
				k = temp[0];
				temp[0] = temp[1];
				temp[1] = temp[2];
				temp[2] = temp[3];
				temp[3] = k;
			}
			{
				temp[0] = getSBoxValue(temp[0]);
				temp[1] = getSBoxValue(temp[1]);
				temp[2] = getSBoxValue(temp[2]);
				temp[3] = getSBoxValue(temp[3]);
			}
			temp[0] = temp[0] xor Rcon[i / Nk];
		}
		else if (Nk > 6 && i % Nk == 4) {
			{
				temp[0] = getSBoxValue(temp[0]);
				temp[1] = getSBoxValue(temp[1]);
				temp[2] = getSBoxValue(temp[2]);
				temp[3] = getSBoxValue(temp[3]);
			}
		}
		RoundKey[i * 4 + 0] = RoundKey[(i - Nk) * 4 + 0] xor temp[0];
		RoundKey[i * 4 + 1] = RoundKey[(i - Nk) * 4 + 1] xor temp[1];
		RoundKey[i * 4 + 2] = RoundKey[(i - Nk) * 4 + 2] xor temp[2];
		RoundKey[i * 4 + 3] = RoundKey[(i - Nk) * 4 + 3] xor temp[3];
		i++;
	}
}
void AddRoundKey(int round) {
	int i, j;
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			state[j][i] xor_eq RoundKey[round * Nb * 4 + i * Nb + j];
		}
	}
}
void SubBytes() {
	int i, j;
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			state[i][j] = getSBoxValue(state[i][j]);
		}
	}
}
void InvSubBytes() {
	int i, j;
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			state[i][j] = getSBoxInvert(state[i][j]);
		}
	}
}
void ShiftRows() {
	unsigned char temp;

	temp = state[1][0];
	state[1][0] = state[1][1];
	state[1][1] = state[1][2];
	state[1][2] = state[1][3];
	state[1][3] = temp;

	temp = state[2][0];
	state[2][0] = state[2][2];
	state[2][2] = temp;
	temp = state[2][1];
	state[2][1] = state[2][3];
	state[2][3] = temp;
	
	temp = state[3][0];
	state[3][0] = state[3][3];
	state[3][3] = state[3][2];
	state[3][2] = state[3][1];
	state[3][1] = temp;
}
void InvShiftRows() {
	unsigned char temp;

	temp = state[1][3];
	state[1][3] = state[1][2];
	state[1][2] = state[1][1];
	state[1][1] = state[1][0];
	state[1][0] = temp;

	temp = state[2][0];
	state[2][0] = state[2][2];
	state[2][2] = temp;
	temp = state[2][1];
	state[2][1] = state[2][3];
	state[2][3] = temp;

	temp = state[3][0];
	state[3][0] = state[3][1];
	state[3][1] = state[3][2];
	state[3][2] = state[3][3];
	state[3][3] = temp;
}
void MixColumns() {
	int i;
	unsigned char Tmp, Tm, t;
	for (i = 0; i < 4; i++) {
		t = state[0][i];
		Tmp = state[0][i] xor state[1][i] xor state[2][i] xor state[3][i];
		Tm = state[0][i] xor state[1][i]; Tm = xtime(Tm); state[0][i] xor_eq Tm xor Tmp;
		Tm = state[1][i] xor state[2][i]; Tm = xtime(Tm); state[1][i] xor_eq Tm xor Tmp;
		Tm = state[2][i] xor state[3][i]; Tm = xtime(Tm); state[2][i] xor_eq Tm xor Tmp;
		Tm = state[3][i] xor t; Tm = xtime(Tm); state[3][i] xor_eq Tm xor Tmp;
	}
}
void InvMixColumns() {
	int i;
	unsigned char a, b, c, d;
	for (i = 0; i < 4; i++) {
		a = state[0][i];
		b = state[1][i];
		c = state[2][i];
		d = state[3][i];
		state[0][i] = Multiply(a, 0x0e) xor Multiply(b, 0x0b) xor Multiply(c, 0x0d) xor Multiply(d, 0x09);
		state[1][i] = Multiply(a, 0x09) xor Multiply(b, 0x0e) xor Multiply(c, 0x0b) xor Multiply(d, 0x0d);
		state[2][i] = Multiply(a, 0x0d) xor Multiply(b, 0x09) xor Multiply(c, 0x0e) xor Multiply(d, 0x0b);
		state[3][i] = Multiply(a, 0x0b) xor Multiply(b, 0x0d) xor Multiply(c, 0x09) xor Multiply(d, 0x0e);
	}
}
void Encryption(std::vector<unsigned char> &Text, std::vector<unsigned char> &Cipher, std::vector<unsigned char> &Key, int k) {
	KeyExpansion(Key);
	int i, j, round = 0;
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			state[j][i] = Text[k + i * 4 + j];
		}
	}
	AddRoundKey(0);
	for (round = 1; round < Nr; round++) {
		SubBytes();
		ShiftRows();
		MixColumns();
		AddRoundKey(round);
	}
	SubBytes();
	ShiftRows();
	AddRoundKey(Nr);
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			Cipher[k + i * 4 + j] = state[j][i];
		}
	}
}
void Decryption(std::vector<unsigned char> &Cipher, std::vector<unsigned char> &Text, std::vector<unsigned char> &Key, int k) {
	KeyExpansion(Key);
	int i, j, round = 0;
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			state[j][i] = Cipher[k + i * 4 + j];
		}
	}
	AddRoundKey(Nr);
	for (round = Nr - 1; round > 0; round--) {
		InvShiftRows();
		InvSubBytes();
		AddRoundKey(round);
		InvMixColumns();
	}
	InvShiftRows();
	InvSubBytes();
	AddRoundKey(0);
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++) {
			Text[k + i * 4 + j] = state[j][i];
		}
	}
}
void Dec2Hex(std::vector<unsigned char> &mas_in, std::vector<unsigned char> &mas_out, int k) {
	std::stringstream ss, sg;
	for (int i = 0; i < k; i++) {
		ss << std::hex << (int)mas_in[i];
	}
	std::string res = ss.str();
	unsigned int buffer;
	int offset = 0;
	while (offset < res.length()) {
		sg.clear();
		sg << std::hex << res.substr(offset, 2);
		sg >> buffer;
		mas_out.push_back(static_cast<unsigned char>(buffer));
		offset += 2;
	}
}
void StringHex2UCharHex(std::string string_in, std::vector<unsigned char> &vec_out) {
	std::stringstream sg;
	unsigned int buffer;
	int offset = 0;
	while (offset < string_in.length()) {
		sg.clear();
		sg << std::hex << string_in.substr(offset, 2);
		sg >> buffer;
		vec_out.push_back(static_cast<unsigned char>(buffer));
		offset += 2;
	}
}
struct Encrypt {
	void ECB(std::vector<unsigned char> &Text, std::vector<unsigned char> &Key, int amount_of_blocks, std::string OutFileName) {
		std::vector<unsigned char> Cipher(amount_of_blocks * BlockSize);
		int counter = 0, j = 0;
		while (counter < amount_of_blocks) {
			Encryption(Text, Cipher, Key, j);
			j += BlockSize;
			counter++;
		}
		Write_and_show(Cipher, OutFileName, "ECB");
	}
	void CBC(std::vector<unsigned char> &Text, std::vector<unsigned char> &Key, int amount_of_blocks, std::string OutFileName) {
		std::vector<unsigned char> Temp(BlockSize), Cipher;
		std::cout << "Enter initialization vector pls: ";
		for (int i = 0; i < Temp.size(); i++) {
			std::cin >> Temp[i];
		}
		int counter = 0, j = 0;
		while (counter < amount_of_blocks) {
			for (int i = 0; i < Temp.size(); i++) {
				Temp[i] xor_eq Text[j + i];
			}
			Encryption(Temp, Temp, Key, 0);
			for (int i = 0; i < Temp.size(); i++) {
				Cipher.push_back(Temp[i]);
			}
			j += BlockSize;
			counter++;
		}
		Write_and_show(Cipher, OutFileName, "CBC");
	}
	void CFB(std::vector<unsigned char> &Text, std::vector<unsigned char> &Key, int amount_of_blocks, std::string OutFileName) {
		std::vector<unsigned char> Temp(BlockSize), Cipher;
		std::cout << "Enter initialization vector pls: ";
		for (int i = 0; i < Temp.size(); i++) {
			std::cin >> Temp[i];
		}
		int counter = 0, j = 0;
		while (counter < amount_of_blocks) {
			Encryption(Temp, Temp, Key, 0);
			for (int i = 0; i < Temp.size(); i++) {
				Temp[i] xor_eq Text[i + j];
				Cipher.push_back(Temp[i]);
			}
			j += BlockSize;
			counter++;
		}
		Write_and_show(Cipher, OutFileName, "CFB");
	}
	void OFB(std::vector<unsigned char> &Text, std::vector<unsigned char> &Key, int amount_of_blocks, std::string OutFileName) {
		std::vector<unsigned char> Temp(BlockSize), Cipher;
		std::cout << "Enter initialization vector pls: ";
		for (int i = 0; i < Temp.size(); i++) {
			std::cin >> Temp[i];
		}
		int counter = 0, j = 0;
		while (counter < amount_of_blocks) {
			Encryption(Temp, Temp, Key, 0);
			for (int i = 0; i < Temp.size(); i++) {
				Cipher.push_back(Temp[i] xor Text[i + j]);
			}
			j += BlockSize;
			counter++;
		}
		Write_and_show(Cipher, OutFileName, "OFB");
	}
	void CTR(std::vector<unsigned char> &Text, std::vector<unsigned char> &Key, int amount_of_blocks, std::string OutFileName1, std::string OutFileName2) {
		std::vector<unsigned char> Temp(BlockSize), Temp1, Cipher;
		std::fstream out(OutFileName2, std::ios::out);
		if (out.is_open()) {
			int counter = 0, j = 0;
			while (counter < amount_of_blocks) {
				for (int i = 0; i < Temp.size(); i++) {
					Temp[i] = (unsigned char)std::rand() % 224 + 32;
				}
				Dec2Hex(Temp, Temp1, BlockSize);
				out << "Counter value for " << counter + 1 << " block" << std::endl;
				for (int i = 0; i < Temp1.size(); i++) {
					out << std::hex << (int)Temp1[i];
				}
				out << std::endl;
				Encryption(Temp1, Temp, Key, 0);
				Temp1.clear();
				for (int i = 0; i < Temp.size(); i++) {
					Cipher.push_back(Temp[i] xor Text[i + j]);
				}
				j += BlockSize;
				counter++;
			}
			Write_and_show(Cipher, OutFileName1, "CTR");
		}
		else {
			std::cout << "File for counter values was not open, fix the problem and try again" << std::endl;
			exit(0);
		}
	}
	void Write_and_show(std::vector<unsigned char> &Cipher, std::string FileName, std::string mode) {
		std::fstream out(FileName, std::ios::out);
		if (out.is_open()) {
			out << "Encrypted in " << mode << " mode text:" << std::endl;
			for (int i = 0; i < Cipher.size(); i++) {
				if ((int)Cipher[i] / BlockSize < 1) {
					out << '0';
				}
				out << std::hex << (int)Cipher[i] << " ";
			}
			out.close();
			std::cout << "Cipher: " << std::endl;
			std::copy(Cipher.begin(), Cipher.end(), std::ostream_iterator<unsigned int>(std::cout << std::hex, " "));
		}
		else {
			std::cout << "File for decrypted text was not open, fix the problem and try again" << std::endl;
			exit(0);
		}
	}
};
struct Decrypt {
	void ECB(std::vector<unsigned char> &Cipher, std::vector<unsigned char> &Key, int amount_of_blocks, std::string OutFileName) {
		std::vector<unsigned char> Text(amount_of_blocks * BlockSize);
		int counter = 0, j = 0;
		while (counter < amount_of_blocks) {
			Decryption(Cipher, Text, Key, j);
			j += BlockSize;
			counter++;
		}
		Write_and_show(Text, OutFileName, "ECB");
	}
	void CBC(std::vector<unsigned char> &Cipher, std::vector<unsigned char> &Key, int amount_of_blocks, std::string OutFileName) {
		std::vector<unsigned char> Temp(BlockSize), IV(BlockSize), Text;
		std::cout << "Enter initialization vector pls: ";
		for (int i = 0; i < IV.size(); i++) {
			std::cin >> IV[i];
		}
		for (int i = 0; i < Temp.size(); i++) {
			Temp[i] = Cipher[i];
		}
		Decryption(Temp, Temp, Key, 0);
		for (int i = 0; i < Temp.size(); i++) {
			Text.push_back(Temp[i] xor IV[i]);
		}
		int counter = 0, j = BlockSize;
		while (counter < amount_of_blocks - 1) {
			for (int i = 0; i < Temp.size(); i++) {
				Temp[i] = Cipher[j + i];
			}
			Decryption(Temp, Temp, Key, 0);
			for (int i = 0; i < Temp.size(); i++) {
				Text.push_back(Temp[i] xor Cipher[j - BlockSize + i]);
			}
			j += BlockSize;
			counter++;
		}
		Write_and_show(Text, OutFileName, "CBC");
	}
	void CFB(std::vector<unsigned char> &Cipher, std::vector<unsigned char> &Key, int amount_of_blocks, std::string OutFileName) {
		std::vector<unsigned char> Temp(BlockSize), Text;
		std::cout << "Enter initialization vector pls: ";
		for (int i = 0; i < Temp.size(); i++) {
			std::cin >> Temp[i];
		}
		int counter = 0, j = 0;
		while (counter < amount_of_blocks) {
			Encryption(Temp, Temp, Key, 0);
			for (int i = 0; i < Temp.size(); i++) {
				Temp[i] xor_eq Cipher[i + j];
				Text.push_back(Temp[i]);
				Temp[i] = Cipher[i + j];
			}
			j += BlockSize;
			counter++;
		}
		Write_and_show(Text, OutFileName, "CFB");
	}
	void OFB(std::vector<unsigned char> &Cipher, std::vector<unsigned char> &Key, int amount_of_blocks, std::string OutFileName) {
		std::vector<unsigned char> Temp(BlockSize), Text;
		std::cout << "Enter initialization vector pls: ";
		for (int i = 0; i < Temp.size(); i++) {
			std::cin >> Temp[i];
		}
		int counter = 0, j = 0;
		while (counter < amount_of_blocks) {
			Encryption(Temp, Temp, Key, 0);
			for (int i = 0; i < Temp.size(); i++) {
				Text.push_back(Temp[i] xor Cipher[i + j]);
			}
			j += BlockSize;
			counter++;
		}
		Write_and_show(Text, OutFileName, "OFB");
	}
	void CTR(std::vector<unsigned char> &Cipher, std::vector<unsigned char> &Key, int amount_of_blocks, std::string InFileName, std::string OutFileName) {
		std::vector<unsigned char> Temp1, Temp2, Temp3(BlockSize), Text;
		std::string Temp;
		std::fstream in(InFileName, std::ios::in);
		if (in.is_open()) {
			if (in.peek() == EOF) {
				std::cout << "File with counter values is empty. Input it in file of firstly encrypt the text and get it." << std::endl;
			}
			else {
				int counter = 0, y = 0, j = 0;
				while (y <= amount_of_blocks) {
					std::getline(in, Temp);
					Temp.clear();
					std::getline(in, Temp);
					StringHex2UCharHex(Temp, Temp1);
					y++;
				}
				while (counter < amount_of_blocks) {
					for (int i = 0; i < BlockSize; i++) {
						Temp2.push_back(Temp1[i + j]);
					}
					Encryption(Temp2, Temp3, Key, 0);
					Temp2.clear();
					for (int i = 0; i < Temp3.size(); i++) {
						Text.push_back(Temp3[i] xor Cipher[i + j]);
					}
					j += BlockSize;
					counter++;
				}
				Write_and_show(Text, OutFileName, "CTR");
			}
		}
		else {
			std::cout << "File with counter values was not open, fix the problem and try again" << std::endl;
			exit(0);
		}
	}
	void Write_and_show(std::vector<unsigned char> &Text, std::string FileName, std::string mode) {
		std::fstream out(FileName, std::ios::out);
		if (out.is_open()) {
			out << "Decrypted in " << mode << " mode text:" << std::endl;
			for (int i = 0; i < Text.size(); i++) {
				out << Text[i];
			}
			out.close();
			std::cout << "Text: " << std::endl;
			std::copy(Text.begin(), Text.end(), std::ostream_iterator<unsigned int>(std::cout << std::hex, " "));
		}
		else {
			std::cout << "File for encrypted text was not open, fix the problem and try again" << std::endl;
			exit(0);
		}
	}
};
int main() {
	srand(time(NULL));
	SetConsoleOutputCP(1251);
	SetConsoleCP(1251);
	setlocale(0, "");
	while (true) {
		char Main_answer;
		std::cout << "This program can encrypt text or decrypt it with AES-128." << std::endl
			<< "Press 1 for encryption" << std::endl
			<< "Press 2 for decryption" << std::endl;
		std::cin >> Main_answer;
		switch (Main_answer) {
		case '1': {
			float amount_of_characters = 0;
			std::vector<unsigned char> Text(100000), Key, TextHex, KeyDec(BlockSize);
			Text.assign(Text.size(), '0');
			std::string TextFileName = "text.txt", CipherFileName = "cipher.txt";
			std::fstream create(TextFileName, std::ios::out);
			if (create.is_open()) {
				create.close();
				std::cout << "File for text was created or already exists. Enter text and save the file. Press any key to continue:" << std::endl;
				_getch();
				std::fstream in(TextFileName, std::ios::in);
				while (true) {
					system("text.txt");
					if (in.peek() == EOF) {
						std::cout << "File with text is empty. Press any key to try again or press 'n' to close program" << std::endl;
						char q = _getch();
						if (q == 'n') {
							std::cout << "Good luck then. " << std::endl;
							exit(0);
						}
						continue;
					}
					else {
						break;
					}
				}
				for (int i = 0; !in.eof(); i++) {
					in >> Text[i];
					amount_of_characters++;
				}
				in.close();
				amount_of_characters = BlockSize * ceil((amount_of_characters - 1) / BlockSize);
				Dec2Hex(Text, TextHex, amount_of_characters);
				char Encr_answer;
				std::cout << "There are 5 modes for encryption: " << std::endl
					<< "1. ECB" << std::endl
					<< "2. CBC" << std::endl
					<< "3. CFB" << std::endl
					<< "4. OFB" << std::endl
					<< "5. CTR" << std::endl;
				std::cout << "Choose mode: ";
				std::cin >> Encr_answer;
				std::cout << "Input the key pls: ";
				for (int i = 0; i < BlockSize; i++) {
					std::cin >> KeyDec[i];
				}
				Dec2Hex(KeyDec, Key, BlockSize);
				Encrypt point_encr;
				switch (Encr_answer) {
				case '1': {
					point_encr.ECB(TextHex, Key, amount_of_characters / BlockSize, CipherFileName);
					break;
				}
				case '2': {
					point_encr.CBC(TextHex, Key, amount_of_characters / BlockSize, CipherFileName);
					break;
				}
				case '3': {
					point_encr.CFB(TextHex, Key, amount_of_characters / BlockSize, CipherFileName);
					break;
				}
				case '4': {
					point_encr.OFB(TextHex, Key, amount_of_characters / BlockSize, CipherFileName);
					break;
				}
				case '5': {
					std::string CountersFileName = "Counters.txt";
					point_encr.CTR(TextHex, Key, amount_of_characters / BlockSize, CipherFileName, CountersFileName);
					break;
				}
				default: {
					std::cout << "Incorrect input" << std::endl;
					break;
				}
				}
				break;
			}
			else {
				std::cout << "File for text was not open, fix the problem and try again" << std::endl;
				exit(0);
			}
		}
		case '2': {
			std::string Cipher, CipherFileName = "cipher.txt", TextFileName = "decipher.txt";
			std::vector<unsigned char> CipherHex, Key, KeyDec(BlockSize);
			std::fstream in(CipherFileName, std::ios::in);
			if (in.is_open()) {
				if (in.peek() == EOF) {
					std::cout << "File with decrypted text is empty. Input cipher in the file of firstly encrypt it." << std::endl;
					exit(0);
				}
				else {
					std::getline(in, Cipher);
					Cipher.clear();
					unsigned char temp;
					for (int i = 0; !in.eof(); i++) {
						in >> temp;
						Cipher += temp;
					}
					in.close();
					Cipher.erase(Cipher.end() - 1);
					StringHex2UCharHex(Cipher, CipherHex);
					char Decr_answer;
					std::cout << "There are 5 modes for decryption: " << std::endl
						<< "1. ECB" << std::endl
						<< "2. CBC" << std::endl
						<< "3. CFB" << std::endl
						<< "4. OFB" << std::endl
						<< "5. CTR" << std::endl;
					std::cout << "Choose mode: ";
					std::cin >> Decr_answer;
					std::cout << "Input the key pls: ";
					for (int i = 0; i < BlockSize; i++) {
						std::cin >> KeyDec[i];
					}
					Dec2Hex(KeyDec, Key, BlockSize);
					Decrypt point_decr;
					switch (Decr_answer) {
					case '1': {
						point_decr.ECB(CipherHex, Key, CipherHex.size() / BlockSize, TextFileName);
						break;
					}
					case '2': {
						point_decr.CBC(CipherHex, Key, CipherHex.size() / BlockSize, TextFileName);
						break;
					}
					case '3': {
						point_decr.CFB(CipherHex, Key, CipherHex.size() / BlockSize, TextFileName);
						break;
					}
					case '4': {
						point_decr.OFB(CipherHex, Key, CipherHex.size() / BlockSize, TextFileName);
						break;
					}
					case '5': {
						std::string CountersFileName = "Counters.txt";
						point_decr.CTR(CipherHex, Key, CipherHex.size() / BlockSize, CountersFileName, TextFileName);
						break;
					}
					default: {
						std::cout << "Incorrect input";
						break;
					}
					}
					break;
				}
			}
			else {
				std::cout << "File with encrypted text was not open, fix the problem and try again" << std::endl;
				exit(0);
			}
		}
		default: {
			std::cout << "Incorrect input";
			break;
		}
		}
		char repeat_answer;
		std::cout << std::endl;
		m:
		std::cout << "Again? (y/n) ";
		std::cin >> repeat_answer;
		if (repeat_answer == 'y' or repeat_answer == 'Y') {
			continue;
		}
		else if (repeat_answer == 'n' or repeat_answer == 'N') {
			std::cout << "Good luck then." << std::endl;
			break;
		}
		else {
			std::cout << "Try again" << std::endl;
			goto m;
		}
	}
	return 0;
}
