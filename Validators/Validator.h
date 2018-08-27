#ifndef PARSER_VALIDATOR_H
#define PARSER_VALIDATOR_H

#include <memory>
#include <string>
#include <vector>

struct Validator {
  virtual bool validate(const std::string& value) const = 0;
};


#endif //PARSER_VALIDATOR_H
