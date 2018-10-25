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

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "../../../include/Resources/Body/CaseResource.h"
#include "../../../include/Resources/Body/ChoiceResource.h"
#include "../../../include/Resources/Body/JsonBodyField.h"
#include "../../../include/Resources/Body/LeafListResource.h"
#include "../../../include/Resources/Body/LeafResource.h"
#include "../../../include/Resources/Body/ListResource.h"
#include "../../../include/Resources/Body/ParentResource.h"

namespace polycube::polycubed::Rest::Resources::Body {
std::unique_ptr<Body::JsonBodyField> AbstractFactory::JsonBodyField() const {
  return std::make_unique<Body::JsonBodyField>();
}

std::unique_ptr<Body::JsonBodyField> AbstractFactory::JsonBodyField(
    LY_DATA_TYPE type,
    std::vector<std::shared_ptr<Validators::Validator>> &&validators) const {
  return std::make_unique<Body::JsonBodyField>(type, std::move(validators));
}
}  // namespace polycube::polycubed::Rest::Resources::Body
