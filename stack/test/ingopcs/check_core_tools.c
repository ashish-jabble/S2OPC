/*
 * Entry point for tools tests. Tests use libcheck.
 *
 * If you want to debug the exe, you should define env var CK_FORK=no
 * http://check.sourceforge.net/doc/check_html/check_4.html#No-Fork-Mode
 *
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


#include <stdlib.h>
#include <stdio.h>
#include <check.h>
#include "sopc_encoder.h"
#include "check_stack.h"
#include "sopc_helper_endianess_cfg.h"

START_TEST(test_ua_encoder_endianess_mgt)
{
    int16_t v16 = 0;
    uint16_t vu16 = 0;
    int32_t v32 = 0;
    uint32_t vu32 = 0;
    int64_t v64 = 0;
    uint64_t vu64 = 0;
    float vfloat = 0.0;
    double vdouble = 0.0;

    uint8_t* bytes = NULL;

    // Test encoding with same endianess in machine and UA binary
    sopc_endianess = SOPC_Endianess_LittleEndian;
    bytes = (uint8_t*) &v16;
    bytes[0] = 0xAB;
    bytes[1] = 0xBC;
    SOPC_EncodeDecode_Int16(&v16);
    ck_assert(bytes[0] == 0xAB && bytes[1] == 0xBC);

    bytes = (uint8_t*) &vu16;
    bytes[0] = 0xAB;
    bytes[1] = 0xBC;
    SOPC_EncodeDecode_UInt16(&vu16);
    ck_assert(bytes[0] == 0xAB && bytes[1] == 0xBC);

    bytes = (uint8_t*) &v32;
    bytes[0] = 0xAB;
    bytes[1] = 0xBC;
    bytes[2] = 0xCD;
    bytes[3] = 0xDE;
    SOPC_EncodeDecode_Int32(&v32);
    ck_assert(bytes[0] == 0xAB && bytes[1] == 0xBC &&
              bytes[2] == 0xCD && bytes[3] == 0xDE);

    bytes = (uint8_t*) &vu32;
    bytes[0] = 0xAB;
    bytes[1] = 0xBC;
    bytes[2] = 0xCD;
    bytes[3] = 0xDE;
    SOPC_EncodeDecode_UInt32(&vu32);
    ck_assert(bytes[0] == 0xAB && bytes[1] == 0xBC &&
              bytes[2] == 0xCD && bytes[3] == 0xDE);

    bytes = (uint8_t*) &v64;
    bytes[0] = 0x00;
    bytes[1] = 0x11;
    bytes[2] = 0x22;
    bytes[3] = 0x33;
    bytes[4] = 0xAB;
    bytes[5] = 0xBC;
    bytes[6] = 0xCD;
    bytes[7] = 0xDE;
    SOPC_EncodeDecode_Int64(&v64);
    ck_assert(bytes[0] == 0x00 && bytes[1] == 0x11 &&
              bytes[2] == 0x22 && bytes[3] == 0x33 &&
              bytes[4] == 0xAB && bytes[5] == 0xBC &&
              bytes[6] == 0xCD && bytes[7] == 0xDE);

    bytes = (uint8_t*) &vu64;
    bytes[0] = 0x00;
    bytes[1] = 0x11;
    bytes[2] = 0x22;
    bytes[3] = 0x33;
    bytes[4] = 0xAB;
    bytes[5] = 0xBC;
    bytes[6] = 0xCD;
    bytes[7] = 0xDE;
    SOPC_EncodeDecode_UInt64(&vu64);
    ck_assert(bytes[0] == 0x00 && bytes[1] == 0x11 &&
              bytes[2] == 0x22 && bytes[3] == 0x33 &&
              bytes[4] == 0xAB && bytes[5] == 0xBC &&
              bytes[6] == 0xCD && bytes[7] == 0xDE);

    sopc_floatEndianess = SOPC_Endianess_LittleEndian;
    bytes = (uint8_t*) &vfloat;
    bytes[0] = 0x00;
    bytes[1] = 0x00;
    bytes[2] = 0xD0;
    bytes[3] = 0xC0;
    SOPC_EncodeDecode_Float(&vfloat);
    ck_assert(bytes[0] == 0x00 && bytes[1] == 0x00 &&
              bytes[2] == 0xD0 && bytes[3] == 0xC0);

    bytes = (uint8_t*) &vdouble;
    bytes[0] = 0x00;
    bytes[1] = 0x00;
    bytes[2] = 0x00;
    bytes[3] = 0x00;
    bytes[4] = 0x00;
    bytes[5] = 0x00;
    bytes[6] = 0x1A;
    bytes[7] = 0xC0;
    SOPC_EncodeDecode_Double(&vdouble);
    ck_assert(bytes[0] == 0x00 && bytes[1] == 0x00 &&
              bytes[2] == 0x00 && bytes[3] == 0x00 &&
              bytes[4] == 0x00 && bytes[5] == 0x00 &&
              bytes[6] == 0x1A && bytes[7] == 0xC0);


    // Test encoding with different endianess in machine and UA binary
    sopc_endianess = SOPC_Endianess_BigEndian;
    bytes = (uint8_t*) &v16;
    bytes[0] = 0xAB;
    bytes[1] = 0xBC;
    SOPC_EncodeDecode_Int16(&v16);
    ck_assert(bytes[1] == 0xAB && bytes[0] == 0xBC);

    bytes = (uint8_t*) &vu16;
    bytes[0] = 0xAB;
    bytes[1] = 0xBC;
    SOPC_EncodeDecode_UInt16(&vu16);
    ck_assert(bytes[1] == 0xAB && bytes[0] == 0xBC);

    bytes = (uint8_t*) &v32;
    bytes[0] = 0xAB;
    bytes[1] = 0xBC;
    bytes[2] = 0xCD;
    bytes[3] = 0xDE;
    SOPC_EncodeDecode_Int32(&v32);
    ck_assert(bytes[3] == 0xAB && bytes[2] == 0xBC &&
              bytes[1] == 0xCD && bytes[0] == 0xDE);

    bytes = (uint8_t*) &vu32;
    bytes[0] = 0xAB;
    bytes[1] = 0xBC;
    bytes[2] = 0xCD;
    bytes[3] = 0xDE;
    SOPC_EncodeDecode_UInt32(&vu32);
    ck_assert(bytes[3] == 0xAB && bytes[2] == 0xBC &&
              bytes[1] == 0xCD && bytes[0] == 0xDE);

    bytes = (uint8_t*) &v64;
    bytes[0] = 0x00;
    bytes[1] = 0x11;
    bytes[2] = 0x22;
    bytes[3] = 0x33;
    bytes[4] = 0xAB;
    bytes[5] = 0xBC;
    bytes[6] = 0xCD;
    bytes[7] = 0xDE;
    SOPC_EncodeDecode_Int64(&v64);
    ck_assert(bytes[7] == 0x00 && bytes[6] == 0x11 &&
              bytes[5] == 0x22 && bytes[4] == 0x33 &&
              bytes[3] == 0xAB && bytes[2] == 0xBC &&
              bytes[1] == 0xCD && bytes[0] == 0xDE);

    bytes = (uint8_t*) &vu64;
    bytes[0] = 0x00;
    bytes[1] = 0x11;
    bytes[2] = 0x22;
    bytes[3] = 0x33;
    bytes[4] = 0xAB;
    bytes[5] = 0xBC;
    bytes[6] = 0xCD;
    bytes[7] = 0xDE;
    SOPC_EncodeDecode_UInt64(&vu64);
    ck_assert(bytes[7] == 0x00 && bytes[6] == 0x11 &&
              bytes[5] == 0x22 && bytes[4] == 0x33 &&
              bytes[3] == 0xAB && bytes[2] == 0xBC &&
              bytes[1] == 0xCD && bytes[0] == 0xDE);

    sopc_floatEndianess = SOPC_Endianess_BigEndian;
    bytes = (uint8_t*) &vfloat;
    bytes[0] = 0xC0;
    bytes[1] = 0xD0;
    bytes[2] = 0x00;
    bytes[3] = 0x00;
    SOPC_EncodeDecode_Float(&vfloat);
    ck_assert(bytes[0] == 0x00 && bytes[1] == 0x00 &&
              bytes[2] == 0xD0 && bytes[3] == 0xC0);

    bytes = (uint8_t*) &vdouble;
    bytes[0] = 0xC0;
    bytes[1] = 0x1A;
    bytes[2] = 0x00;
    bytes[3] = 0x00;
    bytes[4] = 0x00;
    bytes[5] = 0x00;
    bytes[6] = 0x00;
    bytes[7] = 0x00;
    SOPC_EncodeDecode_Double(&vdouble);
    ck_assert(bytes[0] == 0x00 && bytes[1] == 0x00 &&
              bytes[2] == 0x00 && bytes[3] == 0x00 &&
              bytes[4] == 0x00 && bytes[5] == 0x00 &&
              bytes[6] == 0x1A && bytes[7] == 0xC0);

}
END_TEST

START_TEST(test_ua_encoder_basic_types)
{
    SOPC_Helper_EndianessCfg_Initialize(); // Necessary to initialize endianess configuration
    SOPC_StatusCode status = STATUS_OK;
    SOPC_Buffer* buffer = SOPC_Buffer_Create(100);

    SOPC_Buffer* bufferFull = SOPC_Buffer_Create(8);

    // Test Byte nominal and degraded cases
    //// Nominal write
    SOPC_Byte byte = 0xAE;
    status = SOPC_Byte_Write(&byte, buffer);
    ck_assert(status == STATUS_OK);
    ck_assert(buffer->data[0] == 0xAE);
    //// Degraded write
    status = SOPC_Byte_Write(NULL, buffer);
    ck_assert(status != STATUS_OK);
    status = SOPC_Byte_Write(&byte, NULL);
    ck_assert(status != STATUS_OK);
    bufferFull->position = 8; // Set buffer full
    status = SOPC_Byte_Write(&byte, bufferFull);
    ck_assert(status != STATUS_OK);

    //// Nominal read
    byte = 0x00;
    SOPC_Buffer_SetPosition(buffer, 0); // Reset position for reading
    status = SOPC_Byte_Read(&byte, buffer);
    ck_assert(status == STATUS_OK);
    ck_assert(byte == 0xAE);
    //// Degraded read
    status = SOPC_Byte_Read(NULL, buffer);
    ck_assert(status != STATUS_OK);
    status = SOPC_Byte_Read(&byte, NULL);
    ck_assert(status != STATUS_OK);
    status = SOPC_Byte_Read(&byte, buffer); // Nothing to read anymore
    ck_assert(status != STATUS_OK);

    // Test Boolean nominal and degraded cases
    SOPC_Buffer_Reset(buffer);
    //// Nominal write
    SOPC_Boolean boolv = false;
    status = SOPC_Boolean_Write(&boolv, buffer);
    ck_assert(status == STATUS_OK);
    ck_assert(buffer->data[0] == false);
    boolv = 1; // not FALSE
    status = SOPC_Boolean_Write(&boolv, buffer);
    ck_assert(status == STATUS_OK);
    ck_assert(buffer->data[1] == 1);
    boolv = 2; // not FALSE
    status = SOPC_Boolean_Write(&boolv, buffer);
    ck_assert(status == STATUS_OK);
    ck_assert(buffer->data[2] == 1); // True value always encoded as 1

    //// Degraded write
    status = SOPC_Boolean_Write(NULL, buffer);
    ck_assert(status != STATUS_OK);
    status = SOPC_Boolean_Write(&boolv, NULL);
    ck_assert(status != STATUS_OK);
    status = SOPC_Boolean_Write(&boolv, bufferFull); // Test with full buffer
    ck_assert(status != STATUS_OK);

    //// Nominal read
    boolv = 4;
    SOPC_Buffer_SetPosition(buffer, 0); // Reset position for reading
    buffer->data[2] = 2; // Simulates a true value received as 2
    status = SOPC_Boolean_Read(&boolv, buffer);
    ck_assert(status == STATUS_OK);
    ck_assert(boolv == false);
    status = SOPC_Boolean_Read(&boolv, buffer);
    ck_assert(status == STATUS_OK);
    ck_assert(boolv == 1);
    status = SOPC_Boolean_Read(&boolv, buffer);
    ck_assert(status == STATUS_OK);
    ck_assert(boolv == 1); // True value always decoded as 1
    //// Degraded read
    status = SOPC_Boolean_Read(NULL, buffer);
    ck_assert(status != STATUS_OK);
    status = SOPC_Boolean_Read(&boolv, NULL);
    ck_assert(status != STATUS_OK);
    status = SOPC_Boolean_Read(&boolv, buffer); // Nothing to read anymore
    ck_assert(status != STATUS_OK);

    // Test SByteuv16nal and degraded cases
    SOPC_Buffer_Reset(buffer);
    //// Nominal write
    SOPC_SByte sbyte = -1;
    status = SOPC_SByte_Write(&sbyte, buffer);
    ck_assert(status == STATUS_OK);
    ck_assert(buffer->data[0] == 0xFF);
    //// Degraded write
    status = SOPC_SByte_Write(NULL, buffer);
    ck_assert(status != STATUS_OK);
    status = SOPC_SByte_Write(&sbyte, NULL);
    ck_assert(status != STATUS_OK);
    status = SOPC_SByte_Write(&sbyte, bufferFull);
    ck_assert(status != STATUS_OK);

    //// Nominal read
    sbyte = 0x00;
    SOPC_Buffer_SetPosition(buffer, 0); // Reset position for reading
    status = SOPC_SByte_Read(&sbyte, buffer);
    ck_assert(status == STATUS_OK);
    ck_assert(sbyte == -1);
    //// Degraded read
    status = SOPC_SByte_Read(NULL, buffer);
    ck_assert(status != STATUS_OK);
    status = SOPC_SByte_Read(&sbyte, NULL);
    ck_assert(status != STATUS_OK);
    status = SOPC_SByte_Read(&sbyte, buffer); // Nothing to read anymore
    ck_assert(status != STATUS_OK);

    // Test Int16 nominal and degraded cases
    SOPC_Buffer_Reset(buffer);
    //// Nominal write
    int16_t v16 = -2;

    status = SOPC_Int16_Write(&v16, buffer);
    ck_assert(status == STATUS_OK);
    ////// Little endian encoded
    ck_assert(buffer->data[0] == 0xFE &&
              buffer->data[1] == 0xFF);
    //// Degraded write
    status = SOPC_Int16_Write(NULL, buffer);
    ck_assert(status != STATUS_OK);
    status = SOPC_Int16_Write(&v16, NULL);
    ck_assert(status != STATUS_OK);
    status = bufferFull->position = 7; // Set buffer almost full (1 byte left)
    status = SOPC_Int16_Write(&v16, bufferFull);
    ck_assert(status != STATUS_OK);

    //// Nominal read
    v16 = 0;
    SOPC_Buffer_SetPosition(buffer, 0); // Reset position for reading
    status = SOPC_Int16_Read(&v16, buffer);
    ck_assert(status == STATUS_OK);
    ck_assert(v16 == -2);
    //// Degraded read
    status = SOPC_Int16_Read(NULL, buffer);
    ck_assert(status != STATUS_OK);
    status = SOPC_Int16_Read(&v16, NULL);
    ck_assert(status != STATUS_OK);
    status = SOPC_Int16_Read(&v16, buffer); // Nothing to read anymore
    ck_assert(status != STATUS_OK);

    // Test UInt16 nominal and degraded cases
    SOPC_Buffer_Reset(buffer);
    //// Nominal write
    uint16_t vu16 = 2;

    status = SOPC_UInt16_Write(&vu16, buffer);
    ck_assert(status == STATUS_OK);
    ////// Little endian encoded
    ck_assert(buffer->data[0] == 0x02 &&
              buffer->data[1] == 0x00);
    //// Degraded write
    status = SOPC_UInt16_Write(NULL, buffer);
    ck_assert(status != STATUS_OK);
    status = SOPC_UInt16_Write(&vu16, NULL);
    ck_assert(status != STATUS_OK);
    status = bufferFull->position = 7; // Set buffer almost full (1 byte left)
    status = SOPC_UInt16_Write(&vu16, bufferFull);
    ck_assert(status != STATUS_OK);

    //// Nominal read
    vu16 = 0;
    SOPC_Buffer_SetPosition(buffer, 0); // Reset position for reading
    status = SOPC_UInt16_Read(&vu16, buffer);
    ck_assert(status == STATUS_OK);
    ck_assert(vu16 == 2);
    //// Degraded read
    status = SOPC_UInt16_Read(NULL, buffer);
    ck_assert(status != STATUS_OK);
    status = SOPC_UInt16_Read(&vu16, NULL);
    ck_assert(status != STATUS_OK);
    status = SOPC_UInt16_Read(&vu16, buffer); // Nothing to read anymore
    ck_assert(status != STATUS_OK);

    // Test Int32 nominal and degraded cases
    SOPC_Buffer_Reset(buffer);
    //// Nominal write
    int32_t v32 = -2;

    status = SOPC_Int32_Write(&v32, buffer);
    ck_assert(status == STATUS_OK);
    ////// Little endian encoded
    ck_assert(buffer->data[0] == 0xFE &&
              buffer->data[1] == 0xFF &&
              buffer->data[2] == 0xFF &&
              buffer->data[3] == 0xFF);
    //// Degraded write
    status = SOPC_Int32_Write(NULL, buffer);
    ck_assert(status != STATUS_OK);
    status = SOPC_Int32_Write(&v32, NULL);
    ck_assert(status != STATUS_OK);
    status = bufferFull->position = 5; // Set buffer almost full (3 byte left)
    status = SOPC_Int32_Write(&v32, bufferFull);
    ck_assert(status != STATUS_OK);

    //// Nominal read
    v32 = 0;
    SOPC_Buffer_SetPosition(buffer, 0); // Reset position for reading
    status = SOPC_Int32_Read(&v32, buffer);
    ck_assert(status == STATUS_OK);
    ck_assert(v32 == -2);
    //// Degraded read
    status = SOPC_Int32_Read(NULL, buffer);
    ck_assert(status != STATUS_OK);
    status = SOPC_Int32_Read(&v32, NULL);
    ck_assert(status != STATUS_OK);
    status = SOPC_Int32_Read(&v32, buffer); // Nothing to read anymore
    ck_assert(status != STATUS_OK);

    // Test UInt32 nominal and degraded cases
    SOPC_Buffer_Reset(buffer);
    //// Nominal write
    uint32_t vu32 = 1048578;

    status = SOPC_UInt32_Write(&vu32, buffer);
    ck_assert(status == STATUS_OK);
    ////// Little endian encoded
    ck_assert(buffer->data[0] == 0x02 &&
              buffer->data[1] == 0x00 &&
              buffer->data[2] == 0x10 &&
              buffer->data[3] == 0x00);
    //// Degraded write
    status = SOPC_UInt32_Write(NULL, buffer);
    ck_assert(status != STATUS_OK);
    status = SOPC_UInt32_Write(&vu32, NULL);
    ck_assert(status != STATUS_OK);
    status = bufferFull->position = 5; // Set buffer almost full (3 byte left)
    status = SOPC_UInt32_Write(&vu32, bufferFull);
    ck_assert(status != STATUS_OK);

    //// Nominal read
    vu32 = 0;
    SOPC_Buffer_SetPosition(buffer, 0); // Reset position for reading
    status = SOPC_UInt32_Read(&vu32, buffer);
    ck_assert(status == STATUS_OK);
    ck_assert(vu32 == 1048578);
    //// Degraded read
    status = SOPC_UInt32_Read(NULL, buffer);
    ck_assert(status != STATUS_OK);
    status = SOPC_UInt32_Read(&vu32, NULL);
    ck_assert(status != STATUS_OK);
    status = SOPC_UInt32_Read(&vu32, buffer); // Nothing to read anymore
    ck_assert(status != STATUS_OK);

    // Test Int64 nominal and degraded cases
    SOPC_Buffer_Reset(buffer);
    //// Nominal write
    int64_t v64 = -2;

    status = SOPC_Int64_Write(&v64, buffer);
    ck_assert(status == STATUS_OK);
    ////// Little endian encoded
    ck_assert(buffer->data[0] == 0xFE &&
              buffer->data[1] == 0xFF &&
              buffer->data[2] == 0xFF &&
              buffer->data[3] == 0xFF &&
              buffer->data[4] == 0xFF &&
              buffer->data[5] == 0xFF &&
              buffer->data[6] == 0xFF &&
              buffer->data[7] == 0xFF);
    //// Degraded write
    status = SOPC_Int64_Write(NULL, buffer);
    ck_assert(status != STATUS_OK);
    status = SOPC_Int64_Write(&v64, NULL);
    ck_assert(status != STATUS_OK);
    status = bufferFull->position = 1; // Set buffer almost full (7 byte left)
    status = SOPC_Int64_Write(&v64, bufferFull);
    ck_assert(status != STATUS_OK);

    //// Nominal read
    v64 = 0;
    SOPC_Buffer_SetPosition(buffer, 0); // Reset position for reading
    status = SOPC_Int64_Read(&v64, buffer);
    ck_assert(status == STATUS_OK);
    ck_assert(v64 == -2);
    //// Degraded read
    status = SOPC_Int64_Read(NULL, buffer);
    ck_assert(status != STATUS_OK);
    status = SOPC_Int64_Read(&v64, NULL);
    ck_assert(status != STATUS_OK);
    status = SOPC_Int64_Read(&v64, buffer); // Nothing to read anymore
    ck_assert(status != STATUS_OK);

    // Test UInt64 nominal and degraded cases
    SOPC_Buffer_Reset(buffer);
    //// Nominal write
    uint64_t vu64 = 0x100000000000002;

    status = SOPC_UInt64_Write(&vu64, buffer);
    ck_assert(status == STATUS_OK);
    ////// Little endian encoded
    ck_assert(buffer->data[0] == 0x02 &&
              buffer->data[1] == 0x00 &&
              buffer->data[2] == 0x00 &&
              buffer->data[3] == 0x00 &&
              buffer->data[4] == 0x00 &&
              buffer->data[5] == 0x00 &&
              buffer->data[6] == 0x00 &&
              buffer->data[7] == 0x01);
    //// Degraded write
    status = SOPC_UInt64_Write(NULL, buffer);
    ck_assert(status != STATUS_OK);
    status = SOPC_UInt64_Write(&vu64, NULL);
    ck_assert(status != STATUS_OK);
    status = bufferFull->position = 1; // Set buffer almost full (7 byte left)
    status = SOPC_UInt64_Write(&vu64, bufferFull);
    ck_assert(status != STATUS_OK);

    //// Nominal read
    vu64 = 0;
    SOPC_Buffer_SetPosition(buffer, 0); // Reset position for reading
    status = SOPC_UInt64_Read(&vu64, buffer);
    ck_assert(status == STATUS_OK);
    ck_assert(vu64 == 0x100000000000002);
    //// Degraded read
    status = SOPC_UInt64_Read(NULL, buffer);
    ck_assert(status != STATUS_OK);
    status = SOPC_UInt64_Read(&vu64, NULL);
    ck_assert(status != STATUS_OK);
    status = SOPC_UInt64_Read(&vu64, buffer); // Nothing to read anymore
    ck_assert(status != STATUS_OK);

    // Test Float nominal and degraded cases
    SOPC_Buffer_Reset(buffer);
    //// Nominal write
    float vfloat = -6.5;

    status = SOPC_Float_Write(&vfloat, buffer);
    ck_assert(status == STATUS_OK);
    ////// Little endian encoded
    ck_assert(buffer->data[0] == 0x00 &&
              buffer->data[1] == 0x00 &&
              buffer->data[2] == 0xD0 &&
              buffer->data[3] == 0xC0);
    //// Degraded write
    status = SOPC_Float_Write(NULL, buffer);
    ck_assert(status != STATUS_OK);
    status = SOPC_Float_Write(&vfloat, NULL);
    ck_assert(status != STATUS_OK);
    status = bufferFull->position = 5; // Set buffer almost full (3 byte left)
    status = SOPC_Float_Write(&vfloat, bufferFull);
    ck_assert(status != STATUS_OK);

    //// Nominal read
    vfloat = 0;
    SOPC_Buffer_SetPosition(buffer, 0); // Reset position for reading
    status = SOPC_Float_Read(&vfloat, buffer);
    ck_assert(status == STATUS_OK);
    ck_assert(vfloat == -6.5);
    //// Degraded read
    status = SOPC_Float_Read(NULL, buffer);
    ck_assert(status != STATUS_OK);
    status = SOPC_Float_Read(&vfloat, NULL);
    ck_assert(status != STATUS_OK);
    status = SOPC_Float_Read(&vfloat, buffer); // Nothing to read anymore
    ck_assert(status != STATUS_OK);

    // Test Double nominal and degraded cases
    SOPC_Buffer_Reset(buffer);
    //// Nominal write
    double vdouble = -6.5;

    status = SOPC_Double_Write(&vdouble, buffer);
    ck_assert(status == STATUS_OK);
    ////// Little endian encoded
    ck_assert(buffer->data[0] == 0x00 &&
              buffer->data[1] == 0x00 &&
              buffer->data[2] == 0x00 &&
              buffer->data[3] == 0x00 &&
              buffer->data[4] == 0x00 &&
              buffer->data[5] == 0x00 &&
              buffer->data[6] == 0x1A &&
              buffer->data[7] == 0xC0);
    //// Degraded write
    status = SOPC_Double_Write(NULL, buffer);
    ck_assert(status != STATUS_OK);
    status = SOPC_Double_Write(&vdouble, NULL);
    ck_assert(status != STATUS_OK);
    status = bufferFull->position = 1; // Set buffer almost full (7 byte left)
    status = SOPC_Double_Write(&vdouble, bufferFull);
    ck_assert(status != STATUS_OK);

    //// Nominal read
    vdouble = 0;
    SOPC_Buffer_SetPosition(buffer, 0); // Reset position for reading
    status = SOPC_Double_Read(&vdouble, buffer);
    ck_assert(status == STATUS_OK);
    ck_assert(vdouble == -6.5);
    //// Degraded read
    status = SOPC_Double_Read(NULL, buffer);
    ck_assert(status != STATUS_OK);
    status = SOPC_Double_Read(&vdouble, NULL);
    ck_assert(status != STATUS_OK);
    status = SOPC_Double_Read(&vdouble, buffer); // Nothing to read anymore
    ck_assert(status != STATUS_OK);

    // Test DateTime nominal and degraded cases
    SOPC_Buffer_Reset(buffer);
    //// Nominal write
    SOPC_DateTime vDate;
    SOPC_DateTime_Initialize(&vDate);
    SOPC_DateTime_FromInt64(&vDate, -2);

    status = SOPC_DateTime_Write(&vDate, buffer);
    ck_assert(status == STATUS_OK);
    ////// Little endian encoded
    ck_assert(buffer->data[0] == 0xFE &&
              buffer->data[1] == 0xFF &&
              buffer->data[2] == 0xFF &&
              buffer->data[3] == 0xFF &&
              buffer->data[4] == 0xFF &&
              buffer->data[5] == 0xFF &&
              buffer->data[6] == 0xFF &&
              buffer->data[7] == 0xFF);
    //// Degraded write
    status = SOPC_DateTime_Write(NULL, buffer);
    ck_assert(status != STATUS_OK);
    status = SOPC_DateTime_Write(&vDate, NULL);
    ck_assert(status != STATUS_OK);
    status = bufferFull->position = 1; // Set buffer almost full (7 byte left)
    status = SOPC_DateTime_Write(&vDate, bufferFull);
    ck_assert(status != STATUS_OK);

    //// Nominal read
    SOPC_DateTime_Clear(&vDate);
    SOPC_Buffer_SetPosition(buffer, 0); // Reset position for reading
    status = SOPC_DateTime_Read(&vDate, buffer);
    ck_assert(status == STATUS_OK);
    ck_assert(SOPC_DateTime_ToInt64(&vDate) == -2);
    //// Degraded read
    status = SOPC_DateTime_Read(NULL, buffer);
    ck_assert(status != STATUS_OK);
    status = SOPC_DateTime_Read(&vDate, NULL);
    ck_assert(status != STATUS_OK);
    status = SOPC_DateTime_Read(&vDate, buffer); // Nothing to read anymore
    ck_assert(status != STATUS_OK);

    SOPC_Buffer_Delete(buffer);
    SOPC_Buffer_Delete(bufferFull);
}
END_TEST

START_TEST(test_ua_encoder_other_types)
{
    SOPC_Helper_EndianessCfg_Initialize(); // Necessary to initialize endianess configuration
    SOPC_StatusCode status = STATUS_OK;
    SOPC_Buffer* buffer = SOPC_Buffer_Create(100);

    SOPC_Buffer* bufferFull = SOPC_Buffer_Create(8);

    //////////////////////////////////////////////
    // Test ByteString nominal and degraded cases
    //// Nominal write
    SOPC_ByteString* bs = SOPC_ByteString_Create();
    SOPC_ByteString* bs2 = SOPC_ByteString_Create();
    uint8_t boyString[3] = {0x42, 0x6F, 0x79}; // Boy

    ////// Empty string
    bs->Length = 0;
    status = SOPC_ByteString_Write(bs, buffer);
    ck_assert(status == STATUS_OK);
    ////// -1 length must be encoded for null string
    ck_assert(buffer->data[0] == 0xFF);
    ck_assert(buffer->data[1] == 0xFF);
    ck_assert(buffer->data[2] == 0xFF);
    ck_assert(buffer->data[3] == 0xFF);

    SOPC_Buffer_Reset(buffer);
    bs->Length = -1;
    status = SOPC_ByteString_Write(bs, buffer);
    ck_assert(status == STATUS_OK);
    ////// -1 length must be encoded for null string
    ck_assert(buffer->data[0] == 0xFF);
    ck_assert(buffer->data[1] == 0xFF);
    ck_assert(buffer->data[2] == 0xFF);
    ck_assert(buffer->data[3] == 0xFF);

    SOPC_Buffer_Reset(buffer);
    bs->Length = -10;
    status = SOPC_ByteString_Write(bs, buffer);
    ck_assert(status == STATUS_OK);
    ////// -1 length must be encoded for null string
    ck_assert(buffer->data[0] == 0xFF);
    ck_assert(buffer->data[1] == 0xFF);
    ck_assert(buffer->data[2] == 0xFF);
    ck_assert(buffer->data[3] == 0xFF);

    /////// Non empty bytestring
    SOPC_Buffer_Reset(buffer);
    status = SOPC_ByteString_CopyFromBytes(bs, boyString, 3);
    ck_assert(status == STATUS_OK);
    status = SOPC_ByteString_Write(bs, buffer);
    ck_assert(status == STATUS_OK);
    ck_assert(buffer->data[0] == 0x03);
    ck_assert(buffer->data[1] == 0x00);
    ck_assert(buffer->data[2] == 0x00);
    ck_assert(buffer->data[3] == 0x00);
    ck_assert(buffer->data[4] == 0x42);
    ck_assert(buffer->data[5] == 0x6F);
    ck_assert(buffer->data[6] == 0x79);


    //// Degraded write
    status = SOPC_ByteString_Write(NULL, buffer);
    ck_assert(status != STATUS_OK);
    status = SOPC_ByteString_Write(bs, NULL);
    ck_assert(status != STATUS_OK);
    bufferFull->position = 2; // Set buffer almost full (6 byte left)
    status = SOPC_ByteString_Write(bs, bufferFull);
    ck_assert(status != STATUS_OK);

    //// Nominal read
    SOPC_Buffer_SetPosition(buffer, 0); // Reset position for reading
    SOPC_ByteString_Clear(bs2);
    SOPC_ByteString_Initialize(bs2);
    status = SOPC_ByteString_Read(bs2, buffer);
    ck_assert(status == STATUS_OK);
    ck_assert(SOPC_ByteString_Equal(bs, bs2) != false);
    ck_assert(bs2->Length == 3);
    ck_assert(bs2->Data[0] == 0x42);
    ck_assert(bs2->Data[1] == 0x6F);
    ck_assert(bs2->Data[2] == 0x79);

    ////// Read 0 length bytestring
    buffer->data[0] = 0x00;
    buffer->data[1] = 0x00;
    buffer->data[2] = 0x00;
    buffer->data[3] = 0x00;
    SOPC_Buffer_SetPosition(buffer, 0); // Reset position for reading
    SOPC_ByteString_Clear(bs2);
    SOPC_ByteString_Initialize(bs2);status = SOPC_ByteString_Read(bs2, buffer);
    ck_assert(status == STATUS_OK);
    ck_assert(bs2->Length == -1); // Null bytestring always decoded -1

    ////// Read negative length bytestring
    buffer->data[0] = 0xFF;
    buffer->data[1] = 0x00;
    buffer->data[2] = 0x00;
    buffer->data[3] = 0xFF;
    SOPC_Buffer_SetPosition(buffer, 0); // Reset position for reading
    SOPC_ByteString_Clear(bs2);
    SOPC_ByteString_Initialize(bs2);
    status = SOPC_ByteString_Read(bs2, buffer);
    ck_assert(status == STATUS_OK);
    ck_assert(bs2->Length == -1); // Null bytestring always decoded -1

    ////// Read -1 length bytestring
    buffer->data[0] = 0xFF;
    buffer->data[1] = 0xFF;
    buffer->data[2] = 0xFF;
    buffer->data[3] = 0xFF;
    SOPC_Buffer_SetPosition(buffer, 0); // Reset position for reading
    SOPC_ByteString_Clear(bs2);
    SOPC_ByteString_Initialize(bs2);
    status = SOPC_ByteString_Read(bs2, buffer);
    ck_assert(status == STATUS_OK);
    ck_assert(bs2->Length == -1); // Null bytestring always decoded -1
    //// Degraded read
    status = SOPC_ByteString_Read(NULL, buffer);
    ck_assert(status != STATUS_OK);
    status = SOPC_ByteString_Read(bs, NULL);
    ck_assert(status != STATUS_OK);
    status = SOPC_ByteString_Read(bs, buffer); // Nothing to read anymore
    ck_assert(status != STATUS_OK);

    SOPC_ByteString_Delete(bs);
    SOPC_ByteString_Delete(bs2);
    bs = NULL;
    bs2 = NULL;


    /////////////////////////////////////////
    // Test String nominal and degraded cases
    //// Nominal write
    SOPC_Buffer_Reset(buffer);
    SOPC_String str;
    SOPC_String_Initialize(&str);
    SOPC_String str2;
    SOPC_String_Initialize(&str2);

    ////// Empty string
    str.Length = 0;
    status = SOPC_String_Write(&str, buffer);
    ck_assert(status == STATUS_OK);
    ////// -1 length must be encoded for null string
    ck_assert(buffer->data[0] == 0xFF);
    ck_assert(buffer->data[1] == 0xFF);
    ck_assert(buffer->data[2] == 0xFF);
    ck_assert(buffer->data[3] == 0xFF);

    SOPC_Buffer_Reset(buffer);
    str.Length = -1;
    status = SOPC_String_Write(&str, buffer);
    ck_assert(status == STATUS_OK);
    ////// -1 length must be encoded for null string
    ck_assert(buffer->data[0] == 0xFF);
    ck_assert(buffer->data[1] == 0xFF);
    ck_assert(buffer->data[2] == 0xFF);
    ck_assert(buffer->data[3] == 0xFF);

    SOPC_Buffer_Reset(buffer);
    str.Length = -10;
    status = SOPC_String_Write(&str, buffer);
    ck_assert(status == STATUS_OK);
    ////// -1 length must be encoded for null string
    ck_assert(buffer->data[0] == 0xFF);
    ck_assert(buffer->data[1] == 0xFF);
    ck_assert(buffer->data[2] == 0xFF);
    ck_assert(buffer->data[3] == 0xFF);

    /////// Non empty bytestring
    SOPC_Buffer_Reset(buffer);
    status = SOPC_String_AttachFromCstring(&str, "Boy");
    ck_assert(status == STATUS_OK);
    status = SOPC_String_Write(&str, buffer);
    ck_assert(status == STATUS_OK);
    ck_assert(buffer->data[0] == 0x03);
    ck_assert(buffer->data[1] == 0x00);
    ck_assert(buffer->data[2] == 0x00);
    ck_assert(buffer->data[3] == 0x00);
    ck_assert(buffer->data[4] == 0x42);
    ck_assert(buffer->data[5] == 0x6F);
    ck_assert(buffer->data[6] == 0x79);


    //// Degraded write
    status = SOPC_String_Write(NULL, buffer);
    ck_assert(status != STATUS_OK);
    status = SOPC_String_Write(&str, NULL);
    ck_assert(status != STATUS_OK);
    bufferFull->position = 2; // Set buffer almost full (6 byte left)
    status = SOPC_String_Write(&str, bufferFull);
    ck_assert(status != STATUS_OK);

    //// Nominal read
    SOPC_Buffer_SetPosition(buffer, 0); // Reset position for reading
    SOPC_String_Clear(&str2);
    SOPC_String_Initialize(&str2);
    status = SOPC_String_Read(&str2, buffer);
    ck_assert(status == STATUS_OK);
    ck_assert(SOPC_String_Equal(&str, &str2) != false);
    ck_assert(memcmp(SOPC_String_GetRawCString(&str2), "Boy", 3) == 0);

    ////// Read 0 length bytestring
    buffer->data[0] = 0x00;
    buffer->data[1] = 0x00;
    buffer->data[2] = 0x00;
    buffer->data[3] = 0x00;
    SOPC_Buffer_SetPosition(buffer, 0); // Reset position for reading
    SOPC_String_Clear(&str2);
    SOPC_String_Initialize(&str2);status = SOPC_String_Read(&str2, buffer);
    ck_assert(status == STATUS_OK);
    ck_assert(str2.Length == -1); // Null bytestring always decoded -1

    ////// Read negative length bytestring
    buffer->data[0] = 0xFF;
    buffer->data[1] = 0x00;
    buffer->data[2] = 0x00;
    buffer->data[3] = 0xFF;
    SOPC_Buffer_SetPosition(buffer, 0); // Reset position for reading
    SOPC_String_Clear(&str2);
    SOPC_String_Initialize(&str2);
    status = SOPC_String_Read(&str2, buffer);
    ck_assert(status == STATUS_OK);
    ck_assert(str2.Length == -1); // Null bytestring always decoded -1

    ////// Read -1 length bytestring
    buffer->data[0] = 0xFF;
    buffer->data[1] = 0xFF;
    buffer->data[2] = 0xFF;
    buffer->data[3] = 0xFF;
    SOPC_Buffer_SetPosition(buffer, 0); // Reset position for reading
    SOPC_String_Clear(&str2);
    SOPC_String_Initialize(&str2);
    status = SOPC_String_Read(&str2, buffer);
    ck_assert(status == STATUS_OK);
    ck_assert(str2.Length == -1); // Null bytestring always decoded -1
    //// Degraded read
    status = SOPC_String_Read(NULL, buffer);
    ck_assert(status != STATUS_OK);
    status = SOPC_String_Read(&str, NULL);
    ck_assert(status != STATUS_OK);
    status = SOPC_String_Read(&str, buffer); // Nothing to read anymore
    ck_assert(status != STATUS_OK);

    SOPC_String_Clear(&str);
    SOPC_String_Clear(&str2);

    //////////////////////////////////////////
    // Test XmlElement nominal and degraded cases
    //// Nominal write
    SOPC_Buffer_Reset(buffer);
    SOPC_XmlElement xmlElt;
    SOPC_XmlElement_Initialize(&xmlElt);
    SOPC_XmlElement xmlElt2;
    SOPC_XmlElement_Initialize(&xmlElt2);
    uint8_t balA[3] = {0x3C, 0x41, 0x3E}; // <A>

    ////// Empty string
    xmlElt.Length = 0;
    status = SOPC_XmlElement_Write(&xmlElt, buffer);
    ck_assert(status == STATUS_OK);
    ////// -1 length must be encoded for null string
    ck_assert(buffer->data[0] == 0xFF);
    ck_assert(buffer->data[1] == 0xFF);
    ck_assert(buffer->data[2] == 0xFF);
    ck_assert(buffer->data[3] == 0xFF);

    SOPC_Buffer_Reset(buffer);
    xmlElt.Length = -1;
    status = SOPC_XmlElement_Write(&xmlElt, buffer);
    ck_assert(status == STATUS_OK);
    ////// -1 length must be encoded for null string
    ck_assert(buffer->data[0] == 0xFF);
    ck_assert(buffer->data[1] == 0xFF);
    ck_assert(buffer->data[2] == 0xFF);
    ck_assert(buffer->data[3] == 0xFF);

    SOPC_Buffer_Reset(buffer);
    xmlElt.Length = -10;
    status = SOPC_XmlElement_Write(&xmlElt, buffer);
    ck_assert(status == STATUS_OK);
    ////// -1 length must be encoded for null string
    ck_assert(buffer->data[0] == 0xFF);
    ck_assert(buffer->data[1] == 0xFF);
    ck_assert(buffer->data[2] == 0xFF);
    ck_assert(buffer->data[3] == 0xFF);

    /////// Non empty bytestring
    SOPC_Buffer_Reset(buffer);
    xmlElt.Data = malloc(sizeof(SOPC_Byte) * 3);
    ck_assert(xmlElt.Data != NULL);
    ck_assert(memcpy(xmlElt.Data, balA, 3) == xmlElt.Data);
    xmlElt.Length = 3;
    status = SOPC_XmlElement_Write(&xmlElt, buffer);
    ck_assert(status == STATUS_OK);
    ck_assert(buffer->data[0] == 0x03);
    ck_assert(buffer->data[1] == 0x00);
    ck_assert(buffer->data[2] == 0x00);
    ck_assert(buffer->data[3] == 0x00);
    ck_assert(buffer->data[4] == 0x3C);
    ck_assert(buffer->data[5] == 0x41);
    ck_assert(buffer->data[6] == 0x3E);


    //// Degraded write
    status = SOPC_XmlElement_Write(NULL, buffer);
    ck_assert(status != STATUS_OK);
    status = SOPC_XmlElement_Write(&xmlElt, NULL);
    ck_assert(status != STATUS_OK);
    bufferFull->position = 2; // Set buffer almost full (6 byte left)
    status = SOPC_XmlElement_Write(&xmlElt, bufferFull);
    ck_assert(status != STATUS_OK);

    //// Nominal read
    SOPC_Buffer_SetPosition(buffer, 0); // Reset position for reading
    SOPC_XmlElement_Clear(&xmlElt2);
    SOPC_XmlElement_Initialize(&xmlElt2);
    status = SOPC_XmlElement_Read(&xmlElt2, buffer);
    ck_assert(status == STATUS_OK);
    ck_assert(memcmp(xmlElt.Data, xmlElt2.Data, 3) == 00);
    ck_assert(xmlElt2.Length == 3);
    ck_assert(xmlElt2.Data[0] == 0x3C);
    ck_assert(xmlElt2.Data[1] == 0x41);
    ck_assert(xmlElt2.Data[2] == 0x3E);

    ////// Read 0 length bytestring
    buffer->data[0] = 0x00;
    buffer->data[1] = 0x00;
    buffer->data[2] = 0x00;
    buffer->data[3] = 0x00;
    SOPC_Buffer_SetPosition(buffer, 0); // Reset position for reading
    SOPC_XmlElement_Clear(&xmlElt2);
    SOPC_XmlElement_Initialize(&xmlElt2);
    status = SOPC_XmlElement_Read(&xmlElt2, buffer);
    ck_assert(status == STATUS_OK);
    ck_assert(xmlElt2.Length == -1); // Null bytestring always decoded -1

    ////// Read negative length bytestring
    buffer->data[0] = 0xFF;
    buffer->data[1] = 0x00;
    buffer->data[2] = 0x00;
    buffer->data[3] = 0xFF;
    SOPC_Buffer_SetPosition(buffer, 0); // Reset position for reading
    SOPC_XmlElement_Clear(&xmlElt2);
    SOPC_XmlElement_Initialize(&xmlElt2);
    status = SOPC_XmlElement_Read(&xmlElt2, buffer);
    ck_assert(status == STATUS_OK);
    ck_assert(xmlElt2.Length == -1); // Null bytestring always decoded -1

    ////// Read -1 length bytestring
    buffer->data[0] = 0xFF;
    buffer->data[1] = 0xFF;
    buffer->data[2] = 0xFF;
    buffer->data[3] = 0xFF;
    SOPC_Buffer_SetPosition(buffer, 0); // Reset position for reading
    SOPC_XmlElement_Clear(&xmlElt2);
    SOPC_XmlElement_Initialize(&xmlElt2);
    status = SOPC_XmlElement_Read(&xmlElt2, buffer);
    ck_assert(status == STATUS_OK);
    ck_assert(xmlElt2.Length == -1); // Null bytestring always decoded -1
    //// Degraded read
    status = SOPC_XmlElement_Read(NULL, buffer);
    ck_assert(status != STATUS_OK);
    status = SOPC_XmlElement_Read(&xmlElt, NULL);
    ck_assert(status != STATUS_OK);
    status = SOPC_XmlElement_Read(&xmlElt, buffer); // Nothing to read anymore
    ck_assert(status != STATUS_OK);

    SOPC_XmlElement_Clear(&xmlElt);
    SOPC_XmlElement_Clear(&xmlElt2);

    // Resize "full" msg buffer
    /////////////////////////////////////////
    SOPC_Buffer_Delete(bufferFull);
    bufferFull = SOPC_Buffer_Create(32);
    /////////////////////////////////////////
    // Test GUID nominal and degraded cases
    //// Nominal write
    SOPC_Buffer_Reset(buffer);
    SOPC_Guid guid;
    SOPC_Guid_Initialize(&guid);
    SOPC_Guid guid2;
    SOPC_Guid_Initialize(&guid2);
    guid.Data1 = 0x72962B91;
    guid.Data2 = 0xFA75;
    guid.Data3 = 0x4ae6;
    guid.Data4[0] = 0x8D;
    guid.Data4[1] = 0x28;
    guid.Data4[2] = 0xB4;
    guid.Data4[3] = 0x04;
    guid.Data4[4] = 0xDC;
    guid.Data4[5] = 0x7D;
    guid.Data4[6] = 0xAF;
    guid.Data4[7] = 0x63;

    status = SOPC_Guid_Write(&guid, buffer);
    ck_assert(status == STATUS_OK);
    ck_assert(buffer->data[0] == 0x91);
    ck_assert(buffer->data[1] == 0x2B);
    ck_assert(buffer->data[2] == 0x96);
    ck_assert(buffer->data[3] == 0x72);
    ck_assert(buffer->data[4] == 0x75);
    ck_assert(buffer->data[5] == 0xFA);
    ck_assert(buffer->data[6] == 0xE6);
    ck_assert(buffer->data[7] == 0x4A);
    ck_assert(buffer->data[8] == 0x8D);
    ck_assert(buffer->data[9] == 0x28);
    ck_assert(buffer->data[10] == 0xB4);
    ck_assert(buffer->data[11] == 0x04);
    ck_assert(buffer->data[12] == 0xDC);
    ck_assert(buffer->data[13] == 0x7D);
    ck_assert(buffer->data[14] == 0xAF);
    ck_assert(buffer->data[15] == 0x63);


    //// Degraded write
    status = SOPC_Guid_Write(NULL, buffer);
    ck_assert(status != STATUS_OK);
    status = SOPC_Guid_Write(&guid, NULL);
    ck_assert(status != STATUS_OK);
    bufferFull->position = 17; // Set buffer almost full (15 byte left)
    status = SOPC_Guid_Write(&guid, bufferFull);
    ck_assert(status != STATUS_OK);

    //// Nominal read
    SOPC_Buffer_SetPosition(buffer, 0); // Reset position for reading
    status = SOPC_Guid_Read(&guid2, buffer);
    ck_assert(status == STATUS_OK);
    ck_assert(memcmp(&guid, &guid2, sizeof(SOPC_Guid)) == 0);

    //// Degraded read
    status = SOPC_Guid_Read(NULL, buffer);
    ck_assert(status != STATUS_OK);
    status = SOPC_Guid_Read(&guid, NULL);
    ck_assert(status != STATUS_OK);
    status = SOPC_Guid_Read(&guid, buffer); // Nothing to read anymore
    ck_assert(status != STATUS_OK);

    SOPC_Guid_Clear(&guid);
    SOPC_Guid_Clear(&guid2);

    /////////////////////////////////////////
    // Test NodeId nominal and degraded cases
    //// Nominal write
    SOPC_Buffer_Reset(buffer);
    SOPC_NodeId nodeId;
    SOPC_NodeId_Initialize(&nodeId);
    SOPC_NodeId nodeId2;
    SOPC_NodeId_Initialize(&nodeId2);

    // Two bytes node id
    nodeId.IdentifierType = IdentifierType_Numeric;
    nodeId.Data.Numeric = 114;
    status = SOPC_NodeId_Write(&nodeId, buffer);
    ck_assert(status == STATUS_OK);
    ck_assert(buffer->data[0] == 0x00);
    ck_assert(buffer->data[1] == 0x72);

    // Four bytes node id
    nodeId.IdentifierType = IdentifierType_Numeric;
    nodeId.Namespace = 5;
    nodeId.Data.Numeric = 1025;
    status = SOPC_NodeId_Write(&nodeId, buffer);
    ck_assert(status == STATUS_OK);
    ck_assert(buffer->data[2] == 0x01);
    ck_assert(buffer->data[3] == 0x05);
    ck_assert(buffer->data[4] == 0x01);
    ck_assert(buffer->data[5] == 0x04);

    // Numeric node id
    nodeId.IdentifierType = IdentifierType_Numeric;
    nodeId.Namespace = 5;
    nodeId.Data.Numeric = 0x1FFFF;
    status = SOPC_NodeId_Write(&nodeId, buffer);
    ck_assert(status == STATUS_OK);
    ck_assert(buffer->data[6] == 0x02);
    ck_assert(buffer->data[7] == 0x05);
    ck_assert(buffer->data[8] == 0x00);
    ck_assert(buffer->data[9] == 0xFF);
    ck_assert(buffer->data[10] == 0xFF);
    ck_assert(buffer->data[11] == 0x01);
    ck_assert(buffer->data[12] == 0x00);

    // TODO: write all other types possibles !

    //// Degraded write
    status = SOPC_NodeId_Write(NULL, buffer);
    ck_assert(status != STATUS_OK);
    status = SOPC_NodeId_Write(&nodeId, NULL);
    ck_assert(status != STATUS_OK);
    bufferFull->position = 26; // Set buffer almost full (6 byte left)
    status = SOPC_NodeId_Write(&nodeId, bufferFull);
    ck_assert(status != STATUS_OK);

    //// Nominal read
    ////// Two bytes NodeId
    SOPC_Buffer_SetPosition(buffer, 0); // Reset position for reading
    status = SOPC_NodeId_Read(&nodeId2, buffer);
    ck_assert(status == STATUS_OK);
    ck_assert(nodeId2.Namespace == 0);
    ck_assert(nodeId2.Data.Numeric == 114);

    ////// Four bytes NodeId
    SOPC_NodeId_Clear(&nodeId2);
    status = SOPC_NodeId_Read(&nodeId2, buffer);
    ck_assert(status == STATUS_OK);
    ck_assert(nodeId2.Namespace == 5);
    ck_assert(nodeId2.Data.Numeric == 1025);

    ////// Numeric NodeId
    SOPC_NodeId_Clear(&nodeId2);
    status = SOPC_NodeId_Read(&nodeId2, buffer);
    ck_assert(status == STATUS_OK);
    ck_assert(nodeId2.Namespace == 5);
    ck_assert(nodeId2.Data.Numeric == 0x1FFFF);

    // TODO: read all other types possibles !

    //// Degraded read
    status = SOPC_NodeId_Read(NULL, buffer);
    ck_assert(status != STATUS_OK);
    status = SOPC_NodeId_Read(&nodeId, NULL);
    ck_assert(status != STATUS_OK);
    status = SOPC_NodeId_Read(&nodeId, buffer); // Nothing to read anymore
    ck_assert(status != STATUS_OK);

    SOPC_NodeId_Clear(&nodeId);
    SOPC_NodeId_Clear(&nodeId2);


    SOPC_Buffer_Delete(buffer);
    SOPC_Buffer_Delete(bufferFull);
}
END_TEST

Suite *tests_make_suite_core_tools(void)
{
    Suite *s;
    TCase *tc_encoder;

    s = suite_create("Core Tools");
    tc_encoder = tcase_create("UA Encoder");
    tcase_add_test(tc_encoder, test_ua_encoder_endianess_mgt);
    tcase_add_test(tc_encoder, test_ua_encoder_basic_types);
    tcase_add_test(tc_encoder, test_ua_encoder_other_types);
    suite_add_tcase(s, tc_encoder);

    return s;
}
