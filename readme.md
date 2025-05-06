# Glich an Extendable Script Language Library.

![Glich logo](./image/glich32.svg)

The library is intended to provide the bases for an updated HistoryCal script
and a new HistoryGen script language, as well as possible others.

The documentation for the projects is being created on a
[website](https://nickmat.github.io/glich/website/index.htm)
as part of this repository.

Create the following projects
 - **hic.lib** The main **glc** and **hic** language library
 - **glcs.exe** A interactive terminal for the **glich** script language.
 - **glich** Run a **glich** script and shut down.
 - **glctest.exe** Runs a suite of test **glich** scripts.
 - **glcunit** A unit test program to test the library.
 - **glcsoak** A unit test program to exhaustively test the **hics** library.

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
