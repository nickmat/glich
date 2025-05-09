#!/bin/bash
mkdir -p gnu
cd gnu
cmake -G "Unix Makefiles" ../../ -DCMAKE_POLICY_VERSION_MINIMUM=3.5
make