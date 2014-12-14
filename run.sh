#!/bin/bash

rm -rf results;

mkdir -p results;
mkdir -p results/de;
mkdir -p results/fr;
mkdir -p results/pt;
mkdir -p results/it;
mkdir -p results/en;

cd sentences/de;
rm *normalized.txt;
cd ../fr;
rm *normalized.txt;
cd ../pt;
rm *normalized.txt;
cd ../it;
rm *normalized.txt;
cd ../en;
rm *normalized.txt;

cd ../../source;

echo -e "Compiling Program"
make clean all;
echo -e ""
echo -e "Running Tests"
echo -e ""

for TEST in $(ls ../sentences/de); do
NAME=`echo $TEST | cut -d'.' --complement -f2-`

echo -e "Running Test: $TEST"
./mp2 ../sentences/de/$TEST ../results/de/$NAME-result.txt

done

for TEST in $(ls ../sentences/en); do
NAME=`echo $TEST | cut -d'.' --complement -f2-`

echo -e "Running Test: $TEST"
./mp2 ../sentences/en/$TEST ../results/en/$NAME-result.txt

done

for TEST in $(ls ../sentences/fr); do
NAME=`echo $TEST | cut -d'.' --complement -f2-`

echo -e "Running Test: $TEST"
./mp2 ../sentences/fr/$TEST ../results/fr/$NAME-result.txt

done

for TEST in $(ls ../sentences/it); do
NAME=`echo $TEST | cut -d'.' --complement -f2-`

echo -e "Running Test: $TEST"
./mp2 ../sentences/it/$TEST ../results/it/$NAME-result.txt

done

for TEST in $(ls ../sentences/pt); do
NAME=`echo $TEST | cut -d'.' --complement -f2-`

echo -e "Running Test: $TEST"
./mp2 ../sentences/pt/$TEST ../results/pt/$NAME-result.txt

done

echo -e ""
echo -e "Finished Running Tests"
echo -e ""

echo -e "Normalized Input Files at: /corpora/normalized/"
echo -e "Normalized Input Sentence at: /corpora/normalized/"
echo -e "N-Gram counts at: /corpora/counts/"
echo -e "N-Gram smoothed counts at: /corpora/counts/smoothed/"
echo -e "N-Gram probabilities at: /corpora/counts/probs/"
echo -e "N-Gram smoothed probabilities at: /corpora/counts/probs/smoothed/"
echo -e "Test Results at: /results/"