/////////////////////////////////////////////////////////////////////////////
//                                 microdata.c
/////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2016 Jim Youngquist under the MIT license, see LICENSE
//

#include "microdata.h"

#ifdef __cplusplus
extern "C" {
#endif

/////////////////////////////////////////////////////////////////////////////
//                                    Pack*                                   
///////////////////////////////////////////////////////////////////////////// 
#define PACK_SHIFT(Bytes) \
  *buffer++ = (uint8_t) ((value >> (8*Bytes)) & 0xFF)

#define PACK1 PACK_SHIFT(0)
#define PACK2 PACK_SHIFT(0); PACK_SHIFT(1)
#define PACK4 PACK_SHIFT(0); PACK_SHIFT(1); PACK_SHIFT(2); PACK_SHIFT(3)

uint8_t* PackUint8  (uint8_t* buffer, uint8_t value)  { PACK1; return buffer; }
uint8_t* PackUint16 (uint8_t* buffer, uint16_t value) { PACK2; return buffer; }
uint8_t* PackUint32 (uint8_t* buffer, uint32_t value) { PACK4; return buffer; }
uint8_t* PackInt8   (uint8_t* buffer, int8_t value)   { PACK1; return buffer; }
uint8_t* PackInt16  (uint8_t* buffer, int16_t value)  { PACK2; return buffer; }
uint8_t* PackInt32  (uint8_t* buffer, int32_t value)  { PACK4; return buffer; }


/////////////////////////////////////////////////////////////////////////////
//                                   Unpack*                                  
/////////////////////////////////////////////////////////////////////////////
#define UNPACK_SHIFT(Bytes, Type) \
  *value += (Type)(buffer[Bytes] << (8*Bytes))

#define UNPACK1(Type) UNPACK_SHIFT(0, Type); buffer += 1
#define UNPACK2(Type) UNPACK_SHIFT(0, Type); UNPACK_SHIFT(1, Type); buffer += 2
#define UNPACK4(Type) UNPACK_SHIFT(0, Type); UNPACK_SHIFT(1, Type); \
                      UNPACK_SHIFT(2, Type); UNPACK_SHIFT(3, Type); buffer += 4

uint8_t* UnpackUint8  (uint8_t* buffer, uint8_t  *value) 
{ *value = 0; UNPACK1(uint8_t); return buffer; }
uint8_t* UnpackUint16 (uint8_t* buffer, uint16_t *value) 
{ *value = 0; UNPACK2(uint16_t); return buffer; }
uint8_t* UnpackUint32 (uint8_t* buffer, uint32_t *value) 
{ *value = 0; UNPACK4(uint32_t); return buffer; }
uint8_t* UnpackInt8   (uint8_t* buffer, int8_t   *value)
{ *value = 0; UNPACK1(int8_t); return buffer; }
uint8_t* UnpackInt16  (uint8_t* buffer, int16_t  *value) 
{ *value = 0; UNPACK2(int16_t); return buffer; }
uint8_t* UnpackInt32  (uint8_t* buffer, int32_t  *value) 
{ *value = 0; UNPACK4(int32_t); return buffer; }


/////////////////////////////////////////////////////////////////////////////
//                                     SLIP                                    
/////////////////////////////////////////////////////////////////////////////
#define SLIP_END SLIP_FRAME_END
#define SLIP_ESC 0xDB
#define SLIP_ESC_END 0xDC
#define SLIP_ESC_ESC 0xDD


//---------------------------------------------------------------------------
SIZE_T SlipEncode (const uint8_t* src, uint8_t* dest, SIZE_T size)
{
  uint8_t *start = dest;
  // Any SLIP special characters seen in bytes need to be escaped.
  for (SIZE_T idx=0; idx != size; ++idx) {
    uint8_t next_byte = src[idx];
    switch(next_byte) {
    case SLIP_END:
      *dest++ = SLIP_ESC;
      *dest++ = SLIP_ESC_END;
      break;
    case SLIP_ESC:
      *dest++ = SLIP_ESC;
      *dest++ = SLIP_ESC_ESC;
      break;
    default:
      *dest++ = next_byte;
    }
  }
  *dest++ = SLIP_END;
  return dest - start;
}


//---------------------------------------------------------------------------
enum SlipState {
  ERROR,
  NORMAL,
  FINISHED,
  IN_ESC
};

SIZE_T SlipDecode (const uint8_t* src, uint8_t* dest, SIZE_T capacity)
{
  enum SlipState state = NORMAL;
  SIZE_T len = 0;

  while (state != FINISHED) {
    uint8_t byte = *src++;

    switch(state) {
    case ERROR:
      return 0;
      break;

    case NORMAL:
      switch(byte) {
      case SLIP_ESC: // begin escape sequence
        state = IN_ESC;
        break;

      case SLIP_END: // found the end of the data
        state = FINISHED;
        break;

      default:
        state = NORMAL;
        ++len;
        // Check for overflow of destination buffer
        if (len > capacity) {
          state = ERROR;
          break;
        }
        *dest++ = byte;
        break;
      }
      break;

    case IN_ESC:
      switch(byte) {
      case SLIP_ESC_END:
        state = NORMAL;
        ++len;
        // Check for overflow of destination buffer
        if (len > capacity) {
          state = ERROR;
          break;
        }
        *dest++ = SLIP_END;
        break;

      case SLIP_ESC_ESC:
        state = NORMAL;
        ++len;
        // Check for overflow of destination buffer
        if (len > capacity) {
          state = ERROR;
          break;
        }
        *dest++ = SLIP_ESC;
        break;

      default:
        state = ERROR;
        break;
      }
      break;

    case FINISHED:
      break;
    }
  }

  return len;
}

#ifdef __cplusplus
}
#endif
