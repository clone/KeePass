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
#include <string>
#include <vector>

typedef std::basic_string<TCHAR> std_string;

/*
The CommandLineTokens class provides access to each token on the command line,
with semantics somewhat different from argc/argv.

With argc/argv, I found the following undocumented behavior:

1. A quoted string is mapped into an unquoted string.  For example,
           "12345" --> 12345

2. Backspace is the escape character for inputting a literal quote.  Examples: 
           123\"45 --> 123"45
           "123\"45" --> 123"45

3. Backspace is also used for inputting backspace-quote, backspace-backspace-quote, 
   backspace-backspace-backspace-quote, etc.  Examples:
           123\\"45 --> 123\"45
           123\\\"45 --> 123\"45
           123\\\\"45 --> 123\\"45
           123\\\\\"45 --> 123\\"45
   In general, 2n or 2n+1 backspace characters followed by quote maps into 
   n backspace characters followed by quote.


The special interpretation of backspace-quote is unsatisfactory in KeePass 
for at least two reasons:

1. A master password command line argument can contain a quote or n
   backslash characters followed by quote.  There should be a reasonably 
   natural way to code them.

2. Some KeePass plugins (which reuse KeePass command processing code) have
   path command line arguments such as /backup.path:"C:\"   It would be
   very unfriendly if users had to enter this as /backup.path:"C:\\"


The CommandLineTokens class parses the raw command line from Win32 
GetCommandLine() into tokens using the following simple rules:

1. A quoted string is mapped into an unquoted string.  For example,
           "12345" --> 12345                  (SpaceBalls password)

2. Quote is the escape character for inputting a literal quote.  Examples: 
           123""45 --> 123"45
           "123""45" --> 123"45

Thus, the backslash has no special status, and can be used in a natural
way in passwords and paths.

If the input command line contains an unmatched quote, the 
displayIgnoredMessage() member function is invoked (displaying a
suitable MessageBox), and the final (incomplete) token is not appended 
to the container.  displayIgnoredMessage() is public for reuse by 
the CmdArgs class.

CommandLineTokens uses the singleton design pattern.
CommandLineTokens objects are immutable.
*/

class CommandLineTokens {
public:
    typedef std::vector<std_string> string_vector;
    typedef string_vector::const_iterator const_iterator;

    const_iterator begin() const {return m_TokenContainer.begin();}
    const_iterator   end() const {return m_TokenContainer.end();}

    static const CommandLineTokens& instance();
    static void displayIgnoredMessage(const std_string& argument, const std_string& reason);

private:
    CommandLineTokens();

    string_vector m_TokenContainer;        
};
