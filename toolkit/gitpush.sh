#!/bin/sh

PUSH_TO_ALL=0
if [[ $# -eq 1 ]]; then
    if [ $1 = "all" ]; then
        PUSH_TO_ALL=1
    fi
fi

echo "+++ push to github +++"
git push github master

echo "+++ push to bitbucket +++"
git push bitbucket master

if [ $PUSH_TO_ALL -eq 1 ] ; then
    echo "+++ push to googlecode +++"
    git push googlecode master
    echo "+++ push to sourceforge +++"
    git push sourceforge master
fi


