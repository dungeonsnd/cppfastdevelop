#!/bin/sh

./astyle --options=none --style=bsd --indent=spaces=3 -Z -K -c --lineend=linux --suffix=none --max-code-length=80 --align-pointer=middle --min-conditional-indent=0 --indent-col1-comments --indent-preprocessor --recursive --quiet "../../*.cpp"

./astyle --options=none --style=bsd --indent=spaces=3 -Z -K -c --lineend=linux --suffix=none --max-code-length=80 --align-pointer=middle --min-conditional-indent=0 --indent-col1-comments --indent-preprocessor --recursive --quiet "../../*.c"

./astyle --options=none --style=bsd --indent=spaces=3 -Z -K -c --lineend=linux --suffix=none --max-code-length=80 --align-pointer=middle --min-conditional-indent=0 --indent-col1-comments --indent-preprocessor --recursive --quiet "../../*.hpp"

./astyle --options=none --style=bsd --indent=spaces=3 -Z -K -c --lineend=linux --suffix=none --max-code-length=80 --align-pointer=middle --min-conditional-indent=0 --indent-col1-comments --indent-preprocessor --recursive --quiet "../../*.h"
