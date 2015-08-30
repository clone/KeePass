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
#pragma once
#include <cassert>
#include <string>

typedef std::basic_string<TCHAR> std_string;
/*
The FullPathName class is a wrapper for the Win32 SDK GetFullPathName() function,
which converts an input path name to a canonical form.  Convenience member
functions provide the complete canonical form, as well as its path and filename components.

The input path name may be relative or absolute, may contain a mixture of
"\" and "/" separators, and may or may not end in a separator.  If it ends
in a separator, it's considered a PATH_ONLY path; otherwise, the last part
is considered to be a filename component.  (Exception:  If the input path
begins witn "\\", it is always considered PATH_ONLY.  A separator is appended
by FullPathName if needed.)

In canonical form, relative paths are converted to absolute paths, and all "/"
separators are replaced by "\" (though this is not documented in GetFullPathName()).

If input path name has no non-blank characters (and perhaps in some other cases), 
then the FullPathName object state is INVALID_PATH.  

If input path name ends in "\" or "/", then the FullPathName object state is PATH_ONLY;
otherwise, the state is PATH_AND_FILENAME.

FullPathName objects are immutable.
*/

class FullPathName {
public:
    enum STATE {UNINITIALIZED     = 1<<0,  // Only if default constructed.
                INVALID_PATH      = 1<<1,  // path is invalid.
                PATH_ONLY         = 1<<2,  // path ends in "\"
                PATH_AND_FILENAME = 1<<3}; // path does not end in "\"; last part is filename.

    FullPathName() : m_state(UNINITIALIZED) {}
    explicit FullPathName(const std_string& pathName);
    STATE getState() const {return m_state;}

    const std_string& getFullPathName() const {
        assert(m_state==PATH_ONLY || m_state==PATH_AND_FILENAME);  // Precondition
        return m_FullPathName;
        }

    const std_string& getPathOnly() const {
        assert(m_state==PATH_ONLY || m_state==PATH_AND_FILENAME);  // Precondition
        return m_PathOnly;
        }

    const std_string& getFileName() const {        
        assert(m_state==PATH_AND_FILENAME);  // Precondition
        return m_FileName;
        }

private:
    STATE      m_state;
    std_string m_FullPathName;
    std_string m_PathOnly;
    std_string m_FileName;
};
