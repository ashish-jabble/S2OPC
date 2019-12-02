/*
 * Licensed to Systerel under one or more contributor license
 * agreements. See the NOTICE file distributed with this work
 * for additional information regarding copyright ownership.
 * Systerel licenses this file to you under the Apache
 * License, Version 2.0 (the "License"); you may not use this
 * file except in compliance with the License. You may obtain
 * a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include "service_set_discovery_server_data_bs.h"

#include <assert.h>

/*------------------------
   INITIALISATION Clause
  ------------------------*/
void service_set_discovery_server_data_bs__INITIALISATION(void) {}

/*--------------------
   OPERATIONS Clause
  --------------------*/
void service_set_discovery_server_data_bs__get_RegisteredServer_ServerUri(
    const constants__t_RegisteredServer_i service_set_discovery_server_data_bs__p_reg_server,
    constants__t_ServerUri* const service_set_discovery_server_data_bs__p_server_uri)
{
    *service_set_discovery_server_data_bs__p_server_uri =
        &service_set_discovery_server_data_bs__p_reg_server->ServerUri;
}

void service_set_discovery_server_data_bs__has_ServerCapabilities(
    const constants__t_MdnsDiscoveryConfig_i service_set_discovery_server_data_bs__p_mdns_config,
    const constants__t_ServerCapabilities service_set_discovery_server_data_bs__p_server_capabilities,
    t_bool* const service_set_discovery_server_data_bs__p_bool)
{
    *service_set_discovery_server_data_bs__p_bool = true;
    int32_t nb_capabilities = service_set_discovery_server_data_bs__p_server_capabilities.NoOfServerCapabilityFilter;
    SOPC_String* capabilities = service_set_discovery_server_data_bs__p_server_capabilities.ServerCapabilityFilter;
    OpcUa_MdnsDiscoveryConfiguration* mdnsConfig = service_set_discovery_server_data_bs__p_mdns_config;
    if (nb_capabilities <= 0)
    {
        return; // No filtering to do
    }
    else if (nb_capabilities > mdnsConfig->NoOfServerCapabilities)
    {
        // More required capabilities than capabilities in the mDNS configuration: cannot be compliant
        *service_set_discovery_server_data_bs__p_bool = false;
        return;
    }
    // Otherwise all capabilities shall be present in the mDNS config
    int32_t comparison = -1;
    SOPC_ReturnStatus status = SOPC_STATUS_NOK;
    bool allCapabilitiesFound = true;
    for (int32_t i = 0; allCapabilitiesFound && i < nb_capabilities; i++)
    {
        SOPC_String* reqCapability = &capabilities[i];
        bool capabilityAvailable = false;
        for (int32_t j = 0; !capabilityAvailable && j < mdnsConfig->NoOfServerCapabilities; j++)
        {
            status = SOPC_String_Compare(reqCapability, &mdnsConfig->ServerCapabilities[j], true, &comparison);
            assert(SOPC_STATUS_OK == status);
            capabilityAvailable = (0 == comparison);
        }
        allCapabilitiesFound &= capabilityAvailable;
    }
    *service_set_discovery_server_data_bs__p_bool = allCapabilitiesFound;
}

void service_set_discovery_server_data_bs__is_empty_ServerUri(
    const constants__t_ServerUri service_set_discovery_server_data_bs__p_server_uri,
    t_bool* const service_set_discovery_server_data_bs__p_bool)
{
    *service_set_discovery_server_data_bs__p_bool = service_set_discovery_server_data_bs__p_server_uri->Length <= 0;
}

void service_set_discovery_server_data_bs__is_equal_ServerUri(
    const constants__t_ServerUri service_set_discovery_server_data_bs__p_left,
    const constants__t_ServerUri service_set_discovery_server_data_bs__p_right,
    t_bool* const service_set_discovery_server_data_bs__p_bool)
{
    int32_t comparison = -1;
    SOPC_ReturnStatus status = SOPC_String_Compare(service_set_discovery_server_data_bs__p_left,
                                                   service_set_discovery_server_data_bs__p_right, true, &comparison);
    *service_set_discovery_server_data_bs__p_bool = (SOPC_STATUS_OK == status && 0 == comparison);
}
