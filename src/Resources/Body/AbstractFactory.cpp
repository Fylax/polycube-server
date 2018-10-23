/*
 * Copyright 2018 The Polycube Authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "../../../include/Resources/Body/AbstractFactory.h"

#include <Body/AbstractFactory.h>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace polycube::polycubed::Rest::Resources::Body {

std::unique_ptr<CaseResource> AbstractFactory::Case(
    std::string name, std::string module,
    std::shared_ptr<ParentResource> parent) {
  return std::make_unique<CaseResource>(std::move(name), std::move(module),
                                        std::move(parent));
}

std::unique_ptr<ChoiceResource> AbstractFactory::Choice(
    std::string name, std::string module,
    std::shared_ptr<ParentResource> parent, bool mandatory,
    std::unique_ptr<const std::string> &&default_case) {
  return std::make_unique<ChoiceResource>(std::move(name), std::move(module),
                                          std::move(parent), mandatory,
                                          std::move(default_case));
}

std::unique_ptr<Body::JsonBodyField> AbstractFactory::JsonBodyField() {
  return std::make_unique<Body::JsonBodyField>();
}

std::unique_ptr<Body::JsonBodyField> AbstractFactory::JsonBodyField(
    LY_DATA_TYPE type,
    std::vector<std::shared_ptr<Validators::Validator>> &&validators) {
  return std::make_unique<Body::JsonBodyField>(type, std::move(validators));
}

std::unique_ptr<LeafResource> AbstractFactory::Leaf(
    std::string name, std::string module,
    std::shared_ptr<ParentResource> parent,
    std::unique_ptr<Body::JsonBodyField> &&field, bool configuration,
    bool mandatory, std::unique_ptr<const std::string> &&default_value) {
  return std::make_unique<LeafResource>(
      std::move(name), std::move(module), std::move(parent), std::move(field),
      configuration, mandatory, std::move(default_value));
}

std::unique_ptr<LeafListResource> AbstractFactory::LeafList(
    std::string name, std::string module,
    std::shared_ptr<ParentResource> parent,
    std::unique_ptr<Body::JsonBodyField> &&field, bool configuration,
    bool mandatory, std::vector<std::string> &&default_value) {
  return std::make_unique<LeafListResource>(
      std::move(name), std::move(module), std::move(parent), std::move(field),
      configuration, mandatory, std::move(default_value));
}

std::unique_ptr<ListResource> AbstractFactory::List(
    std::string name, std::string module,
    std::shared_ptr<ParentResource> parent,
    std::vector<std::pair<std::string,
                          std::vector<std::unique_ptr<Validators::Validator>>>>
        &&keys) {
  return std::make_unique<ListResource>(std::move(name), std::move(module),
                                        std::move(parent), std::move(keys));
}

std::unique_ptr<ParentResource> AbstractFactory::Generic(
    std::string name, std::string module,
    std::shared_ptr<ParentResource> parent) {
  return std::make_unique<ParentResource>(std::move(name), std::move(module),
                                          std::move(parent));
}
}  // namespace polycube::polycubed::Rest::Resources::Body
