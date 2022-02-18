#include "custom_exception.hpp"

  const char *  custom_exception::what() const throw() {
    return error_msg;
  }
