set -eou pipefail

GREEN='\033[0;32m'
RED='\033[0;31m'
BLUE='\e[0;34m'
NC='\033[0m'

PROGRAM=./huffArchiver
RTESTS='test_data/roundabout'

echo -e "${BLUE}Running tests...${NC}"
echo ""

cd test

run_tests() {
	local test_number="$1"
	local test_name="$2"
	local input="$3"
	local extension="${input##*.}"
	local symbol_size="$4"

	local compressed
	local restored

	compressed=$(mktemp --suffix=.huff)
	restored=$(mktemp --suffix=".$extension")

	echo "Test $test_number: $test_name (symbol size = $symbol_size)"

	"$PROGRAM" compress "$input" "$compressed" "$symbol_size"
    	"$PROGRAM" decompress "$compressed" "$restored"

	if cmp -s "$input" "$restored"; then
        	echo -e "${GREEN}PASS${NC}"
    	else
		echo -e "${RED}FAIL${NC}"
		diff -u "$input" "$restored" || true
		rm -f "$compressed" "$restored"
		exit 1
	fi

	rm -f "$compressed" "$restored"
}

for size in 1 2 3 4; do
	run_tests "$size" "Empty file" "$RTESTS/empty/input.txt" "$size"
done

run_tests 5 "One symbol" "$RTESTS/one_symbol_1/input.txt" 1

for size in 1 2 3 4; do
	run_tests $((size + 5)) "One type of symbol" "$RTESTS/one_symbol_2/input.txt" "$size"
done

for size in 1 2 3 4; do
	run_tests $((size + 9)) "Multiple types of symbols" "$RTESTS/multiple_symbols_$size/input.txt" "$size"
done

for size in 1 2 3 4; do
	run_tests $((size + 13)) "Symbols of equal frequencies" "$RTESTS/equal_freq_$size/input.txt" "$size"
done

run_tests 18 "Scewed frequencies" "$RTESTS/scewed_freq_1/input.txt" 1

run_tests 19 "Scewed frequencies" "$RTESTS/scewed_freq_2/input.txt" 3
