/* vi: set ts=2 shiftwidth=2 expandtab:
 *
 * Copyright (C) 2001-2006  Simon Baldwin (simon_baldwin@yahoo.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef _CWDICTIONARY_H
#define _CWDICTIONARY_H

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct dictionary_s dictionary;

extern void dictionary_unload (void);
extern int dictionary_load (const char *file);
extern const dictionary *dictionary_iterate (const dictionary *dict);
extern int dictionary_write (const char *file);
extern const char *get_dictionary_description (const dictionary *dict);
extern int get_dictionary_group_size (const dictionary *dict);
extern const char *get_dictionary_random_word (const dictionary *dict);

#if defined(__cplusplus)
}
#endif
#endif  /* _CWDICTIONARY_H */
