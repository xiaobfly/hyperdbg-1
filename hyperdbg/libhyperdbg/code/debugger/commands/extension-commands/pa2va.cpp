/**
 * @file pa2va.cpp
 * @author Sina Karvandi (sina@hyperdbg.org)
 * @brief !pa2va command
 * @details
 * @version 0.1
 * @date 2020-07-16
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
 * @brief help of the !pa2va command
 *
 * @return VOID
 */
VOID
CommandPa2vaHelp()
{
    ShowMessages("!pa2va : converts virtual address to physical address.\n\n");

    ShowMessages("syntax : \t!pa2va [PhysicalAddress (hex)] [pid ProcessId (hex)]\n");

    ShowMessages("\n");
    ShowMessages("\t\te.g : !pa2va nt!ExAllocatePoolWithTag\n");
    ShowMessages("\t\te.g : !pa2va nt!ExAllocatePoolWithTag+5\n");
    ShowMessages("\t\te.g : !pa2va @rax+5\n");
    ShowMessages("\t\te.g : !pa2va fffff801deadbeef\n");
    ShowMessages("\t\te.g : !pa2va fffff801deadbeef pid 0xc8\n");
}

/**
 * @brief !pa2va command handler
 *
 * @param CommandTokens
 * @param Command
 *
 * @return VOID
 */
VOID
CommandPa2va(vector<CommandToken> CommandTokens, string Command)
{
    BOOL                              Status;
    ULONG                             ReturnedLength;
    UINT64                            TargetPa;
    UINT32                            Pid            = 0;
    DEBUGGER_VA2PA_AND_PA2VA_COMMANDS AddressDetails = {0};

    if (CommandTokens.size() == 1 || CommandTokens.size() >= 5 || CommandTokens.size() == 3)
    {
        ShowMessages("incorrect use of the '%s'\n\n",
                     GetCaseSensitiveStringFromCommandToken(CommandTokens.at(0)).c_str());

        CommandPa2vaHelp();
        return;
    }

    //
    // By default if the user-debugger is active, we use these commands
    // on the memory layout of the debuggee process
    //
    if (g_ActiveProcessDebuggingState.IsActive)
    {
        Pid = g_ActiveProcessDebuggingState.ProcessId;
    }

    if (CommandTokens.size() == 2)
    {
        //
        // It's just a address for current process
        //
        if (!SymbolConvertNameOrExprToAddress(GetCaseSensitiveStringFromCommandToken(CommandTokens.at(1)), &TargetPa))
        {
            //
            // Couldn't resolve or unknown parameter
            //
            ShowMessages("err, couldn't resolve error at '%s'\n",
                         GetCaseSensitiveStringFromCommandToken(CommandTokens.at(1)).c_str());
            return;
        }
    }
    else
    {
        //
        // It might be address + pid
        //
        if (CompareLowerCaseStrings(CommandTokens.at(1), "pid"))
        {
            if (!ConvertTokenToUInt32(CommandTokens.at(2), &Pid))
            {
                ShowMessages("incorrect address, please enter a valid process id\n");
                return;
            }
            if (!SymbolConvertNameOrExprToAddress(GetCaseSensitiveStringFromCommandToken(CommandTokens.at(3)), &TargetPa))
            {
                //
                // Couldn't resolve or unknown parameter
                //
                ShowMessages("err, couldn't resolve error at '%s'\n",
                             GetCaseSensitiveStringFromCommandToken(CommandTokens.at(3)).c_str());
                return;
            }
        }
        else if (CompareLowerCaseStrings(CommandTokens.at(2), "pid"))
        {
            if (!SymbolConvertNameOrExprToAddress(GetCaseSensitiveStringFromCommandToken(CommandTokens.at(1)), &TargetPa))
            {
                //
                // Couldn't resolve or unknown parameter
                //
                ShowMessages("err, couldn't resolve error at '%s'\n",
                             GetCaseSensitiveStringFromCommandToken(CommandTokens.at(1)).c_str());

                return;
            }
            if (!ConvertTokenToUInt32(CommandTokens.at(3), &Pid))
            {
                ShowMessages("incorrect address, please enter a valid process id\n");
                return;
            }
        }
        else
        {
            ShowMessages("incorrect use of the '%s'\n\n",
                         GetCaseSensitiveStringFromCommandToken(CommandTokens.at(0)).c_str());
            CommandPa2vaHelp();
            return;
        }
    }

    //
    // Prepare the buffer
    // We use same buffer for input and output
    //
    AddressDetails.PhysicalAddress    = TargetPa;
    AddressDetails.ProcessId          = Pid; // it's null if in debugger mode
    AddressDetails.IsVirtual2Physical = FALSE;

    if (g_IsSerialConnectedToRemoteDebuggee)
    {
        //
        // Check to prevent using process id in !pa2va command
        //
        if (Pid != 0)
        {
            ShowMessages(ASSERT_MESSAGE_CANNOT_SPECIFY_PID);
            return;
        }

        //
        // Send the request over serial kernel debugger
        //

        KdSendVa2paAndPa2vaPacketToDebuggee(&AddressDetails);
    }
    else
    {
        AssertShowMessageReturnStmt(g_DeviceHandle, ASSERT_MESSAGE_DRIVER_NOT_LOADED, AssertReturn);

        if (Pid == 0)
        {
            Pid                      = GetCurrentProcessId();
            AddressDetails.ProcessId = Pid;
        }

        //
        // Send IOCTL
        //
        Status = DeviceIoControl(
            g_DeviceHandle,                           // Handle to device
            IOCTL_DEBUGGER_VA2PA_AND_PA2VA_COMMANDS,  // IO Control Code (IOCTL)
            &AddressDetails,                          // Input Buffer to driver.
            SIZEOF_DEBUGGER_VA2PA_AND_PA2VA_COMMANDS, // Input buffer length
            &AddressDetails,                          // Output Buffer from driver.
            SIZEOF_DEBUGGER_VA2PA_AND_PA2VA_COMMANDS, // Length of output
                                                      // buffer in bytes.
            &ReturnedLength,                          // Bytes placed in buffer.
            NULL                                      // synchronous call
        );

        if (!Status)
        {
            ShowMessages("ioctl failed with code 0x%x\n", GetLastError());
            return;
        }

        if (AddressDetails.KernelStatus == DEBUGGER_OPERATION_WAS_SUCCESSFUL)
        {
            //
            // Show the results
            //
            ShowMessages("%llx\n", AddressDetails.VirtualAddress);
        }
        else
        {
            //
            // An err occurred, no results
            //
            ShowErrorMessage(AddressDetails.KernelStatus);
        }
    }
}
