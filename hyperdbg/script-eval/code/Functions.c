/**
 * @file Functions.c
 * @author M.H. Gholamrezaei (mh@hyperdbg.org)
 * @author Sina Karvandi (sina@hyperdbg.org)
 * @brief Script engine functions implementations
 * @details
 * @version 0.2
 * @date 2022-06-29
 *
 * @copyright This project is released under the GNU Public License v3.
 *
 */
#include "pch.h"

//
// User-mode Global Variables
//
#ifdef SCRIPT_ENGINE_USER_MODE

extern UINT64  g_CurrentExprEvalResult;
extern BOOLEAN g_CurrentExprEvalResultHasError;

#endif // SCRIPT_ENGINE_USER_MODE

//
// *** Definitions ***
//
UINT64
GetValue(PGUEST_REGS                      GuestRegs,
         PACTION_BUFFER                   ActionBuffer,
         PSCRIPT_ENGINE_GENERAL_REGISTERS ScriptGeneralRegisters,
         PSYMBOL                          Symbol,
         BOOLEAN                          ReturnReference);

//
// *** Functions ***
//

/**
 * @brief Implementation of eq function
 *
 * @param Address
 * @param Value
 * @param HasError
 * @return BOOLEAN
 */
BOOLEAN
ScriptEngineFunctionEq(UINT64 Address, QWORD Value, BOOL * HasError)
{
    UNREFERENCED_PARAMETER(HasError);

#ifdef SCRIPT_ENGINE_KERNEL_MODE

    if (!CheckAccessValidityAndSafety(Address, sizeof(QWORD)))
    {
        //
        // Instead of indicating an error, just return false
        // to assign it as a return result to a variable
        //
        // *HasError = TRUE;

        return FALSE;
    }

#endif // SCRIPT_ENGINE_KERNEL_MODE

#ifdef SCRIPT_ENGINE_USER_MODE
    *(UINT64 *)Address = Value;
#endif // SCRIPT_ENGINE_USER_MODE

#ifdef SCRIPT_ENGINE_KERNEL_MODE
    MemoryMapperWriteMemorySafeOnTargetProcess(Address, &Value, sizeof(QWORD));
#endif // SCRIPT_ENGINE_KERNEL_MODE

    return TRUE;
}

/**
 * @brief Implementation of ed function
 *
 * @param Address
 * @param Value
 * @param HasError
 * @return BOOLEAN
 */
BOOLEAN
ScriptEngineFunctionEd(UINT64 Address, DWORD Value, BOOL * HasError)
{
    UNREFERENCED_PARAMETER(HasError);

#ifdef SCRIPT_ENGINE_KERNEL_MODE

    if (!CheckAccessValidityAndSafety(Address, sizeof(DWORD)))
    {
        //
        // Instead of indicating an error, just return false
        // to assign it as a return result to a variable
        //
        // *HasError = TRUE;

        return FALSE;
    }

#endif // SCRIPT_ENGINE_KERNEL_MODE

#ifdef SCRIPT_ENGINE_USER_MODE
    *(DWORD *)Address = Value;
#endif // SCRIPT_ENGINE_USER_MODE

#ifdef SCRIPT_ENGINE_KERNEL_MODE
    MemoryMapperWriteMemorySafeOnTargetProcess(Address, &Value, sizeof(DWORD));
#endif // SCRIPT_ENGINE_KERNEL_MODE

    return TRUE;
}

/**
 * @brief Implementation of eb function
 *
 * @param Address
 * @param Value
 * @param HasError
 * @return BOOLEAN
 */
BOOLEAN
ScriptEngineFunctionEb(UINT64 Address, BYTE Value, BOOL * HasError)
{
    UNREFERENCED_PARAMETER(HasError);

#ifdef SCRIPT_ENGINE_KERNEL_MODE

    if (!CheckAccessValidityAndSafety(Address, sizeof(BYTE)))
    {
        //
        // Instead of indicating an error, just return false
        // to assign it as a return result to a variable
        //
        // *HasError = TRUE;

        return FALSE;
    }

#endif // SCRIPT_ENGINE_KERNEL_MODE

#ifdef SCRIPT_ENGINE_USER_MODE
    *(BYTE *)Address = Value;
#endif // SCRIPT_ENGINE_USER_MODE

#ifdef SCRIPT_ENGINE_KERNEL_MODE
    MemoryMapperWriteMemorySafeOnTargetProcess(Address, &Value, sizeof(BYTE));
#endif // SCRIPT_ENGINE_KERNEL_MODE

    return TRUE;
}

/**
 * @brief Implementation of eq function (Physical Memory)
 *
 * @param Address
 * @param Value
 * @param HasError
 * @return BOOLEAN
 */
BOOLEAN
ScriptEngineFunctionEqPa(UINT64 Address, QWORD Value, BOOL * HasError)
{
    UNREFERENCED_PARAMETER(HasError);

#ifdef SCRIPT_ENGINE_KERNEL_MODE

    if (!CheckAddressPhysical(Address))
    {
        //
        // Instead of indicating an error, just return false
        // to assign it as a return result to a variable
        //
        // *HasError = TRUE;

        return FALSE;
    }

#endif // SCRIPT_ENGINE_KERNEL_MODE

#ifdef SCRIPT_ENGINE_USER_MODE

    ShowMessages("err, using physical address functions (eq_pa) is not possible in user-mode\n");
    return FALSE;

#endif // SCRIPT_ENGINE_USER_MODE

#ifdef SCRIPT_ENGINE_KERNEL_MODE
    MemoryMapperWriteMemorySafeByPhysicalAddress(Address, (UINT64)&Value, sizeof(QWORD));
#endif // SCRIPT_ENGINE_KERNEL_MODE

    return TRUE;
}

/**
 * @brief Implementation of ed function (Physical Memory)
 *
 * @param Address
 * @param Value
 * @param HasError
 * @return BOOLEAN
 */
BOOLEAN
ScriptEngineFunctionEdPa(UINT64 Address, DWORD Value, BOOL * HasError)
{
    UNREFERENCED_PARAMETER(HasError);

#ifdef SCRIPT_ENGINE_KERNEL_MODE

    if (!CheckAddressPhysical(Address))
    {
        //
        // Instead of indicating an error, just return false
        // to assign it as a return result to a variable
        //
        // *HasError = TRUE;

        return FALSE;
    }

#endif // SCRIPT_ENGINE_KERNEL_MODE

#ifdef SCRIPT_ENGINE_USER_MODE

    ShowMessages("err, using physical address functions (ed_pa) is not possible in user-mode\n");
    return FALSE;

#endif // SCRIPT_ENGINE_USER_MODE

#ifdef SCRIPT_ENGINE_KERNEL_MODE
    MemoryMapperWriteMemorySafeByPhysicalAddress(Address, (UINT64)&Value, sizeof(DWORD));
#endif // SCRIPT_ENGINE_KERNEL_MODE

    return TRUE;
}

/**
 * @brief Implementation of eb function (Physical Memory)
 *
 * @param Address
 * @param Value
 * @param HasError
 * @return BOOLEAN
 */
BOOLEAN
ScriptEngineFunctionEbPa(UINT64 Address, BYTE Value, BOOL * HasError)
{
    UNREFERENCED_PARAMETER(HasError);

#ifdef SCRIPT_ENGINE_KERNEL_MODE

    if (!CheckAddressPhysical(Address))
    {
        //
        // Instead of indicating an error, just return false
        // to assign it as a return result to a variable
        //
        // *HasError = TRUE;

        return FALSE;
    }

#endif // SCRIPT_ENGINE_KERNEL_MODE

#ifdef SCRIPT_ENGINE_USER_MODE

    ShowMessages("err, using physical address functions (eb_pa) is not possible in user-mode\n");
    return FALSE;

#endif // SCRIPT_ENGINE_USER_MODE

#ifdef SCRIPT_ENGINE_KERNEL_MODE
    MemoryMapperWriteMemorySafeByPhysicalAddress(Address, (UINT64)&Value, sizeof(BYTE));
#endif // SCRIPT_ENGINE_KERNEL_MODE

    return TRUE;
}

/**
 * @brief Check whether the address is valid or not
 *
 * @param Address
 * @param Length
 * @return BOOLEAN
 */
BOOLEAN
ScriptEngineFunctionCheckAddress(UINT64 Address, UINT32 Length)
{
#ifdef SCRIPT_ENGINE_USER_MODE

    if (CheckAccessValidityAndSafety(Address, Length))
    {
        return TRUE;
    }

#endif // SCRIPT_ENGINE_USER_MODE

#ifdef SCRIPT_ENGINE_KERNEL_MODE

    if (CheckAccessValidityAndSafety(Address, Length))
    {
        return TRUE;
    }

#endif // SCRIPT_ENGINE_KERNEL_MODE

    return FALSE;
}

/**
 * @brief A VMX-compatible equivalent of memcpy function in C
 *
 * @param Destination
 * @param Source
 * @param Num
 * @param HasError
 * @return VOID
 */
VOID
ScriptEngineFunctionMemcpy(UINT64 Destination, UINT64 Source, UINT32 Num, BOOL * HasError)
{
    UINT64 PrevReadLen                                              = 0;
    BYTE   MovingBuffer[DebuggerScriptEngineMemcpyMovingBufferSize] = {0};

#ifdef SCRIPT_ENGINE_USER_MODE

    //
    // Check the destination address
    //
    if (!CheckAccessValidityAndSafety(Destination, Num))
    {
        *HasError = TRUE;
        return;
    }

    //
    // Check the source address
    //
    if (!CheckAccessValidityAndSafety(Source, Num))
    {
        *HasError = TRUE;
        return;
    }

    //
    // Address is valid, perform the memcpy in user-mode
    //
    memcpy((void *)Destination, (void *)Source, Num);

#endif // SCRIPT_ENGINE_USER_MODE

#ifdef SCRIPT_ENGINE_KERNEL_MODE

    //
    // Check the destination address
    //
    if (!CheckAccessValidityAndSafety(Destination, Num))
    {
        *HasError = TRUE;
        return;
    }

    //
    // Check the source address
    //
    if (!CheckAccessValidityAndSafety(Source, Num))
    {
        *HasError = TRUE;
        return;
    }

    //
    // Address is valid, perform the memcpy in kernel-mode (VMX-root mode)
    //
    while (Num > 0)
    {
        //
        // Check the target buffer size
        //
        if (Num > DebuggerScriptEngineMemcpyMovingBufferSize)
        {
            //
            // *** The size of read buffer is greater to maximum the moving buffer size ***
            //

            //
            // Read memory into the buffer
            //
            MemoryMapperReadMemorySafeOnTargetProcess(Source + PrevReadLen, MovingBuffer, DebuggerScriptEngineMemcpyMovingBufferSize);

            //
            // Write the moving buffer into the target buffer
            //
            MemoryMapperWriteMemorySafeOnTargetProcess(Destination + PrevReadLen, MovingBuffer, DebuggerScriptEngineMemcpyMovingBufferSize);

            //
            // Computing the bytes that we read
            //
            PrevReadLen += DebuggerScriptEngineMemcpyMovingBufferSize;
            Num -= DebuggerScriptEngineMemcpyMovingBufferSize;
        }
        else
        {
            //
            // *** The size of read buffer is lower than or equal to the moving buffer size ***
            //

            //
            // Read memory into the buffer
            //
            MemoryMapperReadMemorySafeOnTargetProcess(Source + PrevReadLen, MovingBuffer, Num);

            //
            // Write the moving buffer into the target buffer
            //
            MemoryMapperWriteMemorySafeOnTargetProcess(Destination + PrevReadLen, MovingBuffer, Num);

            //
            // Computing the bytes that we gonna read
            //
            PrevReadLen += Num;
            Num = 0; // or Num -= Num;
        }
    }

#endif // SCRIPT_ENGINE_KERNEL_MODE
}

/**
 * @brief A VMX-compatible equivalent of memcpy function in C for physical memory
 *
 * @param Destination
 * @param Source
 * @param Num
 * @param HasError
 * @return VOID
 */
VOID
ScriptEngineFunctionMemcpyPa(UINT64 Destination, UINT64 Source, UINT32 Num, BOOL * HasError)
{
    UINT64 PrevReadLen                                              = 0;
    BYTE   MovingBuffer[DebuggerScriptEngineMemcpyMovingBufferSize] = {0};

#ifdef SCRIPT_ENGINE_USER_MODE

    //
    // Show an error message in user-mode
    //
    ShowMessages("err, using physical address functions (memcpy_pa) is not possible in user-mode\n");
    return;

#endif // SCRIPT_ENGINE_USER_MODE

#ifdef SCRIPT_ENGINE_KERNEL_MODE

    //
    // Check the destination address (physical)
    //
    if (!CheckAddressPhysical(Destination))
    {
        *HasError = TRUE;
        return;
    }

    //
    // Check the source address (physical)
    //
    if (!CheckAddressPhysical(Source))
    {
        *HasError = TRUE;
        return;
    }

    //
    // Address is valid, perform the memcpy in kernel-mode (VMX-root mode)
    //
    while (Num > 0)
    {
        //
        // Check the target buffer size
        //
        if (Num > DebuggerScriptEngineMemcpyMovingBufferSize)
        {
            //
            // *** The size of read buffer is greater to maximum the moving buffer size ***
            //

            //
            // Read memory into the buffer
            //
            MemoryMapperReadMemorySafeByPhysicalAddress(Source + PrevReadLen, (UINT64)MovingBuffer, DebuggerScriptEngineMemcpyMovingBufferSize);

            //
            // Write the moving buffer into the target buffer
            //
            MemoryMapperWriteMemorySafeByPhysicalAddress(Destination + PrevReadLen, (UINT64)MovingBuffer, DebuggerScriptEngineMemcpyMovingBufferSize);

            //
            // Computing the bytes that we read
            //
            PrevReadLen += DebuggerScriptEngineMemcpyMovingBufferSize;
            Num -= DebuggerScriptEngineMemcpyMovingBufferSize;
        }
        else
        {
            //
            // *** The size of read buffer is lower than or equal to the moving buffer size ***
            //

            //
            // Read memory into the buffer
            //
            MemoryMapperReadMemorySafeByPhysicalAddress(Source + PrevReadLen, (UINT64)MovingBuffer, Num);

            //
            // Write the moving buffer into the target buffer
            //
            MemoryMapperWriteMemorySafeByPhysicalAddress(Destination + PrevReadLen, (UINT64)MovingBuffer, Num);

            //
            // Computing the bytes that we gonna read
            //
            PrevReadLen += Num;
            Num = 0; // or Num -= Num;
        }
    }

#endif // SCRIPT_ENGINE_KERNEL_MODE
}

//
// Convert virtual address to physical address
//
UINT64
ScriptEngineFunctionVirtualToPhysical(UINT64 Address)
{
#ifdef SCRIPT_ENGINE_USER_MODE

    //
    // There is no conversion in user-mode
    //
    return NULL;
#endif // SCRIPT_ENGINE_USER_MODE

#ifdef SCRIPT_ENGINE_KERNEL_MODE

    return VirtualAddressToPhysicalAddressOnTargetProcess((PVOID)Address);

#endif // SCRIPT_ENGINE_KERNEL_MODE
}

/**
 * @brief Convert physical address to virtual address
 *
 * @param Address
 * @return UINT64
 */
UINT64
ScriptEngineFunctionPhysicalToVirtual(UINT64 Address)
{
#ifdef SCRIPT_ENGINE_USER_MODE

    //
    // There is no conversion in user-mode
    //
    return NULL;
#endif // SCRIPT_ENGINE_USER_MODE

#ifdef SCRIPT_ENGINE_KERNEL_MODE

    return PhysicalAddressToVirtualAddressOnTargetProcess((PVOID)Address);

#endif // SCRIPT_ENGINE_KERNEL_MODE
}

/**
 * @brief Implementation of print function
 *
 * @param Tag
 * @param ImmediateMessagePassing
 * @param Value
 * @return VOID
 */
VOID
ScriptEngineFunctionPrint(UINT64 Tag, BOOLEAN ImmediateMessagePassing, UINT64 Value)
{
#ifdef SCRIPT_ENGINE_USER_MODE
    ShowMessages("%llx\n", Value);
#endif // SCRIPT_ENGINE_USER_MODE

#ifdef SCRIPT_ENGINE_KERNEL_MODE

    //
    // Prepare a buffer to bypass allocating a huge stack space for logging
    //
    char   TempBuffer[20] = {0};
    UINT32 TempBufferLen  = sprintf(TempBuffer, "%llx", Value);

    LogSimpleWithTag((UINT32)Tag, ImmediateMessagePassing, TempBuffer, TempBufferLen + 1);

#endif // SCRIPT_ENGINE_KERNEL_MODE
}

/**
 * @brief Implementation of test_statement function
 *
 * @param Tag
 * @param ImmediateMessagePassing
 * @param Value
 * @return VOID
 */
VOID
ScriptEngineFunctionTestStatement(UINT64 Tag, BOOLEAN ImmediateMessagePassing, UINT64 Value)
{
    UNREFERENCED_PARAMETER(Tag);
    UNREFERENCED_PARAMETER(ImmediateMessagePassing);
    UNREFERENCED_PARAMETER(Value);

#ifdef SCRIPT_ENGINE_USER_MODE

    g_CurrentExprEvalResult         = Value;
    g_CurrentExprEvalResultHasError = FALSE;

#endif // SCRIPT_ENGINE_USER_MODE
}

/**
 * @brief Implementation of spinlock_lock function
 *
 * @param Lock
 * @param HasError
 * @return VOID
 */
VOID
ScriptEngineFunctionSpinlockLock(volatile LONG * Lock, BOOL * HasError)
{
#ifdef SCRIPT_ENGINE_USER_MODE

    SpinlockLock(Lock);

#endif // SCRIPT_ENGINE_USER_MODE

#ifdef SCRIPT_ENGINE_KERNEL_MODE

    if (!CheckAccessValidityAndSafety((UINT64)Lock, sizeof(LONG)))
    {
        *HasError = TRUE;
        return;
    }

    SpinlockLock(Lock);

#endif // SCRIPT_ENGINE_KERNEL_MODE
}

/**
 * @brief Implementation of spinlock_unlock function
 *
 * @param Lock
 * @param HasError
 * @return VOID
 */
VOID
ScriptEngineFunctionSpinlockUnlock(volatile LONG * Lock, BOOL * HasError)
{
#ifdef SCRIPT_ENGINE_USER_MODE

    SpinlockUnlock(Lock);

#endif // SCRIPT_ENGINE_USER_MODE

#ifdef SCRIPT_ENGINE_KERNEL_MODE

    if (!CheckAccessValidityAndSafety((UINT64)Lock, sizeof(LONG)))
    {
        *HasError = TRUE;
        return;
    }

    SpinlockUnlock(Lock);

#endif // SCRIPT_ENGINE_KERNEL_MODE
}

/**
 * @brief Implementation of spinlock_lock_custom_wait function
 *
 * @param Lock
 * @param MaxWait
 * @param HasError
 * @return VOID
 */
VOID
ScriptEngineFunctionSpinlockLockCustomWait(volatile long * Lock, unsigned MaxWait, BOOL * HasError)
{
#ifdef SCRIPT_ENGINE_USER_MODE

    SpinlockLockWithCustomWait(Lock, MaxWait);

#endif // SCRIPT_ENGINE_USER_MODE

#ifdef SCRIPT_ENGINE_KERNEL_MODE

    if (!CheckAccessValidityAndSafety((UINT64)Lock, sizeof(LONG)))
    {
        *HasError = TRUE;
        return;
    }

    SpinlockLockWithCustomWait(Lock, MaxWait);

#endif // SCRIPT_ENGINE_KERNEL_MODE
}

/**
 * @brief Implementation of strlen function
 *
 * @param Address
 * @return UINT64
 */
UINT64
ScriptEngineFunctionStrlen(const char * Address)
{
    UINT64 Result = 0;
#ifdef SCRIPT_ENGINE_USER_MODE
    Result = strlen(Address);
#endif // SCRIPT_ENGINE_USER_MODE

#ifdef SCRIPT_ENGINE_KERNEL_MODE
    Result = VmFuncVmxCompatibleStrlen(Address);
#endif // SCRIPT_ENGINE_KERNEL_MODE

    return Result;
}

/**
 * @brief Implementation of disassemble_len function
 *
 * @param Address
 * @param Is32Bit
 *
 * @return UINT64
 */
UINT64
ScriptEngineFunctionDisassembleLen(PVOID Address, BOOLEAN Is32Bit)
{
    UINT64 Result = 0;
#ifdef SCRIPT_ENGINE_USER_MODE
    Result = HyperDbgLengthDisassemblerEngine((unsigned char *)Address, MAXIMUM_INSTR_SIZE, Is32Bit ? FALSE : TRUE);
#endif // SCRIPT_ENGINE_USER_MODE

#ifdef SCRIPT_ENGINE_KERNEL_MODE
    Result = DisassemblerLengthDisassembleEngineInVmxRootOnTargetProcess(Address, Is32Bit);
#endif // SCRIPT_ENGINE_KERNEL_MODE

    return Result;
}

/**
 * @brief Implementation of wcslen function
 *
 * @param Address
 * @return UINT64
 */
UINT64
ScriptEngineFunctionWcslen(const wchar_t * Address)
{
    UINT64 Result = 0;

#ifdef SCRIPT_ENGINE_USER_MODE
    Result = wcslen(Address);
#endif // SCRIPT_ENGINE_USER_MODE

#ifdef SCRIPT_ENGINE_KERNEL_MODE
    Result = VmFuncVmxCompatibleWcslen(Address);
#endif // SCRIPT_ENGINE_KERNEL_MODE

    return Result;
}

/**
 * @brief Implementation of interlocked_exchange function
 *
 * @param Target
 * @param Value
 * @param HasError
 * @return long long
 */
long long
ScriptEngineFunctionInterlockedExchange(long long volatile * Target,
                                        long long            Value,
                                        BOOL *               HasError)
{
    long long Result = 0;

#ifdef SCRIPT_ENGINE_KERNEL_MODE

    if (!CheckAccessValidityAndSafety((UINT64)Target, sizeof(long long)))
    {
        *HasError = TRUE;
        return (long long)NULL;
    }

#endif // SCRIPT_ENGINE_KERNEL_MODE

    Result = InterlockedExchange64(Target, Value);

    return Result;
}

/**
 * @brief Implementation of interlocked_exchange_add function
 *
 * @param Addend
 * @param Value
 * @param HasError
 * @return long long
 */
long long
ScriptEngineFunctionInterlockedExchangeAdd(long long volatile * Addend,
                                           long long            Value,
                                           BOOL *               HasError)
{
    long long Result = 0;

#ifdef SCRIPT_ENGINE_KERNEL_MODE

    if (!CheckAccessValidityAndSafety((UINT64)Addend, sizeof(long long)))
    {
        *HasError = TRUE;
        return (long long)NULL;
    }

#endif // SCRIPT_ENGINE_KERNEL_MODE

    Result = InterlockedExchangeAdd64(Addend, Value);

    return Result;
}

/**
 * @brief Implementation of interlocked_exchange_increment function
 *
 * @param Addend
 * @param HasError
 * @return long long
 */
long long
ScriptEngineFunctionInterlockedIncrement(long long volatile * Addend,
                                         BOOL *               HasError)
{
    long long Result = 0;

#ifdef SCRIPT_ENGINE_KERNEL_MODE

    if (!CheckAccessValidityAndSafety((UINT64)Addend, sizeof(long long)))
    {
        *HasError = TRUE;
        return (long long)NULL;
    }

#endif // SCRIPT_ENGINE_KERNEL_MODE

    Result = InterlockedIncrement64(Addend);

    return Result;
}

/**
 * @brief Implementation of interlocked_exchange_decrement function
 *
 * @param Addend
 * @param HasError
 * @return long long
 */
long long
ScriptEngineFunctionInterlockedDecrement(long long volatile * Addend,
                                         BOOL *               HasError)
{
    long long Result = 0;

#ifdef SCRIPT_ENGINE_KERNEL_MODE

    if (!CheckAccessValidityAndSafety((UINT64)Addend, sizeof(long long)))
    {
        *HasError = TRUE;
        return (long long)NULL;
    }

#endif // SCRIPT_ENGINE_KERNEL_MODE

    Result = InterlockedDecrement64(Addend);

    return Result;
}

/**
 * @brief Implementation of interlocked_compare_exchange function
 *
 * @param Destination
 * @param ExChange
 * @param Comperand
 * @param HasError
 * @return long long
 */
long long
ScriptEngineFunctionInterlockedCompareExchange(
    long long volatile * Destination,
    long long            ExChange,
    long long            Comperand,
    BOOL *               HasError)
{
    long long Result = 0;

#ifdef SCRIPT_ENGINE_KERNEL_MODE

    if (!CheckAccessValidityAndSafety((UINT64)Destination, sizeof(long long)))
    {
        *HasError = TRUE;
        return (long long)NULL;
    }

#endif // SCRIPT_ENGINE_KERNEL_MODE

    Result = InterlockedCompareExchange64(Destination, ExChange, Comperand);

    return Result;
}

/**
 * @brief Implementation of event_enable function
 *
 * @param EventId
 *
 * @return VOID
 */
VOID
ScriptEngineFunctionEventEnable(UINT64 EventId)
{
#ifdef SCRIPT_ENGINE_USER_MODE
    ShowMessages("err, enabling events is not possible in user-mode\n");
#endif // SCRIPT_ENGINE_USER_MODE

#ifdef SCRIPT_ENGINE_KERNEL_MODE
    if (!DebuggerEnableEvent(EventId + DebuggerEventTagStartSeed))
    {
        LogInfo("Invalid tag id (%x)", EventId);
    }
#endif // SCRIPT_ENGINE_KERNEL_MODE
}

/**
 * @brief Implementation of event_disable function
 *
 * @param EventId
 *
 * @return VOID
 */
VOID
ScriptEngineFunctionEventDisable(UINT64 EventId)
{
#ifdef SCRIPT_ENGINE_USER_MODE
    ShowMessages("err, disabling events is not possible in user-mode\n");
#endif // SCRIPT_ENGINE_USER_MODE

#ifdef SCRIPT_ENGINE_KERNEL_MODE
    if (!DebuggerDisableEvent(EventId + DebuggerEventTagStartSeed))
    {
        LogInfo("Invalid tag id (%x)", EventId);
    }
#endif // SCRIPT_ENGINE_KERNEL_MODE
}

/**
 * @brief Implementation of event_clear function
 *
 * @param EventId
 *
 * @return VOID
 */
VOID
ScriptEngineFunctionEventClear(UINT64 EventId)
{
#ifdef SCRIPT_ENGINE_USER_MODE
    ShowMessages("err, disabling events is not possible in user-mode\n");
#endif // SCRIPT_ENGINE_USER_MODE

#ifdef SCRIPT_ENGINE_KERNEL_MODE

    BOOLEAN PoolManagerAllocatedMemory = FALSE;

    if (g_KernelDebuggerState && EnableInstantEventMechanism)
    {
        PoolManagerAllocatedMemory = TRUE;
    }

    if (!DebuggerClearEvent(EventId + DebuggerEventTagStartSeed, VmFuncVmxGetCurrentExecutionMode(), PoolManagerAllocatedMemory))
    {
        LogInfo("Invalid tag id (%x)", EventId);
    }
#endif // SCRIPT_ENGINE_KERNEL_MODE
}

/**
 * @brief Implementation of pause function
 *
 * @param ActionDetail
 * @param GuestRegs
 *
 * @return VOID
 */
VOID
ScriptEngineFunctionPause(
    ACTION_BUFFER * ActionDetail,
    PGUEST_REGS     GuestRegs)
{
#ifdef SCRIPT_ENGINE_USER_MODE
    ShowMessages("err, breaking is not possible in user-mode\n");
#endif // SCRIPT_ENGINE_USER_MODE

#ifdef SCRIPT_ENGINE_KERNEL_MODE

    //
    // pause(); function is only working when kernel debugger is working
    // it's not designed to work on vmi-mode (local debugging)
    //
    if (g_KernelDebuggerState && g_DebuggeeHaltReason == DEBUGGEE_PAUSING_REASON_NOT_PAUSED)
    {
        DEBUGGER_TRIGGERED_EVENT_DETAILS TriggeredEventDetail = {0};
        ULONG                            CurrentCore          = KeGetCurrentProcessorNumberEx(NULL);

        //
        // Make the details of context
        //
        TriggeredEventDetail.Tag     = ActionDetail->Tag;
        TriggeredEventDetail.Context = (PVOID)ActionDetail->Context;

        if (ActionDetail->CallingStage == 1)
        {
            TriggeredEventDetail.Stage = VMM_CALLBACK_CALLING_STAGE_POST_EVENT_EMULATION;
        }
        else
        {
            TriggeredEventDetail.Stage = VMM_CALLBACK_CALLING_STAGE_PRE_EVENT_EMULATION;
        }

        if (VmFuncVmxGetCurrentExecutionMode() == TRUE)
        {
            //
            // The guest is already in vmx-root mode
            // Halt other cores
            //

            KdHandleBreakpointAndDebugBreakpointsCallback(
                CurrentCore,
                DEBUGGEE_PAUSING_REASON_DEBUGGEE_EVENT_TRIGGERED,
                &TriggeredEventDetail);
        }
        else
        {
            //
            // The guest is on vmx non-root mode, the first parameter
            // is context and the second parameter is tag
            //
            VmFuncVmxVmcall(DEBUGGER_VMCALL_VM_EXIT_HALT_SYSTEM_AS_A_RESULT_OF_TRIGGERING_EVENT,
                            (UINT64)&TriggeredEventDetail,
                            (UINT64)GuestRegs,
                            (UINT64)NULL);
        }
    }
    else
    {
        LogInfo("The 'pause();' function is either called from the vmi-mode or is "
                "evaluated by the '?' command. It's not allowed to use it on vmi-mode "
                "(local debugging) or by the '?' command");
    }

#endif // SCRIPT_ENGINE_KERNEL_MODE
}

/**
 * @brief Implementation of flush function
 *
 * @return VOID
 */
VOID
ScriptEngineFunctionFlush()
{
#ifdef SCRIPT_ENGINE_USER_MODE
    ShowMessages("err, it's not possible to flush buffers in user-mode\n");
#endif // SCRIPT_ENGINE_USER_MODE

#ifdef SCRIPT_ENGINE_KERNEL_MODE

    //
    // Mark all buffers as read
    //
    LogMarkAllAsRead(TRUE);
    LogMarkAllAsRead(FALSE);

#endif // SCRIPT_ENGINE_KERNEL_MODE
}

/**
 * @brief Implementation of event_ignore function
 * @param State
 * @param ActionDetail
 *
 * @return VOID
 */
VOID
ScriptEngineFunctionShortCircuitingEvent(UINT64 State, ACTION_BUFFER * ActionDetail)
{
#ifdef SCRIPT_ENGINE_USER_MODE
    ShowMessages("err, it's not possible to short-circuit events in user-mode\n");
#endif // SCRIPT_ENGINE_USER_MODE

#ifdef SCRIPT_ENGINE_KERNEL_MODE

    if (ActionDetail->CallingStage == 1)
    {
        LogWarning("Warning, calling the 'event_sc' function in the 'post' calling stage doesn't make sense as the emulation is already performed!\n"
                   "You can use this function in the 'pre' calling stage");
        return;
    }

    ULONG CurrentCore = KeGetCurrentProcessorNumberEx(NULL);

    if (State != 0)
    {
        g_DbgState[CurrentCore].ShortCircuitingEvent = TRUE;
    }
    else
    {
        g_DbgState[CurrentCore].ShortCircuitingEvent = FALSE;
    }

#endif // SCRIPT_ENGINE_KERNEL_MODE
}

/**
 * @brief Implementation of formats function
 *
 * @param Tag
 * @param ImmediateMessagePassing
 * @param Value
 * @return VOID
 */
VOID
ScriptEngineFunctionFormats(UINT64 Tag, BOOLEAN ImmediateMessagePassing, UINT64 Value)
{
#ifdef SCRIPT_ENGINE_USER_MODE

    ScriptEngineFunctionTestStatement(Tag, ImmediateMessagePassing, Value);

#endif // SCRIPT_ENGINE_USER_MODE

#ifdef SCRIPT_ENGINE_KERNEL_MODE
    if (g_KernelDebuggerState)
    {
        KdSendFormatsFunctionResult(Value);
    }
    else
    {
        //
        // Prepare a buffer to bypass allocating a huge stack space for logging
        //
        char   TempBuffer[20] = {0};
        UINT32 TempBufferLen  = sprintf(TempBuffer, "%llx\n", Value);

        LogSimpleWithTag((UINT32)Tag, ImmediateMessagePassing, TempBuffer, TempBufferLen + 1);
    }
#endif // SCRIPT_ENGINE_KERNEL_MODE
}

/**
 * @brief Custom VMX-root compatible strlen
 *
 * @param StrAddr
 * @param IsWstring
 * @return UINT32
 */
UINT32
CustomStrlen(UINT64 StrAddr, BOOLEAN IsWstring)
{
#ifdef SCRIPT_ENGINE_USER_MODE

    if (IsWstring)
    {
        return (UINT32)wcslen((const wchar_t *)StrAddr);
    }
    else
    {
        return (UINT32)strlen((const char *)StrAddr);
    }
#endif // SCRIPT_ENGINE_USER_MODE

#ifdef SCRIPT_ENGINE_KERNEL_MODE
    if (IsWstring)
    {
        return VmFuncVmxCompatibleWcslen((const wchar_t *)StrAddr);
    }
    else
    {
        return VmFuncVmxCompatibleStrlen((const CHAR *)StrAddr);
    }
#endif // SCRIPT_ENGINE_KERNEL_MODE
}

/**
 * @brief Check if string is safe to be accessed or not (in vmx-root mode)
 *
 * @param StrAddr
 * @param IsWstring
 * @return BOOLEAN
 */
BOOLEAN
CheckIfStringIsSafe(UINT64 StrAddr, BOOLEAN IsWstring)
{
#ifdef SCRIPT_ENGINE_USER_MODE
    return TRUE;
#endif // SCRIPT_ENGINE_USER_MODE

#ifdef SCRIPT_ENGINE_KERNEL_MODE

    //
    // At least two chars (wchar_t is 4 byte)
    //
    if (CheckAccessValidityAndSafety(StrAddr, IsWstring ? 4 : 2))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
#endif // SCRIPT_ENGINE_KERNEL_MODE
}

/**
 * @brief Apply format specifiers (%d, %x, %llx, etc.)
 *
 * @param CurrentSpecifier
 * @param FinalBuffer
 * @param CurrentProcessedPositionFromStartOfFormat
 * @param CurrentPositionInFinalBuffer
 * @param Val
 * @param SizeOfFinalBuffer
 * @return VOID
 */
VOID
ApplyFormatSpecifier(const CHAR * CurrentSpecifier, CHAR * FinalBuffer, PUINT32 CurrentProcessedPositionFromStartOfFormat, PUINT32 CurrentPositionInFinalBuffer, UINT64 Val, UINT32 SizeOfFinalBuffer)
{
    UINT32 TempBufferLen      = 0;
    CHAR   TempBuffer[50 + 1] = {
        0}; // Maximum uint64_t is 18446744073709551615 + 1 thus its 20 character
              // for maximum buffer + 1 end char null but we alloc 50 to be sure

    *CurrentProcessedPositionFromStartOfFormat =
        *CurrentProcessedPositionFromStartOfFormat + (UINT32)strlen(CurrentSpecifier);
    sprintf_s(TempBuffer, sizeof(TempBuffer), CurrentSpecifier, Val);
    TempBufferLen = (UINT32)strlen(TempBuffer);

    //
    // Check final buffer capacity
    //
    if (*CurrentPositionInFinalBuffer + TempBufferLen > SizeOfFinalBuffer)
    {
        //
        // Over passed buffer
        //
        return;
    }

    memcpy(&FinalBuffer[*CurrentPositionInFinalBuffer], TempBuffer, TempBufferLen);

    *CurrentPositionInFinalBuffer = *CurrentPositionInFinalBuffer + TempBufferLen;
}

/**
 * @brief Convert WCHAR* to CHAR*
 *
 * @param src
 * @param dest
 * @param dest_len
 * @return size_t
 */
size_t
WcharToChar(const wchar_t * src, char * dest, size_t dest_len)
{
    wchar_t Code;
    size_t  i;

    i = 0;

    while ((src[i] != '\0') && i < (dest_len - 1))
    {
        Code = src[i];
        if (Code < 128)
            dest[i] = (char)Code;
        else
        {
            dest[i] = '?';
            if (Code >= 0xD800 && Code <= 0xD8FF)
            {
                //
                // Lead surrogate, skip the next code unit, which is the trail
                //
                i++;
            }
        }
        i++;
    }

    return i - 1;
}

/**
 * @brief Apply string format specifiers (%s, %ws, etc.)
 *
 * @param CurrentSpecifier
 * @param FinalBuffer
 * @param CurrentProcessedPositionFromStartOfFormat
 * @param CurrentPositionInFinalBuffer
 * @param Val
 * @param IsWstring
 * @param SizeOfFinalBuffer
 * @return BOOLEAN
 */
BOOLEAN
ApplyStringFormatSpecifier(const CHAR * CurrentSpecifier, CHAR * FinalBuffer, PUINT32 CurrentProcessedPositionFromStartOfFormat, PUINT32 CurrentPositionInFinalBuffer, UINT64 Val, BOOLEAN IsWstring, UINT32 SizeOfFinalBuffer)
{
    UINT32  StringSize;
    wchar_t WstrBuffer[50];
    CHAR    AsciiBuffer[sizeof(WstrBuffer) / 2];
    UINT32  StringSizeInByte; /* because of wide-char */
    UINT32  CountOfBlocks;
    UINT32  CopiedBlockLen;

    //
    // First we have to check if string is valid or not
    //
    if (!CheckIfStringIsSafe(Val, IsWstring))
    {
        return FALSE;
    }

    //
    // get the length of the string (format) identifier
    //
    *CurrentProcessedPositionFromStartOfFormat += (UINT32)strlen(CurrentSpecifier);

    //
    // Get string len
    //
    StringSize = CustomStrlen(Val, IsWstring);

    //
    // Check final buffer capacity
    //
    if (*CurrentPositionInFinalBuffer + StringSize > SizeOfFinalBuffer)
    {
        //
        // Over passed buffer
        //
        return TRUE;
    }

    //
    // Move the buffer string into the target buffer
    //
    if (IsWstring)
    {
        //
        // Parse wstring
        //
        StringSizeInByte = StringSize * 2; /* because of wide-char */

        //
        // compute the ceiling
        //
        if (StringSizeInByte % sizeof(WstrBuffer) == 0)
        {
            CountOfBlocks = StringSizeInByte / sizeof(WstrBuffer);
        }
        else
        {
            CountOfBlocks = (StringSizeInByte / sizeof(WstrBuffer)) + 1;
        }

        for (size_t i = 0; i < CountOfBlocks; i++)
        {
            //
            // Zero the buffers
            //
            RtlZeroMemory(WstrBuffer, sizeof(WstrBuffer));
            RtlZeroMemory(AsciiBuffer, sizeof(AsciiBuffer));

            //
            // Check for the last block
            //
            if (i == CountOfBlocks - 1)
            {
                //
                // A portion of block
                //

#ifdef SCRIPT_ENGINE_USER_MODE
                memcpy(WstrBuffer, (void *)(Val + (i * sizeof(WstrBuffer))), StringSizeInByte % sizeof(WstrBuffer));
#endif // SCRIPT_ENGINE_USER_MODE

#ifdef SCRIPT_ENGINE_KERNEL_MODE
                MemoryMapperReadMemorySafeOnTargetProcess(
                    (UINT64)(Val + (i * sizeof(WstrBuffer))),
                    WstrBuffer,
                    StringSizeInByte % sizeof(WstrBuffer));
#endif // SCRIPT_ENGINE_KERNEL_MODE
            }
            else
            {
                //
                // A complete block
                //

#ifdef SCRIPT_ENGINE_USER_MODE
                memcpy(WstrBuffer, (void *)(Val + (i * sizeof(WstrBuffer))), sizeof(WstrBuffer));
#endif // SCRIPT_ENGINE_USER_MODE

#ifdef SCRIPT_ENGINE_KERNEL_MODE
                MemoryMapperReadMemorySafeOnTargetProcess(
                    (UINT64)(Val + (i * sizeof(WstrBuffer))),
                    WstrBuffer,
                    sizeof(WstrBuffer));
#endif // SCRIPT_ENGINE_KERNEL_MODE
            }

            //
            // Here we have the filled WstrBuffer
            // We should convert WstrBuffer to AsciiBuffer
            //
            CopiedBlockLen =
                (UINT32)WcharToChar(WstrBuffer, AsciiBuffer, sizeof(AsciiBuffer) + 1);

            //
            // Now we should move the AsciiBuffer to the target buffer
            // (when we filled AsciiBuffer the memory here is safe so we
            // can use memcpy in both user-mode and vmx-root mode)
            //
            memcpy(&FinalBuffer[*CurrentPositionInFinalBuffer], (void *)AsciiBuffer, CopiedBlockLen + 1);

            *CurrentPositionInFinalBuffer += CopiedBlockLen + 1;
        }
    }
    else
    {
        //
        // Parse string
        //
#ifdef SCRIPT_ENGINE_USER_MODE
        memcpy(&FinalBuffer[*CurrentPositionInFinalBuffer], (void *)Val, StringSize);
#endif // SCRIPT_ENGINE_USER_MODE

#ifdef SCRIPT_ENGINE_KERNEL_MODE
        MemoryMapperReadMemorySafeOnTargetProcess(
            Val,
            &FinalBuffer[*CurrentPositionInFinalBuffer],
            StringSize);
#endif // SCRIPT_ENGINE_KERNEL_MODE

        *CurrentPositionInFinalBuffer += StringSize;
    }

    return TRUE;
}

/**
 * @brief Implementation of printf function
 *
 * @param GuestRegs
 * @param ActionDetail
 * @param ScriptGeneralRegisters
 * @param Tag
 * @param ImmediateMessagePassing
 * @param Format
 * @param ArgCount
 * @param FirstArg
 * @param HasError
 * @return VOID
 */
VOID
ScriptEngineFunctionPrintf(PGUEST_REGS                       GuestRegs,
                           ACTION_BUFFER *                   ActionDetail,
                           SCRIPT_ENGINE_GENERAL_REGISTERS * ScriptGeneralRegisters,
                           UINT64                            Tag,
                           BOOLEAN                           ImmediateMessagePassing,
                           char *                            Format,
                           UINT64                            ArgCount,
                           PSYMBOL                           FirstArg,
                           BOOLEAN *                         HasError)
{
    //
    // *** The printf function ***
    //

    char    FinalBuffer[PacketChunkSize]              = {0};
    UINT32  CurrentPositionInFinalBuffer              = 0;
    UINT32  CurrentProcessedPositionFromStartOfFormat = 0;
    BOOLEAN WithoutAnyFormatSpecifier                 = TRUE;

    UINT64  Val;
    UINT32  Position;
    UINT32  LenOfFormats = (UINT32)strlen(Format) + 1;
    PSYMBOL Symbol;

    *HasError = FALSE;

    for (int i = 0; i < ArgCount; i++)
    {
        WithoutAnyFormatSpecifier = FALSE;
        Symbol                    = FirstArg + i;

        //
        // Address is either wstring (%ws) or string (%s)
        //

        Position = (Symbol->Type >> 32) + 1;

        SYMBOL TempSymbol = {0};
        memcpy(&TempSymbol, Symbol, sizeof(SYMBOL));
        TempSymbol.Type &= 0x7fffffff;

        Val = GetValue(GuestRegs, ActionDetail, ScriptGeneralRegisters, &TempSymbol, FALSE);

        CHAR PercentageChar = Format[Position];

        /*
    printf("position = %d is %c%c \n", Position, PercentageChar,
           IndicatorChar1);
           */

        if (CurrentProcessedPositionFromStartOfFormat != Position)
        {
            //
            // There is some strings before this format specifier
            // we should move it to the buffer
            //
            UINT32 StringLen = Position - CurrentProcessedPositionFromStartOfFormat;

            //
            // Check final buffer capacity
            //
            if (CurrentPositionInFinalBuffer + StringLen < sizeof(FinalBuffer))
            {
                memcpy(&FinalBuffer[CurrentPositionInFinalBuffer],
                       &Format[CurrentProcessedPositionFromStartOfFormat],
                       StringLen);

                CurrentProcessedPositionFromStartOfFormat += StringLen;
                CurrentPositionInFinalBuffer += StringLen;
            }
        }

        //
        // Double check and apply
        //
        if (PercentageChar == '%')
        {
            //
            // Set first character of specifier
            //
            CHAR FormatSpecifier[5] = {0};
            FormatSpecifier[0]      = '%';

            //
            // Read second char
            //
            CHAR IndicatorChar2 = Format[Position + 1];

            //
            // Check if IndicatorChar2 is 2 character long or more
            //
            if (IndicatorChar2 == 'l' || IndicatorChar2 == 'w' ||
                IndicatorChar2 == 'h')
            {
                //
                // Set second char in format specifier
                //
                FormatSpecifier[1] = IndicatorChar2;

                if (IndicatorChar2 == 'l' && Format[Position + 2] == 'l')
                {
                    //
                    // Set third character in format specifier "ll"
                    //
                    FormatSpecifier[2] = 'l';

                    //
                    // Set last character
                    //
                    FormatSpecifier[3] = Format[Position + 3];
                }
                else
                {
                    //
                    // Set last character
                    //
                    FormatSpecifier[2] = Format[Position + 2];
                }
            }
            else
            {
                //
                // It's a one char specifier (Set last character)
                //
                FormatSpecifier[1] = IndicatorChar2;
            }

            //
            // Apply the specifier
            //
            if (!strncmp(FormatSpecifier, "%s", 2))
            {
                //
                // for string
                //
                if (!ApplyStringFormatSpecifier(
                        "%s",
                        FinalBuffer,
                        &CurrentProcessedPositionFromStartOfFormat,
                        &CurrentPositionInFinalBuffer,
                        Val,
                        FALSE,
                        sizeof(FinalBuffer)))
                {
                    *HasError = TRUE;
                    return;
                }
            }
            else if (!strncmp(FormatSpecifier, "%ls", 3) ||
                     !strncmp(FormatSpecifier, "%ws", 3))
            {
                //
                // for wide string (not important if %ls or %ws , only the length is
                // important)
                //
                if (!ApplyStringFormatSpecifier(
                        "%ws",
                        FinalBuffer,
                        &CurrentProcessedPositionFromStartOfFormat,
                        &CurrentPositionInFinalBuffer,
                        Val,
                        TRUE,
                        sizeof(FinalBuffer)))
                {
                    *HasError = TRUE;
                    return;
                }
            }
            else
            {
                ApplyFormatSpecifier(FormatSpecifier, FinalBuffer, &CurrentProcessedPositionFromStartOfFormat, &CurrentPositionInFinalBuffer, Val, sizeof(FinalBuffer));
            }
        }
    }

    if (WithoutAnyFormatSpecifier)
    {
        //
        // Means that it's just a simple print without any format specifier
        //
        if (LenOfFormats < sizeof(FinalBuffer))
        {
            memcpy(FinalBuffer, Format, LenOfFormats);
        }
    }
    else
    {
        //
        // Check if there is anything after the last format specifier
        //
        if (LenOfFormats > CurrentProcessedPositionFromStartOfFormat)
        {
            UINT32 RemainedLen =
                LenOfFormats - CurrentProcessedPositionFromStartOfFormat;

            if (CurrentPositionInFinalBuffer + RemainedLen < sizeof(FinalBuffer))
            {
                memcpy(&FinalBuffer[CurrentPositionInFinalBuffer],
                       &Format[CurrentProcessedPositionFromStartOfFormat],
                       RemainedLen);
            }
        }
    }

//
// Print final result
//
#ifdef SCRIPT_ENGINE_USER_MODE
    printf("%s", FinalBuffer);
#endif // SCRIPT_ENGINE_USER_MODE

#ifdef SCRIPT_ENGINE_KERNEL_MODE

    //
    // Prepare a buffer to bypass allocating a huge stack space for logging
    //
    LogSimpleWithTag((UINT32)Tag, ImmediateMessagePassing, FinalBuffer, (UINT32)strlen(FinalBuffer) + 1);

#endif // SCRIPT_ENGINE_KERNEL_MODE
}

/**
 * @brief Implementation of event_inject function
 *
 * @param InterruptionType
 * @param Vector
 * @param HasError
 * @return VOID
 */
VOID
ScriptEngineFunctionEventInject(UINT32 InterruptionType, UINT32 Vector, BOOL * HasError)
{
    UNREFERENCED_PARAMETER(HasError);

#ifdef SCRIPT_ENGINE_USER_MODE

    ShowMessages("err, event_inject is not supported in user-mode\n");

#endif // SCRIPT_ENGINE_USER_MODE

#ifdef SCRIPT_ENGINE_KERNEL_MODE

    //
    // Validate the arguments
    //
    if (Vector >= 256 || InterruptionType >= 8)
    {
        LogInfo("Err, invalid event vector or interruption type is specified");
        return;
    }
    else
    {
        VmFuncEventInjectInterruption(InterruptionType, Vector, FALSE, 0);
    }

#endif // SCRIPT_ENGINE_KERNEL_MODE
}

/**
 * @brief Implementation of event_inject_error_code function
 *
 * @param InterruptionType
 * @param Vector
 * @param ErrorCode
 * @param HasError
 * @return VOID
 */
VOID
ScriptEngineFunctionEventInjectErrorCode(UINT32 InterruptionType, UINT32 Vector, UINT32 ErrorCode, BOOL * HasError)
{
    UNREFERENCED_PARAMETER(HasError);

#ifdef SCRIPT_ENGINE_USER_MODE

    ShowMessages("err, event_inject is not supported in user-mode\n");

#endif // SCRIPT_ENGINE_USER_MODE

#ifdef SCRIPT_ENGINE_KERNEL_MODE

    //
    // Validate the arguments
    //
    if (Vector >= 256 || InterruptionType >= 8)
    {
        LogInfo("Err, invalid event vector or interruption type is specified");
        return;
    }
    else
    {
        VmFuncEventInjectInterruption(InterruptionType, Vector, TRUE, ErrorCode);
    }

#endif // SCRIPT_ENGINE_KERNEL_MODE
}

/**
 * @brief Implementation of strcmp function
 *
 * @param Address1
 * @param Address2
 *
 * @return UINT64
 */
UINT64
ScriptEngineFunctionStrcmp(const char * Address1, const char * Address2)
{
    UINT64 Result = 0;
#ifdef SCRIPT_ENGINE_USER_MODE
    Result = strcmp(Address1, Address2);
#endif // SCRIPT_ENGINE_USER_MODE

#ifdef SCRIPT_ENGINE_KERNEL_MODE
    Result = VmFuncVmxCompatibleStrcmp(Address1, Address2);
#endif // SCRIPT_ENGINE_KERNEL_MODE

    return Result;
}

/**
 * @brief Implementation of strcmp function
 *
 * @param Address1
 * @param Address2
 * @param Num
 *
 * @return UINT64
 */
UINT64
ScriptEngineFunctionStrncmp(const char * Address1, const char * Address2, size_t Num)
{
    UINT64 Result = 0;
#ifdef SCRIPT_ENGINE_USER_MODE
    Result = strncmp(Address1, Address2, Num);
#endif // SCRIPT_ENGINE_USER_MODE

#ifdef SCRIPT_ENGINE_KERNEL_MODE
    Result = VmFuncVmxCompatibleStrncmp(Address1, Address2, Num);
#endif // SCRIPT_ENGINE_KERNEL_MODE

    return Result;
}

/**
 * @brief Implementation of wcscmp function
 *
 * @param Address1
 * @param Address2
 *
 * @return UINT64
 */
UINT64
ScriptEngineFunctionWcscmp(const wchar_t * Address1, const wchar_t * Address2)
{
    UINT64 Result = 0;
#ifdef SCRIPT_ENGINE_USER_MODE
    Result = wcscmp(Address1, Address2);
#endif // SCRIPT_ENGINE_USER_MODE

#ifdef SCRIPT_ENGINE_KERNEL_MODE
    Result = VmFuncVmxCompatibleWcscmp(Address1, Address2);
#endif // SCRIPT_ENGINE_KERNEL_MODE

    return Result;
}

/**
 * @brief Implementation of wcsncmp function
 *
 * @param Address1
 * @param Address2
 * @param Num
 *
 * @return UINT64
 */
UINT64
ScriptEngineFunctionWcsncmp(const wchar_t * Address1, const wchar_t * Address2, size_t Num)
{
    UINT64 Result = 0;
#ifdef SCRIPT_ENGINE_USER_MODE
    Result = wcsncmp(Address1, Address2, Num);
#endif // SCRIPT_ENGINE_USER_MODE

#ifdef SCRIPT_ENGINE_KERNEL_MODE
    Result = VmFuncVmxCompatibleWcsncmp(Address1, Address2, Num);
#endif // SCRIPT_ENGINE_KERNEL_MODE

    return Result;
}

/**
 * @brief Implementation of memcmp function
 *
 * @param Address1
 * @param Address2
 * @param Count
 * @return UINT64
 */
UINT64
ScriptEngineFunctionMemcmp(const char * Address1, const char * Address2, size_t Count)
{
    UINT64 Result = 0;
#ifdef SCRIPT_ENGINE_USER_MODE
    Result = memcmp(Address1, Address2, Count);
#endif // SCRIPT_ENGINE_USER_MODE

#ifdef SCRIPT_ENGINE_KERNEL_MODE
    Result = VmFuncVmxCompatibleMemcmp(Address1, Address2, Count);
#endif // SCRIPT_ENGINE_KERNEL_MODE

    return Result;
}

/**
 * @brief Implementation of event_trace_instrumentation_step function
 *
 * @return VOID
 */
VOID
ScriptEngineFunctionEventTraceInstrumentationStep()
{
#ifdef SCRIPT_ENGINE_USER_MODE
    ShowMessages("err, it's not possible to call event_trace_instrumentation_step function in the user-mode\n");
#endif // SCRIPT_ENGINE_USER_MODE

#ifdef SCRIPT_ENGINE_KERNEL_MODE

    ULONG CurrentCore = KeGetCurrentProcessorNumberEx(NULL);

    //
    // Call instrumentation step in
    //
    TracingPerformInstrumentationStepIn(&g_DbgState[CurrentCore]);

#endif // SCRIPT_ENGINE_KERNEL_MODE
}

/**
 * @brief Implementation of event_trace_step_in function
 *
 * @return VOID
 */
VOID
ScriptEngineFunctionEventTraceStepIn()
{
#ifdef SCRIPT_ENGINE_USER_MODE
    ShowMessages("err, it's not possible to call event_trace_instrumentation_step function in the user-mode\n");
#endif // SCRIPT_ENGINE_USER_MODE

#ifdef SCRIPT_ENGINE_KERNEL_MODE

    ULONG CurrentCore = KeGetCurrentProcessorNumberEx(NULL);

    //
    // Call instrumentation step in
    //
    TracingPerformRegularStepInInstruction(&g_DbgState[CurrentCore]);

#endif // SCRIPT_ENGINE_KERNEL_MODE
}
