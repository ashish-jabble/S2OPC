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

#include <assert.h>
#include <stddef.h>
#include <string.h>

#include "sopc_sockets_internal_ctx.h"

#include "sopc_async_queue.h"
#include "sopc_buffer.h"
#include "sopc_raw_sockets.h"

SOPC_Socket socketsArray[SOPC_MAX_SOCKETS];
Mutex socketsMutex;

void SOPC_SocketsInternalContext_Initialize()
{
    uint32_t idx = 0;
    memset(socketsArray, 0, sizeof(SOPC_Socket) * SOPC_MAX_SOCKETS);
    for (idx = 0; idx < SOPC_MAX_SOCKETS; idx++)
    {
        socketsArray[idx].socketIdx = idx;
        Socket_Clear(&(socketsArray[idx].sock));
    }
    Mutex_Initialization(&socketsMutex);
}

void SOPC_SocketsInternalContext_Clear()
{
    // Close any not closed remaining socket
    uint32_t idx = 0;
    Mutex_Lock(&socketsMutex);
    for (idx = 0; idx < SOPC_MAX_SOCKETS; idx++)
    {
        if (false != socketsArray[idx].isUsed)
        {
            Socket_Close(&(socketsArray[idx].sock));
            socketsArray[idx].isUsed = false;
        }
    }
    Mutex_Unlock(&socketsMutex);
    Mutex_Clear(&socketsMutex);
}

SOPC_Socket* SOPC_SocketsInternalContext_GetFreeSocketNoLock(bool isListener)
{
    SOPC_Socket* result = NULL;
    uint32_t idx = 1; // index 0 is forbidden => reserved for invalid index
    SOPC_ReturnStatus status = SOPC_STATUS_NOK;
    do
    {
        if (false == socketsArray[idx].isUsed)
        {
            socketsArray[idx].isUsed = true;
            result = &socketsArray[idx];
        }
        idx++;
    } while (NULL == result && idx < SOPC_MAX_SOCKETS);

    if (NULL != result && isListener == false)
    {
        status = SOPC_AsyncQueue_Init(&result->writeQueue, "Socket write msgs");
        assert(SOPC_STATUS_OK == status);
    }
    return result;
}

void SOPC_SocketsInternalContext_CloseSocketNoLock(uint32_t socketIdx)
{
    SOPC_Socket* sock = NULL;
    void* elt = NULL;
    SOPC_ReturnStatus status = SOPC_STATUS_NOK;
    if (socketIdx < SOPC_MAX_SOCKETS && socketsArray[socketIdx].isUsed != false)
    {
        sock = &socketsArray[socketIdx];
        Socket_Close(&sock->sock);
        sock->isUsed = false;
        sock->state = SOCKET_STATE_CLOSED;
        sock->waitTreatNetworkEvent = false;
        sock->isServerConnection = false;
        sock->listenerSocketIdx = 0;
        sock->listenerConnections = 0;
        if (sock->connectAddrs != NULL)
        {
            Socket_AddrInfoDelete((Socket_AddressInfo**) &sock->connectAddrs);
        }
        sock->connectAddrs = NULL;
        sock->nextConnectAttemptAddr = NULL;
        if (sock->writeQueue != NULL)
        {
            // Clear all buffers in the queue
            status = SOPC_AsyncQueue_NonBlockingDequeue(sock->writeQueue, &elt);
            while (SOPC_STATUS_OK == status && NULL != elt)
            {
                SOPC_Buffer_Delete((SOPC_Buffer*) elt);
                elt = NULL;
                status = SOPC_AsyncQueue_NonBlockingDequeue(sock->writeQueue, &elt);
            }
            // Clear the queue
            SOPC_AsyncQueue_Free(&sock->writeQueue);
        }
    }
}

void SOPC_SocketsInternalContext_CloseSocketLock(uint32_t socketIdx)
{
    Mutex_Lock(&socketsMutex);
    SOPC_SocketsInternalContext_CloseSocketNoLock(socketIdx);
    Mutex_Unlock(&socketsMutex);
}
