#include "builtin.h"
#include "parse-options.h"
#include "config.h"
#include "commit.h"

static const char * const ahead_behind_usage[] = {
	N_("git ahead-behind --base=<ref> [ --stdin | <revs> ]"),
	NULL
};

static int handle_arg(struct string_list *tips, const char *arg)
{
	struct string_list_item *item;
	struct commit *c = lookup_commit_reference_by_name(arg);

	if (!c)
		return error(_("could not resolve '%s'"), arg);

	item = string_list_append(tips, arg);
	item->util = c;
	return 0;
}

int cmd_ahead_behind(int argc, const char **argv, const char *prefix)
{
	const char *base_ref = NULL;
	int from_stdin = 0;
	struct string_list tips = STRING_LIST_INIT_DUP;

	struct option ahead_behind_opts[] = {
		OPT_STRING('b', "base", &base_ref, N_("base"), N_("base reference to process")),
		OPT_BOOL(0 , "stdin", &from_stdin, N_("read rev names from stdin")),
		OPT_END()
	};

	argc = parse_options(argc, argv, NULL, ahead_behind_opts,
			     ahead_behind_usage, PARSE_OPT_KEEP_UNKNOWN_OPT);

	if (!base_ref)
		usage_with_options(ahead_behind_usage, ahead_behind_opts);

	git_config(git_default_config, NULL);

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

	return 0;
}
