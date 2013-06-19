#!/bin/sh

if [[ $# -lt 1 ]] ; then
    echo "Usage: ./gitcommit.sh comment"
    exit
fi

cd ../
git add -A .
git commit -m"$1" .


