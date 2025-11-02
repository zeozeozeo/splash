@echo off
if exist "build/" (
    echo removing build directory...
    @RD /S /Q "build/"
)
