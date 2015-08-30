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
#include <string.h>

#define MAX_TRANSLATION_STRINGS 1024

#define ERR_NO                0
#define ERR_CANNOT_OPEN_FILE  1
#define ERR_NOT_ENOUGH_MEMORY 2
#define ERR_WRONG_TERM_MODE   3

#define TRL_MODE_DEF false
#define TRL_MODE_TRL true

// Safely delete pointers
#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if((p) != NULL) { delete (p);     (p) = NULL; } }
#define SAFE_DELETE_ARRAY(p) { if((p) != NULL) { delete [](p);  (p) = NULL; } }
#define SAFE_RELEASE(p)      { if((p) != NULL) { (p)->Release(); (p) = NULL; } }
#endif

static bool m_bTableLoaded = false;
static bool m_bReferenceLoaded = false;

static char *m_pReferenceStrings = NULL;
static char *m_pTranslationStrings = NULL;

static unsigned long m_dwNumRefStrings = 0;
static unsigned long m_dwNumTrlStrings = 0;

static char *m_pDefRefString[MAX_TRANSLATION_STRINGS];
static char *m_pTrlRefString[MAX_TRANSLATION_STRINGS];
static char *m_pDefString[MAX_TRANSLATION_STRINGS];
static char *m_pTrlString[MAX_TRANSLATION_STRINGS];

int LoadReferenceTable(const char *pszFileName);
int LoadTranslationTable(const char *pszFileName);
bool FreeCurrentReferenceTable();
bool FreeCurrentTranslationTable();
void _SortTrlTable();
void _SortRefTable();

const char *TranslateString(const char *pszDefString);
const char *ReferizeString(const char *pszDefString);

void PrintError(int i, char *pAdditionalInfo);

bool CheckTrlRefPair();

void LogPrint(const char *pszString);

int main(int argc, char *argv[])
{
	int i;

	if(argc != 2)
	{
		printf("Usage: ChkTrans.exe <YourTranslationFile.lng>\n");
		return 1;
	}
	
	FILE *fpLogCleaner = fopen("Results.txt", "wb");
	fclose(fpLogCleaner);

	i = LoadReferenceTable("LangRef.src");
	PrintError(i, "LangRef.src");
	if(i != ERR_NO) return 1;

	i = LoadTranslationTable(argv[1]);
	PrintError(i, argv[1]);
	if(i != ERR_NO) return 1;
	
	printf("\n");

	if(CheckTrlRefPair() == false)
	{
		printf("There were some errors in your translation file.\r\nSee file 'Results.txt' for more.\n");
	}
	else
	{
		printf("Your translation file is 100%% perfectly valid and up-to-date.\nNo errors; now go and distribute it :)\n");
	}
	
	printf("\nMake sure you didn't forget to update the ~LANGUAGEVERSION field ;)\n");

	FreeCurrentTranslationTable();
	FreeCurrentReferenceTable();

	return 0;
}

void PrintError(int i, char *pAdditionalInfo)
{
	if(i == ERR_NO)
	{
		printf("Loading file '");
		if(pAdditionalInfo != NULL) printf(pAdditionalInfo);
		printf("' succeeded!\n");
	}
	else if(i == ERR_CANNOT_OPEN_FILE)
	{
		printf("Fatal error: Cannot open file '");
		if(pAdditionalInfo != NULL) printf(pAdditionalInfo);
		printf("'!\n");
	}
	else if(i == ERR_NOT_ENOUGH_MEMORY)
	{
		printf("Fatal error: Cannnot allocate enough memory!\n");
	}
	else if(i == ERR_WRONG_TERM_MODE)
	{
		printf("Error: Wrong termination mode.\nThis means something is wrong with the input file:\n");
		if(pAdditionalInfo != NULL) printf(pAdditionalInfo);
		printf("\n");
	}
}

int LoadTranslationTable(const char *pszFileName)
{
	FILE *fp = NULL;
	unsigned long i = 0;
	unsigned long dwLength = 0;
	bool bMode = false;
	bool bScanning = false;

	if(m_bTableLoaded == true) FreeCurrentTranslationTable();

	fp = fopen(pszFileName, "rb");
	if(fp == NULL) return ERR_CANNOT_OPEN_FILE;

	fseek(fp, 0, SEEK_END);
	dwLength = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	m_pTranslationStrings = new char[dwLength+1];
	if(m_pTranslationStrings == NULL) { fclose(fp); return ERR_NOT_ENOUGH_MEMORY; }
	m_pTranslationStrings[dwLength] = 0;

	fread(m_pTranslationStrings, 1, dwLength, fp);

	fclose(fp);
	fp = NULL;

	m_dwNumTrlStrings = 0;

	bMode = TRL_MODE_DEF;
	bScanning = false;
	for(i = 0; i < dwLength; i++)
	{
		if(m_pTranslationStrings[i] == '|')
		{
			if((bMode == TRL_MODE_DEF) && (bScanning == false))
			{
				m_pDefString[m_dwNumTrlStrings] = &m_pTranslationStrings[i+1];
				m_pTranslationStrings[i] = 0;
				bScanning = true;

				continue;
			}
			
			if((bMode == TRL_MODE_DEF) && (bScanning == true))
			{
				m_pTranslationStrings[i] = 0;
				bMode = TRL_MODE_TRL;
				bScanning = false;

				continue;
			}
			if((bMode == TRL_MODE_TRL) && (bScanning == false))
			{
				m_pTrlString[m_dwNumTrlStrings] = &m_pTranslationStrings[i+1];
				m_pTranslationStrings[i] = 0;
				bScanning = true;

				continue;
			}
			if((bMode == TRL_MODE_TRL) && (bScanning == true))
			{
				m_pTranslationStrings[i] = 0;
				bMode = TRL_MODE_DEF;
				bScanning = false;

				m_dwNumTrlStrings++;
				if(m_dwNumTrlStrings == MAX_TRANSLATION_STRINGS) break;

				continue;
			}
		}
	}

	if(bMode != TRL_MODE_DEF) return ERR_WRONG_TERM_MODE;
	if(bScanning != false) return ERR_WRONG_TERM_MODE;

	for(i = 0; i < m_dwNumTrlStrings; i++)
	{
		if(strlen(m_pTrlString[i]) == 0)
		{
			m_pTrlString[i] = m_pDefString[i];
		}
	}

	_SortTrlTable();
	m_bTableLoaded = true;
	return ERR_NO;
}

bool FreeCurrentTranslationTable()
{
	SAFE_DELETE_ARRAY(m_pTranslationStrings);
	m_dwNumTrlStrings = 0;

	if(m_bTableLoaded == false) return false;
	return true;
}

int LoadReferenceTable(const char *pszFileName)
{
	FILE *fp = NULL;
	unsigned long i = 0;
	unsigned long dwLength = 0;
	bool bMode = false;
	bool bScanning = false;

	if(m_bReferenceLoaded == true) FreeCurrentReferenceTable();

	fp = fopen(pszFileName, "rb");
	if(fp == NULL) return ERR_CANNOT_OPEN_FILE;

	fseek(fp, 0, SEEK_END);
	dwLength = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	m_pReferenceStrings = new char[dwLength+1];
	if(m_pReferenceStrings == NULL) { fclose(fp); return ERR_NOT_ENOUGH_MEMORY; }
	m_pReferenceStrings[dwLength] = 0;

	fread(m_pReferenceStrings, 1, dwLength, fp);

	fclose(fp);
	fp = NULL;

	m_dwNumRefStrings = 0;

	bMode = TRL_MODE_DEF;
	bScanning = false;
	for(i = 0; i < dwLength; i++)
	{
		if(m_pReferenceStrings[i] == '|')
		{
			if((bMode == TRL_MODE_DEF) && (bScanning == false))
			{
				m_pDefRefString[m_dwNumRefStrings] = &m_pReferenceStrings[i+1];
				m_pReferenceStrings[i] = 0;
				bScanning = true;

				continue;
			}
			
			if((bMode == TRL_MODE_DEF) && (bScanning == true))
			{
				m_pReferenceStrings[i] = 0;
				bMode = TRL_MODE_TRL;
				bScanning = false;

				continue;
			}
			if((bMode == TRL_MODE_TRL) && (bScanning == false))
			{
				m_pTrlRefString[m_dwNumRefStrings] = &m_pReferenceStrings[i+1];
				m_pReferenceStrings[i] = 0;
				bScanning = true;

				continue;
			}
			if((bMode == TRL_MODE_TRL) && (bScanning == true))
			{
				m_pReferenceStrings[i] = 0;
				bMode = TRL_MODE_DEF;
				bScanning = false;

				m_dwNumRefStrings++;
				if(m_dwNumRefStrings == MAX_TRANSLATION_STRINGS) break;

				continue;
			}
		}
	}

	if(bMode != TRL_MODE_DEF) return ERR_WRONG_TERM_MODE;
	if(bScanning != false) return ERR_WRONG_TERM_MODE;

	for(i = 0; i < m_dwNumRefStrings; i++)
	{
		if(strlen(m_pTrlRefString[i]) == 0)
		{
			m_pTrlRefString[i] = m_pDefRefString[i];
		}
	}

	_SortRefTable();
	m_bReferenceLoaded = true;
	return ERR_NO;
}

bool FreeCurrentReferenceTable()
{
	SAFE_DELETE_ARRAY(m_pReferenceStrings);
	m_dwNumRefStrings = 0;

	if(m_bReferenceLoaded == false) return false;
	return true;
}

const char *TranslateString(const char *pszDefString)
{
	if(m_dwNumTrlStrings == 0) { printf("Fatal error: ID 12445\n"); return NULL; }
	if(pszDefString == NULL) { printf("Fatal error: ID 78049\n"); return NULL; }

	// Fast binary search on the sorted list of translation strings:
	static int l, r, x, c;
	l = 0; r = ((int)m_dwNumTrlStrings) - 1;
	while(l != r)
	{
		x = (l + r) >> 1;
		c = strcmp(m_pDefString[x], pszDefString);
		if(c < 0) l = x + 1;
		else r = x;
	}
	if(strcmp(m_pDefString[l], pszDefString) == 0) return m_pTrlString[l];
	return NULL;
}

const char *ReferizeString(const char *pszDefString)
{
	if(m_dwNumRefStrings == 0) { printf("Fatal error: ID 42964\n"); return NULL; }
	if(pszDefString == NULL) { printf("Fatal error: ID 89034\n"); return NULL; }

	// Fast binary search on the sorted list of translation strings:
	static int l, r, x, c;
	l = 0; r = ((int)m_dwNumRefStrings) - 1;
	while(l != r)
	{
		x = (l + r) >> 1;
		c = strcmp(m_pDefRefString[x], pszDefString);
		if(c < 0) l = x + 1;
		else r = x;
	}
	if(strcmp(m_pDefRefString[l], pszDefString) == 0) return m_pTrlRefString[l];
	return NULL;
}

void _SortTrlTable()
{
	unsigned long i, j = 0, min;
	char *v;

	if(m_dwNumTrlStrings <= 1) return;

	for(i = 0; i < (m_dwNumTrlStrings - 1); i++)
	{
		min = i;

		for(j = i + 1; j < m_dwNumTrlStrings; j++)
		{
			if(strcmp(m_pDefString[j], m_pDefString[min]) < 0)
				min = j;
		}

		v = m_pDefString[min];
		m_pDefString[min] = m_pDefString[i];
		m_pDefString[i] = v;
		v = m_pTrlString[min];
		m_pTrlString[min] = m_pTrlString[i];
		m_pTrlString[i] = v;
	}
}

void _SortRefTable()
{
	unsigned long i, j = 0, min;
	char *v;

	if(m_dwNumRefStrings <= 1) return;

	for(i = 0; i < (m_dwNumRefStrings - 1); i++)
	{
		min = i;

		for(j = i + 1; j < m_dwNumRefStrings; j++)
		{
			if(strcmp(m_pDefRefString[j], m_pDefRefString[min]) < 0)
				min = j;
		}

		v = m_pDefRefString[min];
		m_pDefRefString[min] = m_pDefRefString[i];
		m_pDefRefString[i] = v;
		v = m_pTrlRefString[min];
		m_pTrlRefString[min] = m_pTrlRefString[i];
		m_pTrlRefString[i] = v;
	}
}

void LogPrint(const char *pszString)
{
	FILE *fp;

	fp = fopen("Results.txt", "ab");
	if(fp == NULL) return;

	fprintf(fp, "%s", pszString);

	fclose(fp);
}

bool CheckTrlRefPair()
{
	unsigned long i;
	char *p;
	bool bNoError = true;

	LogPrint("===============================================================\r\n");
	LogPrint("= The following strings are missing in your translation:\r\n\r\n");

	for(i = 0; i < m_dwNumRefStrings; i++)
	{
		p = (char *)TranslateString(m_pDefRefString[i]);
		if(p == NULL)
		{
			LogPrint("|"); LogPrint(m_pDefRefString[i]); LogPrint("|\r\n");
			bNoError = false;
		}
	}

	LogPrint("\r\n\r\n\r\n===============================================================\r\n");
	LogPrint("= The following strings are not needed any more and you should\r\n");
	LogPrint("= remove them from your translation file:\r\n\r\n");

	for(i = 0; i < m_dwNumTrlStrings; i++)
	{
		p = (char *)ReferizeString(m_pDefString[i]);
		if(p == NULL)
		{
			LogPrint("|"); LogPrint(m_pDefString[i]); LogPrint("|\r\n");
			bNoError = false;
		}
	}
	
	return bNoError;
}

