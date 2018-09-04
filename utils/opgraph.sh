#!/bin/bash

lines=$(cat $1 | grep DOT | cut -d "#" -f 2 | sort)
echo "digraph s{ $lines }" | gvpack -u | dot -Tps > .graph.ps
evince .graph.ps

