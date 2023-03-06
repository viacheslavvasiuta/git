#include "builtin.h"
#include "parse-options.h"
#include "config.h"
#include "commit.h"
#include "commit-reach.h"

static const char * const ahead_behind_usage[] = {
	N_("git ahead-behind --base=<ref> [ --contains ] [ --stdin | <revs> ]"),
	NULL
};

static int ignore_missing;

static int handle_arg(struct string_list *tips, const char *arg)
{
	struct string_list_item *item;
	struct commit *c = lookup_commit_reference_by_name(arg);

	if (!c) {
		if (ignore_missing)
			return 0;
		return error(_("could not resolve '%s'"), arg);
	}

	item = string_list_append(tips, arg);
	item->util = c;
	return 0;
}

int cmd_ahead_behind(int argc, const char **argv, const char *prefix)
{
	const char *base_ref = NULL;
	struct commit *base;
	int from_stdin = 0, contains = 0;
	struct string_list tips = STRING_LIST_INIT_DUP;
	struct commit **commits;
	struct ahead_behind_count *counts;
	size_t i;

	struct option ahead_behind_opts[] = {
		OPT_STRING('b', "base", &base_ref, N_("base"), N_("base reference to process")),
		OPT_BOOL(0 , "stdin", &from_stdin, N_("read rev names from stdin")),
		OPT_BOOL(0 , "ignore-missing", &ignore_missing, N_("ignore missing tip references")),
		OPT_BOOL(0 , "contains", &contains, N_("only check that tips are reachable from the base")),
		OPT_END()
	};

	argc = parse_options(argc, argv, NULL, ahead_behind_opts,
			     ahead_behind_usage, PARSE_OPT_KEEP_UNKNOWN_OPT);

	if (!base_ref)
		usage_with_options(ahead_behind_usage, ahead_behind_opts);

	git_config(git_default_config, NULL);

	base = lookup_commit_reference_by_name(base_ref);
	if (!base)
		die(_("could not resolve '%s'"), base_ref);

	if (from_stdin) {
		struct strbuf line = STRBUF_INIT;

		while (strbuf_getline(&line, stdin) != EOF) {
			if (!line.len)
				break;

			if (handle_arg(&tips, line.buf))
				return 1;
		}

		strbuf_release(&line);
	} else {
		int i;
		for (i = 0; i < argc; ++i) {
			if (handle_arg(&tips, argv[i]))
				return 1;
		}
	}

	/* Early return for no tips. */
	if (!tips.nr)
		return 0;

	if (contains) {
		struct string_list_item *item;

		/* clear out */
		for_each_string_list_item(item, &tips)
			item->util = NULL;

		tips_reachable_from_base(base, &tips);

		for_each_string_list_item(item, &tips) {
			if (item->util)
				printf("%s\n", item->string);
		}

		return 0;
	}
	/* else: not --contains, but normal ahead-behind counting. */

	ALLOC_ARRAY(commits, tips.nr + 1);
	ALLOC_ARRAY(counts, tips.nr);

	for (i = 0; i < tips.nr; i++) {
		commits[i] = tips.items[i].util;
		counts[i].tip_index = i;
		counts[i].base_index = tips.nr;
	}
	commits[tips.nr] = base;

	ahead_behind(commits, tips.nr + 1, counts, tips.nr);

	for (i = 0; i < tips.nr; i++)
		printf("%s %d %d\n", tips.items[i].string,
		       counts[i].ahead, counts[i].behind);

	free(counts);
	free(commits);
	return 0;
}
