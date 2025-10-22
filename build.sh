#!/bin/bash

SRC="src/main/java/"

javac -d "bin" $SRC/*.java $SRC/analysis/*.java $SRC/commands/*.java $SRC/config/*.java $SRC/data/*.java $SRC/generation/*.java $SRC/rendering/*.java
