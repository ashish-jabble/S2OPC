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

#include "sopc_secure_channels_api.h"
#include "sopc_secure_channels_api_internal.h"

#include <assert.h>
#include <stdbool.h>

#include "sopc_chunks_mgr.h"
#include "sopc_event_handler.h"
#include "sopc_secure_channels_internal_ctx.h"
#include "sopc_secure_connection_state_mgr.h"
#include "sopc_secure_listener_state_mgr.h"
#include "sopc_sockets_api.h"

void SOPC_SecureChannels_OnSocketsEvent(SOPC_EventHandler* handler,
                                        int32_t event,
                                        uint32_t eltId,
                                        void* params,
                                        uintptr_t auxParam)
{
    (void) handler;
    SOPC_Sockets_OutputEvent sockEvent = (SOPC_Sockets_OutputEvent) event;

    switch (sockEvent)
    {
    /* Sockets events: */
    /* Sockets manager -> SC listener state manager */
    case SOCKET_LISTENER_OPENED:
    case SOCKET_LISTENER_CONNECTION:
    case SOCKET_LISTENER_FAILURE:
        SOPC_SecureListenerStateMgr_OnSocketEvent(sockEvent, eltId, params, auxParam);
        break;
    /* Sockets manager -> SC connection state manager */
    case SOCKET_CONNECTION:
    case SOCKET_FAILURE:
        SOPC_SecureConnectionStateMgr_OnSocketEvent(sockEvent, eltId, params, auxParam);
        break;
    /* Sockets manager -> Chunks manager */
    case SOCKET_RCV_BYTES:
        SOPC_ChunksMgr_OnSocketEvent(sockEvent, eltId, params, auxParam);
        break;
    }
}

void SOPC_SecureChannels_OnInputEvent(SOPC_EventHandler* handler,
                                      int32_t event,
                                      uint32_t eltId,
                                      void* params,
                                      uintptr_t auxParam)
{
    (void) handler;

    SOPC_SecureChannels_InputEvent scEvent = (SOPC_SecureChannels_InputEvent) event;
    switch (scEvent)
    {
    /* Services events: */
    /* Services manager -> SC listener state manager */
    case EP_OPEN:
    case EP_CLOSE:
        SOPC_SecureListenerStateMgr_Dispatcher(scEvent, eltId, params, auxParam);
        break;
    /* Services manager -> SC connection state manager */
    case SC_CONNECT:
    case TIMER_SC_CONNECTION_TIMEOUT:
    case TIMER_SC_CLIENT_OPN_RENEW:
    case TIMER_SC_REQUEST_TIMEOUT:
    case SC_DISCONNECT:
    case SC_SERVICE_SND_MSG:
        SOPC_SecureConnectionStateMgr_Dispatcher(scEvent, eltId, params, auxParam);
        break;

    /* Internal events */
    /* SC listener manager -> SC connection state manager */
    case INT_EP_SC_CREATE:
    case INT_EP_SC_CLOSE:
        SOPC_SecureConnectionStateMgr_Dispatcher(scEvent, eltId, params, auxParam);
        break;

    /* SC connection manager -> SC listener state manager */
    case INT_EP_SC_CREATED:
    case INT_EP_SC_DISCONNECTED:
        SOPC_SecureListenerStateMgr_Dispatcher(scEvent, eltId, params, auxParam);
        break;

    /* OPC UA chunks message manager -> SC connection manager */
    case INT_SC_RCV_HEL:
    case INT_SC_RCV_ACK:
    case INT_SC_RCV_ERR:
    case INT_SC_RCV_OPN:
    case INT_SC_RCV_CLO:
    case INT_SC_RCV_MSG_CHUNKS:
    case INT_SC_RCV_FAILURE:
    case INT_SC_SND_FAILURE:
    case INT_SC_CLOSE:
        SOPC_SecureConnectionStateMgr_Dispatcher(scEvent, eltId, params, auxParam);
        break;

    /* SC connection manager -> OPC UA chunks message manager */
    case INT_SC_SND_HEL:
    case INT_SC_SND_ACK:
    case INT_SC_SND_ERR:
    case INT_SC_SND_OPN:
    case INT_SC_SND_CLO:
    case INT_SC_SND_MSG_CHUNKS:
        SOPC_ChunksMgr_Dispatcher(scEvent, eltId, params, auxParam);
        break;
    default:
        assert(false);
    }
}

void SOPC_SecureChannels_EnqueueEvent(SOPC_SecureChannels_InputEvent scEvent,
                                      uint32_t id,
                                      void* params,
                                      uintptr_t auxParam)
{
    assert(secureChannelsInputEventHandler != NULL);

    switch (scEvent)
    {
    /* External events */
    case EP_OPEN:
    case EP_CLOSE:
    case SC_CONNECT:
    case TIMER_SC_CLIENT_OPN_RENEW:
    case TIMER_SC_CONNECTION_TIMEOUT:
    case SC_DISCONNECT:
    case SC_SERVICE_SND_MSG:
        SOPC_EventHandler_Post(secureChannelsInputEventHandler, (int32_t) scEvent, id, params, auxParam);
        break;
        /* Internal or invalid events*/
    case INT_EP_SC_CREATE:
    case INT_EP_SC_CLOSE:
    case INT_EP_SC_CREATED:
    case INT_EP_SC_DISCONNECTED:
    case INT_SC_RCV_HEL:
    case INT_SC_RCV_ACK:
    case INT_SC_RCV_ERR:
    case INT_SC_RCV_OPN:
    case INT_SC_RCV_CLO:
    case INT_SC_RCV_MSG_CHUNKS:
    case INT_SC_RCV_FAILURE:
    case INT_SC_SND_FAILURE:
    case INT_SC_SND_HEL:
    case INT_SC_SND_ACK:
    case INT_SC_SND_ERR:
    case INT_SC_SND_OPN:
    case INT_SC_SND_CLO:
    case INT_SC_SND_MSG_CHUNKS:
    case INT_SC_CLOSE:
    default:
        assert(false);
    }
}

void SOPC_SecureChannels_EnqueueInternalEvent(SOPC_SecureChannels_InputEvent scEvent,
                                              uint32_t id,
                                              void* params,
                                              uintptr_t auxParam)
{
    assert(secureChannelsInputEventHandler != NULL);

    switch (scEvent)
    {
    /* Internal events*/
    case INT_EP_SC_CREATE:
    case INT_EP_SC_CLOSE:
    case INT_EP_SC_CREATED:
    case INT_EP_SC_DISCONNECTED:
    case INT_SC_RCV_HEL:
    case INT_SC_RCV_ACK:
    case INT_SC_RCV_ERR:
    case INT_SC_RCV_OPN:
    case INT_SC_RCV_CLO:
    case INT_SC_RCV_MSG_CHUNKS:
    case INT_SC_RCV_FAILURE:
    case INT_SC_SND_FAILURE:
    case INT_SC_SND_HEL:
    case INT_SC_SND_ACK:
    case INT_SC_SND_ERR:
    case INT_SC_SND_OPN:
    case INT_SC_SND_CLO:
    case INT_SC_SND_MSG_CHUNKS:
    case INT_SC_CLOSE:
        SOPC_EventHandler_Post(secureChannelsInputEventHandler, (int32_t) scEvent, id, params, auxParam);
        break;
    /* External or invalid events */
    case EP_OPEN:
    case EP_CLOSE:
    case SC_CONNECT:
    case TIMER_SC_CONNECTION_TIMEOUT:
    case TIMER_SC_CLIENT_OPN_RENEW:
    case TIMER_SC_REQUEST_TIMEOUT:
    case SC_DISCONNECT:
    case SC_SERVICE_SND_MSG:
    default:
        assert(false);
    }
}

void SOPC_SecureChannels_EnqueueInternalEventAsNext(SOPC_SecureChannels_InputEvent scEvent,
                                                    uint32_t id,
                                                    void* params,
                                                    uintptr_t auxParam)
{
    assert(secureChannelsInputEventHandler != NULL);

    switch (scEvent)
    {
    /* Internal events*/
    case INT_EP_SC_CREATE:
    case INT_EP_SC_CLOSE:
    case INT_EP_SC_CREATED:
    case INT_EP_SC_DISCONNECTED:
    case INT_SC_RCV_HEL:
    case INT_SC_RCV_ACK:
    case INT_SC_RCV_ERR:
    case INT_SC_RCV_OPN:
    case INT_SC_RCV_CLO:
    case INT_SC_RCV_MSG_CHUNKS:
    case INT_SC_RCV_FAILURE:
    case INT_SC_SND_FAILURE:
    case INT_SC_SND_HEL:
    case INT_SC_SND_ACK:
    case INT_SC_SND_ERR:
    case INT_SC_SND_OPN:
    case INT_SC_SND_CLO:
    case INT_SC_SND_MSG_CHUNKS:
    case INT_SC_CLOSE:
        SOPC_EventHandler_PostAsNext(secureChannelsInputEventHandler, (int32_t) scEvent, id, params, auxParam);
        break;
        /* External or invalid events */
    case EP_OPEN:
    case EP_CLOSE:
    case SC_CONNECT:
    case TIMER_SC_CONNECTION_TIMEOUT:
    case TIMER_SC_CLIENT_OPN_RENEW:
    case TIMER_SC_REQUEST_TIMEOUT:
    case SC_DISCONNECT:
    case SC_SERVICE_SND_MSG:
    default:
        assert(false);
    }
}

void SOPC_SecureChannels_Initialize()
{
    SOPC_SecureChannelsInternalContext_Initialize();
}

void SOPC_SecureChannels_Clear()
{
    SOPC_SecureChannelsInternalContext_Clear();
}
