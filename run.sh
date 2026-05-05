#!/bin/bash
cd "/mnt/d/CSE323  project/deadlock-defender"
make clean && make 2>&1
echo ""
./deadlock
echo ""
echo "Press Enter to close..."
read _
