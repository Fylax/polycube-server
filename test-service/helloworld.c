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
#ifdef __cplusplus
extern "C" {
#endif

enum ErrorTag {
  /** No error.
   * Message will contain the response body. */
  kOk,
  kCreated,
  /** Generic invalid-value error, no message provided */
  kInvalidValue,
  /** Required body field missing.
   * Message will contain the name of the missing field. */
  kMissingAttribute,
  /** Required path param missing.
   * Message will contain the name of the missing path param. */
  kMissingElement,
  /** A body field value is not correct; e.g., wrong type, out of range,
   * pattern mismatch.
   * Message will contain the name of the malformed field. */
  kBadAttribute,
  /** A path param value is not correct; e.g., wrong type, out of range,
   * pattern mismatch.
   * Message will contain the name of the malformed path param. */
  kBadElement,
  /** Request could not be completed because the relevant data model content
   * already exists.
   * For example, a "create" operation was attempted on data
   * that already exists. */
  kDataExists,
  /** Request could not be completed because the relevant data model content
   * does not exist.
   * For example, a "delete" operation was attempted on data
   * that does not exist. */
  kDataMissing,
  /** Wrong HTTP method used. */
  kOperationNotSupported
};

typedef struct Response {
  const enum ErrorTag error_tag;
  const char *message;
} Response;

const char *data_model() {
  return "module helloworld {\n"
         "  yang-version 1.1;\n"
         "  namespace \"http://polycube.network/helloworld\";\n"
         "  prefix \"helloworld\";\n"
         "\n"
         "  import polycube-base { prefix \"basemodel\"; }\n"
         "\n"
         "  organization \"Polycube open source project\";\n"
         "   description \"YANG data model for the Polycube Helloworld service\";\n"
         "  \n"
         "  basemodel:service-description \"Helloworld Service\";\n"
         "  basemodel:service-version \"2.0\";\n"
         "  basemodel:service-name \"helloworld\";\n"
         "  basemodel:service-min-kernel-version \"4.14.0\";\n"
         "\n"
         "  uses \"basemodel:base-yang-module\";\n"
         "\n"
         "  leaf action {\n"
         "    type enumeration {\n"
         "      enum DROP;\n"
         "      enum SLOWPATH;\n"
         "      enum FORWARD;\n"
         "    }\n"
         "    default DROP;\n"
         "    description \"Action performed on the received packet (i.e., DROP, SLOWPATH, or FORWARD; default: DROP)\";\n"
         "  }\n"
         "}";
}

Response create_helloworld_by_id(const char *name, const char *value) {
  Response returns = {kOk, ""};
  return returns;
}

Response create_helloworld_ports_by_id(const char *name, const char *portsName,
                                       const char *value){
  Response returns = {kOk, ""};
  return returns;
}
Response create_helloworld_ports_list_by_id(const char *name,
                                            const char *value){
  Response returns = {kOk, ""};
  return returns;
}
Response delete_helloworld_by_id(const char *name){
  Response returns = {kOk, ""};
  return returns;
}
Response delete_helloworld_ports_by_id(const char *name, const char *portsName){
  Response returns = {kOk, ""};
  return returns;
}
Response delete_helloworld_ports_list_by_id(const char *name){
  Response returns = {kOk, ""};
  return returns;
}
Response read_helloworld_action_by_id(const char *name){
  Response returns = {kOk, ""};
  return returns;
}
Response read_helloworld_by_id(const char *name){
  Response returns = {kOk, ""};
  return returns;
}
Response read_helloworld_list_by_id(){
  Response returns = {kOk, ""};
  return returns;
}
Response read_helloworld_list_by_id_get_list(){
  Response returns = {kOk, ""};
  return returns;
}
Response read_helloworld_loglevel_by_id(const char *name){
  Response returns = {kOk, ""};
  return returns;
}
Response read_helloworld_ports_by_id(const char *name, const char *portsName){
  Response returns = {kOk, ""};
  return returns;
}
Response read_helloworld_ports_list_by_id(const char *name){
  Response returns = {kOk, ""};
  return returns;
}
Response read_helloworld_ports_list_by_id_get_list(const char *name){
  Response returns = {kOk, ""};
  return returns;
}
Response read_helloworld_ports_peer_by_id(const char *name,
                                          const char *portsName){
  Response returns = {kOk, ""};
  return returns;
}
Response read_helloworld_ports_status_by_id(const char *name,
                                            const char *portsName){
  Response returns = {kOk, ""};
  return returns;
}
Response read_helloworld_ports_uuid_by_id(const char *name,
                                          const char *portsName){
  Response returns = {kOk, ""};
  return returns;
}
Response read_helloworld_type_by_id(const char *name){
  Response returns = {kOk, ""};
  return returns;
}
Response read_helloworld_uuid_by_id(const char *name){
  Response returns = {kOk, ""};
  return returns;
}
Response replace_helloworld_by_id(const char *name, const char *value){
  Response returns = {kOk, ""};
  return returns;
}
Response replace_helloworld_ports_by_id(const char *name, const char *portsName,
                                        const char *value){
  Response returns = {kOk, ""};
  return returns;
}
Response replace_helloworld_ports_list_by_id(const char *name,
                                             const char *value){
  Response returns = {kOk, ""};
  return returns;
}
Response update_helloworld_action_by_id(const char *name, const char *value){
  Response returns = {kOk, ""};
  return returns;
}
Response update_helloworld_by_id(const char *name, const char *value){
  Response returns = {kOk, ""};
  return returns;
}
Response update_helloworld_list_by_id(const char *value){
  Response returns = {kOk, ""};
  return returns;
}
Response update_helloworld_loglevel_by_id(const char *name, const char *value){
  Response returns = {kOk, ""};
  return returns;
}
Response update_helloworld_ports_by_id(const char *name, const char *portsName,
                                       const char *value){
  Response returns = {kOk, ""};
  return returns;
}
Response update_helloworld_ports_list_by_id(const char *name, char *value){
  Response returns = {kOk, ""};
  return returns;
}
Response update_helloworld_ports_peer_by_id(const char *name,
                                            const char *portsName,
                                            const char *value){
  Response returns = {kOk, ""};
  return returns;
}

#ifdef __cplusplus
}
#endif