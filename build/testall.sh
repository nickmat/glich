#!/bin/bash
./gnu/glcunit
cd ../test/glcscripts/suite
../../../build/gnu/glctest ./
cd ../../../build
./gnu/glcsoak --test-type Short