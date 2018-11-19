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
#include "../../../include/Resources/Body/ParentResource.h"

#include <algorithm>
#include <list>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "../../../include/Resources/Body/ChoiceResource.h"

namespace polycube::polycubed::Rest::Resources::Body {

ParentResource::ParentResource(std::string name, std::string module,
                               const ParentResource * const parent,
                               bool configuration, bool container_presence)
    : Resource(std::move(name), std::move(module), parent, configuration),
      children_(),
      container_presence_(container_presence) {}

std::vector<Response> ParentResource::BodyValidate(nlohmann::json &body,
                                                   bool check_mandatory) const {
  std::vector<Response> errors;
  std::vector<std::shared_ptr<Resource>> choices;
  for (auto &child : children_) {
    // Choices are a special case
    if (std::dynamic_pointer_cast<ChoiceResource>(child) != nullptr) {
      choices.push_back(child);
    } else {
      if (body.count(child->Name()) == 0) {
        if (check_mandatory && child->IsMandatory()) {
          errors.push_back({ErrorTag::kMissingAttribute, child->Name().data()});
        }
      } else {
        // non configuration nodes are read only
        if (!child->IsConfiguration()) {
          errors.push_back({ErrorTag::kInvalidValue, ""});
        } else {
          auto child_errors =
              child->BodyValidate(body.at(child->Name()), check_mandatory);
          errors.reserve(errors.size() + child_errors.size());
          // remove current parsed element from the body so that in the
          // eventuality of choices, they will operate only on unparsed elements.
          // moreover, this is required for detecting unparsed elements,
          // that may be typos
          body.erase(child->Name());
          std::copy(std::begin(child_errors), std::end(child_errors),
                    std::back_inserter(errors));
        }
      }
    }
  }

  for (const auto &choice : choices) {
    // notice: body_copy will be stripped of elements correctly parsed
    // by the current choice (if any)
    auto choice_errors = choice->BodyValidate(body, check_mandatory);
    // the method will return a single UnparsableChoice if it was not
    // mandatory and it could not match any case.
    if (choice_errors[0].error_tag != ErrorTag::kUnparsableChoice) {
      errors.reserve(errors.size() + choice_errors.size());
      std::copy(std::begin(choice_errors), std::end(choice_errors),
                std::back_inserter(errors));
    }
  }

  errors.reserve(errors.size() + body.size());
  for (auto &unparsed : body.items()) {
    errors.push_back({ErrorTag::kInvalidValue, unparsed.key().data()});
  }

  return errors;
}

void ParentResource::AddChild(const std::shared_ptr<Resource> &&child) {
  children_.push_back(std::move(child));
}

bool ParentResource::IsMandatory() const {
  if (container_presence_)
    return true;
  return std::end(children_) !=
         std::find_if(std::begin(children_), std::end(children_),
                      [](const std::shared_ptr<Resource> &child) {
                        return child->IsMandatory();
                      });
}

bool ParentResource::IsConfiguration() const {
  return configuration_ ||
         std::end(children_) !=
             std::find_if(std::begin(children_), std::end(children_),
                          [](const std::shared_ptr<Resource> &child) {
                            return child->IsConfiguration();
                          });
}

void ParentResource::SetDefaultIfMissing(nlohmann::json &body) const {
  for (const auto &child : children_) {
    child->SetDefaultIfMissing(body[child->Name()]);
  }
}

bool ParentResource::ValidateXPath(const std::string &xpath) const {
  auto del_pos = xpath.find('/');  // current delimiter
  auto ns_pos = xpath.find(':');   // current namespace delimiter
  std::string name;
  if (ns_pos < del_pos) {  // fully-qualified name
    name = xpath.substr(ns_pos + 1, del_pos);
  } else {
    name = xpath.substr(0, del_pos);
  }
  if (name != name_)
    return false;

  return ValidateXPathChildren(xpath, del_pos);
}

std::shared_ptr<Resource> ParentResource::Child(
    const std::string& child_name) const {
  auto child = std::find_if(std::begin(children_), std::end(children_),
      [=](std::shared_ptr<Resource> r) { return r->Name() == child_name; });
  return (child == std::end(children_)) ? nullptr : *child;
}

bool ParentResource::ValidateXPathChildren(const std::string &xpath,
                                           std::size_t delimiter) const {
  auto del_pos = xpath.find('/', delimiter + 1);  // next token delimiter
  auto ns_pos = xpath.find(':', delimiter + 1);   // next namespace delimiter
  std::string ns;
  if (ns_pos < del_pos) {  // fully-qualified name
    ns = xpath.substr(del_pos + 1, ns_pos);
    // RFC 7951#6.11 states that fully-qualified names are mandatory
    // if the node is in a different module, forbidden otherwise.
    if (ns == module_)
      return false;
  } else {
    // sets namespace to current one to validate XPath only
    // among the children into the same namespace.
    ns = module_;
  }

  // child resources expect XPath without leading '/' character
  // and that namespace is already validated
  auto child_xpath = xpath.substr(del_pos + 1);
  for (const auto &child : children_) {
    if (child->ModuleName() == ns) {
      if (child->ValidateXPath(child_xpath))
        return true;
    }
  }
  return false;
}
}  // namespace polycube::polycubed::Rest::Resources::Body
