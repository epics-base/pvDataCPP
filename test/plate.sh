#!/bin/sh

# Make things golden...

for ff in *Diff
do
    [ -s "$ff" ] || continue

    cp "${ff%Diff}" "${ff%Diff}Gold"
done
