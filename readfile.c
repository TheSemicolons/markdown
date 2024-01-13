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

#include <sys/types.h>

#include <limits.h>

/*
 * FILE*
 * ferror()
 */
#include <stdio.h>

/*
 * free()
 */
#include <stdlib.h>

#include "readfile.h"

/*
 * Read 2 MB at a time.
 */
const size_t	READCHUNK = 2097152;

/*
 * Size	Status
 *
 * !0	Success
 * 0	Invalid Parameters
 * 0	Error Opening File
 * 0	File Stream Error
 * 0	Too Much Input
 * 0	Out Of Memory
 */
FileInfo
readfile(char *filename)
{
	FILE		*in;
	FileInfo	 results;
	char		*data = NULL;
	char		*temp = NULL;
	size_t		 n = 0;
	size_t		 size = 0;
	size_t		 used = 0;

	if (filename[0] == '\0') {
		results.content = "Invalid Parameters";
		results.size = 0;

		return results;
	}

	if ((in = fopen(filename, "r")) == NULL) {
		results.content = "Error Opening File";
		results.size = 0;

		fclose(in);

		return results;
	}

	if (ferror(in)) {
		results.content = "File Stream Error";
		results.size = 0;

		fclose(in);

		return results;
	}

	results.name = filename;

	while (1) {
		if ((size + READCHUNK + 1) > size) {
			size = (used + READCHUNK + 1);

			if (size <= used) {
				free(data);

				results.content = "Too Much Input";
				results.size = 0;

				fclose(in);

				return results;
			}

			temp = realloc(data, size);

			if (temp == NULL) {
				free(data);

				results.content = "Out Of Memory";
				results.size = 0;

				fclose(in);

				return results;
			}

			data = temp;
		}

		n = fread(data + used, 1, READCHUNK, in);

		if (n == 0) {
			break;
		}

		used += n;
	}

	if (ferror(in)) {
		free(data);

		results.content = "File Stream Error";
		results.size = 0;

		fclose(in);

		return results;
	}

	temp = realloc(data, used + 1);

	if (temp == NULL) {
		free(data);

		results.content = "Out Of Memory";
		results.size = 0;

		fclose(in);

		return results;
	}

	data = temp;
	data[used] = '\0';

	results.content = data;
	results.size = used;

	fclose(in);

	return results;
}
