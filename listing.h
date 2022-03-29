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

#ifndef LISTING_H
#define LISTING_H

struct entry {
	char	*folder;
	char	*name;
	char	*path;
	int	 type;
/*
	mode_t	 mode;
	mode_t	 tmode;
	nlink_t	 nlink;
	uid_t	 uid;
	gid_t	 gid;
	off_t	 size;
	struct	 timespec time;
	dev_t	 dev;
	dev_t	 rdev;
	ino_t	 ino;
	ino_t	 tino;
 */
};
typedef struct entry Entry;

void	 addEnt(Entry ***, const char *, const char *, const char *, const int, const size_t);
int	 cmpName(const void *, const void *);
void	 listDir(Entry **, const char *, const int, const int, size_t *);

#endif
