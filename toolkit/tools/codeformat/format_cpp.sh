#!/bin/sh

EXE=./astyle

$EXE --options=none --style=bsd --indent=spaces=4 -Z -K -c --lineend=linux --suffix=none --max-code-length=80 --align-pointer=middle --min-conditional-indent=0 --indent-col1-comments --indent-preprocessor --recursive --quiet "../../../*.cpp"

$EXE --options=none --style=bsd --indent=spaces=4 -Z -K -c --lineend=linux --suffix=none --max-code-length=80 --align-pointer=middle --min-conditional-indent=0 --indent-col1-comments --indent-preprocessor --recursive --quiet "../../../*.c"

$EXE --options=none --style=bsd --indent=spaces=4 -Z -K -c --lineend=linux --suffix=none --max-code-length=80 --align-pointer=middle --min-conditional-indent=0 --indent-col1-comments --indent-preprocessor --recursive --quiet "../../../*.hpp"

$EXE --options=none --style=bsd --indent=spaces=4 -Z -K -c --lineend=linux --suffix=none --max-code-length=80 --align-pointer=middle --min-conditional-indent=0 --indent-col1-comments --indent-preprocessor --recursive --quiet "../../../*.h"
