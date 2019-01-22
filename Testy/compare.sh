#!/bin/bash
FILES=./tests/*.in
i=1
for f in $FILES
do
i=$(($i+1))
out=${f::-2}
out="${out}out"
./kom <$f >pal.out
diff -bq $out pal.out
done
