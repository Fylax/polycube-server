#ifndef PARSER_VALIDATORS_H
#define PARSER_VALIDATORS_H

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "Validator.h"

class Validators {
 public:
  Validators();
  Validators(const std::string& name,
             const std::vector<
                 std::shared_ptr<Validator>
             >& validators);
  void addValidators(const std::shared_ptr<Validators>& validators);

  std::shared_ptr<
      const std::vector<std::shared_ptr<Validator>>
  > getValidators(const std::string& name) const;

 private:
  std::unordered_map<std::string,
      std::shared_ptr<const std::vector<std::shared_ptr<Validator>>>
  > validators_;
};


#endif //PARSER_VALIDATORS_H
