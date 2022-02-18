#ifndef CUSTOMEXCEPTION_HPP
#define CUSTOMEXCEPTION_HPP

#include <exception>
#include <string>

class custom_exception : public std::exception {
  const char * error_msg;

  public:
  custom_exception (const std::string msg) : error_msg(msg.c_str()) {}
  custom_exception (const char * msg) : error_msg(msg) {}

  virtual const char * what() const throw();
  
};

#endif
