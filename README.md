# ft_p
This is a very simple and lightweight server and client for sending data across a network. 
This project uses IPv4 and was developed as an excersize in socket programming. 
The server is multithreaded and simply spawns a new thread to handle each connection.
The client initially connects a control socket, and subsequently negotiates a data socket to handle all requests.

### Build
`git clone https://github.com/jacksonwb/ft_p.git && cd ft_p && make`

### Server
Current working shell directory is used as server root.

`.server port`

### Client
A hostname or an IP address can be specified.

`client host port`


### Commands
`put` - Attempts to send the specified file to the server

`get` - Attempts to get the specified file from the server

`ls` - List files in the current server directory

`cd` - Change to a new directory on the server. Directories above the server root cannot be accessed.

`pwd` - Print the current server directory

`lls` - List files in the current client directory

`lcd` - Change to a new director on the client

`lpwd` - Print the current client directory

`quit` - Terminate the connection and exit the client