/*==========================================================================
Copyright (c) 2006, Bill Rubin <rubin@contractor.net>
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright 
      notice, this list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright 
      notice, this list of conditions and the following disclaimer in the 
      documentation and/or other materials provided with the distribution.

    * Neither the name of Quality Object Software, Inc., nor the names of 
      its contributors may be used to endorse or promote products derived 
      from this software without specific prior written permission.

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
==========================================================================*/
#include "StdAfx.h"
#include "FullPathName.h"
#include <memory>

FullPathName::FullPathName(const std_string& pathName) 
    : m_state(PATH_AND_FILENAME) {  // Optimistic (and typical) state initialization.
	TCHAR* filePart;
    TCHAR* fullPathName;

    // Attempt to use buffer on stack:
    TCHAR stackFullPathName[MAX_PATH];
    const DWORD length = GetFullPathName(pathName.c_str(), MAX_PATH, stackFullPathName, &filePart);
    if(length==0) {
        m_state = INVALID_PATH; // Function failed.
        return;
        }
    
    typedef std::auto_ptr<TCHAR> SmartCharPtr;
    SmartCharPtr heapFullPathName;
    if(length<MAX_PATH) fullPathName = stackFullPathName;

    else { // Result does not fit in stackFullPathName, so use buffer on heap:
        heapFullPathName.reset(new TCHAR[length]);
        const DWORD dw = GetFullPathName(pathName.c_str(), length, heapFullPathName.get(), &filePart);
        if(dw==0) {
            m_state = INVALID_PATH; // Function failed.
            return;
            }
        else fullPathName = heapFullPathName.get();
        }

    // Either PATH_AND_FILENAME or PATH_ONLY.
    m_FullPathName = fullPathName;
    assert(m_FullPathName.length());
    static const TCHAR BACKSLASH = _T('\\');

    if(filePart==0) {
        m_state = PATH_ONLY;

        // If pathName begins with double backslash ("\\") then GetFullPathName() 
        // considers it a path, even if it doesn't end in backslash.  
        // To preserve invariant, we check whether path ends in backslash, and append one if needed:
        if(*m_FullPathName.rbegin()!=BACKSLASH) m_FullPathName += BACKSLASH;

        // If there is no filePart, GetFullPathName() sets filePart to null,
        // instead of to point to the fullPathName's null terminator.  Duh!
        // The following line fixes this problem:
        filePart = &fullPathName[m_FullPathName.length()];
        }
    
    const std_string::size_type fileNameIndex = filePart - fullPathName;      
    assert(fileNameIndex>0);

    m_PathOnly = m_FullPathName.substr(0, fileNameIndex);
    // Postcondition:  Path ends in backslash
    assert(*m_PathOnly.rbegin()==BACKSLASH);

    m_FileName = m_FullPathName.substr(fileNameIndex);
    // Postcondition:  Filename does not contain backslash
    assert(m_FileName.find(BACKSLASH)==std_string::npos);        
}
