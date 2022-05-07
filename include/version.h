#ifndef VERSION__H
#define VERSION__H

#if defined(KERBEROS)
#define DRA_VEREXTRA "-kerberos"
#elif defined(REU)
#define DRA_VEREXTRA "-reu"
#else
#define DRA_VEREXTRA ""
#endif

#define DRA_VER DRA_VERNUM DRA_VEREXTRA " " DRA_VERDATE

#endif
