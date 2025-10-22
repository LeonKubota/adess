#!/bin/bash

SRC="src/main/java/"

javac -d "bin" $SRC/Main.java $SRC/analysis/*.java $SRC/config/*.java $SRC/data/*.java $SRC/generation/*.java $SRC/rendering/*.java

java -cp bin Main
