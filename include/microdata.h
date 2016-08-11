/////////////////////////////////////////////////////////////////////////////
//                                 microdata.h
/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2016 Jim Youngquist under the MIT license, see LICENSE
//

#ifndef  MICRODATA_H_
#define  MICRODATA_H_

#include <stdint.h>
#include <stdbool.h>

// Redefine this for something suitable for your architecture
#define SIZE_T uint8_t

#ifdef __cplusplus
extern "C" {
#endif

// Marker for then end of frame.  Once this has come in, you can run
// SlipDecode over your buffer
#define SLIP_FRAME_END 0xC0

///--------------------------------------------------------------------------
/// \brief Encodes some bytes as a SLIP frame.
///
/// NOTE: Encoding CAN NOT be done in place, that is, src and dest MUST be
/// different.
///
/// \param src  The source buffer.
/// \param dest The output buffer containing the SLIP frame.  Make sure there
///             is enough room here... The most conservative estimate is
///             2*size+1.
/// \param size How many bytes from src to encode.
///
/// \returns The number of bytes in the output buffer that were encoded.  That
///          is, the number of bytes to send down the line.
///
SIZE_T SlipEncode (const uint8_t* src, uint8_t* dest, SIZE_T size);


///--------------------------------------------------------------------------
/// \brief Decodes a SLIP frame from a buffer.
///
/// NOTE: Decoding can be done in place, that is, src and dest can be the same
/// thing!
///
/// \param src  The source buffer containing a valid SLIP frame (has
///             SLIP_FRAME_END somewhere in it).
/// \param dest The output buffer with original data.
/// \param capacity How many bytes the output buffer can handle.
///
/// \returns The number of decoded bytes.  0 means there was a problem!
///
SIZE_T SlipDecode (const uint8_t* src, uint8_t* dest, SIZE_T capacity);


///--------------------------------------------------------------------------
/// \brief Packing data into a buffer.
///
/// \param buffer The input source buffer.
/// \param value  The value to put on the buffer.
///
/// \returns A pointer to the next free spot on the buffer.
///
/// Example usage:
///
/// typedef struct {
///   uint8_t val1;
///   uint8_t val2;
///   int16_t val3;
///   uint32_t val4;
/// } MyData;
///
///  MyData data = {
///    .val1 = 42,
///    .val2 = 55,
///    .val3 = 24000,
///    .val4 = 456234234
///  };
///
///  uint8_t to_send[sizeof(MyData)];
///  uint8_t *cursor = to_send;
///  cursor = PackUint8 (cursor, data.val1);
///  cursor = PackUint8 (cursor, data.val2);
///  cursor = PackInt16 (cursor, data.val3);
///  cursor = PackUint32 (cursor, data.val4);
///
uint8_t* PackUint8  (uint8_t* buffer, uint8_t  value);
uint8_t* PackUint16 (uint8_t* buffer, uint16_t value);
uint8_t* PackUint32 (uint8_t* buffer, uint32_t value);
uint8_t* PackInt8   (uint8_t* buffer, int8_t   value);
uint8_t* PackInt16  (uint8_t* buffer, int16_t  value);
uint8_t* PackInt32  (uint8_t* buffer, int32_t  value);

///--------------------------------------------------------------------------
/// \brief Unpacks data from a buffer.
///
/// \param buffer A buffer holding packed values.
/// \param value A pointer to where to store the unpacked value.
///
/// \returns A pointer to the next spot on the buffer.
///
/// Example usage:
/// // Using the MyData definition from Pack* comment above...
///
/// uint8_t rx_buffer[100];  // <--- contains packed data from somewhere
/// uint8_t *cursor = rx_buffer;
/// MyData data;
/// cursor = UnpackUint8 (cursor, &data.val1);
/// cursor = UnpackUint8 (cursor, &data.val2);
/// cursor = UnpackInt16 (cursor, &data.val3);
/// cursor = UnpackUint32 (cursor, &data.val4);
///
uint8_t* UnpackUint8  (uint8_t* buffer, uint8_t  *value);
uint8_t* UnpackUint16 (uint8_t* buffer, uint16_t *value);
uint8_t* UnpackUint32 (uint8_t* buffer, uint32_t *value);
uint8_t* UnpackInt8   (uint8_t* buffer, int8_t   *value);
uint8_t* UnpackInt16  (uint8_t* buffer, int16_t  *value);
uint8_t* UnpackInt32  (uint8_t* buffer, int32_t  *value);

#ifdef __cplusplus
}

// C++ overloads
uint8_t* Pack (uint8_t* buffer, uint8_t value)  { return PackUint8(buffer, value); }
uint8_t* Pack (uint8_t* buffer, uint16_t value) { return PackUint16(buffer, value); }
uint8_t* Pack (uint8_t* buffer, uint32_t value) { return PackUint32(buffer, value); }
uint8_t* Pack (uint8_t* buffer, int8_t value)   { return PackInt8(buffer, value); }
uint8_t* Pack (uint8_t* buffer, int16_t value)  { return PackInt16(buffer, value); }
uint8_t* Pack (uint8_t* buffer, int32_t value)  { return PackInt32(buffer, value); }
uint8_t* Unpack (uint8_t* buffer, uint8_t *value)  { return UnpackUint8(buffer, value); }
uint8_t* Unpack (uint8_t* buffer, uint16_t *value) { return UnpackUint16(buffer, value); }
uint8_t* Unpack (uint8_t* buffer, uint32_t *value) { return UnpackUint32(buffer, value); }
uint8_t* Unpack (uint8_t* buffer, int8_t *value)   { return UnpackInt8(buffer, value); }
uint8_t* Unpack (uint8_t* buffer, int16_t *value)  { return UnpackInt16(buffer, value); }
uint8_t* Unpack (uint8_t* buffer, int32_t *value)  { return UnpackInt32(buffer, value); }

#endif

#endif  // #ifndef  MICRODATA_H_
