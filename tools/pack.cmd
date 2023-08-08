@echo off

::: Program logo
echo pack.cmd -- Packing RawDev project
echo Packing RawDev repository into archive for manual backup
echo Copyright (C)2020 by Petr Krajnik
echo.

::: Check if the folder exist
echo Checking for rawdev\ folder existence...
if not exist rawdev\ (
	echo Error: Folder 'rawdev/' was not found.
	goto END
)

::: Make the tarball
echo Packing RawDev repository with TAR+GZip...
tar -cz -f rawdev.tgz --exclude "build" --exclude ".vs" rawdev
if not %ERRORLEVEL% equ 0 (
	echo Error: Error when packing data with TAR+GZip.
	goto END
)

::: Finish
echo Saved in archive 'rawdev.tgz'. DONE.
:END

:::
::: Usage:
::: 1) Goto directory that contains rawdev directory.
::: 2) Run 'rawdev\tools\pack.cmd'
::: 3) The result is saved in the current directory.
:::
