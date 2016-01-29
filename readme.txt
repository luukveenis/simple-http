===================================
Name: Luuk Veenis
Class: CSC 361
Date: January 29, 2016
==================================

===============
Compilation
===============

Instructions:

Compiling the code for this assignment is simple, from the
command line simply type:

    % make

Running make will generate the two required binaries:
  - SimpClient
  - SimpServer

===============
The Server
===============

The server can be started in one of two ways:

  1: % ./SimpServer <port-number> <directory>
  2: % ./SimpServer <directory>

The first option specifies the port the server should listen on for requests
and the directory to run the server in. The second option only specifies the
directory the server should run in and will default to using port 80.

Note: The second option will only work if the program is ran using the sudo
      command. This is because all ports under 1024 are privileged and require
      root access.


===============
The Client
===============

The client can only be invoke one way:

  % ./SimpClient <request-url>

This will run the client which will make a GET request to the provided URL.
The client will also assume port 80 if no port is provided in the URL. If the
server is run on a port other than port 80, the request URL must specify that
port number.


===============
Other Notes
===============

One thing I wanted to mention is that the assignment says to call gethostbyname
only if the supplied hostname is not a numeric IP. If you look at my code, you
will notice that I pass the host provided to gethostbyname regardless. This is
because if you look at the man pages for gethostbyname(), it says:

  If name is an IPv4 or IPv6 address, no lookup is performed and gethostbyname()
  simply copies name into the h_name field.

So if a numeric IP is provided, it simply doesn't perform the lookup. This saves
having to perform some sort of operation to determine whether the thing provided
is a numeric IP or not and was a conscious implementation choice.
