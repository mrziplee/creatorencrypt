
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <limits.h>
#include <iostream>
#include <string.h>

typedef unsigned char BYTE;

#define BINARY_FILE_HEADER 0x04034b50
#define BINARY_FILE_HEADER1 0x02014b50

#ifdef __APPLE__
#include "stdint.h"
#endif

#ifdef __linux
#include <sys/types.h>
#endif

#ifdef _MSC_VER
typedef unsigned __int32 uint32_t;
#endif

using namespace std;


int main(int argc, char* argv[])
{
	FILE* pFileIn = NULL;

	if (argc < 2)
	{
		puts("USAGE: changezip <SOURCE_FILE>");
		return 0;
	}

	pFileIn = fopen(argv[1], "rb+");
	if (!pFileIn) {
		puts("open file err");
		return 0;		
	}

	fseek(pFileIn, 0, SEEK_END);
	size_t len = ftell(pFileIn);
	rewind(pFileIn);

	// cout << "file len is " << len << endl;

	BYTE* buffer = (BYTE*)malloc(len);
	size_t readCount = fread(buffer, len, 1, pFileIn);
	assert(readCount == 1);

	uint32_t header = *(uint32_t*)buffer;
	if (header != BINARY_FILE_HEADER) {
		puts("not zip file");
		free(buffer);
		return 0;
	}

	for (int i = 10; i < 14; ++i) {
		buffer[i] = 0;
	}

	unsigned short filenamelen = *(unsigned short*)(buffer + 26);
	unsigned short extralen = *(unsigned short*)(buffer + 28);

	for (int i = 0; i < extralen; ++i) {
		buffer[30 + filenamelen + i] = 0;
	}

	// 找结尾
	unsigned int chooselen = 0;
	for (int i = 30 + extralen + 1; i < len - 4; ++i) {
		if (buffer[i] == 0x50 && buffer[i+1] == 0x4b && buffer[i+2] == 0x01 && buffer[i+3] == 0x02) {
			// cout << "has find tail " << i << endl;
			for (int j = 12; j < 16; ++j) {
				buffer[i + j] = 0;
			}

			unsigned short filenamelen1 = *(unsigned short*)(buffer + i + 28);
			unsigned short extralen1 = *(unsigned short*)(buffer + i + 30);
			unsigned short commentlen1 = *(unsigned short*)(buffer + i + 32);

			// cout << filenamelen1 << " " << extralen1 << " " << commentlen1 << endl;

			for (int j = 0; j < extralen1 + commentlen1; ++j) {
				buffer[i + 46 + filenamelen1 + j] = 0;
			}

			chooselen = i;
			break;
		}
	}

	for (int i = chooselen; i < len - 4; ++i) {
		if (buffer[i] == 0x50 && buffer[i+1] == 0x4b && buffer[i+2] == 0x05 && buffer[i+3] == 0x06) {
			// cout << "has find tail " << i << endl;

			unsigned short commentlen2 = *(unsigned short*)(buffer + i + 20);
			// cout << "comment len2 is " << commentlen2 << endl;

			for (int j = 0; j < commentlen2; ++j) {
				if (i + 20 + j < len)
					buffer[i + 20 + j] = 0;
			}

			break;
		}
	}

	rewind(pFileIn);
	fwrite(buffer, len, 1, pFileIn);
	free(buffer);
	fclose(pFileIn);
	return 0;
}