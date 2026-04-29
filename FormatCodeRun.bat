@echo off
setlocal
set "CF=C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\Llvm\bin\clang-format.exe"

for /f "delims=" %%f in ('
  dir /b /s *.cpp *.hpp *.h ^
  ^| findstr /v /i "\\packages\\" ^
  ^| findstr /v /i "\\.git\\" ^
  ^| findstr /v /i "\\build\\"
') do (
  echo Formatting "%%f"
  "%CF%" -i -style=file "%%f"
)

echo Done!
pause
