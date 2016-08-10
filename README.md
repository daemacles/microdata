# About

A few functions to make sending data to/from a microcontroller over the UART a
bit easier.

# Usage

## To use in your own project:

Copy `microdata.c` and `microdata.h` to your project.

## To run the tests:

    $ mkdir build
    $ cd build
    $ cmake -DCMAKE_RELEASE_TYPE=[Debug|Release|RelWithDebInfo] ..
    $ make

## Example:

On the microcontroller

```c
typedef struct {
  uint8_t val1;
  uint8_t val2;
  int16_t val3;
  uint32_t val4;
} MyData;

void SendData () {
  MyData data = {
    .val1 = 42,
    .val2 = 55,
    .val3 = 24000,
    .val4 = 456234234
  };

  uint8_t to_send[sizeof(MyData)];
  uint8_t *cursor = to_send;
  cursor = PackUint8 (cursor, data.val1);
  cursor = PackUint8 (cursor, data.val2);
  cursor = PackInt16 (cursor, data.val3);
  cursor = PackUint32 (cursor, data.val4);
  uint8_t packed_size = cursor - to_send;
  
  uint8_t slip_buffer[2*sizeof(MyData)];
  uint8_t len = SlipEncode(to_send, slip_buffer, packed_size);
  UartSendBytes(slip_buffer, len);
}
```

On the PC

```c
void ProcessAllTheData () {
  uint8_t rx_buffer[100];
  uint8_t count = 0;
  while (1) {
    rx_buffer[count] = GetByteFromUart();
    if (rx_buffer[count] == SLIP_FRAME_END) {
      int len = SlipDecode(out_buffer, in_buffer, sizeof(in_buffer));
      uint8_t *cursor = in_buffer;
      MyData out_data;
      cursor = UnpackUint8 (cursor, &out_data.val1);
      cursor = UnpackUint8 (cursor, &out_data.val2);
      cursor = UnpackInt16 (cursor, &out_data.val3);
      cursor = UnpackUint32 (cursor, &out_data.val4);
      count = 0;
      
      // Process out_data...
    } else {
      count++;
    }
    
    if (count > sizeof(rx_buffer)) {
      // ERROR: Overflow!
      count = 0;
    }
  }
}
```
