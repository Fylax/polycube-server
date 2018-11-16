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
#include <glob.h>
#include <stdbool.h>
#include <stdint.h>

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

enum ElementType {
  BOOLEAN,
  STRING,
  INT8,
  INT16,
  INT32,
  INT64,
  UINT8,
  UINT16,
  UINT32,
  UINT64,
  DECIMAL
};

union ElementValue {
  bool boolean;
  const char *string;
  int8_t int8;
  int16_t int16;
  int32_t int32;
  int64_t int64;
  uint8_t uint8;
  uint16_t uint16;
  uint32_t uint32;
  uint64_t uint64;
};

typedef struct {
  const char *name;
  enum ElementType type;
  union ElementValue value;
} Key;

const char *data_model() {
  return "module nat {\n"
         "    yang-version 1.1;\n"
         "    namespace \"http://polycube.network/nat\";\n"
         "    prefix \"nat\";\n"
         "\n"
         "    import polycube-base { prefix \"basemodel\"; }\n"
         "    import ietf-inet-types { prefix \"inet\"; }\n"
         "\n"
         "    organization \"Polycube open source project\";\n"
         "    description \"YANG data model for the Polycube NAT service\";\n"
         "\n"
         "    extension cli-example {\n"
         "        argument \"value\";\n"
         "        description \"A sample value used by the CLI generator\";\n"
         "    }\n"
         "\n"
         "    basemodel:service-description \"NAT Service\";\n"
         "    basemodel:service-version \"1.0\";\n"
         "    basemodel:service-name \"nat\";\n"
         "    basemodel:service-min-kernel-version \"4.14.0\";\n"
         "\n"
         "    uses \"basemodel:base-yang-module\" {\n"
         "        augment ports {\n"
         "            leaf type {\n"
         "                type enumeration {\n"
         "                    enum EXTERNAL { description \"External interface "
         "of the NAT\"; }\n"
         "                    enum INTERNAL { description \"Internal interface "
         "of the NAT\"; }\n"
         "                }\n"
         "                mandatory true;\n"
         "                config false;\n"
         "                description \"Type of the NAT interface (e.g., "
         "EXTERNAL or INTERNAL)\";\n"
         "            }\n"
         "\n"
         "            leaf ip {\n"
         "                type inet:ipv4-address;\n"
         "                description \"IP address of the port. If the port is "
         "EXTERNAL this is the external ip address.\";\n"
         "                nat:cli-example \"9.45.21.4\";\n"
         "            }\n"
         "        }\n"
         "    }\n"
         "\n"
         "    grouping snat-rule {\n"
         "        leaf internal-net {\n"
         "            type inet:ipv4-prefix;\n"
         "            mandatory true;\n"
         "            description \"Internal IP address (or subnet)\";\n"
         "            nat:cli-example \"10.0.0.0/24 or 10.0.0.1/32\";\n"
         "        }\n"
         "        leaf external-ip {\n"
         "            type inet:ipv4-address;\n"
         "            mandatory true;\n"
         "            description \"Natted source IP address\";\n"
         "            nat:cli-example \"8.8.8.8\";\n"
         "        }\n"
         "    }\n"
         "\n"
         "    grouping dnat-rule {\n"
         "        leaf external-ip {\n"
         "            type inet:ipv4-address;\n"
         "            mandatory true;\n"
         "            description \"External destination IP address\";\n"
         "            nat:cli-example \"8.8.8.8\";\n"
         "        }\n"
         "        leaf internal-ip {\n"
         "            type inet:ipv4-address;\n"
         "            mandatory true;\n"
         "            description \"Internal destination IP address\";\n"
         "            nat:cli-example \"10.0.0.1\";\n"
         "        }\n"
         "    }\n"
         "\n"
         "    grouping port-forwarding-rule {\n"
         "        leaf external-ip {\n"
         "            type inet:ipv4-address;\n"
         "            mandatory true;\n"
         "            description \"External destination IP address\";\n"
         "            nat:cli-example \"8.8.8.8\";\n"
         "        }\n"
         "        leaf external-port {\n"
         "            type inet:port-number;\n"
         "            mandatory true;\n"
         "            description \"External destination L4 port\";\n"
         "        }\n"
         "        leaf proto {\n"
         "            type string;\n"
         "            description \"L4 protocol (TCP, UDP, ALL)\";\n"
         "        }\n"
         "        leaf internal-ip {\n"
         "            type inet:ipv4-address;\n"
         "            mandatory true;\n"
         "            description \"Internal destination IP address\";\n"
         "            nat:cli-example \"10.0.0.1\";\n"
         "        }\n"
         "        leaf internal-port {\n"
         "            type inet:port-number;\n"
         "            mandatory true;\n"
         "            description \"Internal destination L4 port\";\n"
         "        }\n"
         "    }\n"
         "\n"
         "    container rule {\n"
         "        container snat {\n"
         "            list entry {\n"
         "                key \"id\";\n"
         "                description \"List of Source NAT rules\";\n"
         "                leaf id {\n"
         "                    type uint32;\n"
         "                    description \"Rule identifier\";\n"
         "                }\n"
         "                uses nat:snat-rule;\n"
         "            }\n"
         "            action append {\n"
         "                input {\n"
         "                    uses nat:snat-rule;\n"
         "                }\n"
         "                output {\n"
         "                    leaf id {\n"
         "                        type uint32;\n"
         "                    }\n"
         "                }\n"
         "            }\n"
         "        }\n"
         "\n"
         "        container masquerade {\n"
         "            leaf enabled {\n"
         "                type boolean;\n"
         "            }\n"
         "            action enable {\n"
         "                description \"Enable masquerade as the default "
         "policy\";\n"
         "                output {\n"
         "                    leaf result {\n"
         "                        type boolean;\n"
         "                        description \"True if the operation is "
         "successful\";\n"
         "                    }\n"
         "                }\n"
         "            }\n"
         "            action disable {\n"
         "                description \"Disable masquerade as the default "
         "policy\";\n"
         "                output {\n"
         "                    leaf result {\n"
         "                        type boolean;\n"
         "                        description \"True if the operation is "
         "successful\";\n"
         "                    }\n"
         "                }\n"
         "            }\n"
         "        }\n"
         "\n"
         "        container dnat {\n"
         "            list entry {\n"
         "                key \"id\";\n"
         "                description \"List of Destination NAT rules\";\n"
         "                leaf id {\n"
         "                    type uint32;\n"
         "                    description \"Rule identifier\";\n"
         "                }\n"
         "                uses nat:dnat-rule;\n"
         "            }\n"
         "            action append {\n"
         "                input {\n"
         "                    uses nat:dnat-rule;\n"
         "                }\n"
         "                output {\n"
         "                    leaf id {\n"
         "                        type uint32;\n"
         "                    }\n"
         "                }\n"
         "            }\n"
         "        }\n"
         "\n"
         "        container port-forwarding {\n"
         "            list entry {\n"
         "                key \"id\";\n"
         "                description \"List of port forwarding rules\";\n"
         "                leaf id {\n"
         "                    type uint32;\n"
         "                    description \"Rule identifier\";\n"
         "                }\n"
         "                uses nat:port-forwarding-rule;\n"
         "            }\n"
         "            action append {\n"
         "                input {\n"
         "                    uses nat:port-forwarding-rule;\n"
         "                }\n"
         "                output {\n"
         "                    leaf id {\n"
         "                        type uint32;\n"
         "                    }\n"
         "                }\n"
         "            }\n"
         "        }\n"
         "    }\n"
         "\n"
         "    list natting-table {\n"
         "        key \"internal-src internal-dst internal-sport "
         "internal-dport proto\";\n"
         "\n"
         "        leaf internal-src {\n"
         "            type inet:ipv4-address;\n"
         "            description \"Source IP address\";\n"
         "        }\n"
         "        leaf internal-dst {\n"
         "            type inet:ipv4-address;\n"
         "            description \"Destination IP address\";\n"
         "        }\n"
         "        leaf internal-sport {\n"
         "            type inet:port-number;\n"
         "            description \"Source L4 port number\";\n"
         "        }\n"
         "        leaf internal-dport {\n"
         "            type inet:port-number;\n"
         "            description \"Destination L4 port number\";\n"
         "        }\n"
         "        leaf proto {\n"
         "            type string;\n"
         "            description \"L4 protocol\";\n"
         "        }\n"
         "        leaf originating-rule {\n"
         "            type enumeration {\n"
         "                enum snat;\n"
         "                enum dnat;\n"
         "                enum masquerade;\n"
         "                enum portforwarding;\n"
         "            }\n"
         "            description \"The set of rules that created this "
         "mapping\";\n"
         "        }\n"
         "        leaf external-ip {\n"
         "            type inet:ipv4-address;\n"
         "            description \"Translated IP address\";\n"
         "        }\n"
         "        leaf external-port {\n"
         "            type inet:port-number;\n"
         "            description \"Translated L4 port number\";\n"
         "        }\n"
         "    }\n"
         "}\n";
}

Response create_nat_by_id(const char *name, const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response replace_nat_by_id(const char *name, const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response update_nat_by_id(const char *name, const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response delete_nat_by_id(const char *name) {
  Response returns = {kOk, ""};
  return returns;
}
Response read_nat_by_id(const char *name) {
  Response returns = {kOk, ""};
  return returns;
}
Response read_nat_list_by_id() {
  Response returns = {kOk, ""};
  return returns;
}
Response read_nat_list_by_id_get_list() {
  Response returns = {kOk, ""};
  return returns;
}
Response update_nat_list_by_id(const char *name, const char *value) {
  Response returns = {kOk, ""};
  return returns;
}

Response create_nat_natting_table_by_id(const char *name, Key *keys,
                                        size_t num_keys, const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response create_nat_natting_table_list_by_id(const char *name, Key *keys,
                                             size_t num_keys,
                                             const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response create_nat_ports_by_id(const char *name, Key *keys, size_t num_keys,
                                const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response create_nat_ports_list_by_id(const char *name, Key *keys,
                                     size_t num_keys, const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response create_nat_rule_by_id(const char *name, Key *keys, size_t num_keys,
                               const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response create_nat_rule_dnat_append_by_id(const char *name, Key *keys,
                                           size_t num_keys, const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response create_nat_rule_dnat_by_id(const char *name, Key *keys,
                                    size_t num_keys, const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response create_nat_rule_dnat_entry_by_id(const char *name, Key *keys,
                                          size_t num_keys, const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response create_nat_rule_dnat_entry_list_by_id(const char *name, Key *keys,
                                               size_t num_keys,
                                               const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response create_nat_rule_masquerade_by_id(const char *name, Key *keys,
                                          size_t num_keys, const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response create_nat_rule_masquerade_disable_by_id(const char *name, Key *keys,
                                                  size_t num_keys,
                                                  const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response create_nat_rule_masquerade_enable_by_id(const char *name, Key *keys,
                                                 size_t num_keys,
                                                 const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response create_nat_rule_port_forwarding_append_by_id(const char *name,
                                                      Key *keys,
                                                      size_t num_keys,
                                                      const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response create_nat_rule_port_forwarding_by_id(const char *name, Key *keys,
                                               size_t num_keys,
                                               const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response create_nat_rule_port_forwarding_entry_by_id(const char *name,
                                                     Key *keys, size_t num_keys,
                                                     const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response create_nat_rule_port_forwarding_entry_list_by_id(const char *name,
                                                          Key *keys,
                                                          size_t num_keys,
                                                          const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response create_nat_rule_snat_append_by_id(const char *name, Key *keys,
                                           size_t num_keys, const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response create_nat_rule_snat_by_id(const char *name, Key *keys,
                                    size_t num_keys, const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response create_nat_rule_snat_entry_by_id(const char *name, Key *keys,
                                          size_t num_keys, const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response create_nat_rule_snat_entry_list_by_id(const char *name, Key *keys,
                                               size_t num_keys,
                                               const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response delete_nat_natting_table_by_id(const char *name, Key *keys,
                                        size_t num_keys, const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response delete_nat_natting_table_list_by_id(const char *name, Key *keys,
                                             size_t num_keys,
                                             const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response delete_nat_ports_by_id(const char *name, Key *keys, size_t num_keys,
                                const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response delete_nat_ports_list_by_id(const char *name, Key *keys,
                                     size_t num_keys, const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response delete_nat_rule_by_id(const char *name, Key *keys, size_t num_keys,
                               const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response delete_nat_rule_dnat_by_id(const char *name, Key *keys,
                                    size_t num_keys, const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response delete_nat_rule_dnat_entry_by_id(const char *name, Key *keys,
                                          size_t num_keys, const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response delete_nat_rule_dnat_entry_list_by_id(const char *name, Key *keys,
                                               size_t num_keys,
                                               const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response delete_nat_rule_masquerade_by_id(const char *name, Key *keys,
                                          size_t num_keys, const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response delete_nat_rule_port_forwarding_by_id(const char *name, Key *keys,
                                               size_t num_keys,
                                               const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response delete_nat_rule_port_forwarding_entry_by_id(const char *name,
                                                     Key *keys, size_t num_keys,
                                                     const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response delete_nat_rule_port_forwarding_entry_list_by_id(const char *name,
                                                          Key *keys,
                                                          size_t num_keys,
                                                          const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response delete_nat_rule_snat_by_id(const char *name, Key *keys,
                                    size_t num_keys, const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response delete_nat_rule_snat_entry_by_id(const char *name, Key *keys,
                                          size_t num_keys, const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response delete_nat_rule_snat_entry_list_by_id(const char *name, Key *keys,
                                               size_t num_keys,
                                               const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response read_nat_loglevel_by_id(const char *name, Key *keys, size_t num_keys,
                                 const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response read_nat_natting_table_by_id(const char *name, Key *keys,
                                      size_t num_keys, const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response read_nat_natting_table_external_ip_by_id(const char *name, Key *keys,
                                                  size_t num_keys,
                                                  const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response read_nat_natting_table_external_port_by_id(const char *name, Key *keys,
                                                    size_t num_keys,
                                                    const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response read_nat_natting_table_list_by_id(const char *name, Key *keys,
                                           size_t num_keys, const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response read_nat_natting_table_list_by_id_get_list(const char *name, Key *keys,
                                                    size_t num_keys,
                                                    const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response read_nat_natting_table_originating_rule_by_id(const char *name,
                                                       Key *keys,
                                                       size_t num_keys,
                                                       const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response read_nat_ports_by_id(const char *name, Key *keys, size_t num_keys,
                              const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response read_nat_ports_ip_by_id(const char *name, Key *keys, size_t num_keys,
                                 const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response read_nat_ports_list_by_id(const char *name, Key *keys, size_t num_keys,
                                   const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response read_nat_ports_list_by_id_get_list(const char *name, Key *keys,
                                            size_t num_keys,
                                            const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response read_nat_ports_peer_by_id(const char *name, Key *keys, size_t num_keys,
                                   const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response read_nat_ports_status_by_id(const char *name, Key *keys,
                                     size_t num_keys, const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response read_nat_ports_type_by_id(const char *name, Key *keys, size_t num_keys,
                                   const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response read_nat_ports_uuid_by_id(const char *name, Key *keys, size_t num_keys,
                                   const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response read_nat_rule_by_id(const char *name, Key *keys, size_t num_keys,
                             const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response read_nat_rule_dnat_by_id(const char *name, Key *keys, size_t num_keys,
                                  const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response read_nat_rule_dnat_entry_by_id(const char *name, Key *keys,
                                        size_t num_keys, const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response read_nat_rule_dnat_entry_external_ip_by_id(const char *name, Key *keys,
                                                    size_t num_keys,
                                                    const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response read_nat_rule_dnat_entry_internal_ip_by_id(const char *name, Key *keys,
                                                    size_t num_keys,
                                                    const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response read_nat_rule_dnat_entry_list_by_id(const char *name, Key *keys,
                                             size_t num_keys,
                                             const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response read_nat_rule_dnat_entry_list_by_id_get_list(const char *name,
                                                      Key *keys,
                                                      size_t num_keys,
                                                      const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response read_nat_rule_masquerade_by_id(const char *name, Key *keys,
                                        size_t num_keys, const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response read_nat_rule_masquerade_enabled_by_id(const char *name, Key *keys,
                                                size_t num_keys,
                                                const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response read_nat_rule_port_forwarding_by_id(const char *name, Key *keys,
                                             size_t num_keys,
                                             const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response read_nat_rule_port_forwarding_entry_by_id(const char *name, Key *keys,
                                                   size_t num_keys,
                                                   const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response read_nat_rule_port_forwarding_entry_external_ip_by_id(
    const char *name, Key *keys, size_t num_keys, const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response read_nat_rule_port_forwarding_entry_external_port_by_id(
    const char *name, Key *keys, size_t num_keys, const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response read_nat_rule_port_forwarding_entry_internal_ip_by_id(
    const char *name, Key *keys, size_t num_keys, const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response read_nat_rule_port_forwarding_entry_internal_port_by_id(
    const char *name, Key *keys, size_t num_keys, const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response read_nat_rule_port_forwarding_entry_list_by_id(const char *name,
                                                        Key *keys,
                                                        size_t num_keys,
                                                        const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response read_nat_rule_port_forwarding_entry_list_by_id_get_list(
    const char *name, Key *keys, size_t num_keys, const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response read_nat_rule_port_forwarding_entry_proto_by_id(const char *name,
                                                         Key *keys,
                                                         size_t num_keys,
                                                         const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response read_nat_rule_snat_by_id(const char *name, Key *keys, size_t num_keys,
                                  const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response read_nat_rule_snat_entry_by_id(const char *name, Key *keys,
                                        size_t num_keys, const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response read_nat_rule_snat_entry_external_ip_by_id(const char *name, Key *keys,
                                                    size_t num_keys,
                                                    const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response read_nat_rule_snat_entry_internal_net_by_id(const char *name,
                                                     Key *keys, size_t num_keys,
                                                     const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response read_nat_rule_snat_entry_list_by_id(const char *name, Key *keys,
                                             size_t num_keys,
                                             const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response read_nat_rule_snat_entry_list_by_id_get_list(const char *name,
                                                      Key *keys,
                                                      size_t num_keys,
                                                      const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response read_nat_type_by_id(const char *name, Key *keys, size_t num_keys,
                             const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response read_nat_uuid_by_id(const char *name, Key *keys, size_t num_keys,
                             const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response replace_nat_natting_table_by_id(const char *name, Key *keys,
                                         size_t num_keys, const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response replace_nat_natting_table_list_by_id(const char *name, Key *keys,
                                              size_t num_keys,
                                              const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response replace_nat_ports_by_id(const char *name, Key *keys, size_t num_keys,
                                 const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response replace_nat_ports_list_by_id(const char *name, Key *keys,
                                      size_t num_keys, const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response replace_nat_rule_by_id(const char *name, Key *keys, size_t num_keys,
                                const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response replace_nat_rule_dnat_by_id(const char *name, Key *keys,
                                     size_t num_keys, const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response replace_nat_rule_dnat_entry_by_id(const char *name, Key *keys,
                                           size_t num_keys, const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response replace_nat_rule_dnat_entry_list_by_id(const char *name, Key *keys,
                                                size_t num_keys,
                                                const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response replace_nat_rule_masquerade_by_id(const char *name, Key *keys,
                                           size_t num_keys, const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response replace_nat_rule_port_forwarding_by_id(const char *name, Key *keys,
                                                size_t num_keys,
                                                const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response replace_nat_rule_port_forwarding_entry_by_id(const char *name,
                                                      Key *keys,
                                                      size_t num_keys,
                                                      const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response replace_nat_rule_port_forwarding_entry_list_by_id(const char *name,
                                                           Key *keys,
                                                           size_t num_keys,
                                                           const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response replace_nat_rule_snat_by_id(const char *name, Key *keys,
                                     size_t num_keys, const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response replace_nat_rule_snat_entry_by_id(const char *name, Key *keys,
                                           size_t num_keys, const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response replace_nat_rule_snat_entry_list_by_id(const char *name, Key *keys,
                                                size_t num_keys,
                                                const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response replace_nat_loglevel_by_id(const char *name, Key *keys, size_t num_keys,
                                   const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response update_nat_natting_table_by_id(const char *name, Key *keys,
                                        size_t num_keys, const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response update_nat_natting_table_external_ip_by_id(const char *name, Key *keys,
                                                    size_t num_keys,
                                                    const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response update_nat_natting_table_external_port_by_id(const char *name,
                                                      Key *keys,
                                                      size_t num_keys,
                                                      const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response update_nat_natting_table_list_by_id(const char *name, Key *keys,
                                             size_t num_keys,
                                             const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response update_nat_natting_table_originating_rule_by_id(const char *name,
                                                         Key *keys,
                                                         size_t num_keys,
                                                         const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response update_nat_ports_by_id(const char *name, Key *keys, size_t num_keys,
                                const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response update_nat_ports_ip_by_id(const char *name, Key *keys, size_t num_keys,
                                   const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response update_nat_ports_list_by_id(const char *name, Key *keys,
                                     size_t num_keys, const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response replace_nat_ports_peer_by_id(const char *name, Key *keys,
                                     size_t num_keys, const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response update_nat_rule_by_id(const char *name, Key *keys, size_t num_keys,
                               const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response update_nat_rule_dnat_by_id(const char *name, Key *keys,
                                    size_t num_keys, const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response update_nat_rule_dnat_entry_by_id(const char *name, Key *keys,
                                          size_t num_keys, const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response update_nat_rule_dnat_entry_external_ip_by_id(const char *name,
                                                      Key *keys,
                                                      size_t num_keys,
                                                      const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response update_nat_rule_dnat_entry_internal_ip_by_id(const char *name,
                                                      Key *keys,
                                                      size_t num_keys,
                                                      const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response update_nat_rule_dnat_entry_list_by_id(const char *name, Key *keys,
                                               size_t num_keys,
                                               const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response update_nat_rule_masquerade_by_id(const char *name, Key *keys,
                                          size_t num_keys, const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response update_nat_rule_masquerade_enabled_by_id(const char *name, Key *keys,
                                                  size_t num_keys,
                                                  const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response update_nat_rule_port_forwarding_by_id(const char *name, Key *keys,
                                               size_t num_keys,
                                               const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response update_nat_rule_port_forwarding_entry_by_id(const char *name,
                                                     Key *keys, size_t num_keys,
                                                     const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response update_nat_rule_port_forwarding_entry_external_ip_by_id(
    const char *name, Key *keys, size_t num_keys, const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response update_nat_rule_port_forwarding_entry_external_port_by_id(
    const char *name, Key *keys, size_t num_keys, const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response update_nat_rule_port_forwarding_entry_internal_ip_by_id(
    const char *name, Key *keys, size_t num_keys, const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response update_nat_rule_port_forwarding_entry_internal_port_by_id(
    const char *name, Key *keys, size_t num_keys, const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response update_nat_rule_port_forwarding_entry_list_by_id(const char *name,
                                                          Key *keys,
                                                          size_t num_keys,
                                                          const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response update_nat_rule_port_forwarding_entry_proto_by_id(const char *name,
                                                           Key *keys,
                                                           size_t num_keys,
                                                           const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response update_nat_rule_snat_by_id(const char *name, Key *keys,
                                    size_t num_keys, const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response update_nat_rule_snat_entry_by_id(const char *name, Key *keys,
                                          size_t num_keys, const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response update_nat_rule_snat_entry_external_ip_by_id(const char *name,
                                                      Key *keys,
                                                      size_t num_keys,
                                                      const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response update_nat_rule_snat_entry_internal_net_by_id(const char *name,
                                                       Key *keys,
                                                       size_t num_keys,
                                                       const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response update_nat_rule_snat_entry_list_by_id(const char *name, Key *keys,
                                               size_t num_keys,
                                               const char *value) {
  Response returns = {kOk, ""};
  return returns;
}
Response read_nat_internal_net_by_id(const char *name, Key *keys,
                                     size_t num_keys,
                                     const char *value) {
  Response returns = {kOk, ""};
  return returns;
}


#ifdef __cplusplus
}
#endif