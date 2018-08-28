#include <libyang/libyang.h>

#include <cstring>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "include/Validators/Validators.h"
#include "include/Validators/Validator.h"
#include "include/Validators/EnumValidator.h"
#include "include/Validators/PatternValidator.h"

Validators perTypeValidators[LY_DATA_TYPE_COUNT];


std::shared_ptr<const std::vector<std::shared_ptr<Validator>>>
getValidators(lys_type type);

void parseModule(const lys_module* module);

std::shared_ptr<Validators> parseType(const char* name, lys_type type);

std::shared_ptr<Validators>
parseEnum(const char* name, lys_type_info_enums enums);

std::shared_ptr<Validators>
parseString(const char* name, lys_type_info_str str);

void parseNode(lys_node* data);

void parseGrouping(lys_node_grp* group);

void parseLeaf(lys_node_leaf* leaf);

int main() {
  auto context = ly_ctx_new("/home/nico/dev/iovnet/services/resources/", 0);
  auto added = ly_ctx_set_searchdir(context,
                                    "/home/nico/dev/iovnet/services/iov-helloworld/resources");
  if (!context || added == EXIT_FAILURE) {
    throw std::runtime_error("cannot create new context");
  }

  auto module = ly_ctx_get_module(context, "helloworld",
                                  nullptr, true);
  if (!module) {
    module = ly_ctx_load_module(context, "helloworld",
                                nullptr);
    if (!module) {
      throw std::runtime_error("cannot open yang file");
    }
  }

  for (auto i = 0; i < module->imp_size; ++i) {
    parseModule(module->imp[i].module);
  }
  parseModule(module);

  return 0;
}

std::shared_ptr<const std::vector<std::shared_ptr<Validator>>>
getValidators(lys_type type) {
  bool isDerived = (type.der->type.der != nullptr);
  if (!isDerived) {
    std::shared_ptr<Validators> inplace = parseType("", type);
    return inplace->getValidators("");
  }
  return perTypeValidators[type.base].getValidators(type.der->name);
}

void parseModule(const lys_module* module) {
  auto typedefs = module->tpdf;
  for (auto i = 0; i < module->tpdf_size; ++i) {
    auto current_typedef = typedefs[i];
    perTypeValidators->addValidators(
        parseType(current_typedef.name, current_typedef.type)
    );
  }
  auto data = module->data;
  while (data) {
    parseNode(data);
    data = data->next;
  }
}

std::shared_ptr<Validators> parseType(const char* name, lys_type type) {
  switch (type.base) {
    case LY_TYPE_DER:
    case LY_TYPE_BINARY:
    case LY_TYPE_BITS:
    case LY_TYPE_BOOL:
    case LY_TYPE_DEC64:
    case LY_TYPE_EMPTY:
    case LY_TYPE_ENUM:
      return parseEnum(name, type.info.enums);
    case LY_TYPE_IDENT:
    case LY_TYPE_INST:
    case LY_TYPE_LEAFREF:
    case LY_TYPE_STRING:
      return parseString(name, type.info.str);
    case LY_TYPE_UNION:
    case LY_TYPE_INT8:
    case LY_TYPE_UINT8:
    case LY_TYPE_INT16:
    case LY_TYPE_UINT16:
    case LY_TYPE_INT32:
    case LY_TYPE_UINT32:
    case LY_TYPE_INT64:
    case LY_TYPE_UINT64:
      break;
  }
}

std::shared_ptr<Validators>
parseEnum(const char* name, lys_type_info_enums enums) {
  std::unordered_set<std::string> allowed;
  for (auto i = 0; i < enums.count; ++i) {
    allowed.insert(enums.enm[i].name);
  }
  std::vector<std::shared_ptr<Validator>> validators = {
      std::make_shared<EnumValidator>(allowed)
  };
  return std::make_shared<Validators>(name, validators);
}

std::shared_ptr<Validators>
parseString(const char* name, lys_type_info_str str) {
  std::vector<std::shared_ptr<Validator>> validators;
  for (auto i = 0; i < str.pat_count; ++i) {
    auto current_pattern = str.patterns[i].expr;
    // check match byte
    if (current_pattern[0] == 0x06) {
      // skip first byte
      current_pattern = current_pattern + 1;
    } else {
      throw std::runtime_error("unsupported NACK patterns");
    }
    validators.push_back(std::static_pointer_cast<Validator>(
        std::make_shared<PatternValidator>(current_pattern))
    );
  }
  return std::make_shared<Validators>(name, validators);
}

void parseNode(lys_node* data) {
  if (!data) return;
  switch (data->nodetype) {
    case LYS_UNKNOWN:
      break;
    case LYS_CONTAINER:
      break;
    case LYS_CHOICE:
      break;
    case LYS_LEAF:
      parseLeaf(reinterpret_cast<lys_node_leaf*>(data));
      break;
    case LYS_LEAFLIST:
      break;
    case LYS_LIST:
      break;
    case LYS_ANYXML:
      break;
    case LYS_CASE:
      break;
    case LYS_NOTIF:
      break;
    case LYS_RPC:
      break;
    case LYS_INPUT:
      break;
    case LYS_OUTPUT:
      break;
    case LYS_GROUPING:
      parseGrouping(reinterpret_cast<lys_node_grp*>(data));
      break;
    case LYS_USES:
      break;
    case LYS_AUGMENT:
      break;
    case LYS_ACTION:
      break;
    case LYS_ANYDATA:
      break;
    case LYS_EXT:
      break;
  }
}

void parseGrouping(lys_node_grp* group) {
  auto child = group->child;
  while (child) {
    parseNode(child);
    child = child->next;
  }
}

void parseLeaf(lys_node_leaf* leaf) {
  bool isConfigTrue = (leaf->flags & LYS_CONFIG_SET) &&
                      (leaf->flags & LYS_CONFIG_W);
  auto validators = getValidators(leaf->type);

  int x = 0;
}
