/**
 * @file unhide.cpp
 * @author Sina Karvandi (sina@hyperdbg.org)
 * @brief !unhide command
 * @details
 * @version 0.1
 * @date 2020-07-07
 *
 * @copyright This project is released under the GNU Public License v3.
 *
 */
#include "pch.h"

/**
 * @brief help of the !unhide command
 *
 * @return VOID
 */
VOID
CommandUnhideHelp()
{
    ShowMessages("!unhide : reverts the transparency measures of the '!hide' command and exits the transparent mode.\n\n");

    ShowMessages("syntax : \t!unhide\n");

    ShowMessages("\n");
    ShowMessages("\t\te.g : !unhide\n");
}

/**
 * @brief Disable transparent mode
 *
 * @return BOOLEAN
 */
BOOLEAN
HyperDbgDisableTransparentMode()
{
    BOOLEAN                                     Status;
    ULONG                                       ReturnedLength;
    DEBUGGER_HIDE_AND_TRANSPARENT_DEBUGGER_MODE UnhideRequest = {0};

    //
    // Check if debugger is loaded or not
    //
    AssertShowMessageReturnStmt(g_DeviceHandle, ASSERT_MESSAGE_DRIVER_NOT_LOADED, AssertReturnFalse);

    //
    // We don't wanna hide the debugger and make transparent vm-exits
    //
    UnhideRequest.IsHide = FALSE;

    //
    // Send the request to the kernel
    //
    Status = DeviceIoControl(
        g_DeviceHandle,                                             // Handle to device
        IOCTL_DEBUGGER_HIDE_AND_UNHIDE_TO_TRANSPARENT_THE_DEBUGGER, // IO Control
                                                                    // code
        &UnhideRequest,                                             // Input Buffer to driver.
        SIZEOF_DEBUGGER_HIDE_AND_TRANSPARENT_DEBUGGER_MODE,         // Input buffer length
        &UnhideRequest,                                             // Output Buffer from driver.
        SIZEOF_DEBUGGER_HIDE_AND_TRANSPARENT_DEBUGGER_MODE,         // Length of output
                                                                    // buffer in bytes.
        &ReturnedLength,                                            // Bytes placed in buffer.
        NULL                                                        // synchronous call
    );

    if (!Status)
    {
        ShowMessages("ioctl failed with code 0x%x\n", GetLastError());
        return FALSE;
    }

    if (UnhideRequest.KernelStatus == DEBUGGER_OPERATION_WAS_SUCCESSFUL)
    {
        ShowMessages("transparent debugging successfully disabled :)\n");
        return TRUE;
    }
    else
    {
        ShowErrorMessage(UnhideRequest.KernelStatus);
        return FALSE;
    }
}

/**
 * @brief !unhide command handler
 *
 * @param CommandTokens
 * @param Command
 *
 * @return VOID
 */
VOID
CommandUnhide(vector<CommandToken> CommandTokens, string Command)
{
    if (CommandTokens.size() >= 2)
    {
        ShowMessages("incorrect use of the '%s'\n\n",
                     GetCaseSensitiveStringFromCommandToken(CommandTokens.at(0)).c_str());

        CommandUnhideHelp();
        return;
    }

    //
    // Disable transparent mode
    //
    HyperDbgDisableTransparentMode();
}
