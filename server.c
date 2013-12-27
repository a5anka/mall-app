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

#include <ccn/ccn.h>
#include <ccn/uri.h>

#include "server.h"

// Main data structure used
struct Server_T
{
  struct ccn_charbuf *basename; // The namespace we are serving
  struct ccn         *handler;  // CCNx handler
  const  char        *prefix;
};

// Prototypes for local functions
static void add_prefix_to_server(Server_T,
				 const char *);
static enum ccn_upcall_res incoming_interest(struct ccn_closure *selfp,
					     enum ccn_upcall_kind kind,
					     struct ccn_upcall_info *info);
static Server_T init_server_obj();
static void register_server_in_ccnd(Server_T,
				    const char *);

// Interface function definitions
Server_T
create_server(const char * name)
{
  Server_T tmpl_server = init_server_obj();
  if (tmpl_server == NULL)
    {
      return NULL;
    }

  register_server_in_ccnd(tmpl_server, name);
  return tmpl_server;
}

void
destroy_server(Server_T *server_ptr)
{
  Server_T server_obj = *server_ptr;

  ccn_disconnect(server_obj->handler);
  ccn_destroy(&server_obj->handler);

  ccn_charbuf_destroy(&(server_obj->basename));

  free(server_obj);
  server_ptr = NULL;
}

void server_run(Server_T server, unsigned int timeout)
{
  ccn_run(server->handler, timeout);
}

// Local function definitions
/*
 * Interest handler
 *
 */
static void
add_prefix_to_server(Server_T server, const char *name)
{
  server->prefix = name;
  ccn_name_from_uri(server->basename, name);
}

static enum ccn_upcall_res incoming_interest(struct ccn_closure *selfp,
		  enum ccn_upcall_kind kind,
		  struct ccn_upcall_info *info)
{
  Server_T server = selfp->data;
  struct ccn_charbuf *cob = ccn_charbuf_create();
  struct ccn_signing_params sp = CCN_SIGNING_PARAMS_INIT;

  ccn_sign_content(server->handler, cob, server->basename, &sp, "Message", 8);
  ccn_put(server->handler, cob->buf, cob->length);

  return(CCN_UPCALL_RESULT_OK);
}

static Server_T
init_server_obj()
{
  Server_T tmpl_server;
  tmpl_server = malloc(sizeof(*tmpl_server));
  tmpl_server->basename = ccn_charbuf_create();

  tmpl_server->handler = ccn_create();
  if (tmpl_server->handler == NULL || ccn_connect(tmpl_server->handler, NULL) == -1)
    {
      return NULL;
    }

  return tmpl_server;
}

static void register_server_in_ccnd(Server_T server, const char * name)
{
  add_prefix_to_server(server, name);

  struct ccn_closure *in_interest;
  in_interest = calloc(1, sizeof(in_interest));
  in_interest->p = &incoming_interest;
  in_interest->data = server;

  ccn_set_interest_filter(server->handler, server->basename, in_interest);
}
