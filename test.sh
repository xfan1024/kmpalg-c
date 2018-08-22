#!/bin/bash

kmp_test(){
    pattern="$1"
    string="$2"

    message=`./kmp_test "$pattern" "$string"`
    if test "$?" == "0"; then
        echo "[TEST  OK ] \"$pattern\" \"$string\""
    else
        echo "[TEST FAIL] \"$pattern\" \"$string\""
        printf "%s\n" "$message"
    fi
}

kmp_test abcabcccc aaabbbcccabcabcabcabcabcccabcbacabcabcabcabcccccccccabcbabbbb
kmp_test abcabcddabcabcddabcabcddabcabcddff abcabcddabcabcddabcabcddabcabcddabcabcddabcabcddff
kmp_test "" test_pattern_is_empty_string
kmp_test abaaa abaabaaa
kmp_test test_not_found match_should_be_failed


