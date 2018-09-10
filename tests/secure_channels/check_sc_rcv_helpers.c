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

#include "check_sc_rcv_helpers.h"

#include <assert.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#include <check.h>

#include "event_helpers.h"
#include "hexlify.h"
#include "sopc_buffer.h"
#include "sopc_helper_endianness_cfg.h"
#include "sopc_ieee_check.h"
#include "sopc_secure_channels_api.h"
#include "sopc_time.h"
#include "stub_sc_sopc_sockets_api.h"

SOPC_EventRecorder* servicesEvents = NULL;

void Check_SC_Init()
{
    servicesEvents = SOPC_EventRecorder_Create();
    assert(servicesEvents != NULL);
    SOPC_SecureChannels_SetEventHandler(servicesEvents->eventHandler);
}

void Check_SC_Clear()
{
    SOPC_SecureChannels_SetEventHandler(NULL);
    SOPC_EventRecorder_Delete(servicesEvents);
}

SOPC_Event* Check_Service_Event_Received(SOPC_SecureChannels_OutputEvent event, uint32_t eltId, uintptr_t auxParam)
{
    SOPC_Event* serviceEvent = NULL;
    WaitEvent(servicesEvents->events, (void**) &serviceEvent);

    if (CheckEvent("Services", serviceEvent, (int32_t) event, eltId, auxParam))
    {
        return serviceEvent;
    }
    else
    {
        free(serviceEvent);
        return NULL;
    }
}

SOPC_ReturnStatus Check_Client_Closed_SC(uint32_t scIdx,
                                         uint32_t socketIdx,
                                         uint32_t scConfigIdx,
                                         uint32_t pendingRequestHandle,
                                         SOPC_StatusCode errorStatus)
{
    printf("SC_Rcv_Buffer: Checking client closed SC with status %X\n", errorStatus);

    SOPC_Buffer* buffer = NULL;
    int res = 0;
    char hexOutput[512];

    printf("               - CLO message requested to be sent\n");
    SOPC_Event* event = Check_Socket_Event_Received(SOCKET_WRITE, scConfigIdx, 0);

    if (event == NULL || event->params == NULL)
    {
        free(event);
        return SOPC_STATUS_NOK;
    }

    buffer = (SOPC_Buffer*) event->params;
    free(event);
    event = NULL;

    uint32_t buffer_len = buffer->length;

    res = hexlify(buffer->data, hexOutput, buffer_len);
    SOPC_Buffer_Delete(buffer);

    if ((uint32_t) res != buffer_len)
    {
        return SOPC_STATUS_NOK;
    }

    // Check typ = CLO final = F
    if (buffer_len < 4 || memcmp(hexOutput, "434c4f46", 8) != 0)
    {
        return SOPC_STATUS_NOK;
    }

    printf("               - SOCKET requested to be closed\n");
    event = Check_Socket_Event_Received(SOCKET_CLOSE, socketIdx, 0);

    if (NULL == event)
    {
        return SOPC_STATUS_NOK;
    }

    free(event);
    event = NULL;

    printf("               - pending request indicated as in timeout to Services\n");
    event = Check_Service_Event_Received(SC_REQUEST_TIMEOUT, scIdx, pendingRequestHandle);

    if (NULL == event)
    {
        return SOPC_STATUS_NOK;
    }

    free(event);
    event = NULL;

    printf("               - SC indicated as disconnected to Services\n");
    event = Check_Service_Event_Received(SC_DISCONNECTED, scIdx, errorStatus);

    if (NULL == event)
    {
        return SOPC_STATUS_NOK;
    }

    free(event);

    return SOPC_STATUS_OK;
}