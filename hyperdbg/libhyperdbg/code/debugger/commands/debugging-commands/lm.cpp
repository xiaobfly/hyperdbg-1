/**
 * @file lm.cpp
 * @author Sina Karvandi (sina@hyperdbg.org)
 * @brief lm command
 * @details
 * @version 0.1
 * @date 2020-07-13
 *
 * @copyright This project is released under the GNU Public License v3.
 *
 */
#include "pch.h"

using namespace std;

//
// Global Variables
//
extern ACTIVE_DEBUGGING_PROCESS g_ActiveProcessDebuggingState;

/**
 * @brief help of the lm command
 *
 * @return VOID
 */
VOID
CommandLmHelp()
{
    ShowMessages("lm : lists user/kernel modules' base address, size, name and path.\n\n");

    ShowMessages("syntax : \tlm [m Name (string)] [pid ProcessId (hex)] [Filter (string)]\n");

    ShowMessages("\n");
    ShowMessages("\t\te.g : lm\n");
    ShowMessages("\t\te.g : lm km\n");
    ShowMessages("\t\te.g : lm um\n");
    ShowMessages("\t\te.g : lm m nt\n");
    ShowMessages("\t\te.g : lm km m ntos\n");
    ShowMessages("\t\te.g : lm um m kernel32\n");
    ShowMessages("\t\te.g : lm um m kernel32 pid 1240\n");
}

/**
 * @brief Convert redirection of 32-bit compatibility path
 *
 * @param LocalFilePath
 *
 * @return wstring
 */
std::wstring
CommandLmConvertWow64CompatibilityPaths(const wchar_t * LocalFilePath)
{
    std::wstring filePath(LocalFilePath);

    // Convert the path to lowercase
    std::transform(filePath.begin(), filePath.end(), filePath.begin(), ::tolower);

    // Replace "\windows\system32" with "\windows\syswow64"
    size_t pos = filePath.find(L":\\windows\\system32");
    if (pos != std::string::npos)
    {
        filePath.replace(pos, 18, L":\\Windows\\SysWOW64");
    }

    // Replace "\program files" with "\program files (x86)"
    pos = filePath.find(L":\\program files");
    if (pos != std::string::npos)
    {
        filePath.replace(pos, 15, L":\\Program Files (x86)");
    }

    return filePath;
}

/**
 * @brief show modules for specified user mode process
 * @param ProcessId
 * @param SearchModule
 *
 * @return BOOLEAN
 */
BOOLEAN
CommandLmShowUserModeModule(UINT32 ProcessId, const char * SearchModule)
{
    BOOLEAN                         Status;
    ULONG                           ReturnedLength;
    UINT32                          ModuleDetailsSize    = 0;
    UINT32                          ModulesCount         = 0;
    PUSERMODE_LOADED_MODULE_DETAILS ModuleDetailsRequest = NULL;
    PUSERMODE_LOADED_MODULE_SYMBOLS Modules              = NULL;
    USERMODE_LOADED_MODULE_DETAILS  ModuleCountRequest   = {0};
    size_t                          CharSize             = 0;
    wchar_t *                       WcharBuff            = NULL;
    wstring                         SearchModuleString;

    //
    // Check if debugger is loaded or not
    //
    AssertShowMessageReturnStmt(g_DeviceHandle, ASSERT_MESSAGE_DRIVER_NOT_LOADED, AssertReturnFalse);

    //
    // Set the module details to get the details
    //
    ModuleCountRequest.ProcessId        = ProcessId;
    ModuleCountRequest.OnlyCountModules = TRUE;

    //
    // Send the request to the kernel
    //
    Status = DeviceIoControl(
        g_DeviceHandle,                         // Handle to device
        IOCTL_GET_USER_MODE_MODULE_DETAILS,     // IO Control
                                                // code
        &ModuleCountRequest,                    // Input Buffer to driver.
        sizeof(USERMODE_LOADED_MODULE_DETAILS), // Input buffer length
        &ModuleCountRequest,                    // Output Buffer from driver.
        sizeof(USERMODE_LOADED_MODULE_DETAILS), // Length of output
                                                // buffer in bytes.
        &ReturnedLength,                        // Bytes placed in buffer.
        NULL                                    // synchronous call
    );

    if (!Status)
    {
        ShowMessages("ioctl failed with code 0x%x\n", GetLastError());
        return FALSE;
    }

    //
    // Check if counting modules was successful or not
    //
    if (ModuleCountRequest.Result == DEBUGGER_OPERATION_WAS_SUCCESSFUL)
    {
        ModulesCount = ModuleCountRequest.ModulesCount;

        // ShowMessages("Count of modules : 0x%x\n", ModuleCountRequest.ModulesCount);

        ModuleDetailsSize = sizeof(USERMODE_LOADED_MODULE_DETAILS) +
                            (ModuleCountRequest.ModulesCount * sizeof(USERMODE_LOADED_MODULE_SYMBOLS));

        ModuleDetailsRequest = (PUSERMODE_LOADED_MODULE_DETAILS)malloc(ModuleDetailsSize);

        if (ModuleDetailsRequest == NULL)
        {
            return FALSE;
        }

        RtlZeroMemory(ModuleDetailsRequest, ModuleDetailsSize);

        //
        // Set the module details to get the modules (not count)
        //
        ModuleDetailsRequest->ProcessId        = ProcessId;
        ModuleDetailsRequest->OnlyCountModules = FALSE;

        //
        // Send the request to the kernel
        //
        Status = DeviceIoControl(
            g_DeviceHandle,                         // Handle to device
            IOCTL_GET_USER_MODE_MODULE_DETAILS,     // IO Control
                                                    // code
            ModuleDetailsRequest,                   // Input Buffer to driver.
            sizeof(USERMODE_LOADED_MODULE_DETAILS), // Input buffer length
            ModuleDetailsRequest,                   // Output Buffer from driver.
            ModuleDetailsSize,                      // Length of output
                                                    // buffer in bytes.
            &ReturnedLength,                        // Bytes placed in buffer.
            NULL                                    // synchronous call
        );

        if (!Status)
        {
            free(ModuleDetailsRequest);
            ShowMessages("ioctl failed with code 0x%x\n", GetLastError());
            return FALSE;
        }

        //
        // Show modules list
        //
        if (ModuleCountRequest.Result == DEBUGGER_OPERATION_WAS_SUCCESSFUL)
        {
            Modules = (PUSERMODE_LOADED_MODULE_SYMBOLS)((UINT64)ModuleDetailsRequest +
                                                        sizeof(USERMODE_LOADED_MODULE_DETAILS));
            ShowMessages("user mode\n");
            ShowMessages("start\t\t\tentrypoint\t\tpath\n\n");

            if (SearchModule != NULL)
            {
                CharSize  = strlen(SearchModule) + 1;
                WcharBuff = (wchar_t *)malloc(CharSize * 2);

                if (WcharBuff == NULL)
                {
                    return FALSE;
                }

                RtlZeroMemory(WcharBuff, CharSize);

                mbstowcs(WcharBuff, SearchModule, CharSize);

                SearchModuleString.assign(WcharBuff, wcslen(WcharBuff));
            }

            for (size_t i = 0; i < ModulesCount; i++)
            {
                //
                // Check if we need to search for the module or not
                //
                if (SearchModule != NULL)
                {
                    //
                    // Convert FullPathName to string
                    //
                    std::wstring FullPathName((wchar_t *)Modules[i].FilePath);

                    if (FindCaseInsensitiveW(FullPathName, SearchModuleString, 0) == std::wstring::npos)
                    {
                        //
                        // not found
                        //
                        continue;
                    }
                }

                //
                // Check if module is 32-bit or not
                //
                if (ModuleDetailsRequest->Is32Bit)
                {
                    ShowMessages("%016llx\t%016llx\t%ws\n",
                                 Modules[i].BaseAddress,
                                 Modules[i].Entrypoint,
                                 CommandLmConvertWow64CompatibilityPaths(Modules[i].FilePath).c_str());
                }
                else
                {
                    ShowMessages("%016llx\t%016llx\t%ws\n",
                                 Modules[i].BaseAddress,
                                 Modules[i].Entrypoint,
                                 Modules[i].FilePath);
                }
            }

            if (SearchModule != NULL)
            {
                free(WcharBuff);
            }
        }
        else
        {
            ShowErrorMessage(ModuleCountRequest.Result);
            return FALSE;
        }

        free(ModuleDetailsRequest);
        return TRUE;
    }
    else
    {
        ShowErrorMessage(ModuleCountRequest.Result);
        return FALSE;
    }
}

/**
 * @brief show modules for kernel mode
 * @param SearchModule
 *
 * @return BOOLEAN
 */
BOOLEAN
CommandLmShowKernelModeModule(const char * SearchModule)
{
    PRTL_PROCESS_MODULES ModulesInfo = NULL;
    string               SearchModuleString;

    if (SymbolCheckAndAllocateModuleInformation(&ModulesInfo) == FALSE)
    {
        ShowMessages("err, unable get modules information\n");
        return FALSE;
    }

    if (SearchModule != NULL)
    {
        SearchModuleString.assign(SearchModule, strlen(SearchModule));
    }

    ShowMessages("kernel mode\n");
    ShowMessages("start\t\t\tsize\tname\t\t\t\tpath\n\n");

    for (ULONG i = 0; i < ModulesInfo->NumberOfModules; i++)
    {
        RTL_PROCESS_MODULE_INFORMATION * CurrentModule = &ModulesInfo->Modules[i];

        //
        // Check if we need to search for the module or not
        //
        if (SearchModule != NULL)
        {
            //
            // Convert FullPathName to string
            //
            std::string FullPathName((char *)CurrentModule->FullPathName);

            if (FindCaseInsensitive(FullPathName, SearchModuleString, 0) == std::string::npos)
            {
                //
                // not found
                //
                continue;
            }
        }

        ShowMessages("%s\t", SeparateTo64BitValue((UINT64)CurrentModule->ImageBase).c_str());
        ShowMessages("%x\t", CurrentModule->ImageSize);

        auto   PathName    = CurrentModule->FullPathName + CurrentModule->OffsetToFileName;
        UINT32 PathNameLen = (UINT32)strlen((const char *)PathName);

        ShowMessages("%s\t", PathName);

        if (PathNameLen >= 24)
        {
        }
        else if (PathNameLen >= 16)
        {
            ShowMessages("\t");
        }
        else if (PathNameLen >= 8)
        {
            ShowMessages("\t\t");
        }
        else
        {
            ShowMessages("\t\t\t");
        }

        ShowMessages("%s\n", CurrentModule->FullPathName);
    }

    free(ModulesInfo);

    return TRUE;
}

/**
 * @brief handle lm command
 *
 * @param CommandTokens
 * @param Command
 *
 * @return VOID
 */
VOID
CommandLm(vector<CommandToken> CommandTokens, string Command)
{
    BOOLEAN SetPid                = FALSE;
    BOOLEAN SetSearchFilter       = FALSE;
    BOOLEAN SearchStringEntered   = FALSE;
    BOOLEAN OnlyShowKernelModules = FALSE;
    BOOLEAN OnlyShowUserModules   = FALSE;
    UINT32  TargetPid             = NULL;
    char    Search[MAX_PATH]      = {0};
    char *  SearchString          = NULL;

    //
    // Interpret command specific details (if any)
    //
    for (auto Section : CommandTokens)
    {
        if (CompareLowerCaseStrings(Section, "lm"))
        {
            continue;
        }
        else if (CompareLowerCaseStrings(Section, "pid") && !SetPid)
        {
            SetPid = TRUE;
        }
        else if (CompareLowerCaseStrings(Section, "m") && !SetSearchFilter)
        {
            SetSearchFilter = TRUE;
        }
        else if (SetPid)
        {
            if (!ConvertTokenToUInt32(Section, &TargetPid))
            {
                //
                // couldn't resolve or unknown parameter
                //
                ShowMessages("err, couldn't resolve error at '%s'\n\n",
                             GetCaseSensitiveStringFromCommandToken(Section).c_str());
                CommandLmHelp();
                return;
            }
            SetPid = FALSE;
        }
        else if (SetSearchFilter)
        {
            if (GetCaseSensitiveStringFromCommandToken(Section).length() >= MAX_PATH)
            {
                ShowMessages("err, string is too large for search, please enter "
                             "smaller string\n");

                return;
            }

            SearchStringEntered = TRUE;
            strcpy(Search, GetLowerStringFromCommandToken(Section).c_str());

            SetSearchFilter = FALSE;
        }
        else if (CompareLowerCaseStrings(Section, "km"))
        {
            if (OnlyShowUserModules)
            {
                ShowMessages("err, you cannot use both 'um', and 'km', by default "
                             "HyperDbg shows both user-mode and kernel-mode modules\n");
                return;
            }

            OnlyShowKernelModules = TRUE;
        }
        else if (CompareLowerCaseStrings(Section, "um"))
        {
            if (OnlyShowKernelModules)
            {
                ShowMessages("err, you cannot use both 'um', and 'km', by default "
                             "HyperDbg shows both user-mode and kernel-mode modules\n");
                return;
            }

            OnlyShowUserModules = TRUE;
        }
        else
        {
            //
            // Unknown parameter
            //
            ShowMessages("err, couldn't resolve error at '%s'\n\n",
                         GetCaseSensitiveStringFromCommandToken(Section).c_str());
            CommandLmHelp();
            return;
        }
    }

    if (SetPid)
    {
        ShowMessages("err, please enter a valid process id in hex format, "
                     "or if you want to use it in decimal format, add '0n' "
                     "prefix to the number\n");
        return;
    }

    if (SetSearchFilter)
    {
        ShowMessages("err, please enter a valid string to search in modules\n");
        return;
    }

    //
    // Check if we have string to search
    //
    if (SearchStringEntered)
    {
        SearchString = Search;
    }

    //
    // Show user mode modules
    //
    if (!OnlyShowKernelModules)
    {
        if (TargetPid != NULL)
        {
            CommandLmShowUserModeModule(TargetPid, SearchString);
        }
        else if (g_ActiveProcessDebuggingState.IsActive)
        {
            CommandLmShowUserModeModule(g_ActiveProcessDebuggingState.ProcessId, SearchString);
        }
        else
        {
            CommandLmShowUserModeModule(GetCurrentProcessId(), SearchString);
        }
    }

    //
    // Show kernel mode modules
    //
    if (!OnlyShowUserModules)
    {
        if (!OnlyShowKernelModules)
        {
            ShowMessages("\n==============================================================================\n\n");
        }

        CommandLmShowKernelModeModule(SearchString);
    }
}
