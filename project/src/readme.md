# Readme
## How to build this project
`
$ make
`
### Requirement
* clang++(Both FreeBSD and GNU/Linux)
* GNU Make
* GNU C++ Compiler(If you are using GNU/Linux)
* libmagic
## How to run
`
$ ./server.o <portnum>
`
Put your html files under `./var` \
For a file `./var/index.html`: \
The url might be: `http://example.com/index.html`

### Environment
Tested on CSIE Workstation(Both FreeBSD and Archlinux) (Recommended)
#### Dev Environment
* Arch Linux(x86_64)(Linux 5.9.10)
* clang 11.0.0
#### Deploy Environment
* Ubuntu Server 20.04(x86_64)(Linux 5.4.0)(AWS)
* g++ 9.3.0
## Author
* b07902125@ntu.edu.tw