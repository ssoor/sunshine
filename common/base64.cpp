#include "base64.h"
#include <string.h>
#include <malloc.h>

const char base64char[] = "cyv8qWJnDjwIKYAlVTgs1d0R4fxX79NurtMmi/kCFhHb6oLO2ZQSB35aPpG+ezUE";

char * base64_encode(const void * bindata, int binlength) {
	int i, j;
	unsigned char current;
	char * base64 = (char*)malloc((binlength / 3 + 1) * 4 + 1);
	const unsigned char * strNeedEndata = (const unsigned char *)bindata;

	for (i = 0, j = 0; i < binlength; i += 3)
	{
		current = (strNeedEndata[i] >> 2);
		current &= (unsigned char)0x3F;
		base64[j++] = base64char[(int)current];

		current = ((unsigned char)(strNeedEndata[i] << 4)) & ((unsigned char)0x30);
		if (i + 1 >= binlength)
		{
			base64[j++] = base64char[(int)current];
			base64[j++] = '=';
			base64[j++] = '=';
			break;
		}
		current |= ((unsigned char)(strNeedEndata[i + 1] >> 4)) & ((unsigned char)0x0F);
		base64[j++] = base64char[(int)current];

		current = ((unsigned char)(strNeedEndata[i + 1] << 2)) & ((unsigned char)0x3C);
		if (i + 2 >= binlength)
		{
			base64[j++] = base64char[(int)current];
			base64[j++] = '=';
			break;
		}
		current |= ((unsigned char)(strNeedEndata[i + 2] >> 6)) & ((unsigned char)0x03);
		base64[j++] = base64char[(int)current];

		current = ((unsigned char)strNeedEndata[i + 2]) & ((unsigned char)0x3F);
		base64[j++] = base64char[(int)current];
	}
	base64[j] = '\0';

	return base64;
}

void * base64_decode(const char * base64, size_t * pbinSize) {
	int i, j;
	unsigned char k;
	unsigned char temp[4];
	unsigned char * binDedata = (unsigned char*)malloc(strlen(base64) / 4 * 3);

	for (i = 0, j = 0; base64[i] != '\0'; i += 4)
	{
		memset(temp, 0xFF, sizeof(temp));
		for (k = 0; k < 64; k++)
		{
			if (base64char[k] == base64[i])
				temp[0] = k;
		}
		for (k = 0; k < 64; k++)
		{
			if (base64char[k] == base64[i + 1])
				temp[1] = k;
		}
		for (k = 0; k < 64; k++)
		{
			if (base64char[k] == base64[i + 2])
				temp[2] = k;
		}
		for (k = 0; k < 64; k++)
		{
			if (base64char[k] == base64[i + 3])
				temp[3] = k;
		}

		binDedata[j++] = ((unsigned char)(((unsigned char)(temp[0] << 2)) & 0xFC)) |
			((unsigned char)((unsigned char)(temp[1] >> 4) & 0x03));
		if (base64[i + 2] == '=')
			break;

		binDedata[j++] = ((unsigned char)(((unsigned char)(temp[1] << 4)) & 0xF0)) |
			((unsigned char)((unsigned char)(temp[2] >> 2) & 0x0F));
		if (base64[i + 3] == '=')
			break;

		binDedata[j++] = ((unsigned char)(((unsigned char)(temp[2] << 6)) & 0xF0)) |
			((unsigned char)(temp[3] & 0x3F));
	}

	if (pbinSize) {
		*pbinSize = j;
	}

	return binDedata;
}
