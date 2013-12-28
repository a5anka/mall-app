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

#include <stdio.h>
#include <string.h>

#include <ccn/uri.h>

#include "ccn_handler.h"

struct ccn_charbuf *create_name_for_prefix(const char * name)
{
  struct ccn_charbuf *templ = ccn_charbuf_create();
  ccn_name_from_uri(templ, name);

  return templ;
}

struct ccn_charbuf *create_interest_for_name(struct ccn_charbuf * name)
{
  struct ccn_charbuf *templ = ccn_charbuf_create();
  ccnb_element_begin(templ, CCN_DTAG_Interest);
  ccn_charbuf_append_charbuf(templ, name);
  ccnb_element_end(templ); /* </Interest> */

  return templ;
}

struct ccn_charbuf *create_interest_excluding(struct ccn_charbuf *name,
                                              struct ccn_charbuf *version)
{
  struct ccn_charbuf *templ = ccn_charbuf_create();
  ccn_charbuf_append_charbuf(templ, name);
  ccnb_element_begin(templ, CCN_DTAG_Exclude);
  ccnb_tagged_putf(templ, CCN_DTAG_Any, "");
  ccn_charbuf_append(templ, version->buf+1, version->length-2);
  ccnb_element_end(templ); /* </Exclude> */
  ccnb_tagged_putf(templ, CCN_DTAG_ChildSelector, "%d", 1);

  struct ccn_charbuf *res = create_interest_for_name(templ);

  ccn_charbuf_destroy(&templ);

  return res;
}

struct ccn_charbuf *get_version(struct ccn_upcall_info *info)
{
  struct ccn_charbuf *c = ccn_charbuf_create();
  const unsigned char *ver = NULL;
  size_t ver_size = 0;

  if (info->content_comps->n > info->matched_comps + 1) {
    ccn_name_init(c);
    ccn_ref_tagged_BLOB(CCN_DTAG_Component, info->content_ccnb,
                        info->content_comps->buf[info->matched_comps],
                        info->content_comps->buf[info->matched_comps + 1],
                        &ver,
                        &ver_size);
    //if (res < 0) abort();
    ccn_name_append(c, ver, ver_size);
    return c;
  }

  ccn_charbuf_destroy(&c);
  return NULL;
}

void send_string_with_version(struct ccn *handler,
                              struct ccn_charbuf *prefix,
                              const char *data)
{
  struct ccn_charbuf *name = ccn_charbuf_create();
  struct ccn_charbuf *cob = ccn_charbuf_create();
  struct ccn_signing_params sp = CCN_SIGNING_PARAMS_INIT;
  size_t length = strlen(data);

  ccn_charbuf_append_charbuf(name, prefix);
  ccn_create_version(handler, name, CCN_V_NOW, 0,0);
  ccn_sign_content(handler, cob, name, &sp, data, length);

  ccn_put(handler, cob->buf, cob->length);

  ccn_charbuf_destroy(&name);
  ccn_charbuf_destroy(&cob);
}
