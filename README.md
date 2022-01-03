simulacia baterie:


1) zadat do konzoly 1:
```
socat -d -d pty,raw,echo=0 pty,raw,echo=0
```

vystup je nieco taketo:
```
2022/01/03 19:13:54 socat[34197] N PTY is /dev/pts/7
2022/01/03 19:13:54 socat[34197] N PTY is /dev/pts/8
2022/01/03 19:13:54 socat[34197] N starting data transfer loop with FDs [5,5] and [7,7]
```

2) zadat do konzoly 2:
```
./simulateBattery /dev/pts/7
```
- poznamka 1

3) zadat do konzoly 3:
```
./batteryConsole /dev/pts/8
```
- poznamka 1


poznamka 1:
parameter sa moze lisit, treba dosadit podla konketneho vypisu ziskaneho v kroku 1
