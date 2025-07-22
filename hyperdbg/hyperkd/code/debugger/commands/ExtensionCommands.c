/**
 * @file ExtensionCommands.c
 * @author Sina Karvandi (sina@hyperdbg.org)
 * @brief Implementation of Debugger Commands (Extensions)
 * @details Debugger Commands that start with "!"
 *
 * @version 0.1
 * @date 2020-04-11
 *
 * @copyright This project is released under the GNU Public License v3.
 *
 */
#include "pch.h"

/**
 * @brief Perform actions regarding APIC
 *
 * @param ApicRequest
 *
 * @return UINT32 Size to send to the debuggee
 */
UINT32
ExtensionCommandPerformActionsForApicRequests(PDEBUGGER_APIC_REQUEST ApicRequest)
{
    BOOLEAN                IsUsingX2APIC       = FALSE;
    PLAPIC_PAGE            BufferToStoreLApic  = (LAPIC_PAGE *)(((CHAR *)ApicRequest) + sizeof(DEBUGGER_APIC_REQUEST));
    PIO_APIC_ENTRY_PACKETS BufferToStoreIoApic = (IO_APIC_ENTRY_PACKETS *)(((CHAR *)ApicRequest) + sizeof(DEBUGGER_APIC_REQUEST));

    if (ApicRequest->ApicType == DEBUGGER_APIC_REQUEST_TYPE_READ_LOCAL_APIC)
    {
        if (VmFuncApicStoreLocalApicFields(BufferToStoreLApic, &IsUsingX2APIC))
        {
            //
            // The status was okay
            //
            ApicRequest->KernelStatus  = DEBUGGER_OPERATION_WAS_SUCCESSFUL;
            ApicRequest->IsUsingX2APIC = IsUsingX2APIC;

            return sizeof(DEBUGGER_APIC_REQUEST) + sizeof(LAPIC_PAGE);
        }
        else
        {
            //
            // There was an error performing the action
            //
            ApicRequest->KernelStatus = DEBUGGER_ERROR_APIC_ACTIONS_ERROR;

            return sizeof(DEBUGGER_APIC_REQUEST);
        }
    }
    else if (ApicRequest->ApicType == DEBUGGER_APIC_REQUEST_TYPE_READ_IO_APIC)
    {
        if (VmFuncApicStoreIoApicFields(BufferToStoreIoApic))
        {
            //
            // The status was okay
            //
            ApicRequest->KernelStatus = DEBUGGER_OPERATION_WAS_SUCCESSFUL;

            return sizeof(DEBUGGER_APIC_REQUEST) + sizeof(IO_APIC_ENTRY_PACKETS);
        }
        else
        {
            //
            // There was an error performing the action
            //
            ApicRequest->KernelStatus = DEBUGGER_ERROR_APIC_ACTIONS_ERROR;

            return sizeof(DEBUGGER_APIC_REQUEST);
        }
    }
    else
    {
        //
        // Invalid request
        //
        ApicRequest->KernelStatus = DEBUGGER_ERROR_APIC_ACTIONS_ERROR;

        return sizeof(DEBUGGER_APIC_REQUEST);
    }
}

/**
 * @brief Perform query for IDT entries
 *
 * @param IdtQueryRequest
 * @param ReadFromVmxRoot
 *
 * @return VOID
 */
VOID
ExtensionCommandPerformQueryIdtEntriesRequest(PINTERRUPT_DESCRIPTOR_TABLE_ENTRIES_PACKETS IdtQueryRequest,
                                              BOOLEAN                                     ReadFromVmxRoot)
{
    //
    // Perform the query
    //
    VmFuncIdtQueryEntries(IdtQueryRequest, ReadFromVmxRoot);

    //
    // Operation was successful
    //
    IdtQueryRequest->KernelStatus = DEBUGGER_OPERATION_WAS_SUCCESSFUL;
}

/**
 * @brief routines for !va2pa and !pa2va commands
 *
 * @param AddressDetails
 * @param OperateOnVmxRoot
 * @return VOID
 */
VOID
ExtensionCommandVa2paAndPa2va(PDEBUGGER_VA2PA_AND_PA2VA_COMMANDS AddressDetails, BOOLEAN OperateOnVmxRoot)
{
    if (OperateOnVmxRoot)
    {
        //
        // *** !va2pa and !pa2va in Debugger Mode
        //
        if (AddressDetails->IsVirtual2Physical)
        {
            AddressDetails->PhysicalAddress = VirtualAddressToPhysicalAddressOnTargetProcess((PVOID)AddressDetails->VirtualAddress);

            //
            // Check if address is valid or invalid
            //
            if (AddressDetails->PhysicalAddress == (UINT64)NULL)
            {
                //
                // Invalid address
                //
                AddressDetails->KernelStatus = DEBUGGER_ERROR_INVALID_ADDRESS;
            }
            else
            {
                //
                // Operation was successful
                //
                AddressDetails->KernelStatus = DEBUGGER_OPERATION_WAS_SUCCESSFUL;
            }
        }
        else
        {
            AddressDetails->VirtualAddress =
                PhysicalAddressToVirtualAddressOnTargetProcess((PVOID)AddressDetails->PhysicalAddress);

            //
            // We don't know a way for checking physical address validity
            //
            AddressDetails->KernelStatus = DEBUGGER_OPERATION_WAS_SUCCESSFUL;
        }
    }
    else
    {
        //
        // *** regular !va2pa and !pa2va in VMI Mode
        //

        if (AddressDetails->ProcessId == HANDLE_TO_UINT32(PsGetCurrentProcessId()))
        {
            //
            // It's on current process address space (we process the request
            // based on system process layout (pid = 4))
            //
            if (AddressDetails->IsVirtual2Physical)
            {
                AddressDetails->PhysicalAddress = VirtualAddressToPhysicalAddress((PVOID)AddressDetails->VirtualAddress);

                //
                // Check if address is valid or invalid
                //
                if (AddressDetails->PhysicalAddress == (UINT64)NULL)
                {
                    //
                    // Invalid address
                    //
                    AddressDetails->KernelStatus = DEBUGGER_ERROR_INVALID_ADDRESS;
                }
                else
                {
                    //
                    // Operation was successful
                    //
                    AddressDetails->KernelStatus = DEBUGGER_OPERATION_WAS_SUCCESSFUL;
                }
            }
            else
            {
                AddressDetails->VirtualAddress = PhysicalAddressToVirtualAddress(AddressDetails->PhysicalAddress);

                //
                // We don't know a way for checking physical address validity
                //
                AddressDetails->KernelStatus = DEBUGGER_OPERATION_WAS_SUCCESSFUL;
            }
        }
        else
        {
            //
            // It's on another process address space
            //

            //
            // Check if pid is valid
            //
            if (!CommonIsProcessExist(AddressDetails->ProcessId))
            {
                //
                // Process id is invalid
                //
                AddressDetails->KernelStatus = DEBUGGER_ERROR_INVALID_PROCESS_ID;
                return;
            }

            if (AddressDetails->IsVirtual2Physical)
            {
                AddressDetails->PhysicalAddress = VirtualAddressToPhysicalAddressByProcessId((PVOID)AddressDetails->VirtualAddress, AddressDetails->ProcessId);

                //
                // Check if address is valid or invalid
                //
                if (AddressDetails->PhysicalAddress == (UINT64)NULL)
                {
                    //
                    // Invalid address
                    //
                    AddressDetails->KernelStatus = DEBUGGER_ERROR_INVALID_ADDRESS;
                }
                else
                {
                    //
                    // Operation was successful
                    //
                    AddressDetails->KernelStatus = DEBUGGER_OPERATION_WAS_SUCCESSFUL;
                }
            }
            else
            {
                AddressDetails->VirtualAddress =
                    PhysicalAddressToVirtualAddressByProcessId((PVOID)AddressDetails->PhysicalAddress,
                                                               AddressDetails->ProcessId);

                //
                // We don't know a way for checking physical address validity
                //
                AddressDetails->KernelStatus = DEBUGGER_OPERATION_WAS_SUCCESSFUL;
            }
        }
    }
}

/**
 * @brief routines for !pte command
 *
 * @param PteDetails
 * @param IsOperatingInVmxRoot
 * @return BOOLEAN
 */
BOOLEAN
ExtensionCommandPte(PDEBUGGER_READ_PAGE_TABLE_ENTRIES_DETAILS PteDetails, BOOLEAN IsOperatingInVmxRoot)
{
    BOOLEAN  Result     = FALSE;
    CR3_TYPE RestoreCr3 = {0};

    //
    // Check for validations
    //
    if (IsOperatingInVmxRoot)
    {
        if (!VirtualAddressToPhysicalAddressOnTargetProcess((PVOID)PteDetails->VirtualAddress))
        {
            //
            // Address is not valid (doesn't have Physical Address)
            //
            PteDetails->KernelStatus = DEBUGGER_ERROR_INVALID_ADDRESS;
            return FALSE;
        }

        //
        // Switch on running process's cr3
        //
        RestoreCr3.Flags = SwitchToCurrentProcessMemoryLayout().Flags;
    }
    else
    {
        if (PteDetails->ProcessId != HANDLE_TO_UINT32(PsGetCurrentProcessId()))
        {
            //
            // It's on another process address space
            //

            //
            // Check if pid is valid
            //
            if (!CommonIsProcessExist(PteDetails->ProcessId))
            {
                //
                // Process id is invalid
                //
                PteDetails->KernelStatus = DEBUGGER_ERROR_INVALID_PROCESS_ID;
                return FALSE;
            }

            //
            // Switch to new process's memory layout
            //
            RestoreCr3.Flags = SwitchToProcessMemoryLayout(PteDetails->ProcessId).Flags;
        }

        //
        // Check if address is valid
        //
        if (!VirtualAddressToPhysicalAddress((PVOID)PteDetails->VirtualAddress))
        {
            //
            // Address is not valid (doesn't have Physical Address)
            //
            PteDetails->KernelStatus = DEBUGGER_ERROR_INVALID_ADDRESS;
            Result                   = FALSE;
            goto RestoreTheState;
        }
    }

    //
    // Read the PML4E
    //
    PPAGE_ENTRY Pml4e = MemoryMapperGetPteVa((PVOID)PteDetails->VirtualAddress, PagingLevelPageMapLevel4);
    if (Pml4e)
    {
        PteDetails->Pml4eVirtualAddress = (UINT64)Pml4e;
        PteDetails->Pml4eValue          = Pml4e->Flags;
    }

    //
    // Read the PDPTE
    //
    PPAGE_ENTRY Pdpte = MemoryMapperGetPteVa((PVOID)PteDetails->VirtualAddress, PagingLevelPageDirectoryPointerTable);
    if (Pdpte)
    {
        PteDetails->PdpteVirtualAddress = (UINT64)Pdpte;
        PteDetails->PdpteValue          = Pdpte->Flags;
    }

    //
    // Read the PDE
    //
    PPAGE_ENTRY Pde = MemoryMapperGetPteVa((PVOID)PteDetails->VirtualAddress, PagingLevelPageDirectory);
    if (Pde)
    {
        PteDetails->PdeVirtualAddress = (UINT64)Pde;
        PteDetails->PdeValue          = Pde->Flags;
    }

    //
    // Read the PTE
    //
    PPAGE_ENTRY Pte = MemoryMapperGetPteVa((PVOID)PteDetails->VirtualAddress, PagingLevelPageTable);
    if (Pte)
    {
        PteDetails->PteVirtualAddress = (UINT64)Pte;
        PteDetails->PteValue          = Pte->Flags;
    }

    //
    // Show that the details we retrieved successfully
    //
    PteDetails->KernelStatus = DEBUGGER_OPERATION_WAS_SUCCESSFUL;
    Result                   = TRUE;

RestoreTheState:

    //
    // Check to restore the current cr3 if it's changed
    //
    if (RestoreCr3.Flags != (UINT64)NULL)
    {
        SwitchToPreviousProcess(RestoreCr3);
    }

    return Result;
}

/**
 * @brief routines for !msrread command which
 * @details causes vm-exit on all msr reads
 * @param BitmapMask Bit mask of msr to put on msr bitmap
 * @return VOID
 */
VOID
ExtensionCommandChangeAllMsrBitmapReadAllCores(UINT64 BitmapMask)
{
    //
    // Broadcast to all cores
    //
    BroadcastChangeAllMsrBitmapReadAllCores(BitmapMask);
}

/**
 * @brief routines for disable (reset) !msrread command
 * @return VOID
 */
VOID
ExtensionCommandResetChangeAllMsrBitmapReadAllCores()
{
    //
    // Broadcast to all cores
    //
    BroadcastResetChangeAllMsrBitmapReadAllCores();
}

/**
 * @brief routines for !msrwrite command which
 * @details causes vm-exit on all msr writes
 * @return VOID
 */
VOID
ExtensionCommandChangeAllMsrBitmapWriteAllCores(UINT64 BitmapMask)
{
    //
    // Broadcast to all cores
    //
    BroadcastChangeAllMsrBitmapWriteAllCores(BitmapMask);
}

/**
 * @brief routines for reset !msrwrite command which
 * @return VOID
 */
VOID
ExtensionCommandResetAllMsrBitmapWriteAllCores()
{
    //
    // Broadcast to all cores
    //
    BroadcastResetAllMsrBitmapWriteAllCores();
}

/**
 * @brief routines for !tsc command
 * @details causes vm-exit on all execution of rdtsc/rdtscp
 * @return VOID
 */
VOID
ExtensionCommandEnableRdtscExitingAllCores()
{
    //
    // Broadcast to all cores
    //
    BroadcastEnableRdtscExitingAllCores();
}

/**
 * @brief routines for disabling rdtsc/p exiting
 * @return VOID
 */
VOID
ExtensionCommandDisableRdtscExitingAllCores()
{
    //
    // Broadcast to all cores
    //
    BroadcastDisableRdtscExitingAllCores();
}

/**
 * @brief routines ONLY for disabling !tsc command
 * @return VOID
 */
VOID
ExtensionCommandDisableRdtscExitingForClearingEventsAllCores()
{
    //
    // Broadcast to all cores
    //
    BroadcastDisableRdtscExitingForClearingEventsAllCores();
}

/**
 * @brief routines ONLY for disabling !crwrite command
 * @param Event
 * @return VOID
 */
VOID
ExtensionCommandDisableMov2ControlRegsExitingForClearingEventsAllCores(PDEBUGGER_EVENT Event)
{
    //
    // Broadcast to all cores
    //
    BroadcastDisableMov2ControlRegsExitingForClearingEventsAllCores(&Event->Options);
}

/**
 * @brief routines ONLY for disabling !dr command
 * @return VOID
 */
VOID
ExtensionCommandDisableMov2DebugRegsExitingForClearingEventsAllCores()
{
    //
    // Broadcast to all cores
    //
    BroadcastDisableMov2DebugRegsExitingForClearingEventsAllCores();
}

/**
 * @brief routines for !pmc
 * @details causes vm-exit on all execution of rdpmc
 * @return VOID
 */
VOID
ExtensionCommandEnableRdpmcExitingAllCores()
{
    //
    // Broadcast to all cores
    //
    BroadcastEnableRdpmcExitingAllCores();
}

/**
 * @brief routines for disabling !pmc
 * @return VOID
 */
VOID
ExtensionCommandDisableRdpmcExitingAllCores()
{
    //
    // Broadcast to all cores
    //
    BroadcastDisableRdpmcExitingAllCores();
}

/**
 * @brief routines for !exception command which
 * @details causes vm-exit when exception occurred
 * @param ExceptionIndex index of exception on IDT
 *
 * @return VOID
 */
VOID
ExtensionCommandSetExceptionBitmapAllCores(UINT64 ExceptionIndex)
{
    //
    // Broadcast to all cores
    //
    BroadcastSetExceptionBitmapAllCores(ExceptionIndex);
}

/**
 * @brief routines for disabling exception bitmap
 * @details removes vm-exit when exception occurred
 * @param ExceptionIndex index of exception on IDT
 *
 * @return VOID
 */
VOID
ExtensionCommandUnsetExceptionBitmapAllCores(UINT64 ExceptionIndex)
{
    //
    // Broadcast to all cores
    //
    BroadcastUnsetExceptionBitmapAllCores(ExceptionIndex);
}

/**
 * @brief routines for reset !exception command
 * @return VOID
 */
VOID
ExtensionCommandResetExceptionBitmapAllCores()
{
    //
    // Broadcast to all cores
    //
    BroadcastResetExceptionBitmapAllCores();
}

/**
 * @brief routines for !crwrite
 * @details causes vm-exit on all accesses to debug registers
 * @param Event
 * @return VOID
 */
VOID
ExtensionCommandEnableMovControlRegisterExitingAllCores(PDEBUGGER_EVENT Event)
{
    //
    // Broadcast to all cores
    //
    BroadcastEnableMovControlRegisterExitingAllCores(&Event->Options);
}

/**
 * @brief routines for disabling !crwrite
 * @param Event
 * @return VOID
 */
VOID
ExtensionCommandDisableMovToControlRegistersExitingAllCores(PDEBUGGER_EVENT Event)
{
    //
    // Broadcast to all cores
    //
    BroadcastDisableMovToControlRegistersExitingAllCores(&Event->Options);
}

/**
 * @brief routines for !dr
 * @details causes vm-exit on all accesses to debug registers
 * @return VOID
 */
VOID
ExtensionCommandEnableMovDebugRegistersExitingAllCores()
{
    //
    // Broadcast to all cores
    //
    BroadcastEnableMovDebugRegistersExitingAllCores();
}

/**
 * @brief routines for disabling !dr
 * @return VOID
 */
VOID
ExtensionCommandDisableMovDebugRegistersExitingAllCores()
{
    //
    // Broadcast to all cores
    //
    BroadcastDisableMovDebugRegistersExitingAllCores();
}

/**
 * @brief routines for !interrupt command which
 * @details causes vm-exit when external interrupt occurs
 * @return VOID
 */
VOID
ExtensionCommandSetExternalInterruptExitingAllCores()
{
    //
    // Broadcast to all cores
    //
    BroadcastSetExternalInterruptExitingAllCores();
}

/**
 * @brief routines for ONLY terminate !interrupt command
 * @return VOID
 */
VOID
ExtensionCommandUnsetExternalInterruptExitingOnlyOnClearingInterruptEventsAllCores()
{
    //
    // Broadcast to all cores
    //
    BroadcastUnsetExternalInterruptExitingOnlyOnClearingInterruptEventsAllCores();
}

/**
 * @brief routines for !ioin and !ioout command which
 * @details causes vm-exit on all i/o instructions or one port
 * @return VOID
 */
VOID
ExtensionCommandIoBitmapChangeAllCores(UINT64 Port)
{
    //
    // Broadcast to all cores
    //
    BroadcastIoBitmapChangeAllCores(Port);
}

/**
 * @brief routines for reset !ioin and !ioout command
 * @return VOID
 */
VOID
ExtensionCommandIoBitmapResetAllCores()
{
    //
    // Broadcast to all cores
    //
    BroadcastIoBitmapResetAllCores();
}

/**
 * @brief routines for PCIe tree
 *
 * @param PcitreePacket
 * @param OperateOnVmxRoot
 *
 * @return VOID
 */
VOID
ExtensionCommandPcitree(PDEBUGGEE_PCITREE_REQUEST_RESPONSE_PACKET PcitreePacket, BOOLEAN OperateOnVmxRoot)
{
    DWORD DeviceIdVendorId = 0xFFFFFFFF;
    DWORD ClassCode        = 0xFFFFFFFF;
    UINT8 DevNum           = 0;

    //
    // We currently don't use OperateOnVmxRoot, but we might in the future
    //
    UNREFERENCED_PARAMETER(OperateOnVmxRoot);

    for (UINT8 b = 0; b < BUS_MAX_NUM; b++)
    {
        for (UINT8 d = 0; d < DEVICE_MAX_NUM; d++)
        {
            for (UINT8 f = 0; f < FUNCTION_MAX_NUM; f++)
            {
                DeviceIdVendorId = (DWORD)PciReadCam(b, d, f, 0, sizeof(DWORD));

                if (DeviceIdVendorId != 0xFFFFFFFF)
                {
                    PcitreePacket->DeviceInfoList[DevNum].Bus                  = b;
                    PcitreePacket->DeviceInfoList[DevNum].Device               = d;
                    PcitreePacket->DeviceInfoList[DevNum].Function             = f;
                    PcitreePacket->DeviceInfoList[DevNum].ConfigSpace.VendorId = (UINT16)(DeviceIdVendorId & 0xFFFF);
                    PcitreePacket->DeviceInfoList[DevNum].ConfigSpace.DeviceId = (UINT16)(DeviceIdVendorId >> 16);

                    ClassCode                                                      = (DWORD)PciReadCam(b, d, f, 0, sizeof(DWORD));
                    PcitreePacket->DeviceInfoList[DevNum].ConfigSpace.ClassCode[0] = (UINT8)((ClassCode >> 24) & 0xFF);
                    PcitreePacket->DeviceInfoList[DevNum].ConfigSpace.ClassCode[1] = (UINT8)((ClassCode >> 16) & 0xFF);
                    PcitreePacket->DeviceInfoList[DevNum].ConfigSpace.ClassCode[2] = (UINT8)((ClassCode >> 8) & 0xFF);

                    DevNum++;
                    if (DevNum == DEV_MAX_NUM)
                    {
                        LogError("Reached maximum number of devices (%u) that can be stored in debuggee response packet.\n", DEV_MAX_NUM);
                        break;
                    }
                }
            }
        }
    }
    PcitreePacket->DeviceInfoListNum = DevNum;

    if (PcitreePacket->DeviceInfoListNum)
    {
        PcitreePacket->KernelStatus = DEBUGGER_OPERATION_WAS_SUCCESSFUL;
    }
    else
    {
        PcitreePacket->KernelStatus = DEBUGGER_ERROR_INVALID_ADDRESS;
    }
}

/**
 * @brief Request PCI device info.
 *
 * @param PcidevinfoPacket
 * @param OperateOnVmxRoot
 *
 * @return VOID
 */
VOID
ExtensionCommandPcidevinfo(PDEBUGGEE_PCIDEVINFO_REQUEST_RESPONSE_PACKET PcidevinfoPacket, BOOLEAN OperateOnVmxRoot)
{
    DWORD DeviceIdVendorId = 0xFFFFFFFF;

    //
    // We currently don't use OperateOnVmxRoot, but we might in the future
    //
    UNREFERENCED_PARAMETER(OperateOnVmxRoot);

    DeviceIdVendorId = (DWORD)PciReadCam(PcidevinfoPacket->DeviceInfo.Bus, PcidevinfoPacket->DeviceInfo.Device, PcidevinfoPacket->DeviceInfo.Function, 0, 4);
    if (DeviceIdVendorId != 0xFFFFFFFF)
    {
        DWORD * cs = (DWORD *)&PcidevinfoPacket->DeviceInfo.ConfigSpace; // Overflows into .ConfigSpaceAdditional - no padding due to pack(0)
        for (UINT16 i = 0; i < CAM_CONFIG_SPACE_LENGTH; i += 4)
        {
            *cs = (DWORD)PciReadCam(PcidevinfoPacket->DeviceInfo.Bus, PcidevinfoPacket->DeviceInfo.Device, PcidevinfoPacket->DeviceInfo.Function, (BYTE)i, 4);
            cs++;
        }

        //
        // For endpoints, determine MMIO BAR addressable range and size (if any).
        // Do not determine BAR size if user has requested raw dump.
        //
        if ((PcidevinfoPacket->DeviceInfo.ConfigSpace.CommonHeader.HeaderType & 0x01) << 7 == 0 // Endpoint
            && !PcidevinfoPacket->PrintRaw)
        {
            for (UINT8 i = 0; i < 5; i++)
            {
                if ((PcidevinfoPacket->DeviceInfo.ConfigSpace.DeviceHeader.ConfigSpaceEp.Bar[i] & 0x1) == 0) // Memory I/O
                {
                    if (((PcidevinfoPacket->DeviceInfo.ConfigSpace.DeviceHeader.ConfigSpaceEp.Bar[i] & 0x6) >> 1) == 2) // 64-bit BAR
                    {
                        UINT64 BarMsb = PcidevinfoPacket->DeviceInfo.ConfigSpace.DeviceHeader.ConfigSpaceEp.Bar[i + 1];
                        UINT64 BarLsb = PcidevinfoPacket->DeviceInfo.ConfigSpace.DeviceHeader.ConfigSpaceEp.Bar[i];
                        UINT64 Bar64  = ((BarMsb & 0xFFFFFFFF) << 32) + (BarLsb & 0xFFFFFFF0);

                        PcidevinfoPacket->DeviceInfo.MmioBarInfo[i].Is64Bit = TRUE;
                        if (Bar64 == 0)
                        {
                            PcidevinfoPacket->DeviceInfo.MmioBarInfo[i].IsEnabled = FALSE;
                            continue;
                        }

                        PcidevinfoPacket->DeviceInfo.MmioBarInfo[i].Is64Bit   = TRUE;
                        PcidevinfoPacket->DeviceInfo.MmioBarInfo[i].IsEnabled = TRUE;

                        i++;
                    }
                    else // 32-bit BAR
                    {
                        UINT32 Bar32 = (PcidevinfoPacket->DeviceInfo.ConfigSpace.DeviceHeader.ConfigSpaceEp.Bar[i] & 0xFFFFFFF0);

                        PcidevinfoPacket->DeviceInfo.MmioBarInfo[i].Is64Bit = FALSE;
                        if (Bar32 == 0)
                        {
                            PcidevinfoPacket->DeviceInfo.MmioBarInfo[i].IsEnabled = FALSE;
                            continue;
                        }

                        PcidevinfoPacket->DeviceInfo.MmioBarInfo[i].Is64Bit   = FALSE;
                        PcidevinfoPacket->DeviceInfo.MmioBarInfo[i].IsEnabled = TRUE;
                    }
                }
            }
        }

        PcidevinfoPacket->KernelStatus = DEBUGGER_OPERATION_WAS_SUCCESSFUL;
    }
    else
    {
        PcidevinfoPacket->DeviceInfo.ConfigSpace.CommonHeader.DeviceId = 0xFFFF;
        PcidevinfoPacket->DeviceInfo.ConfigSpace.CommonHeader.VendorId = 0xFFFF;
        PcidevinfoPacket->KernelStatus                                 = DEBUGGER_ERROR_INVALID_ADDRESS;
    }
}
