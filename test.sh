#!/bin/sh

./search_in_stream
./kmp_test abcabcccc aaabbbcccabcabcabcabcabcccabcbacabcabcabcabcccccccccabcbabbbb
./kmp_test abcabcddabcabcddabcabcddabcabcddff abcabcddabcabcddabcabcddabcabcddabcabcddabcabcddff
./kmp_test "" test_pattern_is_empty_string
./kmp_test test_not_found match_should_be_failed

