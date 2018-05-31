
# wiiero-switch

Port of wiiero (Liero for Wii) to Nintendo Switch

Sound is currently broken

## Screenshots

![Screenshot1](https://github.com/KeeganatorP/wiiero-switch/blob/master/screen1small.jpg)
![Screenshot2](https://github.com/KeeganatorP/wiiero-switch/blob/master/screen2small.jpg)
![Screenshot3](https://github.com/KeeganatorP/wiiero-switch/blob/master/screen3small.jpg)

## Compiling

### Prerequisites

Requires SDL-1.2.15 for switch

```
https://github.com/Nop90-Switch/SDL-SWITCH
```

### Building

Run make

```
make
```

Copy

```
wiiero.nro
audio/
lang/
music/
sprites/
```

To `/switch/wiiero/` on SD Card

## Special Thanks

* Joosa Riekkinen (Liero Developer)
* Luca Denevolo (Wiiero Developer)
* devkitpro
* Nop90 (SDL 1.2 port)