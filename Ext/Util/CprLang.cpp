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

	if(!(uDivCount & 1))
	{
		printf("Compilation successful.\r\n");
	}
	else
	{
		printf("Compilation failed. Output is corrupted.\r\n");
	}

	return 0;
}

