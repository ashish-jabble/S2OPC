/*
 *  Copyright (C) 2016 Systerel and others.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "sopc_builtintypes.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "base_tools.h"

void SOPC_Boolean_InitializeAux(void* value){
    SOPC_Boolean_Initialize((SOPC_Boolean*) value);
}

void SOPC_Boolean_Initialize(SOPC_Boolean* b){
    if(b != NULL)
        *b = FALSE;
}

SOPC_StatusCode SOPC_Boolean_CopyAux(void* dest, void* src){
    SOPC_StatusCode status = STATUS_INVALID_PARAMETERS;
    if(NULL != dest && NULL != src){
        *((SOPC_Boolean*) dest) = *((SOPC_Boolean*) src);
        status = STATUS_OK;
    }
    return status;
}

void SOPC_Boolean_ClearAux(void* value){
    SOPC_Boolean_Clear((SOPC_Boolean*) value);
}

void SOPC_Boolean_Clear(SOPC_Boolean* b){
    if(b != NULL)
        *b = FALSE;
}


void SOPC_SByte_InitializeAux(void* value){
    SOPC_SByte_Initialize((SOPC_SByte*) value);
}

void SOPC_SByte_Initialize(SOPC_SByte* sbyte){
    if(sbyte != NULL)
        *sbyte = 0;
}

SOPC_StatusCode SOPC_SByte_CopyAux(void* dest, void* src){
    SOPC_StatusCode status = STATUS_INVALID_PARAMETERS;
    if(NULL != dest && NULL != src){
        *((SOPC_SByte*) dest) = *((SOPC_SByte*) src);
        status = STATUS_OK;
    }
    return status;
}

void SOPC_SByte_ClearAux(void* value){
    SOPC_SByte_Clear((SOPC_SByte*) value);
}

void SOPC_SByte_Clear(SOPC_SByte* sbyte){
    if(sbyte != NULL)
        *sbyte = 0;
}


void SOPC_Byte_InitializeAux(void* value){
    SOPC_Byte_Initialize((SOPC_Byte*) value);
}

void SOPC_Byte_Initialize(SOPC_Byte* byte){
    if(byte != NULL)
        *byte = 0;
}

SOPC_StatusCode SOPC_Byte_CopyAux(void* dest, void* src){
    SOPC_StatusCode status = STATUS_INVALID_PARAMETERS;
    if(NULL != dest && NULL != src){
        *((SOPC_Byte*) dest) = *((SOPC_Byte*) src);
        status = STATUS_OK;
    }
    return status;
}

void SOPC_Byte_ClearAux(void* value){
    SOPC_Byte_Clear((SOPC_Byte*) value);
}

void SOPC_Byte_Clear(SOPC_Byte* byte){
    if(byte != NULL)
        *byte = 0;
}


void SOPC_Int16_InitializeAux(void* value){
    SOPC_Int16_Initialize((int16_t*) value);
}

void SOPC_Int16_Initialize(int16_t* intv){
    if(intv != NULL)
        *intv = 0;
}

SOPC_StatusCode SOPC_Int16_CopyAux(void* dest, void* src){
    SOPC_StatusCode status = STATUS_INVALID_PARAMETERS;
    if(NULL != dest && NULL != src){
        *((int16_t*) dest) = *((int16_t*) src);
        status = STATUS_OK;
    }
    return status;
}

void SOPC_Int16_ClearAux(void* value){
    SOPC_Int16_Clear((int16_t*) value);
}

void SOPC_Int16_Clear(int16_t* intv){
    if(intv != NULL)
        *intv = 0;
}


void SOPC_UInt16_InitializeAux(void* value){
    SOPC_UInt16_Initialize((uint16_t*) value);
}

void SOPC_UInt16_Initialize(uint16_t* uint){
    if(uint != NULL)
        *uint = 0;
}

SOPC_StatusCode SOPC_UInt16_CopyAux(void* dest, void* src){
    SOPC_StatusCode status = STATUS_INVALID_PARAMETERS;
    if(NULL != dest && NULL != src){
        *((uint16_t*) dest) = *((uint16_t*) src);
        status = STATUS_OK;
    }
    return status;
}

void SOPC_UInt16_ClearAux(void* value){
    SOPC_UInt16_Clear((uint16_t*) value);
}
void SOPC_UInt16_Clear(uint16_t* uint){
    if(uint != NULL)
        *uint = 0;
}


void SOPC_Int32_InitializeAux(void* value){
    SOPC_Int32_Initialize((int32_t*) value);
}

void SOPC_Int32_Initialize(int32_t* intv){
    if(intv != NULL)
        *intv = 0;
}

SOPC_StatusCode SOPC_Int32_CopyAux(void* dest, void* src){
    SOPC_StatusCode status = STATUS_INVALID_PARAMETERS;
    if(NULL != dest && NULL != src){
        *((int32_t*) dest) = *((int32_t*) src);
        status = STATUS_OK;
    }
    return status;
}

void SOPC_Int32_ClearAux(void* value){
    SOPC_Int32_Clear((int32_t*) value);
}
void SOPC_Int32_Clear(int32_t* intv){
    if(intv != NULL)
        *intv = 0;
}


void SOPC_UInt32_InitializeAux(void* value){
    SOPC_UInt32_Initialize((uint32_t*) value);
}

void SOPC_UInt32_Initialize(uint32_t* uint){
    if(uint != NULL)
        *uint = 0;
}

SOPC_StatusCode SOPC_UInt32_CopyAux(void* dest, void* src){
    SOPC_StatusCode status = STATUS_INVALID_PARAMETERS;
    if(NULL != dest && NULL != src){
        *((uint32_t*) dest) = *((uint32_t*) src);
        status = STATUS_OK;
    }
    return status;
}

void SOPC_UInt32_ClearAux(void* value){
    SOPC_UInt32_Clear((uint32_t*) value);
}
void SOPC_UInt32_Clear(uint32_t* uint){
    if(uint != NULL)
        *uint = 0;
}


void SOPC_Int64_InitializeAux(void* value){
    SOPC_Int64_Initialize((int64_t*) value);
}

void SOPC_Int64_Initialize(int64_t* intv){
    if(intv != NULL)
        *intv = 0;
}

SOPC_StatusCode SOPC_Int64_CopyAux(void* dest, void* src){
    SOPC_StatusCode status = STATUS_INVALID_PARAMETERS;
    if(NULL != dest && NULL != src){
        *((int64_t*) dest) = *((int64_t*) src);
        status = STATUS_OK;
    }
    return status;
}

void SOPC_Int64_ClearAux(void* value){
    SOPC_Int64_Clear((int64_t*) value);
}
void SOPC_Int64_Clear(int64_t* intv){
    if(intv != NULL)
        *intv = 0;
}


void SOPC_UInt64_InitializeAux(void* value){
    SOPC_UInt64_Initialize((uint64_t*) value);
}

void SOPC_UInt64_Initialize(uint64_t* uint){
    if(uint != NULL)
        *uint = 0;
}

SOPC_StatusCode SOPC_UInt64_CopyAux(void* dest, void* src){
    SOPC_StatusCode status = STATUS_INVALID_PARAMETERS;
    if(NULL != dest && NULL != src){
        *((uint64_t*) dest) = *((uint64_t*) src);
        status = STATUS_OK;
    }
    return status;
}

void SOPC_UInt64_ClearAux(void* value){
    SOPC_UInt64_Clear((uint64_t*) value);
}
void SOPC_UInt64_Clear(uint64_t* uint){
    if(uint != NULL)
        *uint = 0;
}


void SOPC_Float_InitializeAux(void* value){
    SOPC_Float_Initialize((float*) value);
}

void SOPC_Float_Initialize(float* f){
    if(f != NULL)
        *f = 0.0;
}

SOPC_StatusCode SOPC_Float_CopyAux(void* dest, void* src){
    SOPC_StatusCode status = STATUS_INVALID_PARAMETERS;
    if(NULL != dest && NULL != src){
        *((float*) dest) = *((float*) src);
        status = STATUS_OK;
    }
    return status;
}

void SOPC_Float_ClearAux(void* value){
    SOPC_Float_Clear((float*) value);
}

void SOPC_Float_Clear(float* f){
    if(f != NULL)
        *f = 0.0;
}


void SOPC_Double_InitializeAux(void* value){
    SOPC_Double_Initialize((double*) value);
}

void SOPC_Double_Initialize(double* d){
    if(d != NULL)
        *d = 0.0;
}

SOPC_StatusCode SOPC_Double_CopyAux(void* dest, void* src){
    SOPC_StatusCode status = STATUS_INVALID_PARAMETERS;
    if(NULL != dest && NULL != src){
        *((double*) dest) = *((double*) src);
        status = STATUS_OK;
    }
    return status;
}

void SOPC_Double_ClearAux(void* value){
    SOPC_Double_Clear((double*) value);
}

void SOPC_Double_Clear(double* d){
    if(d != NULL)
        *d = 0.0;
}


void SOPC_ByteString_InitializeAux(void* value){
    SOPC_ByteString_Initialize((SOPC_ByteString*) value);
}

void SOPC_ByteString_Initialize(SOPC_ByteString* bstring){
    if(bstring != NULL){
        bstring->Length = -1;
        bstring->Data = NULL;
    }
}

SOPC_ByteString* SOPC_ByteString_Create(){
    SOPC_ByteString* bstring = NULL;
    bstring = (SOPC_ByteString*) malloc(sizeof(SOPC_ByteString));
    SOPC_ByteString_Initialize(bstring);
    return bstring;
}

SOPC_StatusCode SOPC_ByteString_InitializeFixedSize(SOPC_ByteString* bstring, uint32_t size){
    SOPC_StatusCode status = STATUS_INVALID_PARAMETERS;
    if(bstring != NULL){
        status = STATUS_OK;
        SOPC_ByteString_Initialize(bstring);
        bstring->Length = size;
        bstring->Data = (SOPC_Byte*) malloc (sizeof(SOPC_Byte)*size);
        if(bstring->Data != NULL){
            memset(bstring->Data, 0, size);
        }else{
            status = STATUS_NOK;
        }
    }
    return status;
}

SOPC_StatusCode SOPC_ByteString_CopyFromBytes(SOPC_ByteString* dest, SOPC_Byte* bytes, int32_t length)
{
    SOPC_StatusCode status = STATUS_INVALID_PARAMETERS;
    if(dest != NULL && bytes != NULL
       && length > 0){
        dest->Length = length;
        dest->Data = malloc(sizeof(SOPC_Byte)*length);
        if(dest->Data != NULL){
            memcpy(dest->Data, bytes, length);
            status = STATUS_OK;
        }
    }
    return status;
}

SOPC_StatusCode SOPC_ByteString_Copy(SOPC_ByteString* dest, const SOPC_ByteString* src){
    SOPC_StatusCode status = STATUS_INVALID_PARAMETERS;
    if(dest != NULL && dest->Data == NULL &&
       src != NULL && src->Length > 0){
        status = STATUS_OK;
        dest->Length = src->Length;
        dest->Data = (SOPC_Byte*) malloc (sizeof(SOPC_Byte)*dest->Length);
        if(dest->Data != NULL){
            // No need of secure copy, both have same size here
            memcpy(dest->Data, src->Data, dest->Length);
        }else{
            status = STATUS_NOK;
        }
    }
    return status;
}

SOPC_StatusCode SOPC_ByteString_CopyAux(void* dest, void* src){
    return SOPC_ByteString_Copy((SOPC_ByteString*) dest, (SOPC_ByteString*) src);
}

void SOPC_ByteString_ClearAux(void* value){
    SOPC_ByteString_Clear((SOPC_ByteString*) value);
}

void SOPC_ByteString_Clear(SOPC_ByteString* bstring){
    if(bstring != NULL){
        if(bstring->Data != NULL){
            free(bstring->Data);
            bstring->Data = NULL;
        }
        SOPC_ByteString_Initialize(bstring);
    }
}

void SOPC_ByteString_Delete(SOPC_ByteString* bstring){
    if(bstring != NULL){
        SOPC_ByteString_Clear(bstring);
        free(bstring);
    }
}


void SOPC_String_InitializeAux(void* value){
    SOPC_String_Initialize((SOPC_String*) value);
}

void SOPC_String_Initialize(SOPC_String* string){
    if(string != NULL){
        string->Length = -1;
        string->Data = NULL;
        string->DoNotClear = FALSE; // False unless characters attached
    }
}

SOPC_String* SOPC_String_Create(){
    SOPC_String* string = NULL;
    string = (SOPC_String*) malloc(sizeof(SOPC_String));
    SOPC_String_Initialize(string);
    return string;
}

SOPC_StatusCode SOPC_String_AttachFrom(SOPC_String* dest, SOPC_String* src){
    SOPC_StatusCode status = STATUS_INVALID_PARAMETERS;
    if(dest != NULL && src != NULL
       && src->Length > 0 && src->Data != NULL){
        status = STATUS_OK;
        dest->Length = src->Length;
        dest->Data = src->Data;
        dest->DoNotClear = 1; // dest->characters will not be freed on clear
    }
    return status;
}

SOPC_StatusCode SOPC_String_AttachFromCstring(SOPC_String* dest, char* src){
    SOPC_StatusCode status = STATUS_INVALID_PARAMETERS;
    size_t stringLength = 0;
    if(dest != NULL && dest->Data == NULL && src != NULL){
        assert(CHAR_BIT == 8);
        stringLength = strlen(src);
        if(stringLength <= INT32_MAX)
        {
            status = STATUS_OK;
            dest->Length = (int32_t) stringLength;
            dest->Data = (uint8_t*) src;
            dest->DoNotClear = 1; // dest->characters will not be freed on clear
        }
    }
    return status;
}

SOPC_StatusCode SOPC_String_Copy(SOPC_String* dest, const SOPC_String* src){
    SOPC_StatusCode status = STATUS_INVALID_PARAMETERS;
    if(dest != NULL && dest->Data == NULL && src != NULL){
        // Keep null terminator for C string compatibility
        status = STATUS_OK;
        dest->Length = src->Length;
        if(dest->Length > 0){
            dest->Data = (SOPC_Byte*) malloc (sizeof(SOPC_Byte)*dest->Length+1);
            if(dest->Data != NULL){
                // No need of secure copy, both have same size here
                memcpy(dest->Data, src->Data, dest->Length);
                dest->Data[dest->Length] = '\0';
                // Since it is a copy, be sure to clear bytes on clear
                dest->DoNotClear = FALSE;
            }else{
                status = STATUS_NOK;
            }
        }
    }
    return status;
}

SOPC_StatusCode SOPC_String_CopyAux(void* dest, void* src){
    return SOPC_String_Copy((SOPC_String*) dest, (SOPC_String*) src);
}

void SOPC_String_ClearAux(void* value){
    SOPC_String_Clear((SOPC_String*) value);
}

void SOPC_String_Clear(SOPC_String* string){
    if(string != NULL){
        if(string->DoNotClear == FALSE){
           if(string->Data != NULL){
               free(string->Data);
           }
        }
        SOPC_String_Initialize(string);
    }
}

void SOPC_String_Delete(SOPC_String* string){
    if(NULL != string){
        SOPC_String_Clear(string);
        free(string);
    }
}

SOPC_StatusCode SOPC_String_CopyFromCString(SOPC_String* string, const char* cString){
    SOPC_StatusCode status = STATUS_INVALID_PARAMETERS;
    size_t stringLength = 0;
    size_t idx = 0;
    if(string != NULL && string->Data == NULL
       && cString != NULL){
        status = STATUS_OK;

        stringLength = strlen(cString);
        if(stringLength > 0 &&
           stringLength <= INT32_MAX)
        {
            // length without null terminator
            string->Length = (int32_t) stringLength;
            // keep terminator for compatibility with char* strings
            string->Data = (SOPC_Byte*) malloc(sizeof(SOPC_Byte)*(stringLength+1));
            if(string->Data != NULL){
                // Keep null terminator for compatibility with c strings !
                if(CHAR_BIT == 8){
                    memcpy(string->Data, cString, stringLength + 1);
                }else{
                    // On systems for which char is not encoded on 1 byte
                    for(idx = 0; idx < stringLength + 1; idx++){
                        string->Data[idx] = (uint8_t) cString[idx];
                    }
                }
            }else{
                status = STATUS_NOK;
            }
        }else{
            status = STATUS_NOK;
        }
    }

    return status;
}

SOPC_StatusCode SOPC_String_InitializeFromCString(SOPC_String* string, const char* cString){
    SOPC_StatusCode status = STATUS_INVALID_PARAMETERS;

    if(string != NULL){
        SOPC_String_Initialize(string);
        status = SOPC_String_CopyFromCString(string, cString);
    }

    return status;
}


char* SOPC_String_GetCString(const SOPC_String* string){
    char* cString = NULL;
    int32_t idx = 0;
    if(string != NULL &&
       string->Length > 0)
    {
        cString = (char*) malloc(sizeof(char)* (string->Length + 1));
        if(cString != NULL){
            if(CHAR_BIT == 8){
                memcpy(cString, string->Data, string->Length + 1);
            }else{
                // On systems for which char is not encoded on 1 byte
                for(idx = 0; idx < string->Length + 1; idx++){
                    cString[idx] = (char) string->Data[idx];
                }
            }
        }
    }
    return cString;
}

const char* SOPC_String_GetRawCString(const SOPC_String* string){
    char* cString = NULL;
    if(string != NULL &&
       string->Length > 0)
    {
        if(CHAR_BIT == 8){
            cString = (char*) string->Data;
            assert(string->Data[string->Length] == '\0');
        }else{
            assert(FALSE);
        }
    }
    return cString;
}

SOPC_StatusCode SOPC_ByteString_Compare(const SOPC_ByteString* left,
                                        const SOPC_ByteString* right,
                                        int32_t*               comparison)
{
    SOPC_StatusCode status = STATUS_INVALID_PARAMETERS;

    if(left != NULL && right != NULL){
        status = STATUS_OK;
    }

    if(status == STATUS_OK){
        if(left->Length == right->Length ||
           (left->Length <= 0 && right->Length <= 0)){
            if(left->Length <= 0 && right->Length <= 0){
                *comparison = 0;
            }else{
                *comparison = memcmp(left->Data, right->Data, left->Length);
            }
        }else if(left->Length > right->Length){

            *comparison = +1;
        }else{
            *comparison = -1;
        }
    }

    return status;
}

uint8_t SOPC_ByteString_Equal(const SOPC_ByteString* left,
                              const SOPC_ByteString* right)
{
    int32_t compare = 0;
    uint8_t result = FALSE;

    if(SOPC_ByteString_Compare(left, right, &compare) == STATUS_OK){
        result = compare == 0;
    }

    return result;
}

SOPC_StatusCode SOPC_String_Compare(const SOPC_String* left,
                                    const SOPC_String* right,
                                    uint8_t            ignoreCase,
                                    int32_t*           comparison)
{
    if(left == NULL || right == NULL || comparison == NULL)
        return STATUS_INVALID_PARAMETERS;

    if(left->Length == right->Length)
    {
        assert(CHAR_BIT == 8);
        if(left->Length <= 0 && right->Length <= 0){
            *comparison = 0;
        }else if(ignoreCase == FALSE){
            *comparison = strcmp((char*) left->Data, (char*) right->Data);
        }else{
            *comparison = strncmp_ignore_case((char*) left->Data, (char*) right->Data, left->Length);
        }
    }else if(left->Length > right->Length){
         *comparison = +1;
    }else{
        *comparison = -1;
    }

    return STATUS_OK;
}

uint32_t SOPC_String_Equal(const SOPC_String* left,
                           const SOPC_String* right)
{
    int32_t compare = 0;
    uint8_t result = FALSE;

    if(SOPC_String_Compare(left, right, FALSE, &compare) == STATUS_OK){
        result = compare == 0;
    }

    return result;
}


void SOPC_XmlElement_InitializeAux(void* value){
    SOPC_XmlElement_Initialize((SOPC_XmlElement*) value);
}

void SOPC_XmlElement_Initialize(SOPC_XmlElement* xmlElt){
    if(xmlElt != NULL){
        SOPC_ByteString_Initialize((SOPC_ByteString*) xmlElt);
    }
}

SOPC_StatusCode SOPC_XmlElement_Copy(SOPC_XmlElement* dest, const SOPC_XmlElement* src){
    return SOPC_ByteString_Copy((SOPC_ByteString*)dest, (SOPC_ByteString*)src);
}

SOPC_StatusCode SOPC_XmlElement_CopyAux(void* dest, void* src){
    return SOPC_XmlElement_Copy((SOPC_XmlElement*) dest, (SOPC_XmlElement*) src);
}

void SOPC_XmlElement_ClearAux(void* value){
    SOPC_XmlElement_Clear((SOPC_XmlElement*) value);
}

void SOPC_XmlElement_Clear(SOPC_XmlElement* xmlElt){
    SOPC_ByteString_Clear((SOPC_ByteString*) xmlElt);
}



void SOPC_DateTime_InitializeAux(void* value){
    SOPC_DateTime_Initialize((SOPC_DateTime*) value);
}


void SOPC_DateTime_Initialize(SOPC_DateTime* dateTime){
    if(dateTime != NULL){
        dateTime->Low32 = 0;
        dateTime->High32 = 0;
    }
}

SOPC_StatusCode SOPC_DateTime_CopyAux(void* dest, void* src){
    SOPC_StatusCode status = STATUS_INVALID_PARAMETERS;
    if(NULL != dest && NULL != src){
        *((SOPC_DateTime*) dest) = *((SOPC_DateTime*) src);
        status = STATUS_OK;
    }
    return status;
}

void SOPC_DateTime_ClearAux(void* value){
    SOPC_DateTime_Clear((SOPC_DateTime*) value);
}

void SOPC_DateTime_Clear(SOPC_DateTime* dateTime){
    SOPC_DateTime_Initialize(dateTime);
}

int64_t SOPC_DateTime_ToInt64(const SOPC_DateTime* dateTime){
    int64_t result = 0;
    uint64_t shiftHigh = 0;
    if(dateTime != NULL){
        shiftHigh = dateTime->High32;
        result = dateTime->Low32 + (shiftHigh << 32);
    }
    return result;
}

void SOPC_DateTime_FromInt64(SOPC_DateTime* dateTime, int64_t date){
    if(dateTime != NULL){
        dateTime->Low32 = date & 0x00000000FFFFFFFF;
        dateTime->High32 = date >> 32;
    }
}


void SOPC_Guid_InitializeAux(void* value){
    SOPC_Guid_Initialize((SOPC_Guid*) value);
}

void SOPC_Guid_Initialize(SOPC_Guid* guid){
    if(guid != NULL){
        memset(guid, 0, sizeof(SOPC_Guid));
    }
}

SOPC_StatusCode SOPC_Guid_Copy(SOPC_Guid* dest, const SOPC_Guid* src){
    SOPC_StatusCode status = STATUS_INVALID_PARAMETERS;
    if(NULL != dest && NULL != src){
        *dest = *src;
        status = STATUS_OK;
    }
    return status;
}

SOPC_StatusCode SOPC_Guid_CopyAux(void* dest, void* src){
    return SOPC_Guid_Copy((SOPC_Guid*) dest, (SOPC_Guid*) src);
}

void SOPC_Guid_ClearAux(void* value){
    SOPC_Guid_Clear((SOPC_Guid*) value);
}

void SOPC_Guid_Clear(SOPC_Guid* guid){
    if(guid != NULL){
        memset(guid, 0, sizeof(SOPC_Guid));
    }
}


void SOPC_NodeId_InitializeAux(void* value){
    SOPC_NodeId_Initialize((SOPC_NodeId*) value);
}

void SOPC_NodeId_Initialize(SOPC_NodeId* nodeId){
    if(nodeId != NULL){
        memset(nodeId, 0, sizeof(SOPC_NodeId));
    }
}

SOPC_StatusCode SOPC_NodeId_Copy(SOPC_NodeId* dest, const SOPC_NodeId* src){
    SOPC_StatusCode status = STATUS_INVALID_PARAMETERS;
    if(NULL != dest && NULL != src){
        dest->Namespace = src->Namespace;
        dest->IdentifierType = src->IdentifierType;
        switch(src->IdentifierType){
        case IdentifierType_Numeric:
            dest->Data.Numeric = src->Data.Numeric;
            status = STATUS_OK;
            break;
        case IdentifierType_String:
            SOPC_String_Initialize(&dest->Data.String);
            status = SOPC_String_Copy(&dest->Data.String, &src->Data.String);
            break;
        case IdentifierType_Guid:
            dest->Data.Guid = malloc(sizeof(SOPC_Guid));
            status = SOPC_Guid_Copy(dest->Data.Guid, src->Data.Guid);
            break;
        case IdentifierType_ByteString:
            SOPC_ByteString_Initialize(&dest->Data.Bstring);
            status = SOPC_ByteString_Copy(&dest->Data.Bstring, &src->Data.Bstring);
            break;
        }
        if(STATUS_OK != status){
            SOPC_NodeId_Clear(dest);
        }
    }
    return status;
}

SOPC_StatusCode SOPC_NodeId_CopyAux(void* dest, void* src){
    return SOPC_NodeId_Copy((SOPC_NodeId*) dest, (SOPC_NodeId*) src);
}

void SOPC_NodeId_ClearAux(void* value){
    SOPC_NodeId_Clear((SOPC_NodeId*) value);
}

void SOPC_NodeId_Clear(SOPC_NodeId* nodeId){
    if(nodeId != NULL){
        nodeId->Namespace = 0; // OPCUA namespace
        switch(nodeId->IdentifierType){
            case IdentifierType_Numeric:
                SOPC_UInt32_Clear(&nodeId->Data.Numeric);
                break;
            case IdentifierType_String:
                SOPC_String_Clear(&nodeId->Data.String);
                break;
            case IdentifierType_Guid:
                SOPC_Guid_Clear(nodeId->Data.Guid);
                if(nodeId->Data.Guid != NULL){
                    free(nodeId->Data.Guid);
                }
                nodeId->Data.Guid = NULL;
                break;
            case IdentifierType_ByteString:
                SOPC_ByteString_Clear(&nodeId->Data.Bstring);
                break;
        }
        nodeId->IdentifierType = IdentifierType_Numeric;
    }
}

SOPC_StatusCode SOPC_NodeId_Compare(const SOPC_NodeId* left,
                                    const SOPC_NodeId* right,
                                    int32_t*           comparison){
    SOPC_StatusCode status = STATUS_INVALID_PARAMETERS;
    if(NULL != left && NULL != right && comparison != NULL){
        switch(left->IdentifierType){
        case IdentifierType_Numeric:
            if(left->Data.Numeric == right->Data.Numeric){
                *comparison = 0;
            }else if(left->Data.Numeric < right->Data.Numeric){
                *comparison = -1;
            }else{
                *comparison = 1;
            }
            status = STATUS_OK;
            break;
        case IdentifierType_String:
            status = SOPC_String_Compare(&left->Data.String, &right->Data.String, FALSE, comparison);
            break;
        case IdentifierType_Guid:
            if(NULL != left->Data.Guid && NULL != right->Data.Guid){
                *comparison = memcmp(left->Data.Guid, right->Data.Guid, sizeof(SOPC_Guid));
                status = STATUS_OK;
            } // else invalid parameters
            break;
        case IdentifierType_ByteString:
            status = SOPC_ByteString_Compare(&left->Data.Bstring, &right->Data.Bstring, comparison);
            break;
        }
    }
    return status;
}

void SOPC_ExpandedNodeId_InitializeAux(void* value){
    SOPC_ExpandedNodeId_Initialize((SOPC_ExpandedNodeId*) value);
}

void SOPC_ExpandedNodeId_Initialize(SOPC_ExpandedNodeId* expNodeId){
    if(expNodeId != NULL){
        SOPC_String_Initialize(&expNodeId->NamespaceUri);
        SOPC_NodeId_Initialize(&expNodeId->NodeId);
        SOPC_UInt32_Initialize(&expNodeId->ServerIndex);
    }
}

SOPC_StatusCode SOPC_ExpandedNodeId_Copy(SOPC_ExpandedNodeId* dest, const SOPC_ExpandedNodeId* src){
    SOPC_StatusCode status = STATUS_INVALID_PARAMETERS;
    if(NULL != dest && NULL != src){
        status = SOPC_NodeId_Copy(&dest->NodeId, &src->NodeId);
        if(STATUS_OK == status){
            status = SOPC_String_Copy(&dest->NamespaceUri, &src->NamespaceUri);
        }
        if(STATUS_OK == status){
            dest->ServerIndex = src->ServerIndex;
        }
        if(STATUS_OK != status){
            SOPC_ExpandedNodeId_Clear(dest);
        }
    }
    return status;
}

SOPC_StatusCode SOPC_ExpandedNodeId_CopyAux(void* dest, void* src){
    return SOPC_ExpandedNodeId_Copy((SOPC_ExpandedNodeId*) dest, (SOPC_ExpandedNodeId*) src);
}

void SOPC_ExpandedNodeId_ClearAux(void* value){
    SOPC_ExpandedNodeId_Clear((SOPC_ExpandedNodeId*) value);
}

void SOPC_ExpandedNodeId_Clear(SOPC_ExpandedNodeId* expNodeId){
    if(expNodeId != NULL){
        SOPC_String_Clear(&expNodeId->NamespaceUri);
        SOPC_NodeId_Clear(&expNodeId->NodeId);
        SOPC_UInt32_Clear(&expNodeId->ServerIndex);
    }
}


void SOPC_StatusCode_InitializeAux(void* value){
    SOPC_StatusCode_Initialize((SOPC_StatusCode*) value);
}

void SOPC_StatusCode_Initialize(SOPC_StatusCode* status){
    if(status != NULL){
        *status = STATUS_OK;
    }
}

SOPC_StatusCode SOPC_StatusCode_CopyAux(void* dest, void* src){
    SOPC_StatusCode status = STATUS_INVALID_PARAMETERS;
    if(NULL != dest && NULL != src){
        *((SOPC_StatusCode*) dest) = *((SOPC_StatusCode*) src);
        status = STATUS_OK;
    }
    return status;
}

void SOPC_StatusCode_ClearAux(void* value){
    SOPC_StatusCode_Clear((SOPC_StatusCode*) value);
}

void SOPC_StatusCode_Clear(SOPC_StatusCode* status){
    if(status != NULL){
        *status = STATUS_OK;
    }
}


void SOPC_DiagnosticInfo_InitializeAux(void* value){
    SOPC_DiagnosticInfo_Initialize((SOPC_DiagnosticInfo*) value);
}

void SOPC_DiagnosticInfo_Initialize(SOPC_DiagnosticInfo* diagInfo){
    if(diagInfo != NULL){
        diagInfo->SymbolicId = -1;
        diagInfo->NamespaceUri = -1;
        diagInfo->Locale = -1;
        diagInfo->LocalizedText = -1;
        SOPC_String_Initialize(&diagInfo->AdditionalInfo);
        diagInfo->InnerStatusCode = STATUS_OK;
        diagInfo->InnerDiagnosticInfo = NULL;
    }
}

SOPC_StatusCode SOPC_DiagnosticInfo_Copy(SOPC_DiagnosticInfo* dest, SOPC_DiagnosticInfo* src){
    SOPC_StatusCode status = STATUS_INVALID_PARAMETERS;
    if(NULL != dest && NULL != src){
        status = SOPC_DiagnosticInfo_Copy(dest->InnerDiagnosticInfo, src->InnerDiagnosticInfo);
        if(STATUS_OK == status){
            status = SOPC_String_Copy(&dest->AdditionalInfo, &src->AdditionalInfo);
        }
        if(STATUS_OK == status){
            dest->Locale = src->Locale;
            dest->LocalizedText = src->LocalizedText;
            dest->NamespaceUri = src->NamespaceUri;
            dest->SymbolicId = src->SymbolicId;
        }
        if(STATUS_OK != status){
            SOPC_DiagnosticInfo_Clear(dest);
        }
    }
    return status;
}

SOPC_StatusCode SOPC_DiagnosticInfo_CopyAux(void* dest, void* src){
    return SOPC_DiagnosticInfo_Copy((SOPC_DiagnosticInfo*) dest, (SOPC_DiagnosticInfo*) src);
}

void SOPC_DiagnosticInfo_ClearAux(void* value){
    SOPC_DiagnosticInfo_Clear((SOPC_DiagnosticInfo*) value);
}

void SOPC_DiagnosticInfo_Clear(SOPC_DiagnosticInfo* diagInfo){
    if(diagInfo != NULL){
        SOPC_String_Clear(&diagInfo->AdditionalInfo);
        if(diagInfo->InnerDiagnosticInfo != NULL){
            SOPC_DiagnosticInfo_Clear(diagInfo->InnerDiagnosticInfo);
            free(diagInfo->InnerDiagnosticInfo);
        }
        diagInfo->SymbolicId = -1;
        diagInfo->NamespaceUri = -1;
        diagInfo->Locale = -1;
        diagInfo->LocalizedText = -1;
        diagInfo->InnerStatusCode = STATUS_OK;
        diagInfo->InnerDiagnosticInfo = NULL;
    }
}



void SOPC_QualifiedName_InitializeAux(void* value){
    SOPC_QualifiedName_Initialize((SOPC_QualifiedName*) value);
}


void SOPC_QualifiedName_Initialize(SOPC_QualifiedName* qname){
    if(qname != NULL){
        qname->NamespaceIndex = 0;
        SOPC_String_Initialize(&qname->Name);
    }
}

SOPC_StatusCode SOPC_QualifiedName_Copy(SOPC_QualifiedName* dest, const SOPC_QualifiedName* src){
    SOPC_StatusCode status = STATUS_INVALID_PARAMETERS;
    if(NULL != dest && NULL != src){
        status = SOPC_String_Copy(&dest->Name, &src->Name);
        if(STATUS_OK == status){
            dest->NamespaceIndex = src->NamespaceIndex;
        }
    }
    return status;
}

SOPC_StatusCode SOPC_QualifiedName_CopyAux(void* dest, void* src){
    return SOPC_QualifiedName_Copy((SOPC_QualifiedName*) dest, (SOPC_QualifiedName*) src);
}

void SOPC_QualifiedName_ClearAux(void* value){
    SOPC_QualifiedName_Clear((SOPC_QualifiedName*) value);
}

void SOPC_QualifiedName_Clear(SOPC_QualifiedName* qname){
    if(qname != NULL){
        qname->NamespaceIndex = 0;
        SOPC_String_Clear(&qname->Name);
    }
}


void SOPC_LocalizedText_InitializeAux(void* value){
    SOPC_LocalizedText_Initialize((SOPC_LocalizedText*) value);
}

void SOPC_LocalizedText_Initialize(SOPC_LocalizedText* localizedText){
    if(localizedText != NULL){
        SOPC_String_Initialize(&localizedText->Locale);
        SOPC_String_Initialize(&localizedText->Text);
    }
}

SOPC_StatusCode SOPC_LocalizedText_Copy(SOPC_LocalizedText* dest, const SOPC_LocalizedText* src){
    SOPC_StatusCode status = STATUS_INVALID_PARAMETERS;
    if(NULL != dest && NULL != src){
        status = SOPC_String_Copy(&dest->Locale, &src->Locale);
        if(STATUS_OK == status){
            status = SOPC_String_Copy(&dest->Text, &src->Text);
        }
        if(STATUS_OK != status){
            SOPC_LocalizedText_Clear(dest);
        }
    }
    return status;
}

SOPC_StatusCode SOPC_LocalizedText_CopyAux(void* dest, void* src){
    return SOPC_LocalizedText_Copy((SOPC_LocalizedText*) dest, (SOPC_LocalizedText*) src);
}

void SOPC_LocalizedText_ClearAux(void* value){
    SOPC_LocalizedText_Clear((SOPC_LocalizedText*) value);
}

void SOPC_LocalizedText_Clear(SOPC_LocalizedText* localizedText){
    if(localizedText != NULL){
        SOPC_String_Clear(&localizedText->Locale);
        SOPC_String_Clear(&localizedText->Text);
    }
}


void SOPC_ExtensionObject_InitializeAux(void* value){
    SOPC_ExtensionObject_Initialize((SOPC_ExtensionObject*) value);
}

void SOPC_ExtensionObject_Initialize(SOPC_ExtensionObject* extObj){
    if(extObj != NULL){
        memset(extObj, 0, sizeof(SOPC_ExtensionObject));
        SOPC_ExpandedNodeId_Initialize(&extObj->TypeId);
        extObj->Length = -1;
    }
}

SOPC_StatusCode SOPC_ExtensionObject_Copy(SOPC_ExtensionObject* dest, const SOPC_ExtensionObject* src){
    SOPC_StatusCode status = STATUS_INVALID_PARAMETERS;
    if(NULL != dest && NULL != src){
        switch(src->Encoding){
        case SOPC_ExtObjBodyEncoding_None:
            break;
        case SOPC_ExtObjBodyEncoding_ByteString:
            status = SOPC_ByteString_Copy(&dest->Body.Bstring, &src->Body.Bstring);
            break;
        case SOPC_ExtObjBodyEncoding_XMLElement:
            status = SOPC_XmlElement_Copy(&dest->Body.Xml, &src->Body.Xml);
            break;
        case SOPC_ExtObjBodyEncoding_Object:
            if(NULL != src->Body.Object.ObjType && NULL != src->Body.Object.Value){
                dest->Body.Object.Value = malloc(src->Body.Object.ObjType->AllocationSize);
                if(NULL == dest->Body.Object.Value){
                    status = STATUS_NOK;
                }else{
                    // TODO: full copy ? need to provide a copy function in encodeable type ?
                    if(dest->Body.Object.Value ==
                        memcpy(dest->Body.Object.Value, src->Body.Object.Value, src->Body.Object.ObjType->AllocationSize)){
                        dest->Body.Object.ObjType = src->Body.Object.ObjType;
                    }else{
                        status = STATUS_NOK;
                    }
                }
            }
        }
        if(STATUS_OK == status){
            SOPC_ExpandedNodeId_Copy(&dest->TypeId, &src->TypeId);
        }
        if(STATUS_OK == status){
            dest->Encoding = src->Encoding;
            dest->Length = src->Length;
        }else{
            SOPC_ExtensionObject_Clear(dest);
        }
    }
    return status;
}

SOPC_StatusCode SOPC_ExtensionObject_CopyAux(void* dest, void* src){
    return SOPC_ExtensionObject_Copy((SOPC_ExtensionObject*) dest, (SOPC_ExtensionObject*) src);
}

void SOPC_ExtensionObject_ClearAux(void* value){
    SOPC_ExtensionObject_Clear((SOPC_ExtensionObject*) value);
}

void SOPC_ExtensionObject_Clear(SOPC_ExtensionObject* extObj){
    if(extObj != NULL){
        SOPC_ExpandedNodeId_Clear(&extObj->TypeId);
        switch(extObj->Encoding){
            case SOPC_ExtObjBodyEncoding_None:
                break;
            case SOPC_ExtObjBodyEncoding_ByteString:
                SOPC_ByteString_Clear(&extObj->Body.Bstring);
                break;
            case SOPC_ExtObjBodyEncoding_XMLElement:
                SOPC_XmlElement_Clear(&extObj->Body.Xml);
                break;
            case SOPC_ExtObjBodyEncoding_Object:
                extObj->Body.Object.ObjType->Clear(extObj->Body.Object.Value);
                free(extObj->Body.Object.Value);
                extObj->Body.Object.Value = NULL;
                break;
        }
        extObj->Length = -1;
    }
}

void ApplyToVariantNonArrayBuiltInType(SOPC_BuiltinId                    builtInTypeId,
                                       SOPC_VariantValue*                val,
                                       SOPC_EncodeableObject_PfnClear*   clearAuxFunction){
    switch(builtInTypeId){
        case SOPC_Null_Id:
            break;
        case SOPC_Boolean_Id:
            clearAuxFunction((void*)&val->Boolean);
            break;
        case SOPC_SByte_Id:
            clearAuxFunction((void*)&val->Sbyte);
            break;
        case SOPC_Byte_Id:
            clearAuxFunction((void*)&val->Byte);
            break;
        case SOPC_Int16_Id:
            clearAuxFunction((void*)&val->Int16);
            break;
        case SOPC_UInt16_Id:
            clearAuxFunction((void*)&val->Uint16);
            break;
        case SOPC_Int32_Id:
            clearAuxFunction((void*)&val->Int32);
            break;
        case SOPC_UInt32_Id:
            clearAuxFunction((void*)&val->Uint32);
            break;
        case SOPC_Int64_Id:
            clearAuxFunction((void*)&val->Int64);
            break;
        case SOPC_UInt64_Id:
            clearAuxFunction((void*)&val->Uint64);
            break;
        case SOPC_Float_Id:
            clearAuxFunction((void*)&val->Floatv);
            break;
        case SOPC_Double_Id:
            clearAuxFunction((void*)&val->Doublev);
            break;
        case SOPC_String_Id:
            clearAuxFunction((void*)&val->String);
            break;
        case SOPC_DateTime_Id:
            clearAuxFunction((void*)&val->Date);
            break;
        case SOPC_Guid_Id:
            clearAuxFunction((void*)val->Guid);
            break;
        case SOPC_ByteString_Id:
            clearAuxFunction((void*)&val->Bstring);
            break;
        case SOPC_XmlElement_Id:
            clearAuxFunction((void*)&val->XmlElt);
            break;
        case SOPC_NodeId_Id:
            clearAuxFunction((void*)val->NodeId);
            break;
        case SOPC_ExpandedNodeId_Id:
            clearAuxFunction((void*)val->ExpNodeId);
            break;
        case SOPC_StatusCode_Id:
            clearAuxFunction((void*)&val->Status);
            break;
        case SOPC_QualifiedName_Id:
            clearAuxFunction((void*)val->Qname);
            break;
        case SOPC_LocalizedText_Id:
            clearAuxFunction((void*)val->LocalizedText);
            break;
        case SOPC_ExtensionObject_Id:
            clearAuxFunction((void*)val->ExtObject);
            break;
        case SOPC_DataValue_Id:
            clearAuxFunction((void*)val->DataValue);
            break;
        case SOPC_DiagnosticInfo_Id:
            clearAuxFunction((void*)val->DiagInfo);
            break;
        case SOPC_Variant_Id:
            // Part 6 Table 14 (v1.03): "The value shall not be a Variant
            //                           but it could be an array of Variants."
            //Note: Variant is not encoded in INGOPCS stack for this case
            break;
        default:
            break;
    }
}

SOPC_StatusCode AllocVariantArrayBuiltInType(SOPC_BuiltinId          builtInTypeId,
                                             SOPC_VariantArrayValue* array,
                                             int32_t                 length)
{
    SOPC_StatusCode status = STATUS_NOK;
    switch(builtInTypeId){
        case SOPC_Null_Id:
            // mantis #0003682: errata for 1.03 but not confirmed NULL array forbidden
            break; // STATUS_NOK since a NULL must not be an array
        case SOPC_Boolean_Id:
            array->BooleanArr = calloc(length, sizeof(SOPC_Boolean));
            if(NULL != array->BooleanArr) return STATUS_OK;
            break;
        case SOPC_SByte_Id:
            array->SbyteArr = calloc(length, sizeof(SOPC_SByte));
            if(NULL != array->SbyteArr) return STATUS_OK;
            break;
        case SOPC_Byte_Id:
            array->ByteArr = calloc(length, sizeof(SOPC_Byte));
            if(NULL != array->ByteArr) return STATUS_OK;
            break;
        case SOPC_Int16_Id:
            array->Int16Arr = calloc(length, sizeof(int16_t));
            if(NULL != array->Int16Arr) return STATUS_OK;
            break;
        case SOPC_UInt16_Id:
            array->Uint16Arr = calloc(length, sizeof(uint16_t));
            if(NULL != array->Uint16Arr) return STATUS_OK;
            break;
        case SOPC_Int32_Id:
            array->Int32Arr = calloc(length, sizeof(int32_t));
            if(NULL != array->Int32Arr) return STATUS_OK;
            break;
        case SOPC_UInt32_Id:
            array->Uint32Arr = calloc(length, sizeof(uint32_t));
            if(NULL != array->Uint32Arr) return STATUS_OK;
            break;
        case SOPC_Int64_Id:
            array->Int64Arr = calloc(length, sizeof(int64_t));
            if(NULL != array->Int64Arr) return STATUS_OK;
            break;
        case SOPC_UInt64_Id:
            array->Uint64Arr = calloc(length, sizeof(uint64_t));
            if(NULL != array->Uint64Arr) return STATUS_OK;
            break;
        case SOPC_Float_Id:
            array->FloatvArr = calloc(length, sizeof(float));
            if(NULL != array->FloatvArr) return STATUS_OK;
            break;
        case SOPC_Double_Id:
            array->DoublevArr = calloc(length, sizeof(double));
            if(NULL != array->DoublevArr) return STATUS_OK;
            break;
        case SOPC_String_Id:
            array->StringArr = calloc(length, sizeof(SOPC_String));
            if(NULL != array->StringArr) return STATUS_OK;
            break;
        case SOPC_DateTime_Id:
            array->DateArr = calloc(length, sizeof(SOPC_DateTime));
            if(NULL != array->DateArr) return STATUS_OK;
            break;
        case SOPC_Guid_Id:
            array->GuidArr = calloc(length, sizeof(SOPC_Guid));
            if(NULL != array->GuidArr) return STATUS_OK;
            break;
        case SOPC_ByteString_Id:
            array->BstringArr = calloc(length, sizeof(SOPC_ByteString));
            if(NULL != array->BstringArr) return STATUS_OK;
            break;
        case SOPC_XmlElement_Id:
            array->XmlEltArr = calloc(length, sizeof(SOPC_XmlElement));
            if(NULL != array->XmlEltArr) return STATUS_OK;
            break;
        case SOPC_NodeId_Id:
            array->NodeIdArr = calloc(length, sizeof(SOPC_NodeId));
            if(NULL != array->NodeIdArr) return STATUS_OK;
            break;
        case SOPC_ExpandedNodeId_Id:
            array->ExpNodeIdArr = calloc(length, sizeof(SOPC_ExpandedNodeId));
            if(NULL != array->ExpNodeIdArr) return STATUS_OK;
            break;
        case SOPC_StatusCode_Id:
            array->StatusArr = calloc(length, sizeof(SOPC_StatusCode));
            if(NULL != array->StatusArr) return STATUS_OK;
            break;
        case SOPC_QualifiedName_Id:
            array->QnameArr = calloc(length, sizeof(SOPC_QualifiedName));
            if(NULL != array->QnameArr) return STATUS_OK;
            break;
        case SOPC_LocalizedText_Id:
            array->LocalizedTextArr = calloc(length, sizeof(SOPC_LocalizedText));
            if(NULL != array->LocalizedTextArr) return STATUS_OK;
            break;
        case SOPC_ExtensionObject_Id:
            array->ExtObjectArr = calloc(length, sizeof(SOPC_ExtensionObject));
            if(NULL != array->ExtObjectArr) return STATUS_OK;
            break;
        case SOPC_DataValue_Id:
            array->DataValueArr = calloc(length, sizeof(SOPC_DataValue));
            if(NULL != array->DataValueArr) return STATUS_OK;
            break;
        case SOPC_Variant_Id:
            array->VariantArr = calloc(length, sizeof(SOPC_Variant));
            if(NULL != array->VariantArr) return STATUS_OK;
            break;
        case SOPC_DiagnosticInfo_Id:
            array->DiagInfoArr = calloc(length, sizeof(SOPC_DiagnosticInfo));
            if(NULL != array->DiagInfoArr) return STATUS_OK;
            break;
        default:
            break;
    }
    return status;
}

void ClearToVariantArrayBuiltInType(SOPC_BuiltinId                  builtInTypeId,
                                    SOPC_VariantArrayValue*         array,
                                    int32_t*                        length,
                                    SOPC_EncodeableObject_PfnClear* clearAuxFunction){
    switch(builtInTypeId){
        case SOPC_Null_Id:
            // mantis #0003682: errata for 1.03 but not confirmed NULL array forbidden
            break;
        case SOPC_Boolean_Id:
            SOPC_Clear_Array(length, (void**) &array->BooleanArr, sizeof(SOPC_Boolean), clearAuxFunction);
            break;
        case SOPC_SByte_Id:
            SOPC_Clear_Array(length, (void**) &array->SbyteArr, sizeof(SOPC_SByte), clearAuxFunction);
            break;
        case SOPC_Byte_Id:
            SOPC_Clear_Array(length, (void**) &array->ByteArr, sizeof(SOPC_Byte), clearAuxFunction);
            break;
        case SOPC_Int16_Id:
            SOPC_Clear_Array(length, (void**) &array->Int16Arr, sizeof(int16_t), clearAuxFunction);
            break;
        case SOPC_UInt16_Id:
            SOPC_Clear_Array(length, (void**) &array->Uint16Arr, sizeof(uint16_t), clearAuxFunction);
            break;
        case SOPC_Int32_Id:
            SOPC_Clear_Array(length, (void**) &array->Int32Arr, sizeof(int32_t), clearAuxFunction);
            break;
        case SOPC_UInt32_Id:
            SOPC_Clear_Array(length, (void**) &array->Uint32Arr, sizeof(uint32_t), clearAuxFunction);
            break;
        case SOPC_Int64_Id:
            SOPC_Clear_Array(length, (void**) &array->Int64Arr, sizeof(int64_t), clearAuxFunction);
            break;
        case SOPC_UInt64_Id:
            SOPC_Clear_Array(length, (void**) &array->Uint64Arr, sizeof(uint64_t), clearAuxFunction);
            break;
        case SOPC_Float_Id:
            SOPC_Clear_Array(length, (void**) &array->FloatvArr, sizeof(float), clearAuxFunction);
            break;
        case SOPC_Double_Id:
            SOPC_Clear_Array(length, (void**) &array->DoublevArr, sizeof(double), clearAuxFunction);
            break;
        case SOPC_String_Id:
            SOPC_Clear_Array(length, (void**) &array->StringArr, sizeof(SOPC_String), clearAuxFunction);
            break;
        case SOPC_DateTime_Id:
            SOPC_Clear_Array(length, (void**) &array->DateArr, sizeof(SOPC_DateTime), clearAuxFunction);
            break;
        case SOPC_Guid_Id:
            SOPC_Clear_Array(length, (void**) &array->GuidArr, sizeof(SOPC_Guid), clearAuxFunction);
            break;
        case SOPC_ByteString_Id:
            SOPC_Clear_Array(length, (void**) &array->BstringArr, sizeof(SOPC_ByteString), clearAuxFunction);
            break;
        case SOPC_XmlElement_Id:
            SOPC_Clear_Array(length, (void**) &array->XmlEltArr, sizeof(SOPC_XmlElement), clearAuxFunction);
            break;
        case SOPC_NodeId_Id:
            SOPC_Clear_Array(length, (void**) &array->NodeIdArr, sizeof(SOPC_NodeId), clearAuxFunction);
            break;
        case SOPC_ExpandedNodeId_Id:
            SOPC_Clear_Array(length, (void**) &array->ExpNodeIdArr, sizeof(SOPC_ExpandedNodeId), clearAuxFunction);
            break;
        case SOPC_StatusCode_Id:
            SOPC_Clear_Array(length, (void**) &array->StatusArr, sizeof(SOPC_StatusCode), clearAuxFunction);
            break;
        case SOPC_QualifiedName_Id:
            SOPC_Clear_Array(length, (void**) &array->QnameArr, sizeof(SOPC_QualifiedName), clearAuxFunction);
            break;
        case SOPC_LocalizedText_Id:
            SOPC_Clear_Array(length, (void**) &array->LocalizedTextArr, sizeof(SOPC_LocalizedText), clearAuxFunction);
            break;
        case SOPC_ExtensionObject_Id:
            SOPC_Clear_Array(length, (void**) &array->ExtObjectArr, sizeof(SOPC_ExtensionObject), clearAuxFunction);
            break;
        case SOPC_DataValue_Id:
            SOPC_Clear_Array(length, (void**) &array->DataValueArr, sizeof(SOPC_DataValue), clearAuxFunction);
            break;
        case SOPC_Variant_Id:
            SOPC_Clear_Array(length, (void**) &array->VariantArr, sizeof(SOPC_Variant), clearAuxFunction);
            break;
        case SOPC_DiagnosticInfo_Id:
            SOPC_Clear_Array(length, (void**) &array->DiagInfoArr, sizeof(SOPC_DiagnosticInfo), clearAuxFunction);
            break;
        default:
            break;
    }
}

SOPC_StatusCode ApplyOpToVariantNonArrayBuiltInType(SOPC_BuiltinId               builtInTypeId,
                                                    SOPC_VariantValue*           left,
                                                    const SOPC_VariantValue*     right,
                                                    SOPC_EncodeableObject_PfnOp* opAuxFunction){
    SOPC_StatusCode status = STATUS_OK;
    switch(builtInTypeId){
        case SOPC_Null_Id:
            break; // STATUS_NOK since no operation could be applied to a NULL variant
        case SOPC_Boolean_Id:
            status = opAuxFunction((void*) &left->Boolean, (void*) &right->Boolean);
            break;
        case SOPC_SByte_Id:
            status = opAuxFunction((void*) &left->Sbyte, (void*) &right->Sbyte);
            break;
        case SOPC_Byte_Id:
            status = opAuxFunction((void*) &left->Byte, (void*) &right->Byte);
            break;
        case SOPC_Int16_Id:
            status = opAuxFunction((void*) &left->Int16, (void*) &right->Int16);
            break;
        case SOPC_UInt16_Id:
            status = opAuxFunction((void*) &left->Uint16, (void*) &right->Uint16);
            break;
        case SOPC_Int32_Id:
            status = opAuxFunction((void*) &left->Int32, (void*) &right->Int32);
            break;
        case SOPC_UInt32_Id:
            status = opAuxFunction((void*) &left->Uint32, (void*) &right->Uint32);
            break;
        case SOPC_Int64_Id:
            status = opAuxFunction((void*) &left->Int64, (void*) &right->Int64);
            break;
        case SOPC_UInt64_Id:
            status = opAuxFunction((void*) &left->Uint64, (void*) &right->Uint64);
            break;
        case SOPC_Float_Id:
            status = opAuxFunction((void*) &left->Floatv, (void*) &right->Floatv);
            break;
        case SOPC_Double_Id:
            status = opAuxFunction((void*) &left->Doublev, (void*) &right->Doublev);
            break;
        case SOPC_String_Id:
            status = opAuxFunction((void*) &left->String, (void*) &right->String);
            break;
        case SOPC_DateTime_Id:
            status = opAuxFunction((void*) &left->Date, (void*) &right->Date);
            break;
        case SOPC_Guid_Id:
            status = opAuxFunction((void*) left->Guid, (void*) right->Guid);
            break;
        case SOPC_ByteString_Id:
            status = opAuxFunction((void*) &left->Bstring, (void*) &right->Bstring);
            break;
        case SOPC_XmlElement_Id:
            status = opAuxFunction((void*) &left->XmlElt, (void*) &right->XmlElt);
            break;
        case SOPC_NodeId_Id:
            status = opAuxFunction((void*) left->NodeId, (void*) right->NodeId);
            break;
        case SOPC_ExpandedNodeId_Id:
            status = opAuxFunction((void*) left->ExpNodeId, (void*) right->ExpNodeId);
            break;
        case SOPC_StatusCode_Id:
            status = opAuxFunction((void*) &left->Status, (void*) &right->Status);
            break;
        case SOPC_QualifiedName_Id:
            status = opAuxFunction((void*) left->Qname, (void*) right->Qname);
            break;
        case SOPC_LocalizedText_Id:
            status = opAuxFunction((void*) left->LocalizedText, (void*) right->LocalizedText);
            break;
        case SOPC_ExtensionObject_Id:
            status = opAuxFunction((void*) left->ExtObject, (void*) right->ExtObject);
            break;
        case SOPC_DataValue_Id:
            status = opAuxFunction((void*) left->DataValue, (void*) right->DataValue);
            break;
        case SOPC_DiagnosticInfo_Id:
            status = opAuxFunction((void*) left->DiagInfo, (void*) right->DiagInfo);
            break;
        case SOPC_Variant_Id:
            // Part 6 Table 14 (v1.03): "The value shall not be a Variant
            //                           but it could be an array of Variants."
            //Note: Variant is not encoded in INGOPCS stack for this case
            break;
        default:
            break;
    }
    return status;
}

SOPC_StatusCode ApplyOpToVariantArrayBuiltInType(SOPC_BuiltinId                builtInTypeId,
                                                 SOPC_VariantArrayValue*       arrayLeft,
                                                 const SOPC_VariantArrayValue* arrayRight,
                                                 int32_t                       length,
                                                 SOPC_EncodeableObject_PfnOp*  opAuxFunction){
    SOPC_StatusCode status = STATUS_NOK;
    switch(builtInTypeId){
        case SOPC_Null_Id:
            // mantis #0003682: errata for 1.03 but not confirmed NULL array forbidden
            break; // STATUS_NOK since a NULL must not be an array
        case SOPC_Boolean_Id:
            return SOPC_Op_Array(length, (void *) arrayLeft->BooleanArr, (void *) arrayRight->BooleanArr, sizeof(SOPC_Boolean), opAuxFunction);
        case SOPC_SByte_Id:
            return SOPC_Op_Array(length, (void *) arrayLeft->SbyteArr, (void *) arrayRight->SbyteArr, sizeof(SOPC_SByte), opAuxFunction);
        case SOPC_Byte_Id:
            return SOPC_Op_Array(length, (void *) arrayLeft->ByteArr, (void *) arrayRight->ByteArr, sizeof(SOPC_Byte), opAuxFunction);
        case SOPC_Int16_Id:
            return SOPC_Op_Array(length, (void *) arrayLeft->Int16Arr, (void *) arrayRight->Int16Arr, sizeof(int16_t), opAuxFunction);
        case SOPC_UInt16_Id:
            return SOPC_Op_Array(length, (void *) arrayLeft->Uint16Arr, (void *) arrayRight->Uint16Arr, sizeof(uint16_t), opAuxFunction);
        case SOPC_Int32_Id:
            return SOPC_Op_Array(length, (void *) arrayLeft->Int32Arr, (void *) arrayRight->Int32Arr, sizeof(int32_t), opAuxFunction);
        case SOPC_UInt32_Id:
            return SOPC_Op_Array(length, (void *) arrayLeft->Uint32Arr, (void *) arrayRight->Uint32Arr, sizeof(uint32_t), opAuxFunction);
        case SOPC_Int64_Id:
            return SOPC_Op_Array(length, (void *) arrayLeft->Int64Arr, (void *) arrayRight->Int64Arr, sizeof(int64_t), opAuxFunction);
        case SOPC_UInt64_Id:
            return SOPC_Op_Array(length, (void *) arrayLeft->Uint64Arr, (void *) arrayRight->Uint64Arr, sizeof(uint64_t), opAuxFunction);
        case SOPC_Float_Id:
            return SOPC_Op_Array(length, (void *) arrayLeft->FloatvArr, (void *) arrayRight->FloatvArr, sizeof(float), opAuxFunction);
        case SOPC_Double_Id:
            return SOPC_Op_Array(length, (void *) arrayLeft->DoublevArr, (void *) arrayRight->DoublevArr, sizeof(double), opAuxFunction);
        case SOPC_String_Id:
            return SOPC_Op_Array(length, (void *) arrayLeft->StringArr, (void *) arrayRight->StringArr, sizeof(SOPC_String), opAuxFunction);
        case SOPC_DateTime_Id:
            return SOPC_Op_Array(length, (void *) arrayLeft->DateArr, (void *) arrayRight->DateArr, sizeof(SOPC_DateTime), opAuxFunction);
        case SOPC_Guid_Id:
            return SOPC_Op_Array(length, (void *) arrayLeft->GuidArr, (void *) arrayRight->GuidArr, sizeof(SOPC_Guid), opAuxFunction);
        case SOPC_ByteString_Id:
            return SOPC_Op_Array(length, (void *) arrayLeft->BstringArr, (void *) arrayRight->BstringArr, sizeof(SOPC_ByteString), opAuxFunction);
        case SOPC_XmlElement_Id:
            return SOPC_Op_Array(length, (void *) arrayLeft->XmlEltArr, (void *) arrayRight->XmlEltArr, sizeof(SOPC_XmlElement), opAuxFunction);
        case SOPC_NodeId_Id:
            return SOPC_Op_Array(length, (void *) arrayLeft->NodeIdArr, (void *) arrayRight->NodeIdArr, sizeof(SOPC_NodeId), opAuxFunction);
        case SOPC_ExpandedNodeId_Id:
            return SOPC_Op_Array(length, (void *) arrayLeft->ExpNodeIdArr, (void *) arrayRight->ExpNodeIdArr, sizeof(SOPC_ExpandedNodeId), opAuxFunction);
        case SOPC_StatusCode_Id:
            return SOPC_Op_Array(length, (void *) arrayLeft->StatusArr, (void *) arrayRight->StatusArr, sizeof(SOPC_StatusCode), opAuxFunction);
        case SOPC_QualifiedName_Id:
            return SOPC_Op_Array(length, (void *) arrayLeft->QnameArr, (void *) arrayRight->QnameArr, sizeof(SOPC_QualifiedName), opAuxFunction);
        case SOPC_LocalizedText_Id:
            return SOPC_Op_Array(length, (void *) arrayLeft->LocalizedTextArr, (void *) arrayRight->LocalizedTextArr, sizeof(SOPC_LocalizedText), opAuxFunction);
        case SOPC_ExtensionObject_Id:
            return SOPC_Op_Array(length, (void *) arrayLeft->ExtObjectArr, (void *) arrayRight->ExtObjectArr, sizeof(SOPC_ExtensionObject), opAuxFunction);
        case SOPC_DataValue_Id:
            return SOPC_Op_Array(length, (void *) arrayLeft->DataValueArr, (void *) arrayRight->DataValueArr, sizeof(SOPC_DataValue), opAuxFunction);
        case SOPC_Variant_Id:
            return SOPC_Op_Array(length, (void *) arrayLeft->VariantArr, (void *) arrayRight->VariantArr, sizeof(SOPC_Variant), opAuxFunction);
        case SOPC_DiagnosticInfo_Id:
            return SOPC_Op_Array(length, (void *) arrayLeft->DiagInfoArr, (void *) arrayRight->DiagInfoArr, sizeof(SOPC_DiagnosticInfo), opAuxFunction);
        default:
            break;
    }
    return status;
}

void SOPC_Variant_InitializeAux(void* value){
    SOPC_Variant_Initialize((SOPC_Variant*) value);
}

void SOPC_Variant_Initialize(SOPC_Variant* variant){
    if(variant != NULL){
        memset(variant, 0, sizeof(SOPC_Variant));
    }
}
 
void SOPC_Null_ClearAux(void* value){
    (void) value;
}

SOPC_EncodeableObject_PfnClear* GetBuiltInTypeClearFunction(SOPC_BuiltinId builtInTypeId){
    SOPC_EncodeableObject_PfnClear* clearFunction = NULL;
    switch(builtInTypeId){
            case SOPC_Null_Id:
                clearFunction = SOPC_Null_ClearAux;
                break;
            case SOPC_Boolean_Id:
                clearFunction = SOPC_Boolean_ClearAux;
                break;
            case SOPC_SByte_Id:
                clearFunction = SOPC_SByte_ClearAux;
                break;
            case SOPC_Byte_Id:
                clearFunction = SOPC_Byte_ClearAux;
                break;
            case SOPC_Int16_Id:
                clearFunction = SOPC_Int16_ClearAux;
                break;
            case SOPC_UInt16_Id:
                clearFunction = SOPC_UInt16_ClearAux;
                break;
            case SOPC_Int32_Id:
                clearFunction = SOPC_Int32_ClearAux;
                break;
            case SOPC_UInt32_Id:
                clearFunction = SOPC_UInt32_ClearAux;
                break;
            case SOPC_Int64_Id:
                clearFunction = SOPC_Int64_ClearAux;
                break;
            case SOPC_UInt64_Id:
                clearFunction = SOPC_UInt64_ClearAux;
                break;
            case SOPC_Float_Id:
                clearFunction = SOPC_Float_ClearAux;
                break;
            case SOPC_Double_Id:
                clearFunction = SOPC_Double_ClearAux;
                break;
            case SOPC_String_Id:
                clearFunction = SOPC_String_ClearAux;
                break;
            case SOPC_DateTime_Id:
                clearFunction = SOPC_DateTime_ClearAux;
                break;
            case SOPC_Guid_Id:
                clearFunction = SOPC_Guid_ClearAux;
                break;
            case SOPC_ByteString_Id:
                clearFunction = SOPC_ByteString_ClearAux;
                break;
            case SOPC_XmlElement_Id:
                clearFunction = SOPC_XmlElement_ClearAux;
                break;
            case SOPC_NodeId_Id:
                clearFunction = SOPC_NodeId_ClearAux;
                break;
            case SOPC_ExpandedNodeId_Id:
                clearFunction = SOPC_ExpandedNodeId_ClearAux;
                break;
            case SOPC_StatusCode_Id:
                clearFunction = SOPC_StatusCode_ClearAux;
                break;
            case SOPC_QualifiedName_Id:
                clearFunction = SOPC_QualifiedName_ClearAux;
                break;
            case SOPC_LocalizedText_Id:
                clearFunction = SOPC_LocalizedText_ClearAux;
                break;
            case SOPC_ExtensionObject_Id:
                clearFunction = SOPC_ExtensionObject_ClearAux;
                break;
            case SOPC_DataValue_Id:
                clearFunction = SOPC_DataValue_ClearAux;
                break;
            case SOPC_Variant_Id:
                clearFunction = SOPC_Variant_ClearAux;
                break;
            case SOPC_DiagnosticInfo_Id:
                clearFunction = SOPC_DiagnosticInfo_ClearAux;
                break;
            default:
                break;
        }
    return clearFunction;
}

SOPC_StatusCode SOPC_Null_CopyAux(void* dest, void* src){
    (void) dest;
    (void) src;
    return STATUS_OK;
}

SOPC_EncodeableObject_PfnOp* GetBuiltInTypeCopyFunction(SOPC_BuiltinId builtInTypeId){
    SOPC_EncodeableObject_PfnOp* copyFunction = NULL;
    switch(builtInTypeId){
            case SOPC_Null_Id:
                copyFunction = SOPC_Null_CopyAux;
                break;
            case SOPC_Boolean_Id:
                copyFunction = SOPC_Boolean_CopyAux;
                break;
            case SOPC_SByte_Id:
                copyFunction = SOPC_SByte_CopyAux;
                break;
            case SOPC_Byte_Id:
                copyFunction = SOPC_Byte_CopyAux;
                break;
            case SOPC_Int16_Id:
                copyFunction = SOPC_Int16_CopyAux;
                break;
            case SOPC_UInt16_Id:
                copyFunction = SOPC_UInt16_CopyAux;
                break;
            case SOPC_Int32_Id:
                copyFunction = SOPC_Int32_CopyAux;
                break;
            case SOPC_UInt32_Id:
                copyFunction = SOPC_UInt32_CopyAux;
                break;
            case SOPC_Int64_Id:
                copyFunction = SOPC_Int64_CopyAux;
                break;
            case SOPC_UInt64_Id:
                copyFunction = SOPC_UInt64_CopyAux;
                break;
            case SOPC_Float_Id:
                copyFunction = SOPC_Float_CopyAux;
                break;
            case SOPC_Double_Id:
                copyFunction = SOPC_Double_CopyAux;
                break;
            case SOPC_String_Id:
                copyFunction = SOPC_String_CopyAux;
                break;
            case SOPC_DateTime_Id:
                copyFunction = SOPC_DateTime_CopyAux;
                break;
            case SOPC_Guid_Id:
                copyFunction = SOPC_Guid_CopyAux;
                break;
            case SOPC_ByteString_Id:
                copyFunction = SOPC_ByteString_CopyAux;
                break;
            case SOPC_XmlElement_Id:
                copyFunction = SOPC_XmlElement_CopyAux;
                break;
            case SOPC_NodeId_Id:
                copyFunction = SOPC_NodeId_CopyAux;
                break;
            case SOPC_ExpandedNodeId_Id:
                copyFunction = SOPC_ExpandedNodeId_CopyAux;
                break;
            case SOPC_StatusCode_Id:
                copyFunction = SOPC_StatusCode_CopyAux;
                break;
            case SOPC_QualifiedName_Id:
                copyFunction = SOPC_QualifiedName_CopyAux;
                break;
            case SOPC_LocalizedText_Id:
                copyFunction = SOPC_LocalizedText_CopyAux;
                break;
            case SOPC_ExtensionObject_Id:
                copyFunction = SOPC_ExtensionObject_CopyAux;
                break;
            case SOPC_DataValue_Id:
                copyFunction = SOPC_DataValue_CopyAux;
                break;
            case SOPC_Variant_Id:
                copyFunction = SOPC_Variant_CopyAux;
                break;
            case SOPC_DiagnosticInfo_Id:
                copyFunction = SOPC_DiagnosticInfo_CopyAux;
                break;
            default:
                break;
        }
    return copyFunction;
}

SOPC_StatusCode AllocVariantNonArrayBuiltInType(SOPC_BuiltinId     builtInTypeId,
                                                SOPC_VariantValue* val)
{
    SOPC_StatusCode status = STATUS_OK;
    switch(builtInTypeId){
        case SOPC_Null_Id:
        case SOPC_Boolean_Id:
        case SOPC_SByte_Id:
        case SOPC_Byte_Id:
        case SOPC_Int16_Id:
        case SOPC_UInt16_Id:
        case SOPC_Int32_Id:
        case SOPC_UInt32_Id:
        case SOPC_Int64_Id:
        case SOPC_UInt64_Id:
        case SOPC_Float_Id:
        case SOPC_Double_Id:
        case SOPC_String_Id:
        case SOPC_DateTime_Id:
        case SOPC_ByteString_Id:
        case SOPC_XmlElement_Id:
        case SOPC_StatusCode_Id:
            break;
        case SOPC_Guid_Id:
            val->Guid = malloc(sizeof(SOPC_Guid));
            if(NULL != val->Guid){
                memset(val->Guid, 0, sizeof(SOPC_Guid));
            }else{
                status = STATUS_NOK;
            }
            break;
        case SOPC_NodeId_Id:
            val->NodeId = malloc(sizeof(SOPC_NodeId));
            if(NULL != val->NodeId){
                memset(val->NodeId, 0, sizeof(SOPC_NodeId));
            }else{
                status = STATUS_NOK;
            }
            break;
        case SOPC_ExpandedNodeId_Id:
            val->ExpNodeId = malloc(sizeof(SOPC_ExpandedNodeId));
            if(NULL != val->ExpNodeId){
                memset(val->ExpNodeId, 0, sizeof(SOPC_ExpandedNodeId));
            }else{
                status = STATUS_NOK;
            }
            break;
        case SOPC_QualifiedName_Id:
            val->Qname = malloc(sizeof(SOPC_QualifiedName));
            if(NULL != val->Qname){
                memset(val->Qname, 0, sizeof(SOPC_QualifiedName));
            }else{
                status = STATUS_NOK;
            }
            break;
        case SOPC_LocalizedText_Id:
            val->LocalizedText = malloc(sizeof(SOPC_LocalizedText));
            if(NULL != val->LocalizedText){
                memset(val->LocalizedText, 0, sizeof(SOPC_LocalizedText));
            }else{
                status = STATUS_NOK;
            }
            break;
        case SOPC_ExtensionObject_Id:
            val->ExtObject = malloc(sizeof(SOPC_ExtensionObject));
            if(NULL != val->ExtObject){
                memset(val->ExtObject, 0, sizeof(SOPC_ExtensionObject));
            }else{
                status = STATUS_NOK;
            }
            break;
        case SOPC_DataValue_Id:
            val->DataValue = malloc(sizeof(SOPC_DataValue));
            if(NULL != val->DataValue){
                memset(val->DataValue, 0, sizeof(SOPC_DataValue));
            }else{
                status = STATUS_NOK;
            }
            break;
        case SOPC_DiagnosticInfo_Id:
            val->DiagInfo = malloc(sizeof(SOPC_DiagnosticInfo));
            if(NULL != val->DiagInfo){
                memset(val->DiagInfo, 0, sizeof(SOPC_DiagnosticInfo));
            }else{
                status = STATUS_NOK;
            }
            val->DiagInfo = NULL;
            break;
        case SOPC_Variant_Id:
            // Part 6 Table 14 (v1.03): "The value shall not be a Variant
            //                           but it could be an array of Variants."
            //Note: Variant is not encoded in INGOPCS stack for this case
            break;
    }
    return status;
}

void FreeVariantNonArrayBuiltInType(SOPC_BuiltinId     builtInTypeId,
                                    SOPC_VariantValue* val)
{
    switch(builtInTypeId){
        case SOPC_Null_Id:
        case SOPC_Boolean_Id:
        case SOPC_SByte_Id:
        case SOPC_Byte_Id:
        case SOPC_Int16_Id:
        case SOPC_UInt16_Id:
        case SOPC_Int32_Id:
        case SOPC_UInt32_Id:
        case SOPC_Int64_Id:
        case SOPC_UInt64_Id:
        case SOPC_Float_Id:
        case SOPC_Double_Id:
        case SOPC_String_Id:
        case SOPC_DateTime_Id:
        case SOPC_ByteString_Id:
        case SOPC_XmlElement_Id:
        case SOPC_StatusCode_Id:
            break;
        case SOPC_Guid_Id:
            if(NULL != val->Guid){
                free(val->Guid);
            }
            val->Guid = NULL;
            break;
        case SOPC_NodeId_Id:
            if(NULL != val->NodeId){
                free(val->NodeId);
            }
            val->NodeId = NULL;
            break;
        case SOPC_ExpandedNodeId_Id:
            if(NULL != val->ExpNodeId){
                free(val->ExpNodeId);
            }
            val->ExpNodeId = NULL;
            break;
        case SOPC_QualifiedName_Id:
            if(NULL != val->Qname){
                free(val->Qname);
            }
            val->Qname = NULL;
            break;
        case SOPC_LocalizedText_Id:
            if(NULL != val->LocalizedText){
                free(val->LocalizedText);
            }
            val->LocalizedText = NULL;
            break;
        case SOPC_ExtensionObject_Id:
            if(NULL != val->ExtObject){
                free(val->ExtObject);
            }
            val->ExtObject = NULL;
            break;
        case SOPC_DataValue_Id:
            if(NULL != val->DataValue){
                free(val->DataValue);
            }
            val->DataValue = NULL;
            break;
        case SOPC_DiagnosticInfo_Id:
            if(NULL != val->DiagInfo){
                free(val->DiagInfo);
            }
            val->DiagInfo = NULL;
            break;
        case SOPC_Variant_Id:
            // Part 6 Table 14 (v1.03): "The value shall not be a Variant
            //                           but it could be an array of Variants."
            //Note: Variant is not encoded in INGOPCS stack for this case
            break;
    }
}


SOPC_StatusCode SOPC_Variant_Copy(SOPC_Variant* dest, const SOPC_Variant* src){
    SOPC_StatusCode status = STATUS_INVALID_PARAMETERS;
    uint8_t error = FALSE;
    int64_t matrixLength = 1; // For multiplication to compute from dimensions values
    int32_t idx = 0;
    if(dest != NULL && src != NULL){
        SOPC_EncodeableObject_PfnOp* copyFunction = GetBuiltInTypeCopyFunction(src->BuiltInTypeId);
        if(copyFunction == NULL)
            return STATUS_NOK;

        switch(src->ArrayType){
        case SOPC_VariantArrayType_SingleValue:
            status = AllocVariantNonArrayBuiltInType(src->BuiltInTypeId, &dest->Value);
            if(STATUS_OK == status){
                status = ApplyOpToVariantNonArrayBuiltInType(src->BuiltInTypeId,
                                                             &dest->Value,
                                                             &src->Value,
                                                             copyFunction);
            }
            break;
        case SOPC_VariantArrayType_Array:
            if(src->Value.Array.Length >= 0){
                status = AllocVariantArrayBuiltInType(src->BuiltInTypeId,
                                                      &dest->Value.Array.Content,
                                                      src->Value.Array.Length);
                if(STATUS_OK == status){
                    dest->Value.Array.Length = src->Value.Array.Length;
                    status = ApplyOpToVariantArrayBuiltInType(src->BuiltInTypeId,
                                                              &dest->Value.Array.Content,
                                                              &src->Value.Array.Content,
                                                              src->Value.Array.Length,
                                                              copyFunction);
                }
                if(STATUS_OK != status){
                    ClearToVariantArrayBuiltInType(src->BuiltInTypeId,
                                                   &dest->Value.Array.Content,
                                                   &dest->Value.Array.Length,
                                                   GetBuiltInTypeClearFunction(src->BuiltInTypeId));
                }
            }
            break;
        case SOPC_VariantArrayType_Matrix:
            if(src->Value.Matrix.Dimensions == 0){
                matrixLength = 0;
                status = STATUS_OK;
            }else{
                for(idx = 0; idx < src->Value.Matrix.Dimensions && FALSE == error; idx ++){
                    if(src->Value.Matrix.ArrayDimensions[idx] > 0 &&
                            matrixLength * src->Value.Matrix.ArrayDimensions[idx] <= INT32_MAX)
                    {
                        matrixLength *= src->Value.Matrix.ArrayDimensions[idx];
                    }else{
                        error = 1;
                    }
                }
                if(FALSE == error){
                    dest->Value.Matrix.ArrayDimensions = malloc(src->Value.Matrix.Dimensions*sizeof(int32_t));
                    if(NULL != dest->Value.Matrix.ArrayDimensions){
                        dest->Value.Matrix.Dimensions = src->Value.Matrix.Dimensions;
                        memcpy(dest->Value.Matrix.ArrayDimensions, src->Value.Matrix.ArrayDimensions, src->Value.Matrix.Dimensions*sizeof(int32_t));
                        status = AllocVariantArrayBuiltInType(src->BuiltInTypeId,
                                                              &dest->Value.Matrix.Content,
                                                              matrixLength);
                        if(STATUS_OK == status){
                            status = ApplyOpToVariantArrayBuiltInType(src->BuiltInTypeId,
                                                                      &dest->Value.Matrix.Content,
                                                                      &src->Value.Matrix.Content,
                                                                      (int32_t) matrixLength,
                                                                      copyFunction);
                            if(STATUS_OK != status){
                                ClearToVariantArrayBuiltInType(src->BuiltInTypeId,
                                                               &dest->Value.Matrix.Content,
                                                               (int32_t*) &matrixLength,
                                                               GetBuiltInTypeClearFunction(src->BuiltInTypeId));
                                free(dest->Value.Matrix.ArrayDimensions);
                                dest->Value.Matrix.ArrayDimensions = NULL;
                            }
                        }else{
                            free(dest->Value.Matrix.ArrayDimensions);
                            dest->Value.Matrix.ArrayDimensions = NULL;
                        }
                    }
                }
            }
            break;
        }
        if(STATUS_OK == status){
            dest->BuiltInTypeId = src->BuiltInTypeId;
            dest->ArrayType = src->ArrayType;
        }

        return status;
    }

    return status;
}

SOPC_StatusCode SOPC_Variant_CopyAux(void* dest, void* src){
    return SOPC_Variant_Copy((SOPC_Variant*) dest, (SOPC_Variant*) src);
}

void SOPC_Variant_ClearAux(void* value){
    SOPC_Variant_Clear((SOPC_Variant*) value);
}

void SOPC_Variant_Clear(SOPC_Variant* variant){
    uint8_t error = FALSE;
    int64_t matrixLength = 1; // For multiplication to compute from dimensions values
    int32_t idx = 0;
    if(variant != NULL){
        SOPC_EncodeableObject_PfnClear* clearFunction = GetBuiltInTypeClearFunction(variant->BuiltInTypeId);
        if(clearFunction == NULL)
            return;

        switch(variant->ArrayType){
            case SOPC_VariantArrayType_SingleValue:
                ApplyToVariantNonArrayBuiltInType(variant->BuiltInTypeId,
                                                  &variant->Value,
                                                  clearFunction);
                FreeVariantNonArrayBuiltInType(variant->BuiltInTypeId,
                                               &variant->Value);
                break;
            case SOPC_VariantArrayType_Array:
                ClearToVariantArrayBuiltInType(variant->BuiltInTypeId,
                                               &variant->Value.Array.Content,
                                               &variant->Value.Array.Length,
                                               clearFunction);
                break;
            case SOPC_VariantArrayType_Matrix:
                if(variant->Value.Matrix.Dimensions == 0){
                    matrixLength = 0;
                }
                for(idx = 0; idx < variant->Value.Matrix.Dimensions && FALSE == error; idx ++){
                    if(variant->Value.Matrix.ArrayDimensions[idx] > 0 &&
                       matrixLength * variant->Value.Matrix.ArrayDimensions[idx] <= INT32_MAX)
                    {
                        matrixLength *= variant->Value.Matrix.ArrayDimensions[idx];
                    }else{
                        error = 1;
                    }
                }
                if(FALSE == error){
                    free(variant->Value.Matrix.ArrayDimensions);
                    variant->Value.Matrix.ArrayDimensions = NULL;
                    ClearToVariantArrayBuiltInType(variant->BuiltInTypeId,
                                                   &variant->Value.Matrix.Content,
                                                   (int32_t*) &matrixLength,
                                                   clearFunction);
                    variant->Value.Matrix.Dimensions = 0;
                }
                break;
        }

        // Reset internal properties
        SOPC_Variant_Initialize(variant);
    }
}


void SOPC_DataValue_InitializeAux(void* value){
    SOPC_DataValue_Initialize((SOPC_DataValue*) value);
}

void SOPC_DataValue_Initialize(SOPC_DataValue* dataValue){
    if(dataValue != NULL){
        memset(dataValue, 0, sizeof(SOPC_DataValue));
    }
}

SOPC_StatusCode SOPC_DataValue_Copy(SOPC_DataValue* dest, const SOPC_DataValue* src){
    SOPC_StatusCode status = STATUS_INVALID_PARAMETERS;
    if(NULL != dest && NULL != src){
        status = SOPC_Variant_Copy(&dest->Value, &src->Value);
        if(STATUS_OK == status){
            dest->Status = src->Status;
            dest->ServerPicoSeconds = src->ServerPicoSeconds;
            dest->ServerTimestamp = src->ServerTimestamp;
            dest->SourcePicoSeconds = src->SourcePicoSeconds;
            dest->SourceTimestamp = src->SourceTimestamp;
        }
    }
    return status;
}

SOPC_StatusCode SOPC_DataValue_CopyAux(void* dest, void* src){
    return SOPC_DataValue_Copy((SOPC_DataValue*) dest, (SOPC_DataValue*) src);
}

void SOPC_DataValue_ClearAux(void* value){
    SOPC_DataValue_Clear((SOPC_DataValue*) value);
}
void SOPC_DataValue_Clear(SOPC_DataValue* dataValue){
    if(dataValue != NULL){
        SOPC_Variant_Clear(&dataValue->Value);
        SOPC_StatusCode_Clear(&dataValue->Status);
        SOPC_DateTime_Clear(&dataValue->SourceTimestamp);
        SOPC_DateTime_Clear(&dataValue->ServerTimestamp);
        dataValue->SourcePicoSeconds = 0;
        dataValue->ServerPicoSeconds = 0;
    }
}


void SOPC_Initialize_Array(int32_t* noOfElts, void** eltsArray, size_t sizeOfElt,
                           SOPC_EncodeableObject_PfnInitialize* initFct)
{
    (void) initFct;
    (void) sizeOfElt;
    *noOfElts = 0;
    *eltsArray = NULL;
}

SOPC_StatusCode SOPC_Op_Array(int32_t noOfElts, void* eltsArrayLeft, void* eltsArrayRight, size_t sizeOfElt,
                              SOPC_EncodeableObject_PfnOp* opFct)
{
    SOPC_StatusCode status = STATUS_OK;
    size_t idx = 0;
    size_t pos = 0;
    SOPC_Byte* byteArrayLeft = eltsArrayLeft;
    SOPC_Byte* byteArrayRight = eltsArrayRight;
    if(noOfElts >= 0 &&
       (size_t) noOfElts <= SIZE_MAX &&
       byteArrayLeft != NULL && byteArrayRight != NULL)
    {
        for (idx = 0; idx < (size_t) noOfElts && STATUS_OK == status; idx ++){
            pos = idx * sizeOfElt;
            status = opFct(&(byteArrayLeft[pos]), &(byteArrayRight[pos]));
        }
    }else{
        status = STATUS_INVALID_PARAMETERS;
    }
    return status;
}

void SOPC_Clear_Array(int32_t* noOfElts, void** eltsArray, size_t sizeOfElt,
                      SOPC_EncodeableObject_PfnClear* clearFct)
{
    size_t idx = 0;
    size_t pos = 0;
    SOPC_Byte* byteArray = NULL;
    if(noOfElts != NULL && *noOfElts >= 0 &&
       (size_t) *noOfElts <= SIZE_MAX &&
       eltsArray != NULL){
        byteArray = *eltsArray;
        if(byteArray != NULL){
            for (idx = 0; idx < (size_t) *noOfElts; idx ++){
                pos = idx * sizeOfElt;
                clearFct(&(byteArray[pos]));
            }

            free(*eltsArray);
        }
        *noOfElts = 0;
        *eltsArray = NULL;
    }
}
