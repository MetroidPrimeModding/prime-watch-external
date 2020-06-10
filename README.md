## Protocol used for stdin/out

* uint32 message_size
* uint32 message_type
* uint8_t[message_size] data

## Message Types

### `0x0` - get_memory
* uint32 start
* uint32 size

### `0x1` - memory_data
uint8_t[message_size] data

### `0x2` - quit
(no body)