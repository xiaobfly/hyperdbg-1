/**
 * @file hwdbg-scripts.h
 * @author Sina Karvandi (sina@hyperdbg.org)
 * @brief Headers for the hardware scripts for hwdbg
 * @details
 * @version 0.11
 * @date 2024-09-30
 *
 * @copyright This project is released under the GNU Public License v3.
 *
 */
#pragma once

//////////////////////////////////////////////////
//				    Functions                   //
//////////////////////////////////////////////////

VOID
HwdbgScriptPrintScriptBuffer(CHAR * ScriptBuffer, UINT32 ScriptBufferSize);

BOOLEAN
HwdbgScriptCreateHwdbgScript(CHAR *        ScriptBuffer,
                             UINT32        ScriptBufferSize,
                             const TCHAR * HardwareScriptFilePathToSave);

BOOLEAN
HwdbgScriptSendScriptPacket(HWDBG_INSTANCE_INFORMATION * InstanceInfo,
                            const TCHAR *                FileName,
                            UINT32                       NumberOfSymbols,
                            HWDBG_SHORT_SYMBOL *         Buffer,
                            UINT32                       BufferLength);

BOOLEAN
HwdbgScriptGetScriptBufferFromRawString(string   ScriptString,
                                        PVOID *  CodeBuffer,
                                        UINT64 * BufferAddress,
                                        UINT32 * BufferLength,
                                        UINT32 * Pointer);

BOOLEAN
HwdbgScriptRunScript(const CHAR *  Script,
                     const TCHAR * InstanceFilePathToRead,
                     const TCHAR * HardwareScriptFilePathToSave,
                     UINT32        InitialBramBufferSize);
