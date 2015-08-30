/*
  KeePass Password Safe - The Open-Source Password Manager
  Copyright (C) 2003-2005 Dominik Reichl <dominik.reichl@t-online.de>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
	FILE *fpIn = NULL;
	FILE *fpOut = NULL;
	char *pDataIn = NULL;
	char *pDataOut = NULL;
	unsigned long dwSrcSize = 0;
	unsigned long i = 0, j = 0;
	bool bSkip = false;
	unsigned long uDivCount = 0;

	if(argc <= 2)
	{
		printf("Error: Too few command-line arguments!\n");
		return 1;
	}

	fpIn = fopen(argv[1], "rb");
	if(fpIn == NULL)
	{
		printf("Error: Cannot open input file!\n");
		return 2;
	}

	fpOut = fopen(argv[2], "wb");
	if(fpOut == NULL)
	{
		printf("Error: Cannot open output file!\n");
		fclose(fpIn);
		return 2;
	}

	fseek(fpIn, 0, SEEK_END);
	dwSrcSize = ftell(fpIn);
	fseek(fpIn, 0, SEEK_SET);

	pDataIn = new char[dwSrcSize];
	pDataOut = new char[dwSrcSize];

	fread(pDataIn, 1, dwSrcSize, fpIn);

	j = 0;
	bSkip = true;
	for(i = 0; i < dwSrcSize; i++)
	{
		if(pDataIn[i] == '|')
		{
			uDivCount++;

			if(bSkip == true)
			{
				bSkip = false;
				pDataOut[j] = pDataIn[i];
				j++;
			}
			else // bSkip == false
			{
				bSkip = true;
				pDataOut[j] = pDataIn[i];
				j++;
			}
		}
		else
		{
			if(bSkip == false)
			{
				pDataOut[j] = pDataIn[i];
				j++;
			}
		}
	}

	fwrite(pDataOut, 1, j, fpOut);

	fclose(fpOut);
	fclose(fpIn);

	delete []pDataOut;
	delete []pDataIn;

	if((uDivCount % 4) == 0) // !(uDivCount & 1)
	{
		printf("Translation ");
		printf("%12s", argv[2]);
		printf(" successful, %4u strings translated.\r\n", uDivCount / 4);
	}
	else
	{
		printf("Compilation failed. Output is corrupted.\r\n");
	}

	return 0;
}

