/*
 * Copyright (c) 2024 James Anderson <thesemicolons@protonmail.com>
 *
 * Permission to use, copy, modify, and distribute this software for any purpose
 * with or without fee is hereby granted, provided that the above copyright
 * notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

#include <sys/stat.h>
#include <sys/types.h>

#include <limits.h>

/*
 * err()
 */
#include <err.h>

#include <errno.h>

/*
 * fprintf()
 */
#include <stdio.h>

/*
 * exit()
 * free()
 * realpath()
 */
#include <stdlib.h>

/*
 * strlcpy()
 * strncmp()
 */
#include <string.h>

/*
 * char *optarg
 * int optind
 * getopt()
 * pledge()
 * unveil()
 */
#include <unistd.h>

/*
 * listDir()
 */
#include "listing.h"
#include "markdown.h"
#include "readfile.h"

static void __dead	 usage(char *);

int
main(int argc, char *argv[])
{
	int	 ch = 0;
	size_t	 n = 0;
	size_t	 index = 0;
	int	 mflag = 0;
	int	 rflag = 0;
	int	 tflag = 0;
	int	 vflag = 0;
	char	*iflag;
	char	*oflag;
	struct	 stat sb;

	Entry		*ents = NULL;

	if (pledge("cpath rpath stdio unveil wpath", NULL) == -1) {
		err(1, "pledge");
	}

	while ((ch = getopt(argc, argv, "mvi:o:")) != -1) {
		switch (ch) {
		case 'm':
			mflag = 1;
			break;
		case 'v':
			vflag = 1;
			break;
		case 'i':
			iflag = optarg;
			break;
		case 'o':
			oflag = optarg;
			break;
		default:
			usage(argv[0]);
		}
	}

	if (argc < 2) {
		usage(argv[0]);
	}

	argc -= optind;
	argv += optind;

	if (vflag) {
		(void)fprintf(stdout, "Input Directory: %s\n", iflag);
		(void)fprintf(stdout, "Output Directory: %s\n", oflag);
	}

/*
 * Check if input is a directory, and make sure output does not exist.
 */
	if ((stat(iflag, &sb) != 0) || (!S_ISDIR(sb.st_mode))) {
		(void)fprintf(stderr, "Input folder, %s, does not exist.\n", iflag);

		exit(0);
	}

	if ((stat(oflag, &sb) == 0) && S_ISDIR(sb.st_mode)) {
		(void)fprintf(stderr, "Output folder, %s, exists.\n", oflag);

		exit(0);
	}
/*
 * Unveil needed directories, then remove unveil from pledge.
 */
	fprintf(stdout, "iflag: %s\n", iflag);
	if (unveil(iflag, "r") == -1) {
		err(1, "unveil");
	}

	fprintf(stdout, "oflag: %s\n", iflag);
	/*
	if (unveil(oflag, "crw") == -1) {
		err(1, "unveil");
	}

	if (unveil(NULL, NULL) == -1) {
		err(1, "unveil");
	}
	*/

	if (pledge("cpath rpath stdio wpath", NULL) == -1) {
		err(1, "pledge");
	}

/*
 * TEST CODE
 */
	tflag = 0;
/*
 * TEST CODE
 */
	listDir(&ents, iflag, tflag, rflag, &index);
	if (index > 0) {
		for (n = 0; n < index; n++) {
			FileInfo	 f;
			char		 path[255];


			(void)strncpy(path, ents[n].folder, sizeof(path) - 1);
			(void)strncat(path, "/", sizeof(path) - 1 - strlen(path));
			(void)strncat(path, ents[n].name, sizeof(path) - 1 - strlen(path));

			if (vflag) {
				(void)fprintf(stdout, "%zu - %s - %s - %s - %i\n", n, ents[n].path, ents[n].folder, ents[n].name, ents[n].type);
			}

			if (strstr(ents[n].name, ".md")) {
				if (vflag) {
					(void)fprintf(stdout, "markdown file: %s\n", path);
				}

				f = readfile(path);
				parseFile(f, vflag);
				free(f.content);
			}

			free(ents[n].name);
			free(ents[n].path);
			free(ents[n].folder);
		}

		free(ents);
	} else {
		(void)fprintf(stderr, "The file list was empty.\n");
	}

/*
 * We are done with cpath, rpath, and wpath.
 */
	if (pledge("stdio", NULL) == -1) {
		err(1, "pledge");
	}

	return 0;
}

static __dead void
usage(char *progname)
{
	(void)fprintf(stderr, "usage: %s [-m] [-v] [-i source directory] [-o output directory]\n", progname);

	exit(1);
}
