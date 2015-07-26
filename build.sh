#!/bin/sh
FOLDERS="genome intruder kmeans ssca2 vacation yada"
rm lib/*.o || true

for F in $FOLDERS
do
    cd $F
    rm *.o || true
    rm $F
    make -f Makefile HTM_RETRIES="-DHTM_RETRIES=$1" TOTAL_ATTEMPTS="-DTOTAL_ATTEMPTS=$1" APRIORI_LOCK_ATTEMPTS="-DAPRIORI_LOCK_ATTEMPTS=$2" NUMBER_THREADS="-DNUMBER_THREADS=$3" NUMBER_ATOMIC_BLOCKS="-DNUMBER_ATOMIC_BLOCKS=$4" 
    rc=$?
    if [[ $rc != 0 ]] ; then
	echo ""
        echo "=================================== ERROR BUILDING $F - $name ===================================="
	echo ""
        exit 1
    fi
    cd ..
done
