/**
 * @file Msr.h
 * @author Sina Karvandi (sina@hyperdbg.org)
 * @brief Model-Specific Registers definitions
 *
 * @version 0.1
 * @date 2020-04-11
 *
 * @copyright This project is released under the GNU Public License v3.
 *
 */
#pragma once

//////////////////////////////////////////////////
//				    Structures					//
//////////////////////////////////////////////////

/**
 * @brief General MSR Structure
 *
 */
typedef union _MSR
{
    struct
    {
        ULONG Low;
        ULONG High;
    } Fields;

    UINT64 Flags;

} MSR, *PMSR;
