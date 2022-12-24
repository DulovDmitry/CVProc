@echo off 
set /p var= <"D:\Software development\CVProc\build.txt"
set /a var= %var%+1 
echo %var% >"D:\Software development\CVProc\build.txt"
echo build #%var%