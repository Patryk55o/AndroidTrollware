# AndroidTrollware

A project that will (probably) not blow up. but it might blow your eyes (joke)

# ⚠️ HEALTH & SAFETY WARNING — FLASHING EFFECTS ⚠️

This program contains rapid flashing, screen inversion, and visual distortion. These effects may trigger seizures in people with photosensitive epilepsy and may cause dizziness, nausea, headaches, disorientation, or fainting. **Do not run it if you are sensitive to flashing lights, feel unwell, or are unsure about your risk.** Stop immediately if you experience symptoms. This warning is not a guarantee of safety.

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

The executable displays a safety warning and requires confirmation before starting. It plays the floatbeat, opens and moves one Explorer window during the GDI phases, and applies flashing screen effects. Keep `icon.ico` beside `floatbeat.rc` when building the resource file.

You should now have `AndroidSetup.exe`, which can run through Wine or on Windows 10.
