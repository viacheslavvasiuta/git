#!/bin/sh

test_description='Commit walk performance tests'
. ./perf-lib.sh

test_perf_large_repo

test_expect_success 'setup' '
	git for-each-ref --format="%(refname)" "refs/heads/*" "refs/tags/*" >allrefs &&
	sort -r allrefs | head -n 50 >refs &&
	git commit-graph write --reachable
'

test_perf 'ahead-behind counts: git ahead-behind' '
	git ahead-behind --base=HEAD --stdin <refs
'

test_perf 'ahead-behind counts: git rev-list' '
	for r in $(cat refs)
	do
		git rev-list --count "HEAD..$r" || return 1
	done
'

test_perf 'ahead-behind counts: git ahead-behind --contains' '
	git ahead-behind --contains --base=HEAD --stdin <refs
'

test_done
