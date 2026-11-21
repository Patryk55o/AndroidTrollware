# AndroidTrollware

# ⚠️ Epilepsy Warning ⚠️

This virus/trollware has flashing effects, use with caution

# Why was this made?

because yes

# How to build the program bro?

You need these programs (Linux only from these commands, idk how windows), debian commands:

```bash
sudo apt update
sudo apt install mingw-w64
```

To build the icon file:

```bash
x86_64-w64-mingw32-windres floatbeat.rc -O coff -o floatbeat_res.o
```

To build the exec file:

```bash
x86_64-w64-mingw32-gcc -O2 -std=c99 -mwindows \
  AndroidTrollware-Code.c floatbeat_res.o \
  -o AndroidSetup.exe -lwinmm -lgdi32 -lm
```

You should have a exec file that you can run via wine or in Windows
