<h1 align="center">
  <a href="https://github.com/AzerSD/ft_ping">
    ft_ping
  </a>
</h1>

<div align="center">
  Recoding the `ping` command provides you a deeper comprehension of how network tools work, Especially TCP/IP communication between two machines on a network, leading to a more profound understanding of network operations.
  <br />
  <br />
  <a href="https://github.com/dec0dOS/amazing-github-template/issues/new?assignees=&labels=bug&template=01_BUG_REPORT.md&title=bug%3A+">Report a Bug</a>
  .
  <a href="https://github.com/dec0dOS/amazing-github-template/discussions">Ask a Question</a>
</div>

---

<div align="center">

[![license](https://img.shields.io/github/license/dec0dOS/amazing-github-template.svg?style=flat-square)](LICENSE)
[![PRs welcome](https://img.shields.io/badge/PRs-welcome-ff69b4.svg?style=flat-square)](https://github.com/AzerSD/amazing-github-template/issues?q=is%3Aissue+is%3Aopen+label%3A%22help+wanted%22)
[![made with hearth by AzerSD](https://img.shields.io/badge/made%20with%20%E2%99%A5%20by-AzerSD-ff1414.svg?style=flat-square)](https://github.com/AzerSD)

</div>

</details>

---


Allowed function
| Function        | Description
| :--						| :--
| gettimeofday  | Retrieves the current time with microsecond precision. Useful for timestamping packets and measuring network latency. |
| exit  | Terminates the program immediately. Useful for handling exit conditions or errors gracefully. |
| inet_ntop / inet_pton  | Converts IP addresses between human-readable and network binary formats. Essential for handling IP addresses in networking applications. |
| nthos / htons:  | Converts between host byte order and network byte order for 16-bit integers. Ensures consistency in data representation across different systems. |
| signal / alarm / usleep  | Manages signal handling, timers, and microsecond-level sleep. Useful for implementing timeouts and asynchronous behavior in networking code. |
| socket / setsockopt / close  | Creates, configures, and closes network sockets. Essential functions for establishing and managing network connections. |
| sendto / recvmsg  | Sends data to a specific address and receives data along with sender information. Core functions for sending and receiving network packets. |
| getpid / getuid  | Retrieves the process ID and user ID of the current process. Useful for identifying the running process and implementing security checks. |
| getaddrinfo / getnameinfo / freeaddrinfo | Resolves domain names to IP addresses and vice versa. Also frees memory allocated by getaddrinfo. Essential for DNS resolution in networking applications. |
| strerror / gai_strerror  | Converts error codes to human-readable error messages. Useful for diagnosing errors encountered during socket operations and network communication. |
| printf and its family  | Outputs formatted text to the console. Useful for displaying information, debugging, and monitoring the program's behavior. |

---
