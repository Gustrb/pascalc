#!/bin/bash

clang-format --dry-run --Werror src/*.c src/*.h tests/*.c
