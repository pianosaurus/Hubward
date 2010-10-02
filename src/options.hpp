#ifndef H_OPTIONS
#define H_OPTIONS

#include <list>
#include <string>

/* Long options and their arguments. */
typedef std::pair<std::string, std::string> longopt;

/*
 * Output usage help.
 */
void usage(const char* binary);

/*
 * Fill the renderstrs and options lists with strings pased on
 * parsing the incoming arguments (except the first).
 */
void parse_options(int argc, char** argv,
                   std::list<std::string>& renderstrs,
                   std::list<longopt>& options);

#endif
