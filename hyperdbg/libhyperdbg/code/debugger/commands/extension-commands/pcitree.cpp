/**
 * @file pcitree.cpp
 * @author Bj�rn Ruytenberg (bjorn@bjornweb.nl)
 * @brief !pcitree command
 * @details
 * @version 0.10.3
 * @date 2024-10-31
 *
 * @copyright This project is released under the GNU Public License v3.
 *
 */
#include "pch.h"

//
// Global Variables
//
extern BOOLEAN g_IsSerialConnectedToRemoteDebuggee;

/**
 * @brief help of the !pcitree command
 *
 * @return VOID
 */
VOID
CommandPcitreeHelp()
{
    ShowMessages("!pcitree : enumerates all PCIe endpoints on the debuggee.\n\n");

    ShowMessages("syntax : \t!pcitree\n");

    ShowMessages("\n");
    ShowMessages("\t\te.g : !pcitree\n");
}

/**
 * @brief !pcitree command handler
 *
 * @param CommandTokens
 * @param Command
 *
 * @return VOID
 */
VOID
CommandPcitree(vector<CommandToken> CommandTokens, string Command)
{
    BOOL                                     Status;
    ULONG                                    ReturnedLength;
    DEBUGGEE_PCITREE_REQUEST_RESPONSE_PACKET PcitreePacket = {0};

    if (CommandTokens.size() != 1)
    {
        ShowMessages("incorrect use of the '%s'\n\n",
                     GetCaseSensitiveStringFromCommandToken(CommandTokens.at(0)).c_str());
        CommandPcitreeHelp();
        return;
    }

    //
    // Send buffer
    //
    if (g_IsSerialConnectedToRemoteDebuggee)
    {
        KdSendPcitreePacketToDebuggee(&PcitreePacket);
    }
    else
    {
        AssertShowMessageReturnStmt(g_DeviceHandle, ASSERT_MESSAGE_DRIVER_NOT_LOADED, AssertReturn);

        //
        // Send IOCTL
        //
        Status = DeviceIoControl(
            g_DeviceHandle,                                  // Handle to device
            IOCTL_PCIE_ENDPOINT_ENUM,                        // IO Control Code (IOCTL)
            &PcitreePacket,                                  // Input Buffer to driver.
            SIZEOF_DEBUGGEE_PCITREE_REQUEST_RESPONSE_PACKET, // Input buffer length
            &PcitreePacket,                                  // Output Buffer from driver.
            SIZEOF_DEBUGGEE_PCITREE_REQUEST_RESPONSE_PACKET, // Length of output
                                                             // buffer in bytes.
            &ReturnedLength,                                 // Bytes placed in buffer.
            NULL                                             // synchronous call
        );

        if (!Status)
        {
            ShowMessages("ioctl failed with code 0x%x\n", GetLastError());
            return;
        }

        if (PcitreePacket.KernelStatus == DEBUGGER_OPERATION_WAS_SUCCESSFUL)
        {
            //
            // Print PCI device tree
            //
            ShowMessages("%-12s | %-9s | %-17s | %s \n%s\n", "DBDF", "VID:DID", "Vendor Name", "Device Name", "----------------------------------------------------------------------");
            for (UINT8 i = 0; i < (PcitreePacket.DeviceInfoListNum < DEV_MAX_NUM ? PcitreePacket.DeviceInfoListNum : DEV_MAX_NUM); i++)
            {
                Vendor * CurrentVendor     = GetVendorById(PcitreePacket.DeviceInfoList[i].ConfigSpace.VendorId);
                CHAR *   CurrentVendorName = (CHAR *)"N/A";
                CHAR *   CurrentDeviceName = (CHAR *)"N/A";

                if (CurrentVendor != NULL)
                {
                    CurrentVendorName      = CurrentVendor->VendorName;
                    Device * CurrentDevice = GetDeviceFromVendor(CurrentVendor, PcitreePacket.DeviceInfoList[i].ConfigSpace.DeviceId);

                    if (CurrentDevice != NULL)
                    {
                        CurrentDeviceName = CurrentDevice->DeviceName;
                    }
                }

                ShowMessages("%04x:%02x:%02x:%x | %04x:%04x | %-17.*s | %.*s\n",
                             0, // TODO: Add support for domains beyond 0000
                             PcitreePacket.DeviceInfoList[i].Bus,
                             PcitreePacket.DeviceInfoList[i].Device,
                             PcitreePacket.DeviceInfoList[i].Function,
                             PcitreePacket.DeviceInfoList[i].ConfigSpace.VendorId,
                             PcitreePacket.DeviceInfoList[i].ConfigSpace.DeviceId,
                             strnlen_s(CurrentVendorName, PCI_NAME_STR_LENGTH),
                             CurrentVendorName,
                             strnlen_s(CurrentDeviceName, PCI_NAME_STR_LENGTH),
                             CurrentDeviceName

                );

                FreeVendor(CurrentVendor);
            }
            FreePciIdDatabase();
        }
        else
        {
            //
            // An err occurred, no results
            //
            ShowErrorMessage(PcitreePacket.KernelStatus);
        }
    }
}
