@echo off

for /f "delims=" %%f in (bat-list-folders.txt) do (
	for %%i in ("%%f\*") do (
		echo Execute %%i...
		start /wait /min BMPReader.exe "%%i" "1"
		taskkill /im BMPReader.exe /f > nul 2>&1
	)
)

pause
exit