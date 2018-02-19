/*
 *  Copyright (C) 2018 Systerel and others.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "service_response_cb_bs.h"

#include "sopc_services_api.h"
#include "sopc_toolkit_config_internal.h"
#include "sopc_user_app_itf.h"

#include "util_b2c.h"

void service_response_cb_bs__INITIALISATION(void) {}

/*--------------------
   OPERATIONS Clause
  --------------------*/
void service_response_cb_bs__cli_service_response(
    const constants__t_session_i service_response_cb_bs__session,
    const constants__t_msg_i service_response_cb_bs__resp_msg,
    const constants__t_application_context_i service_response_cb_bs__app_context)
{
    if (constants__c_session_indet != service_response_cb_bs__session)
    {
        SOPC_ServicesToApp_EnqueueEvent(SOPC_AppEvent_ComEvent_Create(SE_RCV_SESSION_RESPONSE),
                                        service_response_cb_bs__session, service_response_cb_bs__resp_msg,
                                        service_response_cb_bs__app_context);
    }
    else
    {
        SOPC_ServicesToApp_EnqueueEvent(SOPC_AppEvent_ComEvent_Create(SE_RCV_DISCOVERY_RESPONSE), 0,
                                        service_response_cb_bs__resp_msg, service_response_cb_bs__app_context);
    }
}

void service_response_cb_bs__cli_snd_failure(
    const constants__t_application_context_i service_response_cb_bs__app_context,
    const constants__t_StatusCode_i service_response_cb_bs__error_status)
{
    SOPC_ServicesToApp_EnqueueEvent(SOPC_AppEvent_ComEvent_Create(SE_SND_REQUEST_FAILED),
                                    util_status_code__B_to_return_status_C(service_response_cb_bs__error_status), NULL,
                                    service_response_cb_bs__app_context);
}

void service_response_cb_bs__srv_service_response(
    const constants__t_endpoint_config_idx_i service_response_cb_bs__endpoint_config_idx,
    const constants__t_msg_i service_response_cb_bs__resp_msg,
    const constants__t_application_context_i service_response_cb_bs__app_context)
{
    SOPC_ServicesToApp_EnqueueEvent(SOPC_AppEvent_ComEvent_Create(SE_LOCAL_SERVICE_RESPONSE),
                                    (uint32_t) service_response_cb_bs__endpoint_config_idx,
                                    service_response_cb_bs__resp_msg, service_response_cb_bs__app_context);
}
