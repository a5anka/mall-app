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
#include <stdlib.h>
#include <ccn/charbuf.h>
#include <ccn/indexbuf.h>
#include <ccn/ccn.h>

#include "prefixstore.h"

#define PREFIX_NODE_CAPACITY 10

struct prefixNode
{
  struct ccn_charbuf *prefix;
  struct prefixNode *children[PREFIX_NODE_CAPACITY];
  size_t children_n;
  int dirty;
};


struct PrefixS_T
{
  struct prefixNode *children[PREFIX_NODE_CAPACITY];
  size_t children_n;
};

// Local prototypes
static struct prefixNode *get_matching_head_node(PrefixS_T store,
                                                 const unsigned char *data,
                                                 const struct ccn_indexbuf *indexbuf);
static struct prefixNode *create_prefixNode(struct ccn_charbuf *prefix);
static int get_farthest_matching_index(struct prefixNode *head,
                                       const unsigned char *data,
                                       const struct ccn_indexbuf *indexbuf,
                                       struct prefixNode **parent_ptr);


PrefixS_T create_prefixStore()
{
  PrefixS_T template = calloc(1, sizeof(*template));
  return template;
}

static struct prefixNode *create_tail(const unsigned char *data,
                                      const struct ccn_indexbuf *indexbuf,
                                      int from)
{
  const unsigned char *prefix_comp = NULL;
  size_t prefix_comp_len;
  struct ccn_charbuf *comp = ccn_charbuf_create();
  struct prefixNode *head = NULL, *current = NULL;

  int i;
  for (i = from; i < indexbuf->n-1; i++) {
    ccn_name_comp_get(data, indexbuf, i,
                      &prefix_comp, &prefix_comp_len);
    comp = ccn_charbuf_create();
    ccn_charbuf_append(comp, prefix_comp, prefix_comp_len);

    if (!head)
      {
        head = current = create_prefixNode(comp);
      }
    else
      {
        current->children[current->children_n++] = create_prefixNode(comp);
        current = current->children[current->children_n-1];
      }
  }

  return head;
}

void add_name_to_store(PrefixS_T store, struct ccn_charbuf *name)
{
  struct ccn_indexbuf *components = ccn_indexbuf_create();
  ccn_name_split(name, components);

  struct prefixNode *matching_node = get_matching_head_node(store, name->buf,components);

  if (!matching_node)
    {
      store->children[store->children_n++] = create_tail(name->buf, components, 0);
      return;
    }

  int farthest_match = get_farthest_matching_index(matching_node, name->buf, components,
                                                   &matching_node);

  matching_node->children[matching_node->children_n++] =
    create_tail(name->buf, components, farthest_match);

}

struct ccn_charbuf *get_enumeration_list(PrefixS_T store, struct ccn_charbuf *prefix)
{
  struct ccn_charbuf *component_list = ccn_charbuf_create();

  struct ccn_indexbuf *components = ccn_indexbuf_create();
  ccn_name_split(prefix, components);

  struct prefixNode *matching_node = get_matching_head_node(store, prefix->buf,components);

  if (!matching_node)
    {
      ccn_charbuf_destroy(&component_list);
      return NULL;
    }

  int found = 1, i, res, distance = 1;
  while (found)
    {
      found = 0;
      for (i = 0; i < matching_node->children_n; i++) {
        res = ccn_name_comp_strcmp(prefix->buf, components,
                               distance, (const char *) matching_node->children[i]->prefix->buf);
        if (res == 0)
          {
            matching_node = matching_node->children[i];

            found = 1;
            break;
          }
      }

    }

  for (i = 0; i < matching_node->children_n; i++) {
    if ( i > 0)
      ccn_charbuf_append_string(component_list, ",");

    ccn_charbuf_append_charbuf(component_list, matching_node->children[i]->prefix);
  }

  return component_list;
}

static int get_farthest_matching_index(struct prefixNode *head,
                                       const unsigned char *data,
                                       const struct ccn_indexbuf *indexbuf,
                                       struct prefixNode **parent_ptr)
{
  int i, res, found = 1;
  int distance = 1;

  struct prefixNode *current = head;
  while (found)
    {
      found = 0;
      for (i = 0; i < current->children_n; i++) {
        res = ccn_name_comp_strcmp(data, indexbuf,
                               distance, (const char *) current->children[i]->prefix->buf);
        if (res == 0)
          {
            current = current->children[i];
            distance++;
            found = 1;
            break;
          }
      }

    }
  *parent_ptr = current;

  return distance;
}

static struct prefixNode *get_matching_head_node(PrefixS_T store,
                                                 const unsigned char *data,
                                                 const struct ccn_indexbuf *indexbuf)
{
  int i, res;
  struct prefixNode *matching_node = NULL;

  for (i = 0; i < store->children_n; i++) {
    res = ccn_name_comp_strcmp(data, indexbuf,
                               0, (const char *) store->children[i]->prefix->buf);
    if (res == 0)
      {
        matching_node = store->children[i];
        break;
      }
  }
  return matching_node;
}

static struct prefixNode *create_prefixNode(struct ccn_charbuf *prefix)
{
  struct prefixNode *template = calloc(1, sizeof(*template));
  template->prefix = prefix;
  template->dirty = 1;

  return template;
}
