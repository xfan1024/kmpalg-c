#!/bin/sh

kmp_test(){
    pattern="$1"
    string="$2"
    expect="$3"

    message=`./kmp_test "$pattern" "$string"`
    if test "$?" = "0"; then
        kmp_test_result="match"
    else
        kmp_test_result="notmatch"
    fi
    if  test "$kmp_test_result" = "$expect"; then
        echo "[TEST  OK ] $kmp_test_result \"$pattern\" in \"$string\""
    else
        echo "[TEST FAIL] $kmp_test_result \"$pattern\" in \"$string\""
        printf "%s\n" "$message"
    fi
}

kmp_test_match() {
    kmp_test "$1" "$2" "match"
}

kmp_test_not_match() {
    kmp_test "$1" "$2" "notmatch"
}

./search_in_stream
kmp_test_match abcabcccc aaabbbcccabcabcabcabcabcccabcbacabcabcabcabcccccccccabcbabbbb
kmp_test_match abcabcddabcabcddabcabcddabcabcddff abcabcddabcabcddabcabcddabcabcddabcabcddabcabcddff
kmp_test_match "" test_pattern_is_empty_string
kmp_test_match abaaa abaabaaa
kmp_test_not_match test_not_found match_should_be_failed


