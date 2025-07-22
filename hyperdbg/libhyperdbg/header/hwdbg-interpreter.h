/**
 * @file hwdbg-interpreter.h
 * @author Sina Karvandi (sina@hyperdbg.org)
 * @brief Headers for the interpreter of hwdbg packets and requests
 * @details
 * @version 0.10
 * @date 2024-06-11
 *
 * @copyright This project is released under the GNU Public License v3.
 *
 */
#pragma once

//////////////////////////////////////////////////
//				    Functions                   //
//////////////////////////////////////////////////

VOID
HwdbgShowIntanceInfo(HWDBG_INSTANCE_INFORMATION * InstanceInfo);

BOOLEAN
HwdbgInterpretPacket(PVOID BufferReceived, UINT32 LengthReceived);

BOOLEAN
HwdbgInterpreterFillFileFromMemory(
    HWDBG_INSTANCE_INFORMATION * InstanceInfo,
    const TCHAR *                FileName,
    UINT32 *                     MemoryBuffer,
    size_t                       BufferSize,
    HWDBG_ACTION_ENUMS           RequestedAction);

BOOLEAN
HwdbgInterpreterFillMemoryFromFile(const TCHAR * FileName,
                                   UINT32 *      MemoryBuffer,
                                   size_t        BufferSize);

SIZE_T
HwdbgComputeNumberOfFlipFlopsNeeded(
    HWDBG_INSTANCE_INFORMATION * InstanceInfo,
    UINT32                       NumberOfStages);

BOOLEAN
HwdbgInterpreterSendPacketAndBufferToHwdbg(HWDBG_INSTANCE_INFORMATION * InstanceInfo,
                                           const TCHAR *                FileName,
                                           DEBUGGER_REMOTE_PACKET_TYPE  PacketType,
                                           HWDBG_ACTION_ENUMS           RequestedAction,
                                           CHAR *                       Buffer,
                                           UINT32                       BufferLength);

BOOLEAN
HwdbgReadInstanceInfoFromFile(const TCHAR * FileName, UINT32 * MemoryBuffer, size_t BufferSize);

BOOLEAN
HwdbgWriteTestInstanceInfoRequestIntoFile(HWDBG_INSTANCE_INFORMATION * InstanceInfo,
                                          const CHAR *                 FileName);

BOOLEAN
HwdbgLoadInstanceInfo(const TCHAR * InstanceFilePathToRead, UINT32 InitialBramBufferSize);
