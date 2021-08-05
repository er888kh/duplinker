#pragma once

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>

#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "enames.h"
#include "../constants.hpp"

void
terminate(bool useExit3)
{
	char *s;
	/* Dump core if EF_DUMPCORE environment variable is defined and
	   is a nonempty string; otherwise call exit(3) or _exit(2),
	   depending on the value of 'useExit3'. */
	s = getenv("EF_DUMPCORE");
	if (s != NULL && *s != '\0')
		abort();
	else if (useExit3)
		exit(EXIT_FAILURE);
	else
		_exit(EXIT_FAILURE);
}

void
outputError(const bool useErr, const int err,
		bool flushStdout, const char* format, va_list ap)
{
	char buf[BUF_SIZE], userMsg[BUF_SIZE], errText[BUF_SIZE];

	vsnprintf(userMsg, BUF_SIZE, format, ap);

	if (useErr) {
		snprintf(errText, BUF_SIZE, " [%s, %s]",
				(err > 0 && err <= MAX_ENAME) ?
				ename[err] : "?UNKNOWN?", strerror(err));
	} else {
		snprintf(errText, BUF_SIZE, ":");
	}

	snprintf(buf, BUF_SIZE, "ERROR%s %s\n", errText, userMsg);

	if (flushStdout) {
		fflush(stdout);
	}

	fputs(buf, stderr);
	fflush(stderr);
	return;
}

void
errMsg(const char *format, ...)
{
	va_list argList;
	int savedErrno;
	savedErrno = errno;
	/* In case we change it here */
	va_start(argList, format);
	outputError(true, errno, true, format, argList);
	va_end(argList);
	errno = savedErrno;
	return;
}

void
errExit(const char *format, ...)
{
	va_list argList;
	va_start(argList, format);
	outputError(true, errno, true, format, argList);
	va_end(argList);
	terminate(true);
}

void
err_exit(const char *format, ...)
{
	va_list argList;
	va_start(argList, format);
	outputError(true, errno, false, format, argList);
	va_end(argList);
	terminate(false);
}
 
void
errExitEN(int errnum, const char *format, ...)
{
	va_list argList;
	va_start(argList, format);
	outputError(true, errnum, true, format, argList);
	va_end(argList);
	terminate(true);
}
 
void
fatal(const char *format, ...)
{
	va_list argList;
	va_start(argList, format);
	outputError(false, 0, true, format, argList);
	va_end(argList);
	terminate(true);
}

void
usageErr(const char *format, ...)
{
	va_list argList;
	fflush(stdout);
	/* Flush any pending stdout */
	fprintf(stderr, "Usage: ");
	va_start(argList, format);
	vfprintf(stderr, format, argList);
	va_end(argList);
	fflush(stderr);
	exit(EXIT_FAILURE);
}

void
cmdLineErr(const char *format, ...)
{
	va_list argList;
	fflush(stdout);
	/* Flush any pending stdout */
	fprintf(stderr, "Command-line usage error: ");
	va_start(argList, format);
	vfprintf(stderr, format, argList);
	va_end(argList);
	fflush(stderr);
	exit(EXIT_FAILURE);
	/* In case stderr is not line-buffered */
}
