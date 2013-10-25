#!/bin/sh

g++ -Wall -g redis_access_test.cpp -o redis_access_test -I../../include -lhiredis
mv redis_access_test bin

