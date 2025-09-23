# KiCAD Part Search Plugin

This plugin was created to speed up working with component fields in KiCAD projects, such as adding **datasheet links, part numbers, descriptions, and more**.

It is implemented as a standalone program based on **Qt Widgets** and integrated into KiCAD via an additional button (see [modified KiCAD fork](https://github.com/ivixiz/kicad-source-mirror)).  
The integration is handled by the `OnFindPart()` method inside `dialog_symbol_properties.cpp`, because there is no plugin support in EESchema.

---

## Features
- Simple and optimized interface — designed to minimize the number of clicks and speed up repetitive tasks.  
- Currently supports two suppliers:
  - **Mouser**
  - **Digi-Key**
- Future plans:
  - Support for **LCSC**, **RS Components**, **Farnell**, and others.
- Requires creating an account on Mouser or Digi-Key and obtaining API keys, which should be entered in the plugin settings tab.

---

## Platform Support
- Written using **Qt**, which is cross-platform.  
- Currently **compiled and tested only on Linux Kubuntu 24.04**.  
- Behavior on other systems (Windows, macOS) is **untested** and may require adjustments.

---


## Build Instructions
To compile the plugin:

```bash
cd ./src    # enter the source directory
qmake       # generate Makefile
make        # build
../part_search   # run the program
```

## Screenshots
### Main Window of Plugin
![Main Window](./src/docs/screenshot1.png)
### Settings Tab
![Settings Tab](./src/docs/screenshot2.png)
### Result
![Result](./src/docs/screenshot3.png)