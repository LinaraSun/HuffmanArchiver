set -euo pipefail

export LC_ALL=C

PROGRAM=./huffArchiver
TEST_DIR="test/test_data/comparison"
RUNS=5

GREEN=$'\033[0;32m'
RED=$'\033[0;31m'
NC=$'\033[0m'

echo "file;symbol_size;original_size;compressed_size;avg_compress_s;avg_decompress_s;ratio" > results.csv

run_tests() {
	local file_name="$1"
	local path="$TEST_DIR/$2"
	local symbol_size="$3"
	local extension="${path##*.}"

	original_size=$(stat -c%s "$path")

	local compressed
	local restored

	compressed=$(mktemp --suffix=.huff)
	restored=$(mktemp --suffix=".$extension")

	local compress_sum=0
	local decompress_sum=0

	local t

	echo "Testing $file_name"

	for ((i=1; i<=RUNS; i++)); do
		t=$(
			{ /usr/bin/time -f "%e" \
			"$PROGRAM" compress "$path" "$compressed" "$symbol_size" \
			> /dev/null; } 2>&1
		)
		compress_sum=$(awk "BEGIN {print $compress_sum + $t}")

		t=$(
			{ /usr/bin/time -f "%e" \
			"$PROGRAM" decompress "$compressed" "$restored" \
			>/dev/null; } 2>&1
		)
		decompress_sum=$(awk "BEGIN {print $decompress_sum + $t}")
	done

	compressed_size=$(stat -c%s "$compressed")

	avg_compress=$(awk "BEGIN {print $compress_sum / $RUNS}")
	avg_decompress=$(awk "BEGIN {print $decompress_sum / $RUNS}")

	ratio=$(awk "BEGIN {print $compressed_size / $original_size}")

	printf "%s;%s;%s;%s;%s;%s;%s\n" \
		"$path" \
		"$symbol_size" \
		"$original_size" \
		"$compressed_size" \
		"$avg_compress" \
		"$avg_decompress" \
		"$ratio" \
		>> results.csv

	rm -f "$compressed" "$restored"
}

for format in "txt" "jpg" "bmp" "mp4" "zip"; do
	for size in 1 5 10 20 40; do
		run_tests "$format ${size}mb" "$format/${size}mb.$format" 1
	done
done
