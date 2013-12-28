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
#include <stdio.h>

#include <ccn/ccn.h>
#include <ccn/uri.h>

#include "client.h"
#include "ccn_handler.h"

// Main data structure used
struct Client_T
{
  struct ccn_charbuf *basename; // The namespace we are serving
  struct ccn         *handler;  // CCNx handler
  const  char        *prefix;
  struct ccn_closure *content_closure;
  struct ccn_charbuf *last_version;
};

// Prototypes for local functions
static void add_prefix_to_client(Client_T,
                                 const char *);
static enum ccn_upcall_res incoming_content(struct ccn_closure *,
                                            enum ccn_upcall_kind,
                                            struct ccn_upcall_info *);
static Client_T init_client_obj();

// Interface function definitions
Client_T
create_client(const char * name)
{
  Client_T tmpl_client = init_client_obj();
  if (tmpl_client == NULL)
    {
      return NULL;
    }

  add_prefix_to_client(tmpl_client, name);
  return tmpl_client;
}

void client_run(Client_T client, unsigned int timeout)
{
  ccn_run(client->handler, timeout);
}

void
destroy_client(Client_T *client_ptr)
{
  Client_T client_obj = *client_ptr;

  ccn_disconnect(client_obj->handler);
  ccn_destroy(&client_obj->handler);

  ccn_charbuf_destroy(&(client_obj->basename));
  ccn_charbuf_destroy(&(client_obj->last_version));

  free(client_obj);
  client_ptr = NULL;
}

void send_location_index_request(Client_T client)
{
  struct ccn_charbuf *interest_message;
  interest_message = create_interest_excluding(client->basename, client->last_version);

  ccn_express_interest(client->handler,
                       client->basename,
                       client->content_closure,
                       interest_message);

  ccn_charbuf_destroy(&interest_message);
}

// Local function definitions
static void
add_prefix_to_client(Client_T client, const char *name)
{
  client->prefix = name;
  ccn_name_from_uri(client->basename, name);
}

/*
 * Incoming content Handler
 *
 */
static enum ccn_upcall_res
incoming_content(struct ccn_closure *selfp,
                  enum ccn_upcall_kind kind,
                  struct ccn_upcall_info *info)
{
  size_t size;
  struct ccn_charbuf *content;
  content = ccn_charbuf_create();
  const unsigned char *data = NULL;
  Client_T client = selfp->data;

  switch (kind) {
  case CCN_UPCALL_CONTENT:
    size = info->pco->offset[CCN_PCO_E];
    ccn_charbuf_append(content, info->content_ccnb,size);

    ccn_content_get_value(content->buf, content->length, info->pco, &data, &size);

    printf("Received: %s\n", data);

    ccn_charbuf_destroy(&client->last_version);
    client->last_version = get_version(info);

    return(CCN_UPCALL_RESULT_OK);
  case CCN_UPCALL_FINAL:
    return(CCN_UPCALL_RESULT_OK);
  case CCN_UPCALL_INTEREST_TIMED_OUT:
    printf("UPCALL_INTEREST_TIMEDOUT\n");
    return(CCN_UPCALL_RESULT_ERR);
  default:
    printf("UPCALL_ERROR (Client)\n");
    return(CCN_UPCALL_RESULT_ERR);
  }
  return(CCN_UPCALL_RESULT_OK);
}

static Client_T
init_client_obj()
{
  Client_T tmpl_client;
  tmpl_client = malloc(sizeof(*tmpl_client));
  tmpl_client->basename = ccn_charbuf_create();

  tmpl_client->handler = ccn_create();
  if (tmpl_client->handler == NULL || ccn_connect(tmpl_client->handler, NULL) == -1)
    {
      return NULL;
    }

  tmpl_client->content_closure = calloc(1, sizeof(*tmpl_client->content_closure));
  tmpl_client->content_closure->p = &incoming_content;
  tmpl_client->content_closure->data = tmpl_client;

  tmpl_client->last_version = ccn_charbuf_create();
  ccn_name_from_uri(tmpl_client->last_version, "/%FD%00%FF%FF%FF%FF%FF");

  return tmpl_client;
}
