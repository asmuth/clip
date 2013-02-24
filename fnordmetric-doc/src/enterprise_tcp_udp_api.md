TCP, UDP, WebSockets API
------------------------

protocol is line based, neither request nor response are allowed to
contain \n characters, but both request and response have to end with
a newline (\n) character.

#### TCP / WebSockets:

there is no handshake, you can just open the connection and start sending
commands. the protocol does not support multiplexing/pipelining: after every
newline-terminated command you send you have to read one line from the socket.

websocket connections also speak the tcp protocol after the connection upgrade

the generic error response is

    ERROR something went wrong\n


#### UDP:

you can put multiple lines in one udp packet.
example udp packet that increments four counters:

    SAMPLE my_counter1.sum-3600 123\n
    SAMPLE my_counter2.sum-3600 456\n
    SAMPLE my_counter3.sum-3600 789\n



### Command: SAMPLE

samples/adds a value to a metric

*Format:*

    SAMPLE [metric] [value]

*Response:*

    "OK"

*Example:*

    >> SAMPLE my_application.response_time.avg-30 23
    << OK


### Command: VALUE_AT

retrieves the value of a metric at one point in time

*Format:*

    VALUE_AT [metric] [at]

*Response:*

    Float

*Example:*

     >> VALUE_AT my_application.response_time.avg-30 1382341536
     << 17.42


### Command: VALUES_IN

Retrieves all values of a metric in a time interval

*Format:*

    VALUE_AT [metric] [from] [until]

*Response:*

    Comma-seperated Timestamp:Float tuples

*Example:*

     >> VALUE_AT my_application.response_time.avg-30 13823534644 13823414323
     << 1360804571:4233.52,1360804581:4312.36,1360804591:6323.12,



