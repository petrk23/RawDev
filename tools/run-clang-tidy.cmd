@echo off

set clangTidyPath=C:\Program Files\Microsoft Visual Studio\^
2022\Community\VC\Tools\Llvm\x64\bin\clang-tidy.exe

echo run-clang-tidy.cmd -- Run clang-tidy from MSVS installation
echo Copyright (C) 2020-2025 by Petr Krajnik
echo.
"%clangTidyPath%" %* -- -I src -std=c++20 -Wall -pedantic
