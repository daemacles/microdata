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

uint8_t SerializeMyData (const MyData *data, uint8_t* buffer) {
  buffer = PackUint8 (buffer, data.val1);
  buffer = PackUint8 (buffer, data.val2);
  buffer = PackInt16 (buffer, data.val3);
  buffer = PackUint32 (buffer, data.val4);
  uint8_t packed_size = buffer - to_send;
  return packed_size;
}

void SendData () {
  // Two buffers
  uint8_t pack_buffer[sizeof(MyData)];
  uint8_t slip_buffer[2*sizeof(MyData)];

  MyData data = {
    .val1 = 42,
    .val2 = 55,
    .val3 = 24000,
    .val4 = 456234234
  };

  uint8_t packed_size = SerializeMyData(&data, pack_buffer);
  uint8_t send_len = SlipEncode(to_send, slip_buffer, packed_size);
  UartSendBytes(slip_buffer, send_len);

  // Note, you could also have technically done the packing in-place since 
  // packing will never use more space than the struct.  This will save on
  // the space for a special pack buffer, if you are only ever sending one
  // type of data.
  /*
  uint8_t packed_size = SerializeMyData(&data, (uint8_t*)&data);
  uint8_t send_len = SlipEncode((uint8_t*)&data, slip_buffer, packed_size);
  UartSendBytes(slip_buffer, send_len);
  */
}
```

On the PC with the C API

```c
void DeserializeMyData (uint8_t* buffer, MyData *data) {
  buffer = UnpackUint8 (buffer, &data->val1);
  buffer = UnpackUint8 (buffer, &data->val2);
  buffer = UnpackInt16 (buffer, &data->val3);
  buffer = UnpackUint32 (buffer, &data->val4);
}

void ProcessAllTheData () 
{
  uint8_t rx_buffer[100];
  uint8_t idx = 0;

  // Loop forever reading bytes from the UART
  while (1)
  {
    uint8_t next_byte = GetByteFromUart();
    rx_buffer[idx] = next_byte;  // push on buffer
    if (next_byte == SLIP_FRAME_END)
    {
      // Take advantage of the fact that decoding a SLIP frame can be done
      // in place.
      int len = SlipDecode(rx_buffer, rx_buffer, sizeof(rx_buffer));

      if (len != sizeof(MyData)) {
        // Got unexpected amount of data!
        idx = 0;
        continue;
      }

      // Unpack data from the buffer.
      MyData out_data;
      DeserializeMyData (rx_buffer, &out_data);
      
      // Process out_data...
      // 1. Cool thing
      // 2. Other cool thing

      idx = 0;
    } else {
      idx++;
    }
    
    if (idx > sizeof(rx_buffer)) {
      // ERROR: Overflow!
      idx = 0;
    }
  }
}
```

And with C++

```c++
void DeserializeMyData (uint8_t* buffer, MyData *data);  // As above

void ProcessAllTheData () 
{
  std::vector<uint8_t> rx_buffer;

  // Loop forever reading bytes from the UART
  while (1) 
  {
    uint8_t next_byte = GetByteFromUart();

    // save it
    rx_buffer.push_back(next_byte);  

    if (next_byte == SLIP_FRAME_END) 
    {
      // Take advantage of the fact that decoding a SLIP frame can be done
      // in place.
      int len = SlipDecode(rx_buffer.data(), rx_buffer.data(),
                           rx_buffer.size());

      if (len != sizeof(MyData)) {
        // Got unexpected amount of data!
        rx_buffer.clear();
        continue;
      }

      // Unpack data from the buffer.
      MyData out_data;
      DeserializeMyData (rx_buffer.data(), &out_data);
      
      // Process out_data...
      // 1. Cool thing
      // 2. Other cool thing

      rx_buffer.clear();
    }    

    if (rx_buffer.size() > 100) {
      // ERROR: should have seen a frame by now...
      rx_buffer.clear();
    }
  }
}
```
