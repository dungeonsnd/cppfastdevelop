#!/bin/sh


PUSH_TO_ALL=0
if [[ $# -eq 1 ]]; then
    if [ $1 = "all" ]; then
        PUSH_TO_ALL=1
    fi
fi
echo $PUSH_TO_ALL

echo "push to github +++"
git push origin master

if [ $REBUILD_ALL -eq 1 ] ; then
    echo "push to googlecode and sourceforge +++"
    git push googlecode master
    git push sourceforge master
fi


