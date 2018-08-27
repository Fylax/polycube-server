#ifndef PARSER_VALIDATORS_H
#define PARSER_VALIDATORS_H

#include <libyang/libyang.h>

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "Validator.h"

class Validators {
 public:
  void addValidators(LY_DATA_TYPE type, const std::string& name,
                     const std::vector<
                         std::shared_ptr<Validator>
                     >& validators);

  std::shared_ptr<
      const std::vector<std::shared_ptr<Validator>>
  > getValidators(LY_DATA_TYPE type, const std::string& name) const;

 private:
  std::unordered_map<std::string,
      std::shared_ptr<const std::vector<std::shared_ptr<Validator>>>
  > validators_[LY_DATA_TYPE_COUNT];
};


#endif //PARSER_VALIDATORS_H
