/**
 * @file crwrite.cpp
 * @author angel_killah
 * @brief !crwrite command
 * @details
 * @version 0.1
 * @date 2022-07-05
 *
 * @copyright This project is released under the GNU Public License v3.
 *
 */
#include "pch.h"

/**
 * @brief help of the !crwrite command
 *
 * @return VOID
 */
VOID
CommandCrwriteHelp()
{
    ShowMessages("!crwrite : monitors modification of control registers (CR0 / CR4).\n\n");

    ShowMessages("syntax : \t!crwrite [Cr (hex)] [mask Mask (hex)] [pid ProcessId (hex)] "
                 "[core CoreId (hex)] [imm IsImmediate (yesno)] [sc EnableShortCircuiting (onoff)] "
                 "[stage CallingStage (prepostall)] [buffer PreAllocatedBuffer (hex)] [script { Script (string) }] "
                 "[asm condition { Condition (assembly/hex) }] [asm code { Code (assembly/hex) }] [output {OutputName (string)}]\n");

    ShowMessages("\n");
    ShowMessages("\t\te.g : !crwrite 0\n");
    ShowMessages("\t\te.g : !crwrite 0 0x10000\n");
    ShowMessages("\t\te.g : !crwrite 4 pid 400\n");
    ShowMessages("\t\te.g : !crwrite 4 core 2 pid 400\n");
    ShowMessages("\t\te.g : !crwrite 4 script { printf(\"4th control register is modified at: %%llx\\n\", @rip); }\n");
    ShowMessages("\t\te.g : !crwrite 4 asm code { nop; nop; nop }\n");
}

/**
 * @brief !crwrite command handler
 *
 * @param CommandTokens
 * @param Command
 *
 * @return VOID
 */
VOID
CommandCrwrite(vector<CommandToken> CommandTokens, string Command)
{
    PDEBUGGER_GENERAL_EVENT_DETAIL     Event                 = NULL;
    PDEBUGGER_GENERAL_ACTION           ActionBreakToDebugger = NULL;
    PDEBUGGER_GENERAL_ACTION           ActionCustomCode      = NULL;
    PDEBUGGER_GENERAL_ACTION           ActionScript          = NULL;
    UINT32                             EventLength;
    UINT32                             ActionBreakToDebuggerLength = 0;
    UINT32                             ActionCustomCodeLength      = 0;
    UINT32                             ActionScriptLength          = 0;
    UINT64                             TargetRegister              = 1;
    UINT64                             MaskRegister                = 0xFFFFFFFFFFFFFFFF;
    BOOLEAN                            GetRegister                 = FALSE;
    BOOLEAN                            GetMask                     = FALSE;
    DEBUGGER_EVENT_PARSING_ERROR_CAUSE EventParsingErrorCause;

    if (CommandTokens.size() < 2)
    {
        ShowMessages("incorrect use of the '%s'\n\n",
                     GetCaseSensitiveStringFromCommandToken(CommandTokens.at(0)).c_str());
        CommandCrwriteHelp();
        return;
    }

    //
    // Interpret and fill the general event and action fields
    //
    //
    if (!InterpretGeneralEventAndActionsFields(
            &CommandTokens,
            CONTROL_REGISTER_MODIFIED,
            &Event,
            &EventLength,
            &ActionBreakToDebugger,
            &ActionBreakToDebuggerLength,
            &ActionCustomCode,
            &ActionCustomCodeLength,
            &ActionScript,
            &ActionScriptLength,
            &EventParsingErrorCause))
    {
        return;
    }

    //
    // Interpret command specific details (if any)
    //
    for (auto Section : CommandTokens)
    {
        if (CompareLowerCaseStrings(Section, "!crwrite"))
        {
            continue;
        }
        else if (!GetRegister)
        {
            //
            // It's probably a CR
            //
            if (!ConvertTokenToUInt64(Section, &TargetRegister))
            {
                //
                // Unknown parameter
                //
                ShowMessages("unknown parameter '%s'\n\n",
                             GetCaseSensitiveStringFromCommandToken(Section).c_str());
                CommandCrwriteHelp();

                FreeEventsAndActionsMemory(Event, ActionBreakToDebugger, ActionCustomCode, ActionScript);
                return;
            }
            else
            {
                GetRegister = TRUE;
            }
        }
        else if (!GetMask)
        {
            if (!ConvertTokenToUInt64(Section, &MaskRegister))
            {
                //
                // Unknown parameter
                //
                ShowMessages("unknown parameter '%s'\n\n",
                             GetCaseSensitiveStringFromCommandToken(Section).c_str());
                CommandCrwriteHelp();

                FreeEventsAndActionsMemory(Event, ActionBreakToDebugger, ActionCustomCode, ActionScript);
                return;
            }
            else
            {
                GetMask = TRUE;
            }
        }
        else
        {
            //
            // Unknown parameter
            //
            ShowMessages("unknown parameter '%s'\n\n",
                         GetCaseSensitiveStringFromCommandToken(Section).c_str());
            CommandCrwriteHelp();

            FreeEventsAndActionsMemory(Event, ActionBreakToDebugger, ActionCustomCode, ActionScript);
            return;
        }
    }

    if (TargetRegister != VMX_EXIT_QUALIFICATION_REGISTER_CR0 && TargetRegister != VMX_EXIT_QUALIFICATION_REGISTER_CR4)
    {
        ShowMessages("please choose either 0 for cr0 or 4 for cr4\n");
        CommandCrwriteHelp();

        FreeEventsAndActionsMemory(Event, ActionBreakToDebugger, ActionCustomCode, ActionScript);
        return;
    }

    //
    // Set the target control register
    //
    Event->Options.OptionalParam1 = TargetRegister;

    //
    // Set the mask to filter control register bits
    //
    Event->Options.OptionalParam2 = MaskRegister;

    //
    // Send the ioctl to the kernel for event registration
    //
    if (!SendEventToKernel(Event, EventLength))
    {
        //
        // There was an error, probably the handle was not initialized
        // we have to free the Action before exit, it is because, we
        // already freed the Event and string buffers
        //
        FreeEventsAndActionsMemory(Event, ActionBreakToDebugger, ActionCustomCode, ActionScript);
        return;
    }

    //
    // Add the event to the kernel
    //
    if (!RegisterActionToEvent(Event,
                               ActionBreakToDebugger,
                               ActionBreakToDebuggerLength,
                               ActionCustomCode,
                               ActionCustomCodeLength,
                               ActionScript,
                               ActionScriptLength))
    {
        //
        // There was an error
        //
        FreeEventsAndActionsMemory(Event, ActionBreakToDebugger, ActionCustomCode, ActionScript);
        return;
    }
}
