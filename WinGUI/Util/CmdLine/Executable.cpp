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
#include "Executable.h"
#include <memory>

const FullPathName& Executable::instance() {
    static const Executable singleton;
    return singleton.m_module;
}

namespace {

inline std_string getModuleFileName() {
    enum {INITIAL_MAX_LENGTH = 1024};
	TCHAR stackModuleFileName[INITIAL_MAX_LENGTH];
	const DWORD length = GetModuleFileName(NULL, stackModuleFileName, INITIAL_MAX_LENGTH);
    assert(length);  // Function succeeded.
    if(length<INITIAL_MAX_LENGTH) return stackModuleFileName;

    // Use larger buffer, until successful:
    DWORD max_length = INITIAL_MAX_LENGTH;
    while(true) {
        max_length *= 2;  // Double the maximum length.
        typedef std::auto_ptr<TCHAR> SmartCharPtr;
        SmartCharPtr heapModuleFileName(new TCHAR[max_length]);
       	const DWORD length = GetModuleFileName(NULL, heapModuleFileName.get(), max_length);
        assert(length);  // Function succeeded.
        if(length<max_length) return heapModuleFileName.get();
        }
}

}

Executable::Executable() : m_module(getModuleFileName()) {}
