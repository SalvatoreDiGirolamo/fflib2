#!/bin/bash

cat <(echo "DOT#digraph s{ ") $1 <(echo "DOT#}")| grep DOT | cut -d "#" -f 2 | gvpack -u | dot -Tps > .graph.ps
evince .graph.ps

