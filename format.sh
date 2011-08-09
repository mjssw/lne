#!/bin/sh

find . -name '*.h' -o -name '*.inl' -o -name '*.cpp' -exec astyle --style=kr -n -p -w -Y -L -S -U -T -k3 -xd {} \;
