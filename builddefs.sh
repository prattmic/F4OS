#!/bin/bash

echo "#define   BUILD_TIME  \"`date`\"" > inc/builddefs.h
echo "#define   BUILD_REV   `git shortlog | grep -E '^[ ]+\w+' | wc -l`" >> inc/builddefs.h
