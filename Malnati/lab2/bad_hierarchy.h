#ifndef __BAD_HIERARCHY_H
#define __BAD_HIERARCHY_H

#include <stdexcept>

class bad_hierarchy : public std::logic_error {
public:

	bad_hierarchy() : std::logic_error("Bad text hierarchy") { }

};

#endif