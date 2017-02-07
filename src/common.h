// defines common definitions and macros
#ifndef _COMMON_H_
#define _COMMON_H_

#define DEBUGGING           1

// general definitions
#define NO_ERROR            0
#define ERR_HOST_NOT_FOUND  (-1)
#define ERR_SENT_TO_HOST    (-2)

typedef unsigned int BOOL;
#define FALSE   0
#define TRUE    1

#if DEBUGGING
// not using printf(_fmt, __VA_ARGS__) due to problem when no arguments after format
#define DEBUG_LOG(_arg) printf _arg
#define ERROR_LOG DEBUG_LOG
#else
#define DEBUG_LOG(_arg)
#define ERROR_LOG(_arg) printf _arg
#endif

// colors
#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

// helper macros
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

#endif // _COMMON_H_
