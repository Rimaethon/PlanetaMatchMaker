# Server Message API

This is reference about the message API of the server.

## Message

Message is fixed size binary data to communicate between server and client.

There are three types of message.

- Request: A message from a client which requires one or more replies from the server
- Notice: A message from a client which doesn't require a reply from the server
- Reply: A message from a server to response to the request message from a client

## Communication Flow

To communicate with the server, you should follow below flow.

1. [Client] Connect to the server by TCP
1. [Client] Send authentication request and get a session key
1. Message roop
    1. [Client] Send requests or notices you need with the session key
    1. [Server] Reply to the request with one or more messages
    1. [Client] Process reply

In below situation, the server forces to close the connection immediately without any reply.

- Send invalid message type
- Send not authentication request at first time after connection
- Server internal error occured

## Message Structure

All messages consist of header and body.
The sizes of all messages are in less than or equals 256 bytes.

There are two types of header.

- Request Header: A header for request message and notice message
- Reply Header: A header for reply message

If errors occured while handling messages, the server returns only reply message header with error code.

## Message Header Structure

### Request/Notice Header

The size is 5 bytes.

|Name|Type|Size|Explanation|
|:---|:---|---:|:---|
|message_type|8 bits unsigned integer|1|A type of message.|
|session_key|32 bits unsigned integer|4|A session key which is generated when authentication.|

Options of `message_type` are below.

|Name|Type|Value|
|:---|:---|---:|
|authentication|request|0|
|create_room|request|2|
|list_room|request|4|
|join_room|request|6|
|update_room_status|notice|8|
|connection_test|request|9|
|random_match|request|11|
|keep_alive|notice|12|

### Reply Header

The size is 2 bytes.

|Name|Type|Size|Explanation|
|:---|:---|---:|:---|
|message_type|8 bits unsigned integer|1|A type of message.|
|error_code|8 bits unsigned integer|1|An error code.|

Options of `message_type` are same as one used in request header.

Options of `error_code` are as below.

|Name|Value|Explanation|
|:---|---:|:---|
|ok|0|Request is processed successfully.|
|server_error|1|Server internal error.|
|api_version_mismatch|2|Server api version doesn't match to the version the client required.|
|operation_invalid|3|The operation is invalid in current state.|
|room_not_found|4|Indicated room is not found.|
|request_parameter_wrong|5|Wrong parameters which must be rejected in the client is passed for request.|
|room_password_wrong|6|Indicated password of room is not correct.|
|room_full|7|The number of player reaches limit.|
|room_permission_denied|8|Request is rejected because indicated room is the room which you are not host of or closed.|
|room_count_exceeds_limit|9|The number of room reaches limit.|
|client_already_hosting_room|10|Request is failed because the client is already hosting room.|

## Message Body Structure

### Authentication Request

A request to authenticate and get a session key.

#### Parameters

The size is 26 bytes.

|Name|Type|Size|Explanation|
|:---|:---|---:|:---|
|version|16 bits unsigned integer|2|An API version number the client requires.|
|player_name_t|24 byte length UTF-8 string|24|A name of player. This must not be empty.|

#### Reply

The size is 8 bytes.

|Name|Type|Size|Explanation|
|:---|:---|---:|:---|
|version|16 bits unsigned integer|2|An API version number of the server.|
|session_key|32 bits unsigned integer|4|A generated session key.|
|player_tag|16 bits unsigned integer|2|A tag number of player to avoid duplication of player name.|

#### Error Codes

|Name|Condition|Continuable|
|:---|:---|:---|
|ok|The request is processed succesfully.|yes|
|api_version_mismatch|An API version of server is different from what the client required.|no|
|request_parameter_wrong|A player name is empty.|no|
|operation_invalid|Authentication request is send more than twice.|no|

### Create Room Request

A request to create room.

#### Parameters

The size is 84 bytes.

|Name|Type|Size|Explanation|
|:---|:---|---:|:---|
|password|16 byte length UTF-8 string|16|A password of room you create. If this is empty, the room is created as a public room.|
|max_player_count|8 bits unsigned integer|1|A limit of player count in the room. This must not exceeds the limit which is defined in server setting.|
|connection_establish_mode|8 bits unsigned integer|1|A way how to establish P2P connection.|
|port_number|16 bits unsigned integer|2|A port number which is used for game host. 49152 to 65535 is available. This is used when `connection_establish_mode` is `builtin`.|
|external_id|64 elements byte array.|64|An id where clients connect using external service like Steam Networking. This is used when `connection_establish_mode` is not `builtin`. This is left justified and big endien.|

Options of `connection_establish_mode` are as below.

|Name|Value|Host Identifier|Explanation|
|:---|---:|:---|:---|
|builtin|0|`port_number` property|Use builtin method.|
|steam|1|`external_id` property containing SteamID64 as 64bits unsigned integer|Use Steam relay service.|
|others|255|`external_id` property|Use other external service.|

#### Reply

The size is 4 bytes.

|Name|Type|Size|Explanation|
|:---|:---|---:|:---|
|room_id|32 bits unsigned integer|4|An id of the room created.|

#### Error Codes

|Name|Condition|Continuable|
|:---|:---|:---|
|ok|The request is processed succesfully.|yes|
|client_already_hosting_room|Failed to host new room because the client already hosting room.|yes|
|room_count_exceeds_limit|The number of room exceeds limit.|yes|
|request_parameter_wrong|Max player count exceeds limit. Or indicated port number is invalid.|yes|

### List Room Request

A request to get room informations which matches to requested parameters.

#### Parameters

The size is 30 bytes.

|Name|Type|Size|Explanation|
|:---|:---|---:|:---|
|start_index|16 bits unsigned integer|2|A start index of room data which will be replied from search results.|
|count|16 bits unsigned integer|2|The number of room data which will be replied from search results.|
|sort_kind|8 bits unsigned integer|1|A sort kind of result.|
|search_target_flags|8 bits unsigned integer|1|A flags to indicate search target.|
|search_full_name|player_full_name|24|A query to search room by the room's host player name.|

Options of `sort_kind` are as below.

|Name|Value|
|:---|---:|
|name_ascending|0|
|name_descending|1|
|create_datetime_ascending|2|
|create_datetime_descending|3|

`search_target_flags` are treated as bit flags.
Options are as below.

|Name|Value|
|:---|---:|
|public_room|1|
|private_room|2|
|open_room|4|
|closed_room|8|

`player_full_name` is 24 bytes data as below.

|Name|Type|Size|Explanation|
|:---|:---|---:|:---|
|name|24 byte length UTF-8 string|24|A name of player.|
|tag|16 bits unsigned integer|2|A tag of player to avoid name duplication.|

#### Reply

The size is 246 bytes.

|Name|Type|Size|Explanation|
|:---|:---|---:|:---|
|total_room_count|16 bits unsigned integer|2|The number of rooms existing in the room group in the server.|
|matched_room_count|16 bits unsigned integer|2|The number of rooms which match to the query of the room.|
|reply_room_count|16 bits unsigned integer|2|The number of rooms which is included in reply messages.|
|room_info_list|A 6 elements array of room_info|240|A result room info list.|

`room_info` is 40 bytes data as below.

|Name|Type|Size|Explanation|
|:---|:---|---:|:---|
|room_id|32 bits unsigned integer|4|An id of the room.|
|host_player_full_name|player_full_name|24|A name of player who is hosting the room.|
|setting_flags|8 bits unsigned integer|1|A flags which indicate a setting of the room.|
|max_player_count|8 bits unsigned integer|1|Player capability of this room.|
|current_player_count|8 bits unsigned integer|1|The number of player which joins the room currently.|
|create_datetime|64 bits unsigned integer which indicates unix time|8|A datetime the room created.|
|connection_establish_mode|8 bits unsigned integer|1|A way how to establish P2P connection in the room.|

`setting_flags` are treated as bit flags.
Options are as below.

|Name|Value|
|:---|---:|
|public_room|1|
|open_room|2|

Multiple reply messages are sent if there are more rooms than rooms one reply message can send.
You can obtain the number of reply message (separation) by below expression.

```cpp
separation = floor((reply.reply_room_count + 5) / 6);
```

#### Error Codes

|Name|Condition|Continuable|
|:---|:---|:---|
|ok|The request is processed succesfully.|yes|
|request_parameter_wrong|sort_kind is invalid.|yes|

### Join Room Request

A request to get the information to join the room.

#### Parameters

The size is 21 bytes.

|Name|Type|Size|Explanation|
|:---|:---|---:|:---|
|room_id|32 bits unsigned integer|4|An id of the room you want to join.|
|connection_establish_mode|8 bits unsigned integer|1|An expected way how to establish P2P connection in the room.|
|password|16 byte length UTF-8 string|16|A password of the room you want to join. This is only refered when indicated room is private.|

#### Reply

The size is 82 bytes.

|Name|Type|Size|Explanation|
|:---|:---|---:|:---|
|game_host_endpoint|endpoint|18|An endpoint of game host which is hosting the room you want to join.|
|game_host_external_id|64 elements byte array.|64|An id to connect to the host using external service like Steam Networking. This is left justified and big endien.|

`game_host_endpoint` is 18 bytes data as below.

|Name|Type|Size|Explanation|
|:---|:---|---:|:---|
|ip_address|A 16 elements array of 8 bits unsigned integer|16|A IP address by big endian. In IPv4, IPv4-Mapped IPv6 is used. (example, `::ffff:192.0.0.1`)|
|port_number|16 bits unsigned integer|2|A port number.|

#### Error Codes

|Name|Condition|Continuable|
|:---|:---|:---|
|ok|The request is processed succesfully.|yes|
|room_not_found|Indicated room doesn't exist.|yes|
|room_permission_denied|Indicated room is closed.|yes|
|room_password_wrong|Indicated password is wrong.|yes|
|room_full|The number of player reaches limit.|yes|
|room_connection_establish_mode_mismatch|Connection establish mode of the room host doesn't match expected one in the client.|yes|

### Update Room Status Notice

A notice to inform new status of the room the client hosts.

#### Parameters

The size is 7 bytes.

|Name|Type|Size|Explanation|
|:---|:---|---:|:---|
|room_id|32 bits unsigned integer|4|An id of the room you want to join.|
|status|8 bits unsigned integer|1|A new status of the room.|
|is_current_player_count_changed|boolean|1|A flag which indicates if playr count is updated.|
|current_player_count|8 bits unsigned integer|1|A new player count.|

Options of `status` are as below.

|Name|Value|
|:---|---:|
|open|0|
|close|1|
|remove|2|

#### Error Conditions

Notice message is ignoreed if there are some errors in processing message.

|Condition|Continuable|
|:---|:---|
|Indicated room doesn't exist.|yes|
|The host of indicated room is not you.|yes|
|The number of new player count is invalid.|yes|
|The `status` parameter is invalid.|yes|

### Connection Test Request

A request to check the client is reachable from the internet.

#### Parameters

The size is 3 bytes.

|Name|Type|Size|Explanation|
|:---|:---|---:|:---|
|protocol|8 bits unsigned integer|1|A transport prptocol to use for connection test.|
|port_number|16 bits unsigned integer|2|A port number to use for connection test. 49152 to 65535 is available.|

Options of `prptocol` are as below.

|Name|Value|
|:---|---:|
|TCP|0|
|UDP|1|

#### Reply

The size is 1 byte.

|Name|Type|Size|Explanation|
|:---|:---|---:|:---|
|succeed|boolean|1|Whether the connection test is succeeded.|

#### Error Codes

|Name|Condition|Continuable|
|:---|:---|:---|
|ok|The request is processed succesfully.|yes|
|request_parameter_wrong|Indicated protocol or port number is invalid.|yes|

### Random Match Request

Not implemented now.

### Keep Alive Notice

A notice to notice alival of the client.

#### Parameters

The size is 1 byte.

|Name|Type|Size|Explanation|
|:---|:---|---:|:---|
|dummy|8 bits unsigned integer|1|Dummy data.|

#### Error Conditions

The server does nothing for this notice so there are no errors.
