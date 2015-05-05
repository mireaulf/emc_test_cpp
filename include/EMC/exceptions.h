#ifndef EMC_EXCEPTIONS_H
#define EMC_EXCEPTIONS_H

#include <exception>
namespace emc {
    namespace exceptions {
        namespace lookup_buffer {
            struct empty_string : public std::exception {
            };
            struct too_long_string : public std::exception {
            };
            struct not_found : public std::exception {
            };
        }
    }
}

#endif
