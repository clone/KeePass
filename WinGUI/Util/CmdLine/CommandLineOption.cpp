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
#include "CommandLineOption.h"
#include <cctype>

namespace {

const std_string PREFIXES(_T("/-"));

}


bool CommandLineOption::beginsWithPrefix(const std_string& argument)
    {return PREFIXES.find(argument[0])!=std_string::npos;}


CommandLineOption::CommandLineOption(const std_string& optionName) 
    : m_optionName(optionName) {
    #ifdef _DEBUG
    ASSERT(!optionName.empty());
    typedef std_string::const_iterator const_iterator;

    // For each character ...
    for(const_iterator it = optionName.begin(); it!=optionName.end(); ++it) 
        ASSERT(std::islower(*it) || std::ispunct(*it));  // Character is lower case or punctuation.

    #endif
}


bool CommandLineOption::isOption(const std_string& argument) const {
    typedef std_string::size_type size_type;
    const size_type optionLength = m_optionName.length();

    // If argument is too short, or does not begin with prefix character, it is not an option:      
    if(argument.length()<optionLength + 1 || !beginsWithPrefix(argument)) return false;

    // Examine the option name part ...
    std_string optionName(argument, 1, optionLength);
    typedef std_string::iterator iterator;

    // ... and convert it to lower case:
    for(iterator it = optionName.begin(); it!=optionName.end(); ++it) 
        *it = static_cast<std_string::value_type>(std::tolower(*it));

    return optionName==m_optionName;
}
