# Homework 3
## Problem 1
* a. False, the client will send four requests.
* b. True, persistent http lets the connection open.
* c. False,each TCP fragment can only carry one http request.
* d. False, it means request generation time.
* e. False, HTTP 204 status code means no content.
## Problem 2
* a. \<CR\>\<LF\>.\<CR\>\<LF\>
* b.  HTTP uses “Content-Length header field” to indicate the length of a message body. 
* c. No, since SMTP only accept message in 7bits ASCII format,but HTTP message could be binary.
## Problem 3
We need (RTT1+RTT2+RTT3+ ...+RTTn) time to know the ip address of the server containing the object. And we need RTT0 time to set up TCP connection between local client and server. And also it may need RTT0 time to transfer the object from server to local. So the total time elpases:\
(2*RTT0+RTT1+RTT2+....RTTn)
## Problem 4
* a. Yes, if there are enough peers staying in the swarm for a long enough time, Bob can always receive data through optimistic unchoking by other peers.
* b. Yes, he can run a client on each host, and let each host ask for different chunks of file, then he can combine chunks from these hosts into a single file.
## Problem 5
* a. The client will attempt to set up a TCP connection with a non-existent host. The connection will not be made.
* b. Everything would work fine, since UDP ones do not need to establish a TCP connection.
* c. The client will attempt to set up a TCP connection with the wrong process. And there will be some error.
## Author
* b07902125@ntu.edu.tw