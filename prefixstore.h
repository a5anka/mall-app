/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef _PREFIXSTORE_H_
#define _PREFIXSTORE_H_

#include <ccn/charbuf.h>

typedef struct PrefixS_T *PrefixS_T;

// Create a prefix store object
PrefixS_T create_prefixStore();

// Destroy the prefix store object
void destroy_prefixStore(PrefixS_T *store);

void add_name_to_store(PrefixS_T store, struct ccn_charbuf *name);

struct ccn_charbuf *get_enumeration_list(PrefixS_T store, struct ccn_charbuf *prefix);


#endif /* _PREFIXSTORE_H_ */
