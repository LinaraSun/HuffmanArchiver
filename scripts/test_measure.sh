set -euo pipefail

export LC_ALL=C

PROGRAM=./huffArchiver
TEST_DIR="test/test_data/comparison"
RUNS=5

GREEN=$'\033[0;32m'
RED=$'\033[0;31m'
NC=$'\033[0m'

echo "file;symbol_size;original_size;compressed_size;avg_compress_s;stddev_compress;avg_decompress_s;stddev_decompress;ratio" > test/results.csv

run_tests() {
	local file_name="$1"
	local path="$TEST_DIR/$2"
	local symbol_size="$3"
	local extension="${path##*.}"

	local original_size=$(stat -c%s "$path")

	local compressed
	local restored

	compressed=$(mktemp --suffix=.huff)
	restored=$(mktemp --suffix=".$extension")

	local compress_sum=0
	local decompress_sum=0

	declare -a compress_times
	declare -a decompress_times

	local t

	echo "Testing $file_name"

	for ((i=1; i<=RUNS; i++)); do
		t=$(
			{ /usr/bin/time -f "%e" \
			"$PROGRAM" compress "$path" "$compressed" "$symbol_size" \
			> /dev/null; } 2>&1
		)
		compress_times+=("$t")
		compress_sum=$(awk "BEGIN {print $compress_sum + $t}")

		t=$(
			{ /usr/bin/time -f "%e" \
			"$PROGRAM" decompress "$compressed" "$restored" \
			>/dev/null; } 2>&1
		)
		decompress_times+=("$t")
		decompress_sum=$(awk "BEGIN {print $decompress_sum + $t}")
	done

	if ! cmp -s "$path" "$restored"; then
		echo -e "${RED}FAIL${NC}: $file_name"
		return 1
	fi

	local compressed_size=$(stat -c%s "$compressed")
	local ratio=$(awk "BEGIN {print $compressed_size / $original_size}")

	local avg_compress=$(awk "BEGIN {print $compress_sum / $RUNS}")
	local avg_decompress=$(awk "BEGIN {print $decompress_sum / $RUNS}")

	local stddev_compress=$(
		printf '%s\n' "${compress_times[@]}" |
		awk -v mean="$avg_compress" '
		{
			sum += ($1 - mean)^2
			n++
		}
		END {
			print sqrt(sum / (n - 1))
		}'
	)

	local stddev_decompress=$(
		printf '%s\n' "${decompress_times[@]}" |
		awk -v mean="$avg_decompress" '
		{
			sum += ($1 - mean)^2
			n++
		}
		END {
			print sqrt(sum / (n - 1))
		}'
	)

	printf "%s;%s;%s;%s;%s;%s;%s;%s;%s\n" \
		"$path" \
		"$symbol_size" \
		"$original_size" \
		"$compressed_size" \
		"$avg_compress" \
		"$stddev_compress" \
		"$avg_decompress" \
		"$stddev_decompress" \
		"$ratio" \
		>> test/results.csv

	rm -f "$compressed" "$restored"
}

for format in "txt" "jpg" "bmp" "mp4" "zip"; do
	for size in 1 5 10 20 40; do
		run_tests "$format ${size}mb" "$format/${size}mb.$format" 1
	done
done
