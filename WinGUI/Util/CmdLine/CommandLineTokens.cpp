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
#include "CommandLineTokens.h"
#include "../../../KeePassLibCpp/Util/TranslateEx.h"
#include <cassert>
#include <cctype>
#include <sstream>

namespace {

/*
A Token is essentially a std_string.  The only significant difference
is that Token has its own custom operator>>(std_istream&, Token&)
which reads from stream in a somewhat different way from
operator>>(std_istream&, std_string&).
*/

class Token {
public:
    TCHAR operator+=(const TCHAR c) {m_value += c; return c;}
    operator const std_string&() const {return m_value;}
    void eraseLastChar() {assert(m_value.size()); m_value.erase(m_value.size() - 1);}

private:
    std_string m_value;
};


typedef std::basic_istream<TCHAR> std_istream;

    /*
    As long as operator>>() returns with valid istream,
    it will be called again.  Once it has caused istream to
    become invalid, it will not be called again.
    After returning with invalid stream,
    stream iterator compares equal to default stream iterator,
    thus preventing copying of last token to token container.

    Parts of this function were modelled loosely after code in 
    Stroustrup's "C++ Programming Language" (Special Edition), 
    Sect. 6.1.3, p. 115.
    */
std_istream& operator>>(std_istream& is, Token& token) {
    static const TCHAR QUOTE = _T('"');
    static const Token EMPTY;
    assert(is.good());
    token = EMPTY;
    TCHAR c;

    // Skip whitespace:
    do {
        if(!is.get(c)) return is;  // EOF caused by valid command line (no unmatched quotes)
        } while(std::isspace(c));
    
    bool outsideQuotedString = true;

    // Read stream, beginning with non-whitespace character:
    do {
        if(c==QUOTE) {
            // Look ahead:
            TCHAR nextChar;
            is.get(nextChar);
            assert(is);  // Sentinel guarantees last character is blank.  Since c is a quote, it cannot be last character.

            // Map pair of quotes to literal quote, consuming nextChar:
            if(nextChar==QUOTE) token += QUOTE;
    
            else {  // Non-pair of quotes delimits quoted string:
                outsideQuotedString = !outsideQuotedString;
                is.putback(nextChar);  // Reprocess nextChar.
                }
            }

        else if(outsideQuotedString && std::isspace(c)) return is;  // End of space-delimited token.  (Stream is not at EOF.)

        else token += c;  // Most common case.
        
        } while(is.get(c));

    // Handle unmatched quote:
    assert(is.eof());             // Therefore, this is last token.
    assert(!outsideQuotedString); // Therefore, have unmatched quote.
    token.eraseLastChar();        // Remove sentinel.
    CommandLineTokens::displayIgnoredMessage(token, _T("It contains an unmatched quote"));
    return is;                    // copy() will ignore this token.
}


#ifdef _DEBUG
void display(const CommandLineTokens& commandLineTokens) {  // For debug only.
    typedef CommandLineTokens::const_iterator const_iterator;
    const const_iterator end(commandLineTokens.end());
    TRACE("\nCommand line '%s' is tokenized as follows:\n", GetCommandLine());
    for(const_iterator it = commandLineTokens.begin(); it!=end; ++it) TRACE("     '%s'\n", it->c_str());
    TRACE("\n");
}
#endif

}

// -------------------------------------------------------------------------------------

    /*
    The constructor gets the command line, converts it to an istringstream,
    and then iterates through the stream using a custom stream iterator
    which interprets everything as a Token.  The iteration occurs while
    the copy algorithm copies the stream to the token-container.

    The SENTINEL value assures that last streamed character is blank.
    If the original command line has no unmatched quote, the blank 
    terminates the last token.  If the original command line has 
    an unmatched quote, the blank does not terminate the last token, 
    so operator>>() will try to read an additional character, 
    resulting in EOF.
    */
CommandLineTokens::CommandLineTokens() {
    const std_string SENTINEL(_T(" "));
    std_string commandLine(GetCommandLine());
    commandLine += SENTINEL;  // Assure predictable scanning at end of stream.
    typedef std::basic_istringstream<TCHAR> std_istringstream;
    std_istringstream commandLineStream(commandLine);
    typedef std::istream_iterator<Token, TCHAR> ArgStringIterator;  // Read command line as Tokens!
    const ArgStringIterator end;
    std::copy(ArgStringIterator(commandLineStream), end, std::back_inserter(m_TokenContainer));
    assert(!m_TokenContainer.empty());  // Command line always contains name of executable.
    m_TokenContainer.erase(m_TokenContainer.begin());  // Remove name of executable.
    #ifdef _DEBUG
    display(*this);
    #endif
}

const CommandLineTokens& CommandLineTokens::instance() {
    static const CommandLineTokens singleton;
    return singleton;
}

void CommandLineTokens::displayIgnoredMessage(const std_string& argument, const std_string& reason) {
    std_string CLA(TRL("Command line argument"));
	CLA += _T(" '");

    std_string message(CLA + argument);
	message += _T("' ");
	message += TRL("ignored");
	message += _T(".\r\n\r\n");
	message += TRL("Reason");
	message += _T(": ");
	message += reason;
	message += _T(".");

    VERIFY(MessageBox(AfxGetMainWnd()->m_hWnd, message.c_str(), TRL("KeePass Initialization"), MB_OK | MB_ICONWARNING) != 0);
}