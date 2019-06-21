@echo off
echo clean up...
if exist x64 del /s x64\*.pdb
if exist x64 del /s x64\*.map
for /D %%d in (*) do if exist %%d\x64 rd /s /q %%d\x64
for /D /R %%d in (doxygen.tmp) do if exist %%d rd /s /q %%d

