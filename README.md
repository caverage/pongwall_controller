# Controller

## Protocol Specification

### Packets
All data is encapsulated in packets. This includes `ack` messages, video frames, etc.

*Control Byte:* All packet framing is handled using the Control Byte `0x10`.

*Control Modifier:* The byte immediately following the Control Byte is known as the Control Modifier. A Control Modifier MUST be preceded by a Control Byte.

*Control Code:* A Control Byte followed by a Control Modifier is referred to as a Control Code. Control Codes are referred to by the Control Modifier. E.G. the Control Code `0xAA` is `0x10 0xAA`.

*Start Marker:* Packets MUST begin with the Control Code `0x01`, known as the Start Marker.

*Literal Control Byte:* To send a literal `0x10` as part of the data portion of the packet, the Control Code `0x10` MUST be sent.

*End Marker:* Packets MUST end with the Control Code `0x02`, known as the End Marker.

*Data:* The data portion of the packet is sent between the Start Marker and the End Marker.

### PongWall Frame
Each frame for the PongWall is sent in a single packet of data.

The controller will ask for a frame by sending `0x01` to the server.

The server will send the frame as a single packet.

The controller will send an ACK (`0x06`) when the end sequence is received.
