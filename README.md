# IPK 2023 - Project 2 
# Variant IOTA - Server with remote calculator

## Introduction

This project implements server able to communicate over UDP and TCP.
Furthermore the server contains calculator that calculates expressions in prefix notation.

Grammar for expression notation looks as following:

```
start -> L_PAR OPERATOR <args>
args -> SPACE <expr> SPACE <expr> <margs>
expr -> OPERAND | <start>
margs -> R_PAR | SPACE <expr> <margs>
```
Where
- L_PAR is `'('`
- R_PAR is `')'`
- SPACE is `' '`
- OPERATOR is one of [`'+'`, `'-'`, `'*'`, `'/'`]
- OPERAND is and integer value (such as `'42'`)

So valid expression could look like this `(+ 1 (* 5 2))`.


## Run

First compile server by entering following command to terminal
```console
$ make
```
Next you can run the server by using:
```console
$ ./ipkcpd -h <host> -p <port> -m <mode>
```
Or get help message by using:
```console
$ ./ipkcpd --help
```

Where:
- host - ipv4 address you want server to run on
- port - port number
- mode - mode of communication, either `tcp` or `udp`

Those arguments are NOT optional and are accepted in any order.

## Implementation
Implementation consists of files contained in this repository.

Those are:
- ***ipkcpd.cpp*** - File containing `main` function and being responsible for argument parsing and starting the server with propper mode.
- ***server.[cpp, h]*** - Module responsible for holding server information such as host address, port number and mode server is running in.
- ***tcp.[cpp, h]*** - Module responsible for handling communication with clients via TCP.
- ***tcpmessage.[cpp, h]*** - Module responsible for providing interface to work with TCP formatted textual messages.
- ***udp.[cpp, h]*** - Module responsible for handling communication with clients via UDP.
- ***udpmessage.[cpp, h]*** - Module responsible for providing interface to work with UDP formatted binary messages.
- ***payloadparser.[cpp, h]*** - Module responsible for checking lexical and syntax validity of expressions (contained in payload). It also provides method for evaluation of those expressions.
- ***tokenizer.[cpp, h]*** - Module responsible for dividing expressions into tokens.
- ***narytree.[cpp, h]*** - Module containing N-ary tree in order to store and evaluate expression.
- ***interrupt.[cpp, h]*** - Module responsible for C-c handling.
- ***error.[cpp, h]*** - Module responsible for printing error messages and exitting with correct code.

### UDP
- On initialization of UDP class from udp module, the constructor initializes server socket of type `DGRAM`, surpresses default behaviour of socket in order to be able to use socket on the same address immediately after terminating server and finally binds socket to server ipv4 address entered as program argument. 
- Afterwards method listen is invoked. This method initializes `C-c` handler and then enters loop where server waits for incomming messages from clients.
- When message is received, `opcode` is validated (should be 0 in case of request), payload is parsed and result returned in response.
- If errors occur via exceptions while parsing payload, respective error message is returned instead of result.

UDP message format is binary and looks as following:
```
REQUEST[opcode(1B), payload_length(1B), payload(255B)]
RESPONSE[opcode(1B), status_code(1B), payload_length(1B), payload(255B)]
```
Where:
- opcode - 0 for request and 1 for response
- status_code - 0 for OK and 1 for ERROR

If longer message is sent it is truncated to fist 258B.

### TCP
- On initialization of TCP class from tcp module, welcome socket is initialized, default behaviour of socket in order to be able to use socket on the same address immediately after terminating server is surpressed, socket is bound to the desired ipv4 address and finally server starts listening for incomming connections.
- Await connection method is invoked, `C-c` handler initialized and loop where server manages clients connections entered.
- When request for connection comes in and accept is successful, client is provided with own thread created with callable object `TCPHandleClient` where his needs are handled.
- Specific client handling inside thread, first loads messages and concatenates them until `LF` appears in message.
- Server then takes this first subquery and validates it against `HELLO` query. If it is not successfully validated, `BYE` response is sent and conncection along with thread terminates.
- If no error occurs, server proceeds to handling clients expressions using `SOLVE` query.
- Server again loads and concatenates messages until `LF` appears and then validates this subquery against `SOLVE` query. If it is not successfully validated, `BYE` response is sent and conncection along with thread terminates.
- If no error occurs, server proceeds to parsing expression in payload. After sending result, the server waits for another `SOLVE` query.

Client can send `BYE` query at any time to terminate connection.
Queries can be split into multiple consecutive messages and also multiple queries can be in one message.

Maximum length of message is 2048 characters (including `LF`). If length is exceeded, the first 2048 characters are taken.

Queries are as following:
```
"HELLO" LF
"BYE" LF
"SOLVE" SPACE <expression> LF
"RESULT" SPACE INT LF
```

### Expression parsing
Expressions are parsed using recursive descent to check syntax validity. Lexical validity is checked while tokenizing the expression. During this descent, N-ary expression tree is formed. Parsing is started by invoking `parse` method. When method `calc` is evoked, the expression tree is recursively evaluated. Evaluation comes in such manner that when child of node is of `OPERAND` token type and has nummeric value, the value is then returned, when it is of `OPERATOR` token type, it's subtree is evaluated first. `OPERATOR` tokens contain method for evaluating respective mathematical operation given arguments are provided.
