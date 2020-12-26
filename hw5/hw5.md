# Homework 5
Student ID: B07902125 Name:鄭昊昕 
## Problem 1
* Forwarding: router-local action, transfer a packet from an input link interface to an appropriate output link interface.
* Routing: network-wide process, determines the paths for a packet from source to destination.
## Problem 2
### a.
No VC number can be assigned to the new VC. So this new VC cannot established in the network.
### b.
Each link has two available VC numbers. There are four links, so we have 16 combinations. One example combination is (00,10,00,10)
## Problem 3
### a.
|Prefix Match| Link Interface |
|---|---|
|11100000 00|0|
|11100000 01000000|1|
|11100000|2|
|11100001 0|2|
|otherwise|3|
### b.
* For first address: 5th entry, link interface 3
* For second address: 4th entry, link interface 2
* For third address: 5th entry, link interface 3
## Problem 4
### a.
IP address in 128.119.40.128 to 128.119.40.191
### b.
128.119.40.64/28,128.119.40.80/28,128.119.40.96/28,128.119.40.112/28
## Problem 5
| Step | N' | D(t),p(t) | D(u),p(u) | D(v),p(v) | D(w),p(w)|D(y),p(y)|D(z),p(z)|
|---|---|---|---|---|---|---|---|
|0|x|inf|inf|3,x|6,x|6,x|8,x|
|1|xv|7,v|6,v|3,x|6,x|6,x|8,x|
|2|xvu|7,v|6,v|3,x|6,x|6,x|8,x|
|3|xvuw|7,v|6,v|3,x|6,x|6,x|8,x|
|4|xvuwy|7,v|6,v|3,x|6,x|6,x|8,x|
|5|xvuwyt|7,v|6,v|3,x|6,x|6,x|8,x|
|6|xvuwytz|7,v|6,v|3,x|6,x|6,x|8,x|
## Problem 6
### a.
eBGP
### b.
iBGP
### c.
eBGP
### d.
iBGP
## Problem 7
### a.
32-4=28,the size of the multicast address space = 2^28
### b.
P(two groups choose same address)=1/(2^28)=2^(-28)
### c.
Let x=2^28
P(1000 groups all have different address)={x(x-1)(x-2)*...*(x-999)}/(x^1000) = (1-1/x)(1-2/x)...(1-999/x)
Ignore cross product terms due to large enough x: \
(1-1/x)(1-2/x)....(1-999/x) approximately equal to 1-((1+2+3+...+999)/x)=1-999*1000/2x = 0.998 (Since 2^10=1024,10^3=1000) \
So the probability that they interfere with each other: 1-0.998=0.002