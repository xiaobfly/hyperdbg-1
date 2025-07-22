/**
 * @file Keywords.c
 * @author M.H. Gholamrezaei (mh@hyperdbg.org)
 * @author Sina Karvandi (sina@hyperdbg.org)
 * @brief Script engine keywords implementations
 * @details
 * @version 0.2
 * @date 2022-06-29
 *
 * @copyright This project is released under the GNU Public License v3.
 *
 */
#include "pch.h"

//
// *** Keywords ***
//

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                             //
//                                          For Virtual Memory                                                 //
//                                                                                                             //
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Implementation of poi keyword
 *
 * @param Address
 * @param HasError
 * @return UINT64
 */
UINT64
ScriptEngineKeywordPoi(PUINT64 Address, BOOL * HasError)
{
    UINT64 Result = (UINT64)NULL;

#ifdef SCRIPT_ENGINE_KERNEL_MODE

    if (!CheckAccessValidityAndSafety((UINT64)Address, sizeof(UINT64)))
    {
        *HasError = TRUE;

        return (UINT64)NULL;
    }

#endif // SCRIPT_ENGINE_KERNEL_MODE

#ifdef SCRIPT_ENGINE_USER_MODE
    Result = *Address;
#endif // SCRIPT_ENGINE_USER_MODE

#ifdef SCRIPT_ENGINE_KERNEL_MODE
    MemoryMapperReadMemorySafeOnTargetProcess((UINT64)Address, &Result, sizeof(UINT64));
#endif // SCRIPT_ENGINE_KERNEL_MODE

    return Result;
}

/**
 * @brief Implementation of hi keyword
 *
 * @param Address
 * @param HasError
 * @return WORD
 */
WORD
ScriptEngineKeywordHi(PUINT64 Address, BOOL * HasError)
{
    QWORD Result = NULL64_ZERO;

#ifdef SCRIPT_ENGINE_KERNEL_MODE

    if (!CheckAccessValidityAndSafety((UINT64)Address, sizeof(UINT64)))
    {
        *HasError = TRUE;

        return NULL64_ZERO;
    }

#endif // SCRIPT_ENGINE_KERNEL_MODE

#ifdef SCRIPT_ENGINE_USER_MODE
    Result = *Address;
#endif // SCRIPT_ENGINE_USER_MODE

#ifdef SCRIPT_ENGINE_KERNEL_MODE
    MemoryMapperReadMemorySafeOnTargetProcess((UINT64)Address, &Result, sizeof(UINT64));
#endif // SCRIPT_ENGINE_KERNEL_MODE

    return HIWORD(Result);
}

/**
 * @brief Implementation of low keyword
 *
 * @param Address
 * @param HasError
 * @return WORD
 */
WORD
ScriptEngineKeywordLow(PUINT64 Address, BOOL * HasError)
{
    QWORD Result = NULL64_ZERO;

#ifdef SCRIPT_ENGINE_KERNEL_MODE

    if (!CheckAccessValidityAndSafety((UINT64)Address, sizeof(UINT64)))
    {
        *HasError = TRUE;

        return NULL64_ZERO;
    }

#endif // SCRIPT_ENGINE_KERNEL_MODE

#ifdef SCRIPT_ENGINE_USER_MODE
    Result = *Address;
#endif // SCRIPT_ENGINE_USER_MODE

#ifdef SCRIPT_ENGINE_KERNEL_MODE
    MemoryMapperReadMemorySafeOnTargetProcess((UINT64)Address, &Result, sizeof(UINT64));
#endif // SCRIPT_ENGINE_KERNEL_MODE

    return LOWORD(Result);
}

/**
 * @brief Implementation of db keyword
 *
 * @param Address
 * @param HasError
 * @return BYTE
 */
BYTE
ScriptEngineKeywordDb(PUINT64 Address, BOOL * HasError)
{
    BYTE Result = NULL_ZERO;

#ifdef SCRIPT_ENGINE_KERNEL_MODE

    if (!CheckAccessValidityAndSafety((UINT64)Address, sizeof(BYTE)))
    {
        *HasError = TRUE;

        return NULL_ZERO;
    }

#endif // SCRIPT_ENGINE_KERNEL_MODE

#ifdef SCRIPT_ENGINE_USER_MODE
    Result = (BYTE)*Address;
#endif // SCRIPT_ENGINE_USER_MODE

#ifdef SCRIPT_ENGINE_KERNEL_MODE
    MemoryMapperReadMemorySafeOnTargetProcess((UINT64)Address, &Result, sizeof(BYTE));
#endif // SCRIPT_ENGINE_KERNEL_MODE

    return Result;
}

/**
 * @brief Implementation of dd keyword
 *
 * @param Address
 * @param HasError
 * @return DWORD
 */
DWORD
ScriptEngineKeywordDd(PUINT64 Address, BOOL * HasError)
{
    DWORD Result = NULL_ZERO;

#ifdef SCRIPT_ENGINE_KERNEL_MODE

    if (!CheckAccessValidityAndSafety((UINT64)Address, sizeof(DWORD)))
    {
        *HasError = TRUE;

        return NULL_ZERO;
    }

#endif // SCRIPT_ENGINE_KERNEL_MODE

#ifdef SCRIPT_ENGINE_USER_MODE
    Result = (DWORD)*Address;
#endif // SCRIPT_ENGINE_USER_MODE

#ifdef SCRIPT_ENGINE_KERNEL_MODE
    MemoryMapperReadMemorySafeOnTargetProcess((UINT64)Address, &Result, sizeof(DWORD));
#endif // SCRIPT_ENGINE_KERNEL_MODE

    return Result;
}

/**
 * @brief Implementation of dw keyword
 *
 * @param Address
 * @param HasError
 * @return WORD
 */
WORD
ScriptEngineKeywordDw(PUINT64 Address, BOOL * HasError)
{
    WORD Result = NULL_ZERO;

#ifdef SCRIPT_ENGINE_KERNEL_MODE

    if (!CheckAccessValidityAndSafety((UINT64)Address, sizeof(WORD)))
    {
        *HasError = TRUE;

        return NULL_ZERO;
    }

#endif // SCRIPT_ENGINE_KERNEL_MODE

#ifdef SCRIPT_ENGINE_USER_MODE
    Result = (WORD)*Address;
#endif // SCRIPT_ENGINE_USER_MODE

#ifdef SCRIPT_ENGINE_KERNEL_MODE
    MemoryMapperReadMemorySafeOnTargetProcess((UINT64)Address, &Result, sizeof(WORD));
#endif // SCRIPT_ENGINE_KERNEL_MODE

    return Result;
}

/**
 * @brief Implementation of dq keyword
 *
 * @param Address
 * @param HasError
 * @return QWORD
 */
QWORD
ScriptEngineKeywordDq(PUINT64 Address, BOOL * HasError)
{
    QWORD Result = (QWORD)NULL;

#ifdef SCRIPT_ENGINE_KERNEL_MODE

    if (!CheckAccessValidityAndSafety((UINT64)Address, sizeof(DWORD)))
    {
        *HasError = TRUE;

        return (QWORD)NULL;
    }

#endif // SCRIPT_ENGINE_KERNEL_MODE

#ifdef SCRIPT_ENGINE_USER_MODE
    Result = *Address;
#endif // SCRIPT_ENGINE_USER_MODE

#ifdef SCRIPT_ENGINE_KERNEL_MODE
    MemoryMapperReadMemorySafeOnTargetProcess((UINT64)Address, &Result, sizeof(QWORD));
#endif // SCRIPT_ENGINE_KERNEL_MODE

    return Result;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                             //
//                                         For Physical Memory                                                 //
//                                                                                                             //
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Implementation of poi_pa keyword
 *
 * @param Address
 * @param HasError
 * @return UINT64
 */
UINT64
ScriptEngineKeywordPoiPa(PUINT64 Address, BOOL * HasError)
{
    UINT64 Result = (UINT64)NULL;

#ifdef SCRIPT_ENGINE_KERNEL_MODE

    if (!CheckAddressPhysical((UINT64)Address))
    {
        *HasError = TRUE;

        return (UINT64)NULL;
    }

#endif // SCRIPT_ENGINE_KERNEL_MODE

#ifdef SCRIPT_ENGINE_USER_MODE

    ShowMessages("err, using physical address keywords (poi_pa) is not possible in user-mode\n");
    return (UINT64)NULL;

#endif // SCRIPT_ENGINE_USER_MODE

#ifdef SCRIPT_ENGINE_KERNEL_MODE
    MemoryMapperReadMemorySafeByPhysicalAddress((UINT64)Address, (UINT64)&Result, sizeof(UINT64));
#endif // SCRIPT_ENGINE_KERNEL_MODE

    return Result;
}

/**
 * @brief Implementation of hi keyword
 *
 * @param Address
 * @param HasError
 * @return WORD
 */
WORD
ScriptEngineKeywordHiPa(PUINT64 Address, BOOL * HasError)
{
    QWORD Result = NULL64_ZERO;

#ifdef SCRIPT_ENGINE_KERNEL_MODE

    if (!CheckAddressPhysical((UINT64)Address))
    {
        *HasError = TRUE;

        return NULL64_ZERO;
    }

#endif // SCRIPT_ENGINE_KERNEL_MODE

#ifdef SCRIPT_ENGINE_USER_MODE

    ShowMessages("err, using physical address keywords (hi_pa) is not possible in user-mode\n");
    return (WORD)NULL;

#endif // SCRIPT_ENGINE_USER_MODE

#ifdef SCRIPT_ENGINE_KERNEL_MODE
    MemoryMapperReadMemorySafeByPhysicalAddress((UINT64)Address, (UINT64)&Result, sizeof(UINT64));
#endif // SCRIPT_ENGINE_KERNEL_MODE

    return HIWORD(Result);
}

/**
 * @brief Implementation of low keyword
 *
 * @param Address
 * @param HasError
 * @return WORD
 */
WORD
ScriptEngineKeywordLowPa(PUINT64 Address, BOOL * HasError)
{
    QWORD Result = NULL64_ZERO;

#ifdef SCRIPT_ENGINE_KERNEL_MODE

    if (!CheckAddressPhysical((UINT64)Address))
    {
        *HasError = TRUE;

        return NULL64_ZERO;
    }

#endif // SCRIPT_ENGINE_KERNEL_MODE

#ifdef SCRIPT_ENGINE_USER_MODE

    ShowMessages("err, using physical address keywords (low_pa) is not possible in user-mode\n");
    return (WORD)NULL;

#endif // SCRIPT_ENGINE_USER_MODE

#ifdef SCRIPT_ENGINE_KERNEL_MODE
    MemoryMapperReadMemorySafeByPhysicalAddress((UINT64)Address, (UINT64)&Result, sizeof(UINT64));
#endif // SCRIPT_ENGINE_KERNEL_MODE

    return LOWORD(Result);
}

/**
 * @brief Implementation of db keyword
 *
 * @param Address
 * @param HasError
 * @return BYTE
 */
BYTE
ScriptEngineKeywordDbPa(PUINT64 Address, BOOL * HasError)
{
    BYTE Result = NULL_ZERO;

#ifdef SCRIPT_ENGINE_KERNEL_MODE

    if (!CheckAddressPhysical((UINT64)Address))
    {
        *HasError = TRUE;

        return NULL_ZERO;
    }

#endif // SCRIPT_ENGINE_KERNEL_MODE

#ifdef SCRIPT_ENGINE_USER_MODE

    ShowMessages("err, using physical address keywords (db_pa) is not possible in user-mode\n");
    return (BYTE)NULL;

#endif // SCRIPT_ENGINE_USER_MODE

#ifdef SCRIPT_ENGINE_KERNEL_MODE
    MemoryMapperReadMemorySafeByPhysicalAddress((UINT64)Address, (UINT64)&Result, sizeof(BYTE));
#endif // SCRIPT_ENGINE_KERNEL_MODE

    return Result;
}

/**
 * @brief Implementation of dd keyword
 *
 * @param Address
 * @param HasError
 * @return DWORD
 */
DWORD
ScriptEngineKeywordDdPa(PUINT64 Address, BOOL * HasError)
{
    DWORD Result = NULL_ZERO;

#ifdef SCRIPT_ENGINE_KERNEL_MODE

    if (!CheckAddressPhysical((UINT64)Address))
    {
        *HasError = TRUE;

        return NULL_ZERO;
    }

#endif // SCRIPT_ENGINE_KERNEL_MODE

#ifdef SCRIPT_ENGINE_USER_MODE

    ShowMessages("err, using physical address keywords (dd_pa) is not possible in user-mode\n");
    return (DWORD)NULL;

#endif // SCRIPT_ENGINE_USER_MODE

#ifdef SCRIPT_ENGINE_KERNEL_MODE
    MemoryMapperReadMemorySafeByPhysicalAddress((UINT64)Address, (UINT64)&Result, sizeof(DWORD));
#endif // SCRIPT_ENGINE_KERNEL_MODE

    return Result;
}

/**
 * @brief Implementation of dw keyword
 *
 * @param Address
 * @param HasError
 * @return WORD
 */
WORD
ScriptEngineKeywordDwPa(PUINT64 Address, BOOL * HasError)
{
    WORD Result = NULL_ZERO;

#ifdef SCRIPT_ENGINE_KERNEL_MODE

    if (!CheckAddressPhysical((UINT64)Address))
    {
        *HasError = TRUE;

        return NULL_ZERO;
    }

#endif // SCRIPT_ENGINE_KERNEL_MODE

#ifdef SCRIPT_ENGINE_USER_MODE

    ShowMessages("err, using physical address keywords (dw_pa) is not possible in user-mode\n");
    return (WORD)NULL;

#endif // SCRIPT_ENGINE_USER_MODE

#ifdef SCRIPT_ENGINE_KERNEL_MODE
    MemoryMapperReadMemorySafeByPhysicalAddress((UINT64)Address, (UINT64)&Result, sizeof(WORD));
#endif // SCRIPT_ENGINE_KERNEL_MODE

    return Result;
}

/**
 * @brief Implementation of dq keyword
 *
 * @param Address
 * @param HasError
 * @return QWORD
 */
QWORD
ScriptEngineKeywordDqPa(PUINT64 Address, BOOL * HasError)
{
    QWORD Result = (QWORD)NULL;

#ifdef SCRIPT_ENGINE_KERNEL_MODE

    if (!CheckAddressPhysical((UINT64)Address))
    {
        *HasError = TRUE;

        return (QWORD)NULL;
    }

#endif // SCRIPT_ENGINE_KERNEL_MODE

#ifdef SCRIPT_ENGINE_USER_MODE

    ShowMessages("err, using physical address keywords (dq_pa) is not possible in user-mode\n");
    return (QWORD)NULL;

#endif // SCRIPT_ENGINE_USER_MODE

#ifdef SCRIPT_ENGINE_KERNEL_MODE
    MemoryMapperReadMemorySafeByPhysicalAddress((UINT64)Address, (UINT64)&Result, sizeof(QWORD));
#endif // SCRIPT_ENGINE_KERNEL_MODE

    return Result;
}
