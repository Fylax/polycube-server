#include "../../include/Validators/Validators.h"

#include <memory>
#include <string>
#include <vector>

Validators::Validators(): validators_() {}

Validators::Validators(const std::string& name, const std::vector<
    std::shared_ptr<Validator>>& validators): validators_{
    {name, std::make_shared<const std::vector<
        std::shared_ptr<Validator>
    >>(validators)}} {}

void Validators::addValidators(const std::shared_ptr<Validators>& validators) {
  validators_.insert(validators->validators_.begin(),
      validators->validators_.end());
}

std::shared_ptr<
    const std::vector<std::shared_ptr<Validator>>
> Validators::getValidators(const std::string& name) const {
  bool exists = validators_.count(name) == 1;
  if (exists) {
    return validators_.at(name);
  }
  return std::shared_ptr<
      const std::vector<std::shared_ptr<Validator>>
  >();
}
