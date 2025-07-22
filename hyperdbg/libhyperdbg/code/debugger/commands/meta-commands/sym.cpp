/**
 * @file sym.cpp
 * @author Sina Karvandi (sina@hyperdbg.org)
 * @brief .sym command
 * @details
 * @version 0.1
 * @date 2021-05-27
 *
 * @copyright This project is released under the GNU Public License v3.
 *
 */
#include "pch.h"

//
// Global Variables
//
extern BOOLEAN                  g_IsSerialConnectedToRemoteDebuggee;
extern ACTIVE_DEBUGGING_PROCESS g_ActiveProcessDebuggingState;

/**
 * @brief help of the .sym command
 *
 * @return VOID
 */
VOID
CommandSymHelp()
{
    ShowMessages(".sym : performs the symbol actions.\n\n");

    ShowMessages("syntax : \t.sym [table]\n");
    ShowMessages("syntax : \t.sym [reload] [pid ProcessId (hex)]\n");
    ShowMessages("syntax : \t.sym [download]\n");
    ShowMessages("syntax : \t.sym [load]\n");
    ShowMessages("syntax : \t.sym [unload]\n");
    ShowMessages("syntax : \t.sym [add] [base Address (hex)] [path Path (string)]\n");

    ShowMessages("\n");
    ShowMessages("\t\te.g : .sym table\n");
    ShowMessages("\t\te.g : .sym reload\n");
    ShowMessages("\t\te.g : .sym reload pid 3a24\n");
    ShowMessages("\t\te.g : .sym load\n");
    ShowMessages("\t\te.g : .sym download\n");
    ShowMessages("\t\te.g : .sym add base fffff8077356000 path c:\\symbols\\my_dll.pdb\n");
    ShowMessages("\t\te.g : .sym add base fffff8077356000 path \"c:\\symbols files\\my_dll.pdb\"\n");
    ShowMessages("\t\te.g : .sym unload\n");
}

/**
 * @brief .sym command handler
 *
 * @param CommandTokens
 * @param Command
 * @return VOID
 */
VOID
CommandSym(vector<CommandToken> CommandTokens, string Command)
{
    UINT64 BaseAddress   = NULL;
    UINT32 UserProcessId = NULL;
    string PathToPdb;

    if (CommandTokens.size() == 1)
    {
        ShowMessages("incorrect use of the '%s'\n\n",
                     GetCaseSensitiveStringFromCommandToken(CommandTokens.at(0)).c_str());
        CommandSymHelp();
        return;
    }

    if (CompareLowerCaseStrings(CommandTokens.at(1), "table"))
    {
        //
        // Validate params
        //
        if (CommandTokens.size() != 2)
        {
            ShowMessages("incorrect use of the '%s'\n\n",
                         GetCaseSensitiveStringFromCommandToken(CommandTokens.at(0)).c_str());
            CommandSymHelp();
            return;
        }

        //
        // Show symbol table
        //
        SymbolBuildAndShowSymbolTable();
    }
    else if (CompareLowerCaseStrings(CommandTokens.at(1), "load") || CompareLowerCaseStrings(CommandTokens.at(1), "download"))
    {
        //
        // Validate params
        //
        if (CommandTokens.size() != 2)
        {
            ShowMessages("incorrect use of the '%s'\n\n",
                         GetCaseSensitiveStringFromCommandToken(CommandTokens.at(0)).c_str());
            CommandSymHelp();
            return;
        }

        //
        // Load and download available symbols
        //
        if (CompareLowerCaseStrings(CommandTokens.at(1), "load"))
        {
            SymbolLoadOrDownloadSymbols(FALSE, FALSE);
        }
        else if (CompareLowerCaseStrings(CommandTokens.at(1), "download"))
        {
            SymbolLoadOrDownloadSymbols(TRUE, FALSE);
        }
    }
    else if (CompareLowerCaseStrings(CommandTokens.at(1), "reload"))
    {
        //
        // Validate params
        //
        if (CommandTokens.size() != 2 && CommandTokens.size() != 4)
        {
            ShowMessages("incorrect use of the '%s'\n\n",
                         GetCaseSensitiveStringFromCommandToken(CommandTokens.at(0)).c_str());
            CommandSymHelp();
            return;
        }

        //
        // Check for process id
        //
        if (CommandTokens.size() == 4)
        {
            if (CompareLowerCaseStrings(CommandTokens.at(2), "pid"))
            {
                if (!ConvertTokenToUInt32(CommandTokens.at(3), &UserProcessId))
                {
                    //
                    // couldn't resolve or unknown parameter
                    //
                    ShowMessages("err, couldn't resolve error at '%s'\n\n",
                                 GetCaseSensitiveStringFromCommandToken(CommandTokens.at(3)).c_str());
                    CommandSymHelp();
                    return;
                }
            }
            else
            {
                ShowMessages("incorrect use of the '%s'\n\n",
                             GetCaseSensitiveStringFromCommandToken(CommandTokens.at(0)).c_str());
                CommandSymHelp();
                return;
            }
        }

        //
        // Refresh and reload symbols
        //
        if (g_IsSerialConnectedToRemoteDebuggee)
        {
            //
            // Update symbol table from remote debuggee in debugger-mode
            //
            SymbolReloadSymbolTableInDebuggerMode(UserProcessId);
        }
        else
        {
            //
            // Check if user explicitly specified the process id
            //
            if (UserProcessId == NULL)
            {
                //
                // User didn't explicitly specified the process id, so
                // if it's a user-debugger process, we use the modules
                // of the target user-debuggee's process, otherwise,
                // the current process (HyperDbg's process) is specified
                //
                if (g_ActiveProcessDebuggingState.IsActive)
                {
                    UserProcessId = g_ActiveProcessDebuggingState.ProcessId;
                }
                else
                {
                    UserProcessId = GetCurrentProcessId();
                }
            }

            //
            // Build locally and reload it
            //
            if (SymbolLocalReload(UserProcessId))
            {
                ShowMessages("symbol table updated successfully\n");
            }
        }
    }
    else if (CompareLowerCaseStrings(CommandTokens.at(1), "unload"))
    {
        //
        // Validate params
        //
        if (CommandTokens.size() != 2)
        {
            ShowMessages("incorrect use of the '%s'\n\n",
                         GetCaseSensitiveStringFromCommandToken(CommandTokens.at(0)).c_str());
            CommandSymHelp();
            return;
        }

        //
        // unload without any parameters, means that unload
        // all the symbols
        //
        ScriptEngineUnloadAllSymbolsWrapper();

        //
        // Size is 3 there is module name (not working ! I don't know why)
        //
        // ScriptEngineUnloadModuleSymbolWrapper((char *)SplitCommand.at(2).c_str());
    }
    else if (CompareLowerCaseStrings(CommandTokens.at(1), "add"))
    {
        //
        // Validate params
        //
        if (CommandTokens.size() < 6)
        {
            ShowMessages("incorrect use of the '%s'\n\n",
                         GetCaseSensitiveStringFromCommandToken(CommandTokens.at(0)).c_str());
            CommandSymHelp();
            return;
        }

        if (CompareLowerCaseStrings(CommandTokens.at(2), "base"))
        {
            if (!ConvertTokenToUInt64(CommandTokens.at(3), &BaseAddress))
            {
                ShowMessages("please add a valid hex address to be used as the base address\n\n");
                CommandSymHelp();
                return;
            }

            //
            // Base address is now valid, check if next parameter is path
            //
            if (!CompareLowerCaseStrings(CommandTokens.at(4), "path"))
            {
                ShowMessages("incorrect use of the '%s'\n\n",
                             GetCaseSensitiveStringFromCommandToken(CommandTokens.at(0)).c_str());
                CommandSymHelp();
                return;
            }

            //
            // The rest of command is pdb path
            //
            PathToPdb = GetCaseSensitiveStringFromCommandToken(CommandTokens.at(5));

            //
            // Check if pdb file exists or not
            //
            if (!IsFileExistA(PathToPdb.c_str()))
            {
                ShowMessages("pdb file not found\n");
                return;
            }

            ShowMessages("loading module symbol at '%s'\n", PathToPdb.c_str());

            //
            // Load the pdb file (the validation of pdb file is checked into pdb
            // parsing functions)
            //
            ScriptEngineLoadFileSymbolWrapper(BaseAddress, PathToPdb.c_str(), NULL);
        }
        else
        {
            ShowMessages("incorrect use of the '%s'\n\n",
                         GetCaseSensitiveStringFromCommandToken(CommandTokens.at(0)).c_str());
            CommandSymHelp();
            return;
        }
    }
    else
    {
        ShowMessages("unknown parameter at '%s'\n\n",
                     GetCaseSensitiveStringFromCommandToken(CommandTokens.at(1)).c_str());
        CommandSymHelp();
        return;
    }
}
