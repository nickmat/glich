# Glich an Extendable Script Language Library.

![Glich logo](./image/glich32.svg)

Glich is a script language with a historical calendar creation and manipulation extension.

Glich is implemented as a library and intended to be embedded in a host program. Several such programs are included as part of this project. There is also a very basic IDE project at [Gliched](https://github.com/nickmat/gliched) and a GUI Calendar Conversion program at [HistoryCal](https://github.com/nickmat/HistoryCal). It will also be included in the genealogy program [TheFamilyPack](https://github.com/nickmat/TheFamilyPack) in the future.

This repository contains the following projects
 - **glc.lib** and **hic.lib** The main **glcs** and **hics** language libraries
 - **glich** Run a **glich** script and shut down.
 - **glcs.exe** A interactive terminal for the **glich** script language.
 - **glctest.exe** Runs a suite of test **glich** scripts.
 - **glcunit** A unit test program to test the library.
 - **glcsoak** A unit test program to exhaustively test the **hics** library.

A [Quickstart](quickstart.md) for the core language and a [hics quickstart](hics_quickstart.md) for the historic calendars is included.

Full documentation for the projects is being created on a
[website](https://nickmat.github.io/glich/website/index.htm)
as part of this repository.

### To build the projects on Windows 10 or 11
```
git clone --recurse-submodules https://github.com/nickmat/glich.git
cd glich\build
.\cmake-msvc.bat
msvc\glich.sln
```
### To build the projects on macOS (Tested on Sequoia 15.4.1)
```
git clone --recurse-submodules https://github.com/nickmat/glich.git
cd glich/build
mkdir xc
cd xc
cmake -G Xcode ../../
```
