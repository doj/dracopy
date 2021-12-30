#ifndef VERSION__H
#define VERSION__H

#define DRA_VERNUM "1.1doj"
#define DRA_VERDATE "2021-12-31"

#if defined(KERBEROS)
#define DRA_VEREXTRA "-kerberos"
#elif defined(REU)
#define DRA_VEREXTRA "-reu"
#else
#define DRA_VEREXTRA ""
#endif

#define DRA_VER DRA_VERNUM DRA_VEREXTRA " " DRA_VERDATE

#endif
