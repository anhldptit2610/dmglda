#!/bin/bash

for test_file in /home/lda/jsmoo/misc/tests/GeneratedTests/sm83/v1/*.json; do 
	../build/testing/cpu_test "$test_file"
done
