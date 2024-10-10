@echo off

for %%i in ("images\*") do (
	echo Execute %%i...
	start /wait /min BMPReader.exe "%%i" "2"
	taskkill /im BMPReader.exe /f > nul 2>&1
)

pause
exit