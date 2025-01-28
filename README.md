# ft_ping

## Overview

`ft_ping` is a simple implementation of the classic `ping` utility, which is used to test the reachability 
of a host on an Internet Protocol (IP) network. With this project I aim <b>to deepen my understanding of network communication and the underlying principles of the ICMP protocol.</b>

```python
❯ sudo ./ft_ping 8.8.8.8
[sudo] password for userr: 
PING 8.8.8.8 (8.8.8.8) 56(84) bytes of data.
64 bytes from 8.8.8.8 icmp_seq=1 ttl=116ms time=22.6 ms
64 bytes from 8.8.8.8 icmp_seq=2 ttl=116ms time=22.7 ms
64 bytes from 8.8.8.8 icmp_seq=3 ttl=116ms time=23.1 ms
64 bytes from 8.8.8.8 icmp_seq=4 ttl=116ms time=18.7 ms
64 bytes from 8.8.8.8 icmp_seq=5 ttl=116ms time=18.4 ms
^C
--- 8.8.8.8 ping statistics ---
5 packets transmitted, 5 received, 0.00% packet loss, time 4512ms
```

## Core Concepts

### 1. ICMP Protocol

The Internet Control Message Protocol (ICMP) is a network layer protocol used by network devices to diagnose 
network communication issues.

### 2. Packet Structure

![image](https://github.com/user-attachments/assets/4120d1da-3507-4311-b39e-4412a081a4ef)

## Installation

To compile and run the `ft_ping` program, follow these steps:

1. Clone the repository:
```bash
   git clone https://github.com/AzerSD/ft_ping.git
   cd ft_ping
   make
   ./ft_ping <hostname>
```

# Contributing
Contributions are welcome! Please submit a pull request or open an issue for any improvements or bug fixes.

License
This project is licensed under the MIT License - see the LICENSE file for details.

Copy

### Customization

- **Add specific details**: If there are any unique features or functions in your code, feel free to elaborate on them in the README.
- **Include installation instructions**: If there are dependencies or special instructions, make sure to mention them.
- **Provide examples**: Add more usage examples to clarify how to use your implementation effectively.

Let me know if you need any modifications or additional sections!
sioudazer8@gmail.com
