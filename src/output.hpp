#ifndef H_OUTPUT
#define H_OUTPUT

#include <iostream>

/* Output debug data to cerr if enabled. */
extern std::ostream& debug;
void set_debug(bool enable);

/* Output verbose data to cerr if enabled. */
extern std::ostream& verbose;
void set_verbose(bool enable);

#endif
