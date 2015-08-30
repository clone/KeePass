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

// DR 2008-09-10: Added 'lock' command line option
// DR 2008-11-25: Added 'set-urloverride' command line option
// DR 2009-06-05: Added 'minimize' command line option
// DR 2012-08-14: Added 'pw-stdin' command line option
// DR 2014-10-25: Added 'cfg-local' command line option

#include "StdAfx.h"
#include "CmdArgs.h"
#include "CommandLineTokens.h"
#include "CommandLineOption.h"
#include "../../../KeePassLibCpp/Util/TranslateEx.h"
#include "../../Plugins/PluginMgr.h"

const CmdArgs& CmdArgs::instance() {
    static const CmdArgs singleton;
    return singleton;
}

CmdArgs::CmdArgs() : m_isPreselect(false), m_isReadOnly(false), m_isLock(false),
    m_isMin(false), m_isPwStdIn(false) {

    // Command line options:
    const CommandLineOption keyfile    (_T("keyfile:"));
    const CommandLineOption preselect  (_T("preselect:"));
    const CommandLineOption readonly   (_T("readonly"));
    const CommandLineOption pw         (_T("pw:"));
    const CommandLineOption pwStdIn    (_T("pw-stdin"));
    const CommandLineOption lockcw     (_T("lock"));
    const CommandLineOption mincw      (_T("minimize"));
    const CommandLineOption urloverride(_T("set-urloverride:"));
    // const CommandLineOption cfgLocal   (_T("cfg-local:"));

    // For each command line argument ...
    typedef CommandLineTokens::const_iterator const_iterator;
    const const_iterator end(CommandLineTokens::instance().end());
    for(const_iterator it = CommandLineTokens::instance().begin(); it!=end; ++it) {
        const std_string argument(*it);

        // keyfile and preselect options:
        const bool   isKeyfileOption =   keyfile.isOption(argument);
        const bool isPreselectOption = preselect.isOption(argument);
        assert(!(isKeyfileOption && isPreselectOption));  // isOption() is working correctly.
        if(isKeyfileOption || isPreselectOption) {
            if(m_keyfile.getState()!=FullPathName::UNINITIALIZED) CommandLineTokens::displayIgnoredMessage(argument, TRL("'Keyfile' or 'preselect' option is already specified"));
            else {
                const FullPathName path(isKeyfileOption ? keyfile.optionValue(argument) : preselect.optionValue(argument));
                switch(path.getState()) {
                    case FullPathName::INVALID_PATH:
                        CommandLineTokens::displayIgnoredMessage(argument, TRL("Path is invalid"));
                        break;

                    case FullPathName::PATH_ONLY:
                        if(isKeyfileOption) CommandLineTokens::displayIgnoredMessage(argument, TRL("Filename not found"));
                        else {  // isPreselectOption is true.
                            m_keyfile = path;
                            m_isPreselect = true;
                            }
                        break;

                    case FullPathName::PATH_AND_FILENAME:
                        m_keyfile = path;
                        m_isPreselect = isPreselectOption;
                        break;

                    default:  assert(false);  // Impossible state.
                    }
                }
            }

        // pw option:
        else if(pw.isOption(argument)) {
            if(!m_Password.empty()) CommandLineTokens::displayIgnoredMessage(argument, TRL("'Pw' option is already specified"));
            else {
                const std_string password(pw.optionValue(argument));
                // If password is empty, ignore command argument:
                if(password.empty()) CommandLineTokens::displayIgnoredMessage(argument, TRL("Password is empty"));
                else m_Password = password;
                }
            }

		else if(pwStdIn.isOption(argument)) {
            m_isPwStdIn = true;
            }

        // readonly option:
        else if(readonly.isOption(argument)) {
            m_isReadOnly = true;
            }

        // lock option:
        else if(lockcw.isOption(argument)) {
            m_isLock = true;
            }

        // minimize option:
        else if(mincw.isOption(argument)) {
            m_isMin = true;
            }

        // set-urloverride option:
        else if(urloverride.isOption(argument)) {
            m_urlOverride = urloverride.optionValue(argument);
            }

        // // cfg-local option:
        // else if(cfgLocal.isOption(argument)) {
        //    m_cfgLocal = cfgLocal.optionValue(argument);
        //    }

        // Plugin options:
        else if(CPluginManager::Instance().UsesCmdArg(argument));  // Do nothing.  This option is used by a plugin.

        // Other (that is, unknown) options:
        else if(CommandLineOption::beginsWithPrefix(argument)) CommandLineTokens::displayIgnoredMessage(argument, TRL("Option not recognized"));
        
        // Database argument:
        else {  // argument does not begin with prefix, so it must be the database name.
            if(m_database.getState()!=FullPathName::UNINITIALIZED) CommandLineTokens::displayIgnoredMessage(argument, TRL("Database is already specified"));
            else {
                const FullPathName path(argument);
                switch(path.getState()) {
                    case FullPathName::INVALID_PATH:
                        CommandLineTokens::displayIgnoredMessage(argument, TRL("Path is invalid"));
                        break;

                    case FullPathName::PATH_ONLY:
                        CommandLineTokens::displayIgnoredMessage(argument, TRL("Filename not found"));
                        break;

                    case FullPathName::PATH_AND_FILENAME:
                        m_database = path;
                        break;

                    default:  assert(false);  // Impossible state.
                    }
                }
            }
        }
}
