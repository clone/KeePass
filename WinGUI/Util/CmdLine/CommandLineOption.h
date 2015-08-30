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
The CommandLineOption class encapsulates the semantics of a command line option.
A command line option is assumed to comprise a prefix followed by a 
case-insensitive option name, possibly followed by an option value.

A convenience member function isOption() determines whether a given command line
argument is in the form of an option.  If so, the optionValue() member function
returns the value.
*/

// CommandLineOption objects are immutable.

class CommandLineOption {
public:
    explicit CommandLineOption(const std_string& optionName);  // Precondition:  optionName is lower case or punctuation
    const std_string& optionName() const {return m_optionName;}
    bool isOption(const std_string& argument) const;
    std_string optionValue(const std_string& argument) const {return argument.substr(optionValueOffset());}

    std_string::size_type optionValueOffset() const {return m_optionName.length() + 1;}

    static bool beginsWithPrefix(const std_string& argument);

private:
    const std_string m_optionName;
};
