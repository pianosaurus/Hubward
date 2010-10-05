#ifndef H_INTSTRING
#define H_INTSTRING

#include <string>

/* Some handy string functions. */

/* Convert a (signed) string to an integer. */
int stringtoint(const std::string& s);

/* Fetch the Minecraft save directory with a trailing "/". */
std::string savepath();

#endif
