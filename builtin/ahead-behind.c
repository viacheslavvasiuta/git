#include "builtin.h"
#include "parse-options.h"
#include "config.h"

static const char * const ahead_behind_usage[] = {
	N_("git ahead-behind --base=<ref> [ --stdin | <revs> ]"),
	NULL
};

int cmd_ahead_behind(int argc, const char **argv, const char *prefix)
{
	const char *base_ref = NULL;
	int from_stdin = 0;

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

	return 0;
}
