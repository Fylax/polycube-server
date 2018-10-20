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

#include <pistache/router.h>

#include <list>
#include <memory>
#include <string>
#include <vector>

#include "../../Server/Error.h"
#include "Resource.h"

namespace polycube::polycubed::Rest::Resources::Body {
class ParentResource : public Resource {
 public:
  ParentResource(std::string name, std::string module,
                 std::shared_ptr<ParentResource> parent,
                 bool container_presence = false);

  ~ParentResource() override = default;

  std::vector<Response> BodyValidate(nlohmann::json &body,
                                     bool check_mandatory) const override;

  virtual void AddChild(std::shared_ptr<Resource> child);

  bool IsMandatory() const override;

  bool IsConfiguration() const final;

  void SetDefaultIfMissing(nlohmann::json &body) const override;

  bool ValidateXPath(const std::string &xpath) const override;

 protected:
  std::vector<std::shared_ptr<Resource>> children_;

  bool ValidateXPathChildren(const std::string &xpath,
                             std::size_t delimiter) const;

 private:
  /**
   * MUST be set to true only if the parent is a container
   * and it has no presence flag (or explicitly set to false).
   */
  const bool container_presence_;
};
}  // namespace polycube::polycubed::Rest::Resources::Body
