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

#include <libyang/libyang.h>

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
class ListKey;
class ParentResource;
class Service;

class AbstractFactory {
 public:
  std::unique_ptr<Body::JsonBodyField> JsonBodyField() const;

  std::unique_ptr<Body::JsonBodyField> JsonBodyField(
      LY_DATA_TYPE type,
      std::vector<std::shared_ptr<Validators::Validator>> &&validators) const;

  std::unique_ptr<CaseResource> BodyCase(
      std::string name, std::string module,
      std::shared_ptr<ParentResource> parent) const;

  std::unique_ptr<ChoiceResource> BodyChoice(
      std::string name, std::string module,
      std::shared_ptr<ParentResource> parent, bool mandatory,
      std::unique_ptr<const std::string> &&default_case) const;

  std::unique_ptr<LeafResource> BodyLeaf(
      std::string name, std::string module,
      std::shared_ptr<ParentResource> parent,
      std::unique_ptr<Body::JsonBodyField> &&field, bool configuration,
      bool mandatory, std::unique_ptr<const std::string> &&default_value) const;

  std::unique_ptr<LeafListResource> BodyLeafList(
      std::string name, std::string module,
      std::shared_ptr<ParentResource> parent,
      std::unique_ptr<Body::JsonBodyField> &&field, bool configuration,
      bool mandatory, std::vector<std::string> &&default_value) const;

  std::unique_ptr<ListResource> BodyList(
      std::string name, std::string module,
      std::shared_ptr<ParentResource> parent,
      std::vector<Resources::Body::ListKey> &&keys) const;

  std::unique_ptr<ParentResource> BodyGeneric(
      std::string name, std::string module,
      std::shared_ptr<ParentResource> parent, bool container_presence) const;

 protected:
  AbstractFactory() = default;
  virtual ~AbstractFactory() = default;
};
}  // namespace polycube::polycubed::Rest::Resources::Body
