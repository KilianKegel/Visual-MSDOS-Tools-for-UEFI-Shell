# Visual DOS-Tools for UEFI Shell
Create and debug DOS-Tools for UEFI Shell using Visual Studio 2022.
## Goal
Demonstration off how to implement DOS/UNIX software tools to access
the UEFI platform.

The projects are kept small and simple to introduce the UEFI shell
programming using "ANSI C", easily and quickly.

## Approach
It is intended to use Visual Studio 2022 as the only development environment.
Furthermore it is intended to use ANSI-C library functions as far as possible.

Doing so, the UEFI Shell applications can be easily debugged
on the Windows development platform, simply by building the sourcecode
as a Win64-executable (change the solution configuration from Efi64 to Win64 ).

## HowTo
https://github.com/KilianKegel/HowTo-setup-an-UEFI-Development-PC#howto-setup-an-uefi-development-pc

## Revision history
https://github.com/KilianKegel/torito-C-Library#revision-history
### 20210825
* Update to Microsoft Visual Studio Community 2022 Preview (64-bit), Version 17.0.0 Preview 3.1
### 20210820/R166
* Update to torito-C 20210820/R166
### 20200204/R154
* Update to torito-C 20200204/R154
* adjust solution configurations
* update copyright
### 20191018/R141
* update Torito C Library https://github.com/KilianKegel/torito-C-Library#20191017r141
### 20190922
* add FIND command
* Update to torito-C 20190621/R138 that autodetects I/O redirection UTF16 vs. ASCII
* add support for VS2019 ver 16.2.5
### 20190621
* improve MORE command, added command line parameter
* Update to torito - C 20190621 / R133 that adds VS2019 support, remove library dependancy from compiler version
### 20181129/R116
* Update to torito-C 20181129/R116 that provides various improvements
### 20181112/R110
* Update to torito-C 20181112/R110 that provides scanf-family improvements
### 20180921/R107
* Update to torito-C 20180920/R106 that provides VS2017/15.8 support
* cleanup project settings, remove default C library from solution/project view
### 20180831
* initial revision -> the MORE command

