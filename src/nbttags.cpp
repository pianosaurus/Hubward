#include "nbttags.hpp"

#include <stdexcept>

using std::string;
using std::list;

using namespace NBT;

/* Create a new tag by type id. */
TAG* TAG::newtag(unsigned char type) {
  switch (type) {
  case 0:
    return new TAG_End();

  case 1:
    return new TAG_Byte();

  case 2:
    return new TAG_Short();

  case 3:
    return new TAG_Int();

  case 4:
    return new TAG_Long();

  case 5:
    return new TAG_Float();

  case 6:
    return new TAG_Double();

  case 7:
    return new TAG_Byte_Array();

  case 8:
    return new TAG_String();

  case 9:
    return new TAG_List();

  case 10:
    return new TAG_Compound();

  default:
    throw std::runtime_error("Unknown tag type.");
  }
}

/* Type 1: A single signed byte. */
void TAG_Byte::get(gzFile file, bool named) {
  if (named) {
    TAG_String n;
    n.get(file);
    name = n.payload;
  }

  if ((payload = gzgetc(file)) < 0) {
    int err;
    throw std::runtime_error(gzerror(file, &err));
  }
}

/* Type 2: A signed short 16 bit. */
void TAG_Short::get(gzFile file, bool named) {
  if (named) {
    TAG_String n;
    n.get(file);
    name = n.payload;
  }

  int big, small;
  if ((big = gzgetc(file) < 0) ||
      (small = gzgetc(file)) < 0) {
    int err;
    throw std::runtime_error(gzerror(file, &err));
  }

  payload = (big << 8) + small;
}

/* Type 3: A signed short, 32 bit. */
void TAG_Int::get(gzFile file, bool named) {
  if (named) {
    TAG_String n;
    n.get(file);
    name = n.payload;
  }

  int data[4];
  for (int i = 0; i < 4; i++) {
    if ((data[i]= gzgetc(file)) < 0) {
      int err;
      throw std::runtime_error(gzerror(file, &err));
    }
  }

  payload = (data[0] << 24) + (data[1] << 16) + (data[2] << 8) + data[3];
}

/* Type 4: A signed long, 64 bit. */
void TAG_Long::get(gzFile file, bool named) {
  if (named) {
    TAG_String n;
    n.get(file);
    name = n.payload;
  }


  int data[8];
  for (int i = 0; i < 8; i++) {
    if ((data[i]= gzgetc(file)) < 0) {
      int err;
      throw std::runtime_error(gzerror(file, &err));
    }
  }


  payload = ((long)data[0] << 56) + ((long)data[1] << 48)
    + ((long)data[2] << 40) + ((long)data[3] << 32)
    + (data[4] << 24) + (data[5] << 16)
    + (data[6] << 8) + data[7];
}

/* Type 5: A floating point value, 32 bit. */
void TAG_Float::get(gzFile file, bool named) {
  if (named) {
    TAG_String n;
    n.get(file);
    name = n.payload;
  }

  int data[4];
  for (int i = 0; i < 4; i++) {
    if ((data[i]= gzgetc(file)) < 0) {
      int err;
      throw std::runtime_error(gzerror(file, &err));
    }
  }

  payload = 0; // TODO: Convert data[3..0] to float.
}

/* Type 6: A floating point value, 64 bit. */
void TAG_Double::get(gzFile file, bool named) {
  if (named) {
    TAG_String n;
    n.get(file);
    name = n.payload;
  }

  int data[8];
  for (int i = 0; i < 8; i++) {
    if ((data[i]= gzgetc(file)) < 0) {
      int err;
      throw std::runtime_error(gzerror(file, &err));
    }
  }

  payload = 0; // TODO: Convert data[7..0] to double.
}

/* Type 7: An array of unformatted bytes. */
void TAG_Byte_Array::get(gzFile file, bool named) {
  if (named) {
    TAG_String n;
    n.get(file);
    name = n.payload;
  }

  /* Delete old payload. */
  if (payload) {
    delete [] payload;
    payload = 0;
  }

  /* Get size of payload. */
  TAG_Int len; len.get(file);
  length = len.payload;

  /* Load payload. */
  if (length > 0) {
    payload = new unsigned char[length];
    for (int i = 0; i < length; i++) {
      TAG_Byte data;
      data.get(file);
      payload[i] = data.payload;
    }
  }
}
TAG_Byte_Array::TAG_Byte_Array() : length(0), payload(0) {}
TAG_Byte_Array::~TAG_Byte_Array() {
  if (payload)
    delete [] payload;
}

/* Type 8: An array of bytes, UTF8. */
void TAG_String::get(gzFile file, bool named) {
  if (named) {
    TAG_String n;
    n.get(file);
    name = n.payload;
  }

  /* Delete old payload and get length of new one. */
  payload = "";
  TAG_Short length;
  length.get(file);

  /* Load payload into string. */
  if (length.payload > 0) {
    char* data = new char[length.payload + 1];

    if (gzgets(file, data, length.payload + 1) == Z_NULL) {
      int err;
      throw std::runtime_error(gzerror(file, &err));
    }

    payload = data;
    delete [] data;
  }
}

/* Type 9: A sequential list of single-type tags. */
void TAG_List::get(gzFile file, bool named) {
  if (named) {
    TAG_String n;
    n.get(file);
    name = n.payload;
  }

  /* Delete old payload. */
  if (payload) {
    for (int i = 0; i < length; i++) {
      delete payload[i];
    }
    delete [] payload;
    payload = 0;
    length = 0;
  }

  /* Get new header. */
  TAG_Byte tagid;  tagid.get(file);
  TAG_Int  len; len.get(file);
  length = len.payload;

  /* Load new payload. */
  if (length > 0) {
    payload = new TAG*[length];
    for (int i = 0; i < length; i++) {
      payload[i] = TAG::newtag(tagid.payload);
      payload[i]->get(file);
    }
  }
}
TAG_List::TAG_List() : length(0), payload(0) {}
TAG_List::~TAG_List() {
  if (payload) {
    for (int i = 0; i < length; i++) {
      delete payload[i];
    }
    delete [] payload;
  }
}

/* Type 10: A sequential list of named tags. */
void TAG_Compound::get(gzFile file, bool named) {
  if (named) {
    TAG_String n;
    n.get(file);
    name = n.payload;
  }

  int type;
  while ((type = gzgetc(file)) != 0) {
    if (type == -1) {
      int err;
      throw std::runtime_error(gzerror(file, &err));
    }
    TAG* sub = newtag(type);
    sub->get(file, true);
    payload.push_back(sub);
  }
}
TAG_Compound::~TAG_Compound() {
  while (!payload.empty()) {
    delete payload.back();
    payload.pop_back();
  }
}

/* Print structure of named tags recursively. */
void TAG_Compound::print_structure(std::ostream& out, int indent) {
  for (list<TAG*>::const_iterator it = payload.begin();
       it != payload.end(); ++it) {
    for (int i = 0; i < indent; i++)
      out << " ";
    out << (*it)->tagtype() << " {" << (*it)->name << "}\n";

    /* If this child is a TAG_Compound, recurse. */
    TAG_Compound* child = dynamic_cast<TAG_Compound*>(*it);
    if (child) {
      child->print_structure(out, indent + 4);
    }
  }
}

/* Return a pointer to a child tag (eg. Level.Blocks). */
const TAG* TAG_Compound::fetch(const string name) {
  size_t dotpos = name.find_first_of('.');
  string find = name.substr(0, dotpos);

  for (list<TAG*>::const_iterator it = payload.begin();
       it != payload.end(); ++it) {
    if ((*it)->name == find) {
      if (dotpos == string::npos) {
        return (*it);
      } else {
        TAG_Compound* sub = dynamic_cast<TAG_Compound*>(*it);
        if (sub) {
          return sub->fetch(name.substr(dotpos + 1));
        } else {
          throw std::runtime_error(find + " has no named children.");
        }
      }
    }
  }
  throw std::runtime_error(string("Couldn't find entity ") + name + ".");
}
