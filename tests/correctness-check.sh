#!/bin/bash

# ARGUMENTS:
#	$1 - path to application executable
#   $2 - path to generator executable
#	$3 - other arguments passed to application executable e.g. --double

$2 --fileName __correctness_test --mSize 100 --mCount 2 --seed 123

$1 __correctness_test_1.txt __correctness_test_2.txt __correctness_trivial.txt --triv $4
if [ $? -ne 0 ]; then
	echo "ERROR: The trivial algorithm terminated with a non-zero exit status."
	exit 1
fi

ERROR_COUNTER=0
for THRESHOLD in {1..100}; do
	echo -n "Checking mixed Strassen3 algorithm (threshold: $THRESHOLD)..."
	$1 __correctness_test_1.txt __correctness_test_2.txt __correctness.txt --thres $THRESHOLD $4
	if [ $? -ne 0 ]; then
		echo "ERROR: The mixed Strassen3 algorithm (threshold: $THRESHOLD) terminated with a non-zero exit status."
		((ERROR_COUNTER++))
	fi
	if ! cmp -s __correctness_trivial.txt __correctness.txt ; then
		echo "ERROR: The mixed Strassen3 algorithm (threshold: $THRESHOLD) result differs from the trivial algorithm result."
		((ERROR_COUNTER++))
	fi
	echo "OK";
done

rm __correctness_test_1.txt __correctness_test_2.txt __correctness_trivial.txt __correctness.txt

if [ $ERROR_COUNTER -gt 0 ]; then
	echo "$ERROR_COUNTER results of the mixed Strassen3 algorithm were different than the trivial algorithm result."
else
	echo "All mixed Strassen3 algorithm results were identical to the the trivial algorithm result."
fi
