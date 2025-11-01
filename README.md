# Tested Platforms
The programs have been tested on
* FreeBSD 15.0
* macOS 15.7.1
* macOS 26.0.1
* Ubuntu 24.04
  
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
When running `sender4 127.0.0.1` or `sender6 ::ffff:127.0.0.1` the output of `receiver4` is on all platforms:
```
tos = 0
tos = 1
tos = 2
tos = 1
```
## Output of `receiver6`
When running `sender6 ::1` the output of `receiver6` is on all platforms:
```
tclass = 0
tclass = 1
tclass = 2
tclass = 1
```
This is also the output of `receiver6` for `sender4 127.0.0.1` or `sender6 ::ffff:127.0.0.1` on FreeBSD and MacOS.
On Linux the output of `receiver6` for `sender4 127.0.0.1` or `sender ::ffff:127.0.0.1` is
```
tos = 0
tos = 1
tos = 2
tos = 1
```


