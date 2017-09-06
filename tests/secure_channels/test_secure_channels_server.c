/*
 *  Copyright (C) 2017 Systerel and others.
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

#include <stdint.h>
#include <stdio.h>

#include "sopc_endpoint.h"
#include "sopc_time.h"
#include "crypto_profiles.h"
#include "key_manager.h"
#include "pki_stack.h"

#include "stub_sc_sopc_services_api.h"
#include "sopc_services_events.h"
#include "sopc_sc_events.h"
#include "sopc_toolkit_config.h"
#include "sopc_toolkit_constants.h"
#include "sopc_types.h"
#include "sopc_encoder.h"
#include "sopc_secure_channels_api.h"

static bool cryptoDeactivated = true;

int main(void){
    SOPC_StatusCode status = STATUS_OK;
    PKIProvider *pki = NULL;

    // Sleep timeout in milliseconds
    const uint32_t sleepTimeout = 500;
    // Loop timeout in milliseconds
    const uint32_t loopTimeout = 20000;
    // Counter to stop waiting responses after 5 seconds
    uint32_t loopCpt = 0;

    Certificate *crt_srv = NULL;
    AsymmetricKey *priv_srv = NULL;
    Certificate *crt_ca = NULL;

    // Number of the secu policy configuration
    uint32_t nbOfSecurityPolicyConfigurations = 2;

    // Secu policy configuration: empty
    SOPC_SecurityPolicy secuConfig[nbOfSecurityPolicyConfigurations];
    SOPC_String_Initialize(&secuConfig[0].securityPolicy);
    SOPC_String_Initialize(&secuConfig[1].securityPolicy);

    // Services side stub code
    SOPC_Endpoint_Config epConfig;
    uint32_t             epConfigIdx = 0;
    uint32_t             scConfigIdx = 0;
    uint32_t             scConnectionId = 0;
    SOPC_StubSC_ServicesEventParams* serviceEvent = NULL;

    // Endpoint URL
    char* endpointUrl = "opc.tcp://localhost:8888/myEndPoint";

    // Paths to client certificate/key and server certificate
    // Server certificate name
    char* certificateSrvLocation = "./server_public/server.der";
    // Server private key
    char* keyLocation = "./server_private/server.key";

    // The certificates: load
    if(cryptoDeactivated == false){
        status = KeyManager_Certificate_CreateFromFile(certificateSrvLocation, &crt_srv);

        if(STATUS_OK != status){
            printf("<Stub_Server: Failed to load certificate\n");
        }else{
            printf("<Stub_Server: Server certificate loaded\n");
        }
    }

    // Private key: load
    if(STATUS_OK == status && cryptoDeactivated == false){
        status = KeyManager_AsymmetricKey_CreateFromFile(keyLocation, &priv_srv, NULL, 0);
        if(STATUS_OK != status){
            printf("<Stub_Server: Failed to load private key\n");
        }else{
            printf("<Stub_Server: Server private key loaded\n");
        }
    }

    // Certificate Authority: load
    if(STATUS_OK == status  && cryptoDeactivated == false){
        status = KeyManager_Certificate_CreateFromFile("./trusted/cacert.der", &crt_ca);
        if(STATUS_OK != status){
            printf("<Stub_Server: Failed to load CA\n");
        }else{
            printf("<Stub_Server: CA certificate loaded\n");
        }
    }

    // Init toolkit configuration
    if(STATUS_OK == status){
        status = SOPC_Toolkit_Initialize(NULL);
        if(STATUS_OK != status){
            printf("<Stub_Server: Failed to initialize toolkit\n");
        }else{
            printf("<Stub_Server: Toolkit initialized\n");
        }
    }

    if(STATUS_OK == status){
        status = SOPC_String_AttachFromCstring(&secuConfig[0].securityPolicy, SecurityPolicy_Basic256Sha256_URI);
        secuConfig[0].securityModes = SOPC_SECURITY_MODE_SIGN_MASK | SOPC_SECURITY_MODE_SIGNANDENCRYPT_MASK;
        status = SOPC_String_AttachFromCstring(&secuConfig[1].securityPolicy, SecurityPolicy_None_URI);
        secuConfig[1].securityModes = SOPC_SECURITY_MODE_NONE_MASK;
    }

    // Init PKI provider and parse certificate and private key
    // PKIConfig is just used to create the provider but only configuration of PKIType is useful here (paths not used)
    if(STATUS_OK == status && cryptoDeactivated == false){

        if(STATUS_OK != PKIProviderStack_Create(crt_ca, NULL, &pki)){
            printf("<Stub_Server: Failed to create PKI\n");
        }else{
            printf("<Stub_Server: PKI created\n");
        }
    }

    // Start the server code (connection and default management on secu channel)
    if(STATUS_OK == status){
        epConfig.endpointURL = endpointUrl;
        epConfig.nbSecuConfigs = nbOfSecurityPolicyConfigurations;
        epConfig.secuConfigurations = secuConfig;
        epConfig.serverCertificate = crt_srv;
        epConfig.serverKey = priv_srv;
        epConfig.pki = pki;

        epConfigIdx = SOPC_ToolkitServer_AddEndpointConfig(&epConfig);

        assert(epConfigIdx != 0);
        assert(STATUS_OK == SOPC_Toolkit_Configured());

        SOPC_SecureChannels_EnqueueEvent(EP_OPEN,
                                         epConfigIdx,
                                         NULL,
                                         0);

        printf("<Stub_Server: Opening endpoint connection listener ...\n");
    }

    while ((STATUS_OK == status || OpcUa_BadWouldBlock == status) && serviceEvent == NULL && loopCpt * sleepTimeout <= loopTimeout){
        status = SOPC_AsyncQueue_NonBlockingDequeue(servicesEvents, (void**) &serviceEvent);
        if(STATUS_OK != status){
            loopCpt++;
            SOPC_Sleep(sleepTimeout);
        }
    }

    if(STATUS_OK != status && loopCpt * sleepTimeout > loopTimeout){
        status = OpcUa_BadTimeout;
        printf("<Stub_Server: Timeout before client establish connection\n");
    }else if(STATUS_OK == status && serviceEvent == NULL){
        status = OpcUa_BadUnexpectedError;
    }
    loopCpt = 0;

    if(STATUS_OK == status){
        if(serviceEvent->event == SC_TO_SE_EP_SC_CONNECTED){
            if(serviceEvent->eltId == epConfigIdx &&
               serviceEvent->params != NULL &&
               (*(uint32_t*) serviceEvent->params) != 0 && // SC config index
               serviceEvent->auxParam != 0){ // SC connection index

                scConfigIdx = *(uint32_t*) serviceEvent->params;
                scConnectionId = serviceEvent->auxParam;
                (void) scConfigIdx;
                printf("<Stub_Server: Connection established from a client\n");
            }else{
                printf("<Stub_Server: Unexpected client connection parameters values\n");
                status = OpcUa_BadUnexpectedError;
            }
            serviceEvent = NULL;
        }else{
            printf("<Stub_Server: Unexpected event received '%d'\n", serviceEvent->event);
            status = OpcUa_BadUnexpectedError;
        }
    }


    // Wait for a service level message reception
    while ((STATUS_OK == status || OpcUa_BadWouldBlock == status) && serviceEvent == NULL && loopCpt * sleepTimeout <= loopTimeout){
        status = SOPC_AsyncQueue_NonBlockingDequeue(servicesEvents, (void**) &serviceEvent);
        if(STATUS_OK != status){
            loopCpt++;
            SOPC_Sleep(sleepTimeout);
        }
    }

    if(STATUS_OK != status && loopCpt * sleepTimeout > loopTimeout){
        status = OpcUa_BadTimeout;
        printf("<Stub_Server: Timeout before client sent request\n");
    }else if(STATUS_OK == status && serviceEvent == NULL){
        status = OpcUa_BadUnexpectedError;
    }
    loopCpt = 0;

    if(STATUS_OK == status){
        if(serviceEvent->event == SC_TO_SE_SC_SERVICE_RCV_MSG){
            if(serviceEvent->eltId == scConnectionId &&
               serviceEvent->params != NULL &&
               serviceEvent->auxParam != 0) // request context
            {
                SOPC_EncodeableType* encType = NULL;
                assert(STATUS_OK == SOPC_MsgBodyType_Read((SOPC_Buffer*) serviceEvent->params,
                                                          &encType));
                if(encType == &OpcUa_GetEndpointsRequest_EncodeableType){
                    printf("<Stub_Server: Received a GetEndpoint service request => OK\n");

                    OpcUa_ResponseHeader rHeader;
                    OpcUa_ResponseHeader_Initialize(&rHeader);

                    SOPC_Buffer* buffer = SOPC_Buffer_Create(SOPC_MAX_MESSAGE_LENGTH);
                    assert(NULL != buffer);
                    status = SOPC_Buffer_SetDataLength(buffer, SOPC_UA_SECURE_MESSAGE_HEADER_LENGTH +
                                                               SOPC_UA_SYMMETRIC_SECURITY_HEADER_LENGTH +
                                                               SOPC_UA_SECURE_MESSAGE_SEQUENCE_LENGTH);
                    assert(STATUS_OK == status);
                    status = SOPC_Buffer_SetPosition(buffer, SOPC_UA_SECURE_MESSAGE_HEADER_LENGTH +
                                                             SOPC_UA_SYMMETRIC_SECURITY_HEADER_LENGTH +
                                                             SOPC_UA_SECURE_MESSAGE_SEQUENCE_LENGTH);
                    assert(STATUS_OK == status);
                    status = SOPC_EncodeMsg_Type_Header_Body(buffer,
                                                             &OpcUa_ServiceFault_EncodeableType,
                                                             &OpcUa_ResponseHeader_EncodeableType,
                                                             (void*)&rHeader,
                                                             NULL);
                    assert(STATUS_OK == status);

                    SOPC_SecureChannels_EnqueueEvent(SC_SERVICE_SND_MSG,
                                                     scConnectionId,
                                                     (void*) buffer,
                                                     serviceEvent->auxParam); // request context
                    printf("<Stub_Server: Responding with a service fault message \n");
                }else{
                    printf("<Stub_Server: Received an unexpected service request => NOK\n");
                    status = OpcUa_BadUnknownResponse;
                }
            }else{
                printf("<Stub_Server: Unexpected service received message parameters values\n");
                status = OpcUa_BadUnexpectedError;
            }
        }else{
            printf("<Stub_Server: Unexpected event received '%d'\n", serviceEvent->event);
            status = OpcUa_BadUnexpectedError;
        }
        serviceEvent = NULL;
    }


    // Wait for client disconnection
    while ((STATUS_OK == status || OpcUa_BadWouldBlock == status) && serviceEvent == NULL && loopCpt * sleepTimeout <= loopTimeout){
        status = SOPC_AsyncQueue_NonBlockingDequeue(servicesEvents, (void**) &serviceEvent);
        if(STATUS_OK != status){
            loopCpt++;
            SOPC_Sleep(sleepTimeout);
        }
    }

    if(STATUS_OK != status && loopCpt * sleepTimeout > loopTimeout){
        status = OpcUa_BadTimeout;
        printf("<Stub_Server: Timeout before client closed connection\n");
    }else if(STATUS_OK == status && serviceEvent == NULL){
        status = OpcUa_BadUnexpectedError;
    }
    loopCpt = 0;

    if(STATUS_OK == status){
        if(serviceEvent->event == SC_TO_SE_SC_DISCONNECTED){
            if(serviceEvent->eltId == scConnectionId)
            {
                printf("<Stub_Server: Secure connection disconnected with status '%x'\n", serviceEvent->auxParam);
            }else{
                printf("<Stub_Server: Unexpected secure disconnection parameters values\n");
                status = OpcUa_BadUnexpectedError;
            }
        }else{
            printf("<Stub_Server: Unexpected event received '%d'\n", serviceEvent->event);
            status = OpcUa_BadUnexpectedError;
        }
        serviceEvent = NULL;
    }


    // Close the endpoint connection listener
    SOPC_SecureChannels_EnqueueEvent(EP_CLOSE,
                                     epConfigIdx,
                                     NULL,
                                     0);

    while ((STATUS_OK == status || OpcUa_BadWouldBlock == status) && serviceEvent == NULL && loopCpt * sleepTimeout <= loopTimeout){
        status = SOPC_AsyncQueue_NonBlockingDequeue(servicesEvents, (void**) &serviceEvent);
        if(STATUS_OK != status){
            loopCpt++;
            SOPC_Sleep(sleepTimeout);
        }
    }

    if(STATUS_OK != status && loopCpt * sleepTimeout > loopTimeout){
        status = OpcUa_BadTimeout;
        printf("<Stub_Server: Timeout before server closed listener\n");
    }else if(STATUS_OK == status && serviceEvent == NULL){
        status = OpcUa_BadUnexpectedError;
    }
    loopCpt = 0;

    if(STATUS_OK == status){
        if(serviceEvent->event == SC_TO_SE_EP_CLOSED){
            if(serviceEvent->eltId == epConfigIdx)
            {
                printf("<Stub_Server: Secure listener closed\n");
            }else{
                printf("<Stub_Server: Unexpected secure listener closed parameters values\n");
                status = OpcUa_BadUnexpectedError;
            }
        }else{
            printf("<Stub_Server: Unexpected event received '%d'\n", serviceEvent->event);
            status = OpcUa_BadUnexpectedError;
        }
        serviceEvent = NULL;
    }

    printf ("<Stub_Server: Final status: %x\n", status);
    PKIProviderStack_Free(pki);
    KeyManager_Certificate_Free(crt_srv);
    KeyManager_Certificate_Free(crt_ca);
    KeyManager_AsymmetricKey_Free(priv_srv);
    SOPC_Toolkit_Clear();
    if(STATUS_OK == status){
        printf("<Stub_Server: Stub_Server test: OK\n");
    }else{
        printf("<Stub_Server: Stub_Server test: NOK\n");
    }
    if(status != 0){
        return -1;
    }else{

    }
    return status;
}
