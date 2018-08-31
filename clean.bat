@echo off
echo clean up...
if exist x64 del /s x64\*.pdb
if exist x64 del /s x64\*.map
for /D %%d in (*) do if exist %%d\x64 rd /s /q %%d\x64

