# AndroidTrollware

# ⚠️ Epilepsy Warning ⚠️

This virus/trollware has flashing effects, use with caution

# Why was this made?

because yes

# How to build

You need these programs (Linux only from these commands, idk how windows), debian commands:

```bash
sudo apt update
sudo apt install mingw-w64
```

To build the icon file:

```bash
x86_64-w64-mingw32-windres floatbeat.rc -O coff -o floatbeat_res.o
```

Build the main executable:

```bash
x86_64-w64-mingw32-gcc -O2 -std=c99 -mwindows \
  AndroidTrollware-Code.c floatbeat_res.o \
  -o AndroidSetup.exe -lwinmm -lgdi32 -lshell32 -lole32 -loleaut32 -lm
```

The executable shows a warning before starting. It plays the floatbeat, opens and moves one Explorer window during the GDI phases, and applies flashing screen effects. Keep `icon.ico` beside `floatbeat.rc` when building the resource file.

You should now have `AndroidSetup.exe`, which can run through Wine or on Windows 10.
