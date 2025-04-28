#!/bin/sh
echo "Number of TODOs left: $(grep -r 'TODO' ./* | wc -l)"
