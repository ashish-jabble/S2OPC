/*
 *  Copyright (C) 2016 Systerel and others.
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

#ifndef SOPC_SECURE_CHANNEL_SERVER_ENDPOINT_H_
#define SOPC_SECURE_CHANNEL_SERVER_ENDPOINT_H_

#include "singly_linked_list.h"
#include "sopc_mutexes.h"
#include "sopc_builtintypes.h"
#include "sopc_namespace_table.h"
#include "sopc_tcp_ua_listener.h"
#include "sopc_secure_channel_low_level.h"
#include "crypto_decl.h"

typedef enum SC_EndpointState
{
    SC_Endpoint_Opened,
    SC_Endpoint_Closed,
    SC_Endpoint_Error
} SC_EndpointState;

typedef enum SC_EndpointEvent
{
    SC_EndpointListenerEvent_Opened,
    SC_EndpointListenerEvent_Closed,
    SC_EndpointConnectionEvent_New,
    SC_EndpointConnectionEvent_Renewed,
    SC_EndpointConnectionEvent_Disconnected,
    SC_EndpointConnectionEvent_Request,
    SC_EndpointConnectionEvent_PartialRequest,
    SC_EndpointConnectionEvent_AbortRequest,
    SC_EndpointConnectionEvent_DecoderError
} SC_EndpointEvent;

#define SECURITY_MODE_NONE_MASK 0x01
#define SECURITY_MODE_SIGN_MASK 0x02
#define SECURITY_MODE_SIGNANDENCRYPT_MASK 0x04
#define SECURITY_MODE_ANY_MASK 0x07

/**
 *  \brief Definition of a security policy supported by endpoint
 */
typedef struct SOPC_SecurityPolicy
{
    SOPC_String securityPolicy; /**< Security policy URI supported */
    uint16_t    securityModes;  /**< Mask of security modes supported (use combination of SECURITY_MODE_*_MASK values) */
    void*       padding;        /**< Binary compatibility */
} SOPC_SecurityPolicy;

struct SC_ServerEndpoint;

typedef SOPC_StatusCode (SC_EndpointEvent_CB) (struct SC_ServerEndpoint* sEndpoint,
                                               SC_Connection*            scConnection,
                                               void*                     cbData,
                                               SC_EndpointEvent          event,
                                               SOPC_StatusCode           status,
                                               uint32_t*                 requestId,
                                               SOPC_EncodeableType*      reqEncType,
                                               void*                     reqEncObj);

typedef struct SC_ServerEndpoint
{
    SOPC_NamespaceTable    namespaces;
    SOPC_EncodeableType**  encodeableTypes;
    const PKIProvider*     pkiProvider;
    const Certificate*     serverCertificate;
    const AsymmetricKey*   serverKey;
    SOPC_SecurityPolicy*   securityPolicies;
    uint8_t                nbSecurityPolicies;
    SC_EndpointState       state;
    uint32_t               lastSecureConnectionId; // internal use only (used in secureChannelConnections)
    SLinkedList*           secureChannelConnections;
    TCP_UA_Listener*       transportListener;
    P_Timer                watchdogTimer;
    SC_EndpointEvent_CB*   callback;
    void*                  callbackData;
    void**                 servicesTable; // Table of services defined in upper level
    Mutex                  mutex;
} SC_ServerEndpoint;

SC_ServerEndpoint* SC_ServerEndpoint_Create();

SOPC_StatusCode SC_ServerEndpoint_Configure(SC_ServerEndpoint*     endpoint,
                                            SOPC_NamespaceTable*   namespaceTable,
                                            SOPC_EncodeableType**  encodeableTypes);

SOPC_StatusCode SC_ServerEndpoint_Open(SC_ServerEndpoint*   endpoint,
                                       const char*          endpointURL,
                                       const PKIProvider*   pki,
                                       const Certificate*   serverCertificate,
                                       const AsymmetricKey* serverKey,
                                       uint8_t              nbSecurityPolicies,
                                       SOPC_SecurityPolicy* securityPolicies,
                                       SC_EndpointEvent_CB* callback,
                                       void*                callbackData);

SOPC_StatusCode SC_Send_Response(SC_ServerEndpoint*   sEndpoint,
                                 SC_Connection*       scConnection,
                                 uint32_t             requestId,
                                 SOPC_EncodeableType* responseType,
                                 void*                response);

SOPC_StatusCode SC_ServerEndpoint_Close(SC_ServerEndpoint* endpoint);

void SC_ServerEndpoint_Delete(SC_ServerEndpoint* endpoint);

#endif /* SOPC_SECURE_CHANNEL_SERVER_ENDPOINT_H_ */
