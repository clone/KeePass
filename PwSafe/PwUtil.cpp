/*
  Copyright (c) 2003-2005, Dominik Reichl <dominik.reichl@t-online.de>
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

#include "StdAfx.h"
#include <math.h>

#include "PwUtil.h"

#define CHARSPACE_ESCAPE      60
#define CHARSPACE_ALPHA       26
#define CHARSPACE_NUMBER      10
#define CHARSPACE_SIMPSPECIAL 16
#define CHARSPACE_EXTSPECIAL  17
#define CHARSPACE_HIGH       112

// Very simple password quality estimation function
C_FN_SHARE DWORD EstimatePasswordBits(const TCHAR *pszPassword)
{
	DWORD i, dwLen, dwCharSpace, dwBits;
	BOOL bChLower = FALSE, bChUpper = FALSE, bChNumber = FALSE;
	BOOL bChSimpleSpecial = FALSE, bChExtSpecial = FALSE, bChHigh = FALSE;
	BOOL bChEscape = FALSE;
	TCHAR tch;
	double dblBitsPerChar;

	ASSERT(pszPassword != NULL); if(pszPassword == NULL) return 0;

	dwLen = (DWORD)_tcslen(pszPassword);
	if(dwLen == 0) return 0; // Zero bits of information :)

	for(i = 0; i < dwLen; i++) // Get character types
	{
		tch = pszPassword[i];

		if(tch < _T(' ')) bChEscape = TRUE;
		if((tch >= _T('A')) && (tch <= _T('Z'))) bChUpper = TRUE;
		if((tch >= _T('a')) && (tch <= _T('z'))) bChLower = TRUE;
		if((tch >= _T('0')) && (tch <= _T('9'))) bChNumber = TRUE;
		if((tch >= _T(' ')) && (tch <= _T('/'))) bChSimpleSpecial = TRUE;
		if((tch >= _T(':')) && (tch <= _T('@'))) bChExtSpecial = TRUE;
		if((tch >= _T('[')) && (tch <= _T('`'))) bChExtSpecial = TRUE;
		if((tch >= _T('{')) && (tch <= _T('~'))) bChExtSpecial = TRUE;
		if(tch > _T('~')) bChHigh = TRUE;
	}

	dwCharSpace = 0;
	if(bChEscape == TRUE) dwCharSpace += CHARSPACE_ESCAPE;
	if(bChUpper == TRUE) dwCharSpace += CHARSPACE_ALPHA;
	if(bChLower == TRUE) dwCharSpace += CHARSPACE_ALPHA;
	if(bChNumber == TRUE) dwCharSpace += CHARSPACE_NUMBER;
	if(bChSimpleSpecial == TRUE) dwCharSpace += CHARSPACE_SIMPSPECIAL;
	if(bChExtSpecial == TRUE) dwCharSpace += CHARSPACE_EXTSPECIAL;
	if(bChHigh == TRUE) dwCharSpace += CHARSPACE_HIGH;

	ASSERT(dwCharSpace != 0); if(dwCharSpace == 0) return 0;

	dblBitsPerChar = log((double)dwCharSpace) / log(2.00);
	dwBits = (DWORD)(ceil(dblBitsPerChar * (double)dwLen));

	ASSERT(dwBits != 0);
	return dwBits;
}
