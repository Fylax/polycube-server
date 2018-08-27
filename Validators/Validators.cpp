#include "Validators.h"

#include <memory>
#include <string>
#include <vector>

void Validators::addValidators(LY_DATA_TYPE type, const std::string& name,
                               const std::vector<
                                   std::shared_ptr<Validator>
                               >& validators) {
  validators_[type].emplace(name,
                            std::make_shared<
                                const std::vector<
                                    std::shared_ptr<Validator>
                                >
                            >(validators));
}

std::shared_ptr<
    const std::vector<std::shared_ptr<Validator>>
> Validators::getValidators(LY_DATA_TYPE type, const std::string& name) const {
  bool exists = validators_[type].count(name) == 1;
  if (exists) {
    return validators_[type].at(name);
  }
  return std::shared_ptr<
      const std::vector<std::shared_ptr<Validator>>
  >();
}
