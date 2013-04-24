/* stdbool.h for GNU.  */

#ifndef __STDBOOL_H__
#define __STDBOOL_H__	1

#ifndef __cplusplus

/* The type `bool' must promote to `int' or `unsigned int'.  The constants
   `true' and `false' must have the value 0 and 1 respectively.  */
typedef enum
  {
#if !TARGET_OS_WIN32
    false = 0,
    true = 1
#endif
  } bool;

/* The names `true' and `false' must also be made available as macros.  */
#define false	false
#define true	true

#endif

/* Signal that all the definitions are present.  */
#define __bool_true_false_are_defined	1

#endif	/* stdbool.h */
