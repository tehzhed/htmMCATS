#!/bin/sh
FOLDERS="genome intruder kmeans ssca2 vacation yada"

rm auto-results/*.* || true
rm lib/*.o || true

for F in $FOLDERS
do
    cd $F
    rm *.o || true
    rm $F
    cd ..
done
