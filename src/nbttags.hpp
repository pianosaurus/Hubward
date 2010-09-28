#ifndef H_NBTTAGS
#define H_NBTTAGS

#include <ostream>
#include <string>
#include <list>
#include <zlib.h>

using std::string;
using std::list;

namespace NBT {

  /*
   * NBT (Named Binary Tag) tag types
   */
  class TAG {
  private:
    /* Tags cannot be copied. At least for now. May be added if
       necessary, but make sure to handle all subclasses. */
    TAG(const TAG&);
    TAG& operator=(const TAG&);

  public:
    TAG() {};
    string name;

    /* Create a new tag by type id. */
    static TAG* newtag(unsigned char type);

    /* A string for the tag type. */
    virtual string tagtype() = 0;

    /* Read tag contents from file. */
    virtual void get(gzFile file, bool named = false) = 0;

    /* Make destructor virtual. */
    virtual ~TAG() {};
  };

  /* Type 0: Used to mark end of lists. */
  class TAG_End : public TAG {
  public:
    string tagtype() { return "TAG_End"; };
    void get(gzFile file, bool named = false) {};
  };

  /* Type 1: A single signed byte. */
  class TAG_Byte : public TAG {
  public:
    unsigned char payload;
    string tagtype() { return "TAG_Byte"; };
    void get(gzFile file, bool named = false);
  };

  /* Type 2: A signed short 16 bit. */
  class TAG_Short : public TAG {
  public:
    int payload;
    string tagtype() { return "TAG_Short"; };
    void get(gzFile file, bool named = false);
  };

  /* Type 3: A signed short, 32 bit. */
  class TAG_Int : public TAG {
  public:
    long payload;
    string tagtype() { return "TAG_Int"; };
    void get(gzFile file, bool named = false);
  };

  /* Type 4: A signed long, 64 bit. */
  class TAG_Long : public TAG {
  public:
    long long payload;
    string tagtype() { return "TAG_Long"; };
    void get(gzFile file, bool named = false);
  };

  /* Type 5: A floating point value, 32 bit. */
  class TAG_Float : public TAG {
  public:
    float payload;
    string tagtype() { return "TAG_Float"; };
    void get(gzFile file, bool named = false);
  };

  /* Type 6: A floating point value, 64 bit. */
  class TAG_Double : public TAG {
  public:
    double payload;
    string tagtype() { return "TAG_Double"; };
    void get(gzFile file, bool named = false);
  };

  /* Type 7: An array of unformatted bytes. */
  class TAG_Byte_Array : public TAG {
  public:
    int length;
    unsigned char* payload;
    string tagtype() { return "TAG_Byte_Array"; };
    void get(gzFile file, bool named = false);

    TAG_Byte_Array();
    ~TAG_Byte_Array();
  };

  /* Type 8: An array of bytes, UTF8. */
  class TAG_String : public TAG {
  public:
    string payload;
    string tagtype() { return "TAG_String"; };
    void get(gzFile file, bool named = false);
  };

  /* Type 9: A sequential list of single-type tags. */
  class TAG_List : public TAG {
  public:
    int length;
    TAG** payload;
    string tagtype() { return "TAG_List"; };
    void get(gzFile file, bool named = false);

    TAG_List();
    ~TAG_List();
  };

  /* Type 10: A sequential list of named tags. */
  class TAG_Compound : public TAG {
  public:
    list<TAG*> payload;
    string tagtype() { return "TAG_Compound"; };
    void get(gzFile file, bool named = false);

    /* Print named structure recursively. */
    void print_structure(std::ostream& out, int indent = 0);

    /* Return a pointer to a child tag (eg. Level.Blocks). */
    const TAG* fetch(const string path);

    ~TAG_Compound();
  };

};

#endif
