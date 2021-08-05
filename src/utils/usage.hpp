#pragma once

#include "error_exit.hpp"

void
print_usage(const char* program_name="deduper")
{
	usageErr("%s [config file]\n"
			"if no config file is supplied, the default one"
			"`/etc/deduper/config.json` is used\n", program_name);

	return;
}
