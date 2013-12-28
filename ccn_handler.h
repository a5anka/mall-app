#ifndef _CCN_HADLER_H_
#define _CCN_HADLER_H_

#include <ccn/ccn.h>

struct ccn_charbuf *create_name_for_prefix(const char *);
struct ccn_charbuf *create_interest_for_name(struct ccn_charbuf *);
struct ccn_charbuf *create_interest_excluding(struct ccn_charbuf *,
                                              struct ccn_charbuf *);
struct ccn_charbuf *get_version(struct ccn_upcall_info *info);
void send_string_with_version(struct ccn *, struct ccn_charbuf *, const char *);

#endif /* _CCN_HADLER_H_ */
