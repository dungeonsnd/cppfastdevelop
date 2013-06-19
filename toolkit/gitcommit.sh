#!/bin/sh

if [[ $# -lt 1 ]] ; then
    echo "Usage: ./gitcommit.sh comment"
    exit
fi

git add -A .
git commit -m"$1" .


