/*
  Copyright (c) 2003/2004, Dominik Reichl <dominik.reichl@t-online.de>
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

  - Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer.
  - Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
  - Neither the name of ReichlSoft nor the names of its contributors may be
    used to endorse or promote products derived from this software without
    specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
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

