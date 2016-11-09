/* Copyright (c) 1996-2016, OPC Foundation. All rights reserved.

   The source code in this file is covered under a dual-license scenario:
     - RCL: for OPC Foundation members in good-standing
     - GPL V2: everybody else

   RCL license terms accompanied with this source code. See http://opcfoundation.org/License/RCL/1.00/

   GNU General Public License as published by the Free Software Foundation;
   version 2 of the License are accompanied with this source code. See http://opcfoundation.org/License/GPLv2

   This source code is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#ifndef _OpcUa_String_H_
#define _OpcUa_String_H_ 1

#include "sopc_types_wrapper.h"

OPCUA_BEGIN_EXTERN_C

// Use standard functions if necessary
//#define OpcUa_P_String_StrnCpy  OpcUa_ProxyStub_g_PlatformLayerCalltable->StrnCpy
//#define OpcUa_P_String_StrnCat  OpcUa_ProxyStub_g_PlatformLayerCalltable->StrnCat
//#define OpcUa_P_String_StrLen   OpcUa_ProxyStub_g_PlatformLayerCalltable->StrLen
//#define OpcUa_P_String_StrnCmp  OpcUa_ProxyStub_g_PlatformLayerCalltable->StrnCmp
//#define OpcUa_P_String_StrniCmp OpcUa_ProxyStub_g_PlatformLayerCalltable->StrniCmp

/** 
* @brief Used as length for n-operations to work with full length source strings.
*/
#define OPCUA_STRINGLENZEROTERMINATED   0xffffffffL
#define OPCUA_STRING_LENDONTCARE        OPCUA_STRINGLENZEROTERMINATED

OpcUa_String* OpcUa_String_FromCString(OpcUa_StringA strCString);

/** 
* @brief Initializes a string Structure.
*
* @param pString [out] Pointer to the new string.
*
* @return Status code; @see opcua_statuscodes.h
*/
OpcUa_StatusCode OpcUa_String_Initialize( /*  bi */ OpcUa_String* pString);


/** 
 * @brief Allocates memory for a string of uLength bytes length.
 *
 * @param strSource     [in]  The value of the created string.
 * @param uLength       [in]  Length of string to be created. (OPCUA_STRINGLENZEROTERMINATED Use length of source, must be zero terminated)
 * @param uBufferSize   [in] Length of string buffer to be created. (usefull if larger than uLength)
 * @param bDoCopy       [in]  OpcUa_True: Copy the source string; else: reference it.
 * @param bFreeOnClear  [in]  OpcUa_True: Free the Source string if string gets cleared (only effective, if bDoCopy == OpcUa_False!).
 * @param ppNewString   [out] Pointer to the new string.
 *
 * @return Status code; @see opcua_statuscodes.h
 */
OpcUa_StatusCode    OpcUa_String_CreateNewString(   /*  in */ OpcUa_StringA     strSource,
                                                    /*  in */ OpcUa_UInt32      uLength,
                                                    /*  in */ OpcUa_UInt32      uBufferSize,
                                                    /*  in */ OpcUa_Boolean     bDoCopy,
                                                    /*  in */ OpcUa_Boolean     bFreeOnClear,
                                                    /* out */ OpcUa_String**    ppNewString);

/** 
 * @brief Allocates memory for a string of uLength bytes length when a_bDoCopy is true.
 *
 * @param strSource    [in] The value of the created string.
 * @param uLength      [in] Length of string to be created. (OPCUA_STRINGLENZEROTERMINATED Use length of source, must be zero terminated)
 * @param uBufferSize  [in] Length of string buffer to be created. (usefull if larger than uLength)
 * @param bDoCopy      [in] OpcUa_True: Copy the source string; else: reference it (mind the next parameter!).
 * @param bFreeOnClear [in] OpcUa_True: Free the Source string if string gets cleared (only effective, if bDoCopy == OpcUa_False!).
 * @param pString      [bi] Pointer to the manipulated string.
 *
 * @return Status code; @see opcua_statuscodes.h
 */
OpcUa_StatusCode    OpcUa_String_AttachToString(  /*  in */ OpcUa_StringA strSource,
                                                  /*  in */ OpcUa_UInt32  uLength,
                                                  /*  in */ OpcUa_UInt32  uBufferSize,
                                                  /*  in */ OpcUa_Boolean bDoCopy,
                                                  /*  in */ OpcUa_Boolean bFreeOnClear,
                                                  /*  bi */ OpcUa_String* pString);


/** 
 * @brief Deletes all resources occupied by a OpcUa_String including the OpcUa_String itself.
 *
 * @param ppString [bi] Pointer to the OpcUa_String to be deleted. Set to OpcUa_Null after the call.
 */
OpcUa_Void OpcUa_String_Delete(OpcUa_String** ppString);


/** 
 * @brief Frees all internal resources occupied by a OpcUa_String.
 *
 * @param pString [bi] Pointer to the OpcUa_String to be cleared. 
 */
OpcUa_Void OpcUa_String_Clear(OpcUa_String* pString);

/** 
 * @brief Get a direct pointer to the internal UTF-8 string.
 *
 * @param pString [in] Pointer to the OpcUa_String to get the raw content from. 
 */
OpcUa_CharA* OpcUa_String_GetRawString(const OpcUa_String* pString);

/** 
 * @brief Determine if the string is empty (length zero).
 *
 * @param pString [bi] Pointer to the OpcUa_String to be checked. 
 */
OpcUa_Boolean OpcUa_String_IsEmpty(const OpcUa_String* pString);

/** 
 * @brief Determine if the string is null (nonexistant -> initialize OpcUa_String).
 *
 * @param pString [bi] Pointer to the OpcUa_String to be checked. 
 */
OpcUa_Boolean OpcUa_String_IsNull(const OpcUa_String* pString);

/* C-String Conversion */

/** 
 * @brief Copies a string (maximum bytes of the given len).
 *
 * to simulate a strcpy() pass OPCUA_STRING_LENDONTCARE as a_uLength
 *
 * @param pDestString   [out]    The copy of the string (must be OpcUa_String).
 * @param pSrcString    [in]     The string to copy (can also be OpcUa_StringA).
 * @param uLength       [in]     The number of characters to copy.
 */
OpcUa_StatusCode OpcUa_String_StrnCpy(  OpcUa_String*       pDestString, 
                                        const OpcUa_String* pSrcString, 
                                        OpcUa_UInt32        uLength);

/** 
 * @brief Appends a string to another string.
 *
 * to simulate a strcat() pass OpcUa_StringLenDontCare as a_uLength
 * 
 * @param pDestString       [in/out] The string to append to (must be OpcUa_String).
 * @param pSrcString        [in]     The string to append (can also be OpcUa_StringA).
 * @param uLength           [in]     The maximum number of characters to copy.
 */ 
OpcUa_StatusCode OpcUa_String_StrnCat(  OpcUa_String*       pDestString, 
                                        const OpcUa_String* pSrcString, 
                                        OpcUa_UInt32        uLength);

/** 
 * @brief Returns the length in character of the given OpcUa_String.
 *
 * @param pString [in] The string calculate the length of.
 */ 
OpcUa_UInt32 OpcUa_String_StrLen(const OpcUa_String* pString);

/** 
 * @brief Returns the size in bytes of the given OpcUa_String.
 *
 * @param pString [in] The string calculate the length of.
 */ 
//OpcUa_UInt32 OpcUa_String_StrSize(const OpcUa_String* pString);

/** 
 * @brief Compares two strings.
 *
 * @return 0 if the strings are the same. <0 if pString2 is less than pString2. >0 if PString2 is greater than pString2.
 * 
 * @param pString1    [in] The first string for comparison.
 * @param pString2    [in] The second string for comparison.
 * @param uLength     [in] Number of characters to compare.
 * @param bIgnoreCase [in] True if no case-sensitive comparison is needed.
 */ 
OpcUa_Int32 OpcUa_String_StrnCmp(   const OpcUa_String* pString1, 
                                    const OpcUa_String* pString2, 
                                    OpcUa_UInt32        uLength, 
                                    OpcUa_Boolean       bIgnoreCase);

/** The following functions are convencience functions for easy use only and call the above API only. **/

/** 
 * @brief Attaches a readonly reference for a string constant to a string object.
 *
 * @param pDst [out] The string object.
 * @param pSrc [in]  The string being referenced.
 *
 * @return Status code; @see opcua_statuscodes.h
 */
OpcUa_StatusCode OpcUa_String_AttachReadOnly(OpcUa_String* pDst, const OpcUa_StringA pSrc);

/** 
 * @brief Attaches a copy of a string to a string object.
 *
 * The caller must ensure the string object is cleared before the string constant is deallocated.
 * This call is generally used to assign static string constants to string objects.
 *
 * @param pDst [out] The string object.
 * @param pSrc [in]  The string being copied.
 *
 * @return Status code; @see opcua_statuscodes.h
 */
OpcUa_StatusCode OpcUa_String_AttachCopy(   OpcUa_String*       pDst, 
                                            const OpcUa_StringA pSrc);

/** 
 * @brief Attaches a string to a string object.
 *
 * The string must be allocated with OpcUa_Alloc. The string object will free the string.
 *
 * @param pDst [out] The string object.
 * @param pSrc [in]  The string being referenced and freed on clear.
 *
 * @return Status code; @see opcua_statuscodes.h
 */
OpcUa_StatusCode OpcUa_String_AttachWithOwnership(  OpcUa_String* pDst, 
                                                    OpcUa_StringA pSrc);


OPCUA_END_EXTERN_C

#endif /* _OpcUa_String_H_ */

