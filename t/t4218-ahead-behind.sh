#!/bin/sh

test_description='git ahead-behind command-line options'

. ./test-lib.sh

test_expect_success 'setup simple history' '
	test_commit base &&
	git checkout -b right &&
	test_commit right &&
	git checkout -b left base &&
	test_commit left &&
	git checkout -b merge &&
	git merge right -m "merge"
'

test_expect_success 'git ahead-behind -h' '
	test_must_fail git ahead-behind -h >out &&
	grep "usage:" out
'

test_expect_success 'git ahead-behind without --base' '
	test_must_fail git ahead-behind HEAD 2>err &&
	grep "usage:" err
'

test_expect_success 'git ahead-behind with broken --base' '
	test_must_fail git ahead-behind --base=bogus HEAD 2>err &&
	grep "could not resolve '\''bogus'\''" err
'

test_expect_success 'git ahead-behind with broken tip' '
	test_must_fail git ahead-behind --base=HEAD bogus 2>err &&
	grep "could not resolve '\''bogus'\''" err
'

test_expect_success 'git ahead-behind with broken tip and --ignore-missing' '
	git ahead-behind --base=HEAD --ignore-missing bogus 2>err >out &&
	test_must_be_empty err &&
	test_must_be_empty out
'

test_expect_success 'git ahead-behind --contains with broken tip' '
	test_must_fail git ahead-behind --contains \
		--base=HEAD bogus 2>err &&
	grep "could not resolve '\''bogus'\''" err
'

test_expect_success 'git ahead-behind --contains with broken tip and --ignore-missing' '
	git ahead-behind --base=HEAD --contains \
		--ignore-missing bogus 2>err >out &&
	test_must_be_empty err &&
	test_must_be_empty out
'

test_expect_success 'git ahead-behind without tips' '
	git ahead-behind --base=HEAD 2>err &&
	test_must_be_empty err
'

test_expect_success 'git ahead-behind --base=base' '
	git ahead-behind --base=base base left right merge >actual &&

	cat >expect <<-EOF &&
	base 0 0
	left 1 0
	right 1 0
	merge 3 0
	EOF

	test_cmp expect actual
'

test_expect_success 'git ahead-behind --base=left' '
	git ahead-behind --base=left base left right merge >actual &&

	cat >expect <<-EOF &&
	base 0 1
	left 0 0
	right 1 1
	merge 2 0
	EOF

	test_cmp expect actual
'

test_expect_success 'git ahead-behind --base=right' '
	git ahead-behind --base=right base left right merge >actual &&

	cat >expect <<-EOF &&
	base 0 1
	left 1 1
	right 0 0
	merge 2 0
	EOF

	test_cmp expect actual
'

test_expect_success 'git ahead-behind --base=merge' '
	git ahead-behind --base=merge base left right merge >actual &&

	cat >expect <<-EOF &&
	base 0 3
	left 0 2
	right 0 2
	merge 0 0
	EOF

	test_cmp expect actual
'

test_expect_success 'git ahead-behind --contains --base=base' '
	git ahead-behind --contains --base=base \
		base left right merge >actual &&

	cat >expect <<-EOF &&
	base
	EOF

	test_cmp expect actual
'

test_expect_success 'git ahead-behind --contains --base=left' '
	git ahead-behind --contains --base=left \
		base left right merge >actual &&

	cat >expect <<-EOF &&
	base
	left
	EOF

	test_cmp expect actual
'

test_expect_success 'git ahead-behind --contains --base=right' '
	git ahead-behind --contains --base=right \
		base left right merge >actual &&

	cat >expect <<-EOF &&
	base
	right
	EOF

	test_cmp expect actual
'

test_expect_success 'git ahead-behind --contains --base=merge' '
	git ahead-behind --contains --base=merge \
		base left right merge >actual &&

	cat >expect <<-EOF &&
	base
	left
	right
	merge
	EOF

	test_cmp expect actual
'

test_done
