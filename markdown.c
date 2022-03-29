/*
 * Copyright (c) 2022 James Anderson <thesemicolons@protonmail.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the
 * above copyright notice and this permission notice appear in all
 * copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
 * AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

#include <sys/types.h>

#include <limits.h>

/*
 * regcomp()
 * regerror()
 * regexec()
 * regfree()
 */
#include <regex.h>

/*
 * fprintf()
 */
#include <stdio.h>

/*
 * free()
 */
#include <stdlib.h>

/*
 * strncmp()
 * strtok_r()
 */
#include <string.h>

#include "readfile.h"
#include "markdown.h"

int
match(const char *string, const char *pattern)
{
	int	 status;
	regex_t	 re;

/* If there was an error. */
	if (regcomp(&re, pattern, REG_EXTENDED) != 0) {
		return 0;
	}

	status = regexec(&re, string, (size_t)0, NULL, 0);
	regfree(&re);

/* If there was an error. */
	if (status != 0) {
		return 0;
	}

	return 1;
}

int
parseFile(FileInfo file, int vflag)
{
	int	 codeBlock = 0;
	int	 lineCount = 0;
	int	 len = 0;
	size_t	 size = 0;
	char	*str = file.content;
	char	*start;
	char	*end;
	char	*curLine = NULL;
	char	*prevLine = NULL;

	start = end = str;

	while ((end = strchr(start, '\n'))) {
		len = (int)(end - start + 1);
		size = (size_t)snprintf(NULL, 0, "%.*s", len, start);
		curLine = malloc(size + 1);
		lineCount++;

		(void)snprintf(curLine, size, "%.*s", len, start);

/*
 * Print all the lines, because why not?
 */
		if (vflag) {
			(void)fprintf(stdout, "%d: \"%s\"\n", lineCount, curLine);
		}

/*
 * Block level elements.
 */

/* Parse headers. */
		if ((match(curLine, "^(-|=)+$")) && strncmp(prevLine, "\0", 1) != 0) {
			if (vflag) {
				(void)fprintf(stdout, "%s\n", "Header line.");
			}
		}

		if (match(curLine, "^(#)+[ ]*[^#]+[ ]*(#)*$")) {
			int	c = 1;
			int	h = 1;
			char	regex[255];
			char	hText[255];


/* Parse headers. */
			while (c <= 6) {
				(void)snprintf(regex, sizeof(regex), "^(#){1,%d}[ ]*[^#]+[ ]*(#)*$", c);
				if (match(curLine, regex)) {
/*
 * This removes header marks at the beginning. Need to remove end ones
 * as well. Preferably without reversing the string twice. I hope it's
 * not more regex.
 */
					(void)strlcpy(hText, curLine + c + 1, strlcpy(hText, curLine, sizeof(hText)));

					if (vflag) {
						(void)fprintf(stdout, "Header Text: %s.\n", hText);
					}

					c = 6;
				}

				c++;
				h++;
			}
		}

/* Parse horizontal lines. */
		if ((match(curLine, "^((-|\\*) ?){3,}$")) && strncmp(prevLine, "\0", 1) == 0) {
			if (vflag) {
				(void)fprintf(stdout, "%s\n", "Horizontal rule.");
			}
		}

/* Parse code blocks. */
		if (match(curLine, "^```[A-Za-z0-9]*$")) {
			char	 language[255];

			if (!match(curLine, "^```$")) {
				(void)strlcpy(language, curLine + 3, strlcpy(language, curLine, sizeof(language)) - 2);

				if (vflag) {
					(void)fprintf(stdout, "Code block language: %s\n", language);
				}
			}

			if (codeBlock == 0) {
				if (vflag) {
					(void)fprintf(stdout, "%s\n", "Begin code block.");
				}

				codeBlock = 1;
			} else {
				if (vflag) {
					(void)fprintf(stdout, "%s\n", "End code block.");
				}

				codeBlock = 0;
			}
		}

		start = end + 1;
		prevLine = curLine;
	}

/* Show the file size, just because. */
	if (vflag) {
		(void)fprintf(stdout, "Size: %zu\n", file.size);
	}

	return 0;
}
