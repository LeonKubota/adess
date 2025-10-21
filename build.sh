#!/bin/bash

SRC="src/main/java/"

mkdir -p bin

javac -d "bin" "$SRC/Main.java"

mkdir -p bin/analysis
mkdir -p bin/commands
mkdir -p bin/config
mkdir -p bin/data
mkdir -p bin/generation
mkdir -p bin/rendering
