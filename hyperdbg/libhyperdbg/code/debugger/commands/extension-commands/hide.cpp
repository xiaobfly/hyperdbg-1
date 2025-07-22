/**
 * @file hide.cpp
 * @author Sina Karvandi (sina@hyperdbg.org)
 * @brief !hide command
 * @details
 * @version 0.1
 * @date 2020-07-07
 *
 * @copyright This project is released under the GNU Public License v3.
 *
 */
#include "pch.h"

//
// Global Variables
//
extern UINT64 g_CpuidAverage;
extern UINT64 g_CpuidStandardDeviation;
extern UINT64 g_CpuidMedian;

extern UINT64 g_RdtscAverage;
extern UINT64 g_RdtscStandardDeviation;
extern UINT64 g_RdtscMedian;

extern BOOLEAN                  g_TransparentResultsMeasured;
extern ACTIVE_DEBUGGING_PROCESS g_ActiveProcessDebuggingState;

/**
 * @brief help of the !hide command
 *
 * @return VOID
 */
VOID
CommandHideHelp()
{
    ShowMessages("!hide : tries to make HyperDbg transparent from anti-debugging "
                 "and anti-hypervisor methods (this is a work under progress and new methods will be added frequently).\n\n");

    ShowMessages("syntax : \t!hide\n");

    ShowMessages("\n");
    ShowMessages("\t\te.g : !hide\n");
}

/**
 * @brief Enable transparent mode
 *
 * @return BOOLEAN
 */
BOOLEAN
HyperDbgEnableTransparentMode()
{
    BOOLEAN                                     Status;
    ULONG                                       ReturnedLength;
    DEBUGGER_HIDE_AND_TRANSPARENT_DEBUGGER_MODE HideRequest = {0};

    //
    // Check if debugger is loaded or not
    //
    AssertShowMessageReturnStmt(g_DeviceHandle, ASSERT_MESSAGE_DRIVER_NOT_LOADED, AssertReturnFalse);

    //
    // We wanna hide the debugger and make transparent vm-exits
    //
    HideRequest.IsHide = TRUE;

    //
    // Send the request to the kernel
    //
    Status = DeviceIoControl(
        g_DeviceHandle,                                             // Handle to device
        IOCTL_DEBUGGER_HIDE_AND_UNHIDE_TO_TRANSPARENT_THE_DEBUGGER, // IO Control
                                                                    // code
        &HideRequest,                                               // Input Buffer to driver.
        SIZEOF_DEBUGGER_HIDE_AND_TRANSPARENT_DEBUGGER_MODE,         // Input buffer length
        &HideRequest,                                               // Output Buffer from driver.
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

    if (HideRequest.KernelStatus == DEBUGGER_OPERATION_WAS_SUCCESSFUL)
    {
        ShowMessages("transparent debugging successfully enabled :)\n");

        return TRUE;
    }
    else
    {
        ShowErrorMessage(HideRequest.KernelStatus);
        return FALSE;
    }
}

/**
 * @brief !hide command handler
 *
 * @param CommandTokens
 * @param Command
 * @return VOID
 */
VOID
CommandHide(vector<CommandToken> CommandTokens, string Command)
{
    if (CommandTokens.size() != 1)
    {
        ShowMessages("incorrect use of the '%s'\n\n",
                     GetCaseSensitiveStringFromCommandToken(CommandTokens.at(0)).c_str());
        CommandHideHelp();
        return;
    }

    //
    // Enable transparent mode
    //
    HyperDbgEnableTransparentMode();
}

//
// VOID
// CommandHide(vector<CommandToken> CommandTokens, string Command)
// {
//     BOOLEAN                                      Status;
//     ULONG                                        ReturnedLength;
//     UINT64                                       TargetPid;
//     BOOLEAN                                      TrueIfProcessIdAndFalseIfProcessName;
//     DEBUGGER_HIDE_AND_TRANSPARENT_DEBUGGER_MODE  HideRequest        = {0};
//     PDEBUGGER_HIDE_AND_TRANSPARENT_DEBUGGER_MODE FinalRequestBuffer = 0;
//     size_t                                       RequestBufferSize  = 0;
//
//     if (CommandTokens.size() <= 2 && CommandTokens.size() != 1)
//     {
//         ShowMessages("incorrect use of the '%s'\n\n",
//                      GetCaseSensitiveStringFromCommandToken(CommandTokens.at(0)).c_str());
//         CommandHideHelp();
//         return;
//     }
//
//     //
//     // Find out whether the user enters pid or name
//     //
//     if (CommandTokens.size() == 1)
//     {
//         if (g_ActiveProcessDebuggingState.IsActive)
//         {
//             TrueIfProcessIdAndFalseIfProcessName = TRUE;
//             TargetPid                            = g_ActiveProcessDebuggingState.ProcessId;
//         }
//         else
//         {
//             //
//             // There is no user-debugging process
//             //
//             ShowMessages("you're not attached to any user-mode process, "
//                          "please explicitly specify the process id or process name\n\n");
//             CommandHideHelp();
//             return;
//         }
//     }
//     else if (CompareLowerCaseStrings(CommandTokens.at(1), "pid"))
//     {
//         TrueIfProcessIdAndFalseIfProcessName = TRUE;
//
//         //
//         // Check for the user to not add extra arguments
//         //
//         if (CommandTokens.size() != 3)
//         {
//             ShowMessages("incorrect use of the '%s'\n\n",
//                          GetCaseSensitiveStringFromCommandToken(CommandTokens.at(0)).c_str());
//             CommandHideHelp();
//             return;
//         }
//
//         //
//         // It's just a pid for the process
//         //
//         if (!ConvertTokenToUInt64(CommandTokens.at(2), &TargetPid))
//         {
//             ShowMessages("incorrect process id\n\n");
//             return;
//         }
//     }
//     else if (CompareLowerCaseStrings(CommandTokens.at(1), "name"))
//     {
//         TrueIfProcessIdAndFalseIfProcessName = FALSE;
//     }
//     else
//     {
//         //
//         // Invalid argument for the second parameter to the command
//         //
//         ShowMessages("incorrect use of the '%s'\n\n",
//                      GetCaseSensitiveStringFromCommandToken(CommandTokens.at(0)).c_str());
//         CommandHideHelp();
//         return;
//     }
//
//     //
//     // Check if the user used !measure or not
//     //
//     if (!g_TransparentResultsMeasured || !g_CpuidAverage ||
//         !g_CpuidStandardDeviation || !g_CpuidMedian)
//     {
//         ShowMessages("the average, median and standard deviation is not measured. "
//                      "Did you use '!measure' command?\n");
//         return;
//     }
//
//     //
//     // Check if debugger is loaded or not
//     //
//     AssertShowMessageReturnStmt(g_DeviceHandle, ASSERT_MESSAGE_DRIVER_NOT_LOADED, AssertReturn);
//
//     //
//     // We wanna hide the debugger and make transparent vm-exits
//     //
//     HideRequest.IsHide = TRUE;
//
//     //
//     // Set the measured times cpuid
//     //
//     HideRequest.CpuidAverage           = g_CpuidAverage;
//     HideRequest.CpuidMedian            = g_CpuidMedian;
//     HideRequest.CpuidStandardDeviation = g_CpuidStandardDeviation;
//
//     //
//     // Set the measured times rdtsc/p
//     //
//     HideRequest.RdtscAverage           = g_RdtscAverage;
//     HideRequest.RdtscMedian            = g_RdtscMedian;
//     HideRequest.RdtscStandardDeviation = g_RdtscStandardDeviation;
//
//     HideRequest.TrueIfProcessIdAndFalseIfProcessName =
//         TrueIfProcessIdAndFalseIfProcessName;
//
//     if (TrueIfProcessIdAndFalseIfProcessName)
//     {
//         //
//         // It's a process id
//         //
//         HideRequest.ProcId = (UINT32)TargetPid;
//
//         RequestBufferSize = sizeof(DEBUGGER_HIDE_AND_TRANSPARENT_DEBUGGER_MODE);
//     }
//     else
//     {
//         //
//         // It's a process name
//         //
//         HideRequest.LengthOfProcessName = (UINT32)GetCaseSensitiveStringFromCommandToken(CommandTokens.at(2)).size() + 1;
//         RequestBufferSize               = sizeof(DEBUGGER_HIDE_AND_TRANSPARENT_DEBUGGER_MODE) + GetCaseSensitiveStringFromCommandToken(CommandTokens.at(2)).size() + 1;
//     }
//
//     //
//     // Allocate the requested buffer
//     //
//     FinalRequestBuffer =
//         (PDEBUGGER_HIDE_AND_TRANSPARENT_DEBUGGER_MODE)malloc(RequestBufferSize);
//
//     if (FinalRequestBuffer == NULL)
//     {
//         ShowMessages("insufficient space\n");
//         return;
//     }
//
//     //
//     // Zero the memory
//     //
//     RtlZeroMemory(FinalRequestBuffer, RequestBufferSize);
//
//     //
//     // Copy the buffer on the top of the final buffer
//     // to send the kernel
//     //
//     memcpy(FinalRequestBuffer, &HideRequest, sizeof(DEBUGGER_HIDE_AND_TRANSPARENT_DEBUGGER_MODE));
//
//     //
//     // If it's a name then we should add it to the end of the buffer
//     //
//     if (!TrueIfProcessIdAndFalseIfProcessName)
//     {
//         std::string ProcNameStr = GetCaseSensitiveStringFromCommandToken(CommandTokens.at(2));
//
//         CHAR * ProcName = (CHAR *)ProcNameStr.c_str();
//
//         memcpy(((UINT64 *)((UINT64)FinalRequestBuffer +
//                            sizeof(DEBUGGER_HIDE_AND_TRANSPARENT_DEBUGGER_MODE))),
//                ProcName,
//                GetCaseSensitiveStringFromCommandToken(CommandTokens.at(2)).size());
//     }
//
//     //
//     // Send the request to the kernel
//     //
//
//     Status = DeviceIoControl(
//         g_DeviceHandle,                                             // Handle to device
//         IOCTL_DEBUGGER_HIDE_AND_UNHIDE_TO_TRANSPARENT_THE_DEBUGGER, // IO Control
//                                                                     // code
//         FinalRequestBuffer,                                         // Input Buffer to driver.
//         (DWORD)RequestBufferSize,                                   // Input buffer length
//         FinalRequestBuffer,                                         // Output Buffer from driver.
//         SIZEOF_DEBUGGER_HIDE_AND_TRANSPARENT_DEBUGGER_MODE,         // Length of output
//                                                                     // buffer in bytes.
//         &ReturnedLength,                                            // Bytes placed in buffer.
//         NULL                                                        // synchronous call
//     );
//
//     if (!Status)
//     {
//         ShowMessages("ioctl failed with code 0x%x\n", GetLastError());
//         free(FinalRequestBuffer);
//         return;
//     }
//
//     if (FinalRequestBuffer->KernelStatus == DEBUGGER_OPERATION_WAS_SUCCESSFUL)
//     {
//         ShowMessages("transparent debugging successfully enabled :)\n");
//     }
//     else if (FinalRequestBuffer->KernelStatus ==
//              DEBUGGER_ERROR_UNABLE_TO_HIDE_OR_UNHIDE_DEBUGGER)
//     {
//         ShowMessages("unable to hide the debugger (transparent-debugging) :(\n");
//         free(FinalRequestBuffer);
//         return;
//     }
//     else
//     {
//         ShowMessages("unknown error occurred :(\n");
//         free(FinalRequestBuffer);
//         return;
//     }
//
//     //
//     // free the buffer
//     //
//     free(FinalRequestBuffer);
// }
//
