#include "StdAfx.h"
#include "Util.h"

static Int64 lastTime = -1;
static DWORD lastGetTime;

Int64 GetTime()
{
	//return GetTickCount64();
	if(lastTime == -1)
	{
		lastGetTime = ::timeGetTime();
		lastTime = lastGetTime;
	}
	else
	{
		DWORD time = ::timeGetTime();
		if(time < lastGetTime)
		{
			Int64 dt = lastGetTime - time;
			lastTime += (0x100000000 - dt);
			lastGetTime = time;
		}
		else
		{
			lastTime += time - lastGetTime;
			lastGetTime = time;
		}
	}
	return lastTime;
}

#define MS_VC_EXCEPTION 0x406D1388

#pragma pack(push,8)
typedef struct tagTHREADNAME_INFO
{
   DWORD dwType; // Must be 0x1000.
   LPCSTR szName; // Pointer to name (in user addr space).
   DWORD dwThreadID; // Thread ID (-1=caller thread).
   DWORD dwFlags; // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)

void SetThreadName(DWORD dwThreadID, char* threadName)
{
   Sleep(10);
   THREADNAME_INFO info;
   info.dwType = 0x1000;
   info.szName = threadName;
   info.dwThreadID = dwThreadID;
   info.dwFlags = 0;

   __try
   {
      RaiseException(MS_VC_EXCEPTION, 0, sizeof(info)/sizeof(ULONG_PTR), (ULONG_PTR*)&info);
   }
   __except(EXCEPTION_EXECUTE_HANDLER)
   {
   }
}


//copy from crt

#define NULCHAR    _T('\0')
#define SPACECHAR  _T(' ')
#define TABCHAR    _T('\t')
#define DQUOTECHAR _T('\"')
#define SLASHCHAR  _T('\\')
#define _TSCHAR char
#define _TUCHAR unsigned char

#ifdef WPRFLAG
void wparse_cmdline (
#else  /* WPRFLAG */
void parse_cmdline (
#endif  /* WPRFLAG */
    _TSCHAR *cmdstart,
    _TSCHAR **argv,
    _TSCHAR *args,
    int *numargs,
    int *numchars
    )
{
        _TSCHAR *p;
        _TUCHAR c;
        int inquote;                    /* 1 = inside quotes */
        int copychar;                   /* 1 = copy char to *args */
        unsigned numslash;              /* num of backslashes seen */

        *numchars = 0;
        *numargs = 1;                   /* the program name at least */

        /* first scan the program name, copy it, and count the bytes */
        p = cmdstart;
        if (argv)
            *argv++ = args;

#ifdef WILDCARD
        /* To handle later wild card expansion, we prefix each entry by
        it's first character before quote handling.  This is done
        so _[w]cwild() knows whether to expand an entry or not. */
        if (args)
            *args++ = *p;
        ++*numchars;

#endif  /* WILDCARD */

        /* A quoted program name is handled here. The handling is much
           simpler than for other arguments. Basically, whatever lies
           between the leading double-quote and next one, or a terminal null
           character is simply accepted. Fancier handling is not required
           because the program name must be a legal NTFS/HPFS file name.
           Note that the double-quote characters are not copied, nor do they
           contribute to numchars. */
        inquote = FALSE;
        do {
            if (*p == DQUOTECHAR )
            {
                inquote = !inquote;
                c = (_TUCHAR) *p++;
                continue;
            }
            ++*numchars;
            if (args)
                *args++ = *p;

            c = (_TUCHAR) *p++;
#ifdef _MBCS
            if (_ismbblead(c)) {
                ++*numchars;
                if (args)
                    *args++ = *p;   /* copy 2nd byte too */
                p++;  /* skip over trail byte */
            }
#endif  /* _MBCS */

        } while ( (c != NULCHAR && (inquote || (c !=SPACECHAR && c != TABCHAR))) );

        if ( c == NULCHAR ) {
            p--;
        } else {
            if (args)
                *(args-1) = NULCHAR;
        }

        inquote = 0;

        /* loop on each argument */
        for(;;) {

            if ( *p ) {
                while (*p == SPACECHAR || *p == TABCHAR)
                    ++p;
            }

            if (*p == NULCHAR)
                break;              /* end of args */

            /* scan an argument */
            if (argv)
                *argv++ = args;     /* store ptr to arg */
            ++*numargs;

#ifdef WILDCARD
        /* To handle later wild card expansion, we prefix each entry by
        it's first character before quote handling.  This is done
        so _[w]cwild() knows whether to expand an entry or not. */
        if (args)
            *args++ = *p;
        ++*numchars;

#endif  /* WILDCARD */

        /* loop through scanning one argument */
        for (;;) {
            copychar = 1;
            /* Rules: 2N backslashes + " ==> N backslashes and begin/end quote
               2N+1 backslashes + " ==> N backslashes + literal "
               N backslashes ==> N backslashes */
            numslash = 0;
            while (*p == SLASHCHAR) {
                /* count number of backslashes for use below */
                ++p;
                ++numslash;
            }
            if (*p == DQUOTECHAR) {
                /* if 2N backslashes before, start/end quote, otherwise
                    copy literally */
                if (numslash % 2 == 0) {
                    if (inquote && p[1] == DQUOTECHAR) {
                        p++;    /* Double quote inside quoted string */
                    } else {    /* skip first quote char and copy second */
                        copychar = 0;       /* don't copy quote */
                        inquote = !inquote;
                    }
                }
                numslash /= 2;          /* divide numslash by two */
            }

            /* copy slashes */
            while (numslash--) {
                if (args)
                    *args++ = SLASHCHAR;
                ++*numchars;
            }

            /* if at end of arg, break loop */
            if (*p == NULCHAR || (!inquote && (*p == SPACECHAR || *p == TABCHAR)))
                break;

            /* copy character into argument */
#ifdef _MBCS
            if (copychar) {
                if (args) {
                    if (_ismbblead(*p)) {
                        *args++ = *p++;
                        ++*numchars;
                    }
                    *args++ = *p;
                } else {
                    if (_ismbblead(*p)) {
                        ++p;
                        ++*numchars;
                    }
                }
                ++*numchars;
            }
            ++p;
#else  /* _MBCS */
            if (copychar) {
                if (args)
                    *args++ = *p;
                ++*numchars;
            }
            ++p;
#endif  /* _MBCS */
            }

            /* null-terminate the argument */

            if (args)
                *args++ = NULCHAR;          /* terminate string */
            ++*numchars;
        }

        /* We put one last argument in -- a null ptr */
        if (argv)
            *argv++ = NULL;
        ++*numargs;
}

