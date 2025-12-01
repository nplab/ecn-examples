# Tested Platforms
The programs have been tested on
* FreeBSD 15.0
* macOS 15.7.1
* macOS 26.0.1
* Ubuntu 24.04

# Test Programs
The sender sends initially a UDP packet with the a DSCP code point `CS0` and `Not-ECT`. Then a socket option is used to set the DSCP to `AF41` and `ECT(1)` which corresponds to `0x89`. The second UDP packet is sent using this setting. When sending the third UDP packet, a cmsg is used to specifiy a DSCP of `AF42` and `CE` which corresponds to `0x93`. The fourth UDP packet is then send with a cmsg and therefore use again `AF41` and `ECT(1)` coresponding to `0x89`.

# Compilation
## FreeBSD and MacOS
```
cc -o receiver4 receiver4.c
cc -o receiver6 receiver6.c
cc -o sender4 sender4.c
cc -o sender6 sender6.c
```
## Linux
```
gcc -o receiver4 receiver4.c
gcc -o receiver6 receiver6.c
gcc -o sender4 sender4.c
gcc -o sender6 sender6.c
```
# Execution
## Output of `receiver4`

### FreeBSD
| `sender4 127.0.0.1` | `sender6 ::ffff:127.0.0.1` |
|:-------------------:|:--------------------------:|
|`tos = 0x00`         |`tos = 0x00`                |
|`tos = 0x89`         |`tos = 0x89`                |
|`tos = 0x93`         |`tos = 0x93`                |
|`tos = 0x89`         |`tos = 0x89`                |

### Linux
| `sender4 127.0.0.1` | `sender6 ::ffff:127.0.0.1` |
|:-------------------:|:--------------------------:|
|`tos = 0x00`         |`tos = 0x00`                |
|`tos = 0x89`         |`tos = 0x89`                |
|`tos = 0x93`         |`tos = 0x93`                |
|`tos = 0x89`         |`tos = 0x89`                |

### macOS
| `sender4 127.0.0.1` | `sender6 ::ffff:127.0.0.1` |
|:-------------------:|:--------------------------:|
|`tos = 0x00`         |`tos = 0x00`                |
|`tos = 0x89`         |`tos = 0x01`                |
|`tos = 0x93`         |`tos = 0x93`                |
|`tos = 0x89`         |`tos = 0x01`                |

The observation is that on macOS setting the DSCP value using the `IPPROTO_IPV6`-level socket option `IPV6_TCLASS` is not possible. Only the ECN value is set.

## Output of `receiver6`

### FreeBSD
| `sender6 ::1` | `sender6 ::ffff:127.0.0.1` | `sender4 127.0.0.1` |
|:-------------:|:--------------------------:|:-------------------:|
|`tclass = 0x00`|`tclass = 0x00`             |`tclass = 0x00`      |
|`tclass = 0x89`|`tclass = 0x89`             |`tclass = 0x89`      |
|`tclass = 0x93`|`tclass = 0x93`             |`tclass = 0x93`      |
|`tclass = 0x89`|`tclass = 0x89`             |`tclass = 0x89`      |

### Linux
| `sender6 ::1` | `sender6 ::ffff:127.0.0.1` | `sender4 127.0.0.1` |
|:-------------:|:--------------------------:|:-------------------:|
|`tclass = 0x00`|`tos = 0x00`                |`tos = 0x00`         |
|`tclass = 0x89`|`tos = 0x89`                |`tos = 0x89`         |
|`tclass = 0x93`|`tos = 0x93`                |`tos = 0x93`         |
|`tclass = 0x89`|`tos = 0x89`                |`tos = 0x89`         |

So the observation is that on Linux an `IPPROTO_IP`-`cmsg_level` cmsg with `cmsg_type` of `IP_TOS` is used to report the DSCP and ECN fields on a AF_INET6 socket, if a UDP/IPv4 packet has been received.

### macOS
| `sender6 ::1` | `sender6 ::ffff:127.0.0.1` | `sender4 127.0.0.1` |
|:-------------:|:--------------------------:|:-------------------:|
|`tclass = 0x00`|`tclass = 0x00`             |`tclass = 0x00`      |
|`tclass = 0x89`|`tclass = 0x01`             |`tclass = 0x89`      |
|`tclass = 0x93`|`tclass = 0x93`             |`tclass = 0x93`      |
|`tclass = 0x89`|`tclass = 0x01`             |`tclass = 0x89`      |

The observation is that on macOS setting the DSCP value using the `IPPROTO_IPV6`-level socket option `IPV6_TCLASS` for UDP/IPv4 packets is not possible. Only the ECN value is set.
