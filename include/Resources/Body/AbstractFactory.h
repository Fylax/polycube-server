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
#pragma once

#include <libyang/tree_schema.h>

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace polycube::polycubed::Rest::Validators {
class Validator;
}

namespace polycube::polycubed::Rest::Resources::Body {

class CaseResource;
class ChoiceResource;
class JsonBodyField;
class LeafResource;
class LeafListResource;
class ListResource;
class ParentResource;
class Service;

class AbstractFactory {
 public:
  virtual std::unique_ptr<CaseResource> Case(
      std::string name, std::string module,
      std::shared_ptr<ParentResource> parent);

  virtual std::unique_ptr<ChoiceResource> Choice(
      std::string name, std::string module,
      std::shared_ptr<ParentResource> parent, bool mandatory,
      std::unique_ptr<const std::string> &&default_case);

  std::unique_ptr<Body::JsonBodyField> JsonBodyField();

  std::unique_ptr<Body::JsonBodyField> JsonBodyField(
      LY_DATA_TYPE type,
      std::vector<std::shared_ptr<Validators::Validator>> &&validators);

  virtual std::unique_ptr<LeafResource> Leaf(
      std::string name, std::string module,
      std::shared_ptr<ParentResource> parent,
      std::unique_ptr<Body::JsonBodyField> &&field, bool configuration,
      bool mandatory, std::unique_ptr<const std::string> &&default_value) = 0;

  virtual std::unique_ptr<LeafListResource> LeafList(
      std::string name, std::string module,
      std::shared_ptr<ParentResource> parent,
      std::unique_ptr<Body::JsonBodyField> &&field, bool configuration,
      bool mandatory, std::vector<std::string> &&default_value) = 0;

  virtual std::unique_ptr<ListResource> List(
      std::string name, std::string module,
      std::shared_ptr<ParentResource> parent,
      std::vector<std::pair<
          std::string, std::vector<std::unique_ptr<Validators::Validator>>>>
          &&keys) = 0;

  virtual std::unique_ptr<ParentResource> Generic(
      std::string name, std::string module,
      std::shared_ptr<ParentResource> parent) = 0;

 protected:
  AbstractFactory() = default;
  virtual ~AbstractFactory() = default;
};
}  // namespace polycube::polycubed::Rest::Resources::Body
