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
 * const DT_*
 * const MAXNAMLEN
 * struct DIR
 * struct dirent
 * closedir()
 * opendir()
 * readdir()
 */
#include <dirent.h>

/*
 * const std*
 */
#include <stdio.h>

/*
 * free()
 * reallocarray()
 * realpath()
 * malloc()
 * qsort()
 */
#include <stdlib.h>

/*
 * strlcpy()
 * strncmp()
 */
#include <string.h>

#include "listing.h"

void
addEnt(Entry ***addTo, const char *name, const char *path, const char *folder, const int type, const size_t index)
{
	(*(*addTo)) = reallocarray((*(*addTo)), index + 1, sizeof(Entry));

	(*(*addTo))[index].name = malloc(MAXNAMLEN + 1);
	(void)strlcpy((*(*addTo))[index].name, name, MAXNAMLEN + 1);

	(*(*addTo))[index].path = malloc(strlen(path) + 1);
	(void)strlcpy((*(*addTo))[index].path, path, PATH_MAX + 1);

	(*(*addTo))[index].folder = malloc(strlen(folder) + 1);
	(void)strlcpy((*(*addTo))[index].folder, folder, PATH_MAX + 1);

	(*(*addTo))[index].type = type;
}

int
cmpName(const void *va, const void *vb)
{
	const Entry	*a = ((const Entry*) va);
	const Entry	*b = ((const Entry*) vb);
	int		 nameCompare = strncmp(a->name, b->name, MAXNAMLEN);
	int		 pathCompare = strncmp(a->path, b->path, MAXNAMLEN);

	if (pathCompare == 0) {
		return nameCompare;
	} else {
		return pathCompare;
	}
}

/*
 * listType
 *	0   all
 *	1   directory
 *	2   file
 * recursive
 *	0   no
 *	1   yes
 */
void
listDir(Entry **ents, const char *folder, const int listType, const int recursive, size_t *index)
{
	DIR	*dir;
	struct	 dirent *ent = NULL;
	char	 pathBuf[PATH_MAX];

	if ((dir = opendir(folder)) != NULL) {
		char	*path = realpath(folder, pathBuf);

		while ((ent = readdir(dir)) != NULL) {
			if ((strncmp(ent->d_name, ".", 1) == 0) || (strncmp(ent->d_name, "..", 2) == 0)) {
				continue;
			}

			if ((ent->d_type == DT_REG) && ((listType == 0) || (listType == 2))) {
				addEnt(&ents, ent->d_name, path, folder, ent->d_type, (*index)++);
			}

			if ((ent->d_type == DT_DIR) && ((listType == 0) || (listType == 1))) {
				addEnt(&ents, ent->d_name, path, folder, ent->d_type, (*index)++);
			}

			if ((ent->d_type == DT_DIR) && (recursive == 1)) {
				char subfolder[255];

				if (strncmp(folder, ".", 1) == 0) {
					(void)strncpy(subfolder, ent->d_name, sizeof(subfolder) - 1 - strlen(subfolder));
				} else {
					(void)strncpy(subfolder, folder, sizeof(subfolder) - 1);
					(void)strncat(subfolder, "/", sizeof(subfolder) - 1 - strlen(subfolder));
					(void)strncat(subfolder, ent->d_name, sizeof(subfolder) - 1 - strlen(subfolder));
				}

				listDir(ents, subfolder, listType, recursive, index);
			}
		}

		closedir(dir);

		qsort((*ents), (*index), sizeof(*(*ents)), cmpName);
	} else {
		perror("");

		index = 0;
	}

	return;
}
