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

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "../Body/AbstractFactory.h"

namespace polycube::polycubed::Rest::Resources::Endpoint {

class CaseResource;
class ChoiceResource;
class LeafResource;
class LeafListResource;
class ListResource;
class ParentResource;
class Service;

class AbstractFactory : public Body::AbstractFactory {
 public:
  using AbstractFactory::Case;
  virtual std::unique_ptr<CaseResource> Case(
      std::string name, std::string module,
      std::shared_ptr<ParentResource> parent) = 0;

  using AbstractFactory::Choice;
  virtual std::unique_ptr<ChoiceResource> Choice(
      std::string name, std::string module,
      std::shared_ptr<ParentResource> parent, bool mandatory,
      std::unique_ptr<const std::string> &&default_case) = 0;

  using AbstractFactory::Leaf;
  virtual std::unique_ptr<LeafResource> Leaf(
      std::string name, std::string module,
      std::shared_ptr<ParentResource> parent,
      std::unique_ptr<Body::JsonBodyField> &&field, bool configuration,
      bool mandatory, std::unique_ptr<const std::string> &&default_value) = 0;

  using AbstractFactory::LeafList;
  virtual std::unique_ptr<LeafListResource> LeafList(
      std::string name, std::string module,
      std::shared_ptr<ParentResource> parent,
      std::unique_ptr<Body::JsonBodyField> &&field, bool configuration,
      bool mandatory, std::vector<std::string> &&default_value) = 0;

  using AbstractFactory::List;
  virtual std::unique_ptr<ListResource> List(
      std::string name, std::string module,
      std::shared_ptr<ParentResource> parent,
      std::vector<std::pair<
          std::string, std::vector<std::unique_ptr<Validators::Validator>>>>
          &&keys) = 0;

  using AbstractFactory::Generic;
  virtual std::unique_ptr<ParentResource> Generic(
      std::string name, std::string module,
      std::shared_ptr<ParentResource> parent) = 0;

  virtual std::unique_ptr<Service> Service(std::string name) = 0;

 protected:
  explicit AbstractFactory(std::string base_address)
      : base_address_(std::move(base_address));

 private:
  std::string base_address_;
};
}  // namespace polycube::polycubed::Rest::Resources::Endpoint