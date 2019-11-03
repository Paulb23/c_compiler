#!/usr/bin/env sh

# ensure we are in the tests directory
cd `dirname $0`

for file in *.c
do
	set -x
	filename="${file%.*}";
	
	# delete any old versions
	rm "$filename.s";
	rm "$filename";

	# get the expected result
	expected=$(head -n 1 "$file" | grep -Po '(result=)([0-9]+)' | cut -d '=' -f 2);
	echo "Running test: $file... expecting $expected";

	# compile
	../bin/pcc $file > /dev/null;
	set +x
	if [ ! -f "$filename" ]
	then
		echo "$file failed to compile."
		exit 1
	fi

	# make it executible
	chmod +x "$filename"

	# run and check output is as expected
	./"$filename"
	if [ "$?" == "$expected" ]
	then
		echo "$file test passed."
	else
		echo "$file test failed."
		exit 1
	fi
done
