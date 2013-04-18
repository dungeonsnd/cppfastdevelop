#!/bin/sh

if [[ $# -lt 1 ]] ; then
    echo "Usage: ./gitcommit.sh comment"
    exit
fi


projectdoc/codeformat/format_cpp.sh

git add -A .
git commit -m"$1" .
