# Tested Platforms
The programs have been tested on
* FreeBSD 15.0
* macOS 15.7.1
* macOS 26.0.1
* Ubuntu 24.04

# Test Programs
The sender sends initially a UDP packet with the a DSCP code point `CS0` and `Not-ECT`. Then a socket option is used to set the DSCP to `AF41` and `ECT(1)` which corresponds to `0x89`. The second UDP is sent using this setting. When sending the third UDP packet, a cmsg is used to specifiy a DSCP of `AF42` and `CE` which corresponds to `0x93`. The fourth UDP packet is then send with a cmsg and therefore use again `AF41` and `ECT(1)` coresponding to `0x89`.

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
When running `sender4 127.0.0.1` the output of `receiver4` is on all platforms:
```
tos = 0x00
tos = 0x89
tos = 0x93
tos = 0x89
```
When running `sender6 ::ffff:127.0.0.1` the output is of `receiver4` on macOS:
```
tos = 0x00
tos = 0x01
tos = 0x93
tos = 0x01
```
When running `sender6 ::ffff:127.0.0.1` the output of of `receiver4` is on FreeBSD or Linux:
```
tos = 0x00
tos = 0x89
tos = 0x93
tos = 0x89
```
## Output of `receiver6`
When running `sender6 ::1` the output of `receiver6` is on all platforms:
```
tclass = 0x00
tclass = 0x89
tclass = 0x93
tclass = 0x89
```
This is also the output of `receiver6` for `sender4 127.0.0.1` on FreeBSD and macOS and the output of `receiver6` for `sender6 ::ffff:127.0.0.1` on FreeBSD. On macOS, the output of `receiver6` for `sender6 ::ffff:127.0.0.1` is
```
tclass = 0x00
tclass = 0x01
tclass = 0x93
tclass = 0x01
```
On Linux the output of `receiver6` for `sender4 127.0.0.1` or `sender6 ::ffff:127.0.0.1` is
```
tos = 0x00
tos = 0x89
tos = 0x93
tos = 0x89
```


