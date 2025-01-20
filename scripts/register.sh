#!/bin/bash

echo "set(HEADERS \${HEADERS}" > cmake/headerlist.cmake
find include -type f -printf '\t%p\n' >> cmake/headerlist.cmake
echo ")" >> cmake/headerlist.cmake

echo "set(SOURCES \${SOURCES}" > cmake/sourcelist.cmake
find src -type f -printf '\t%p\n' >> cmake/sourcelist.cmake
echo ")" >> cmake/sourcelist.cmake
