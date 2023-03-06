#!/bin/sh

test_description='git ahead-behind command-line options'

. ./test-lib.sh

test_expect_success 'git ahead-behind -h' '
	test_must_fail git ahead-behind -h >out &&
	grep "usage:" out
'

test_expect_success 'git ahead-behind without --base' '
	test_must_fail git ahead-behind HEAD 2>err &&
	grep "usage:" err
'

test_done
