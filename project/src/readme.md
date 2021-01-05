# Readme
## How to build this project
`
$ make phase2
`
### Requirement
* clang++(Both FreeBSD and GNU/Linux)
* GNU Make
* GNU C++ Compiler(If you are using GNU/Linux)
* libmagic
* libsqlite3
* libcurl
* pthread
## How to run
`
$ ./server.o <portnum> <mode>
`
mode:http 或是 https \
Put  `cert.pem` and `key.pem` under `/tmp/ssl/` (Password: 0000)\
Put your html files under `./var` \
For a file `./var/index.html`: \
The url might be: `http://example.com/index.html`

### Environment
Tested on CSIE Workstation(Both FreeBSD and Archlinux) 
使用CSIE Linux工作站可確保編譯成功(Recommended)
#### Dev Environment
* Arch Linux(x86_64)(Linux 5.9.10)
* clang 11.0.0
#### Deploy Environment
* Ubuntu Server 20.04(x86_64)(Linux 5.4.0)(AWS)
* g++ 9.3.0
## Author
* b07902125@ntu.edu.tw