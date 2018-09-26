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

#include <assert.h>
#include <errno.h>
#include <inttypes.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sopc_atomic.h"
#include "sopc_crypto_profiles.h"
#include "sopc_crypto_provider.h"
#include "sopc_pki_stack.h"

#include "opcua_identifiers.h"
#include "opcua_statuscodes.h"
#include "sopc_encodeable.h"
#include "sopc_time.h"

#include "sopc_toolkit_async_api.h"
#include "sopc_toolkit_config.h"

#include "embedded/loader.h"
#include "runtime_variables.h"

#ifdef WITH_EXPAT
#include "uanodeset_expat/loader.h"
#endif

#define ENDPOINT_URL "opc.tcp://localhost:4841"
#define APPLICATION_URI "urn:S2OPC:localhost"
#define PRODUCT_URI "urn:S2OPC:localhost"

static const char* app_namespace_uris[] = {(const char*) PRODUCT_URI, NULL};

static int32_t endpointClosed = 0;
static bool secuActive = true;

volatile sig_atomic_t stopServer = 0;

typedef enum
{
    AS_LOADER_EMBEDDED,
#ifdef WITH_EXPAT
    AS_LOADER_EXPAT,
#endif
} AddressSpaceLoader;

void Test_StopSignal(int sig)
{
    (void) sig;
    if (stopServer != 0)
    {
        exit(1);
    }
    else
    {
        stopServer = 1;
    }
}

void Test_ComEvent_FctServer(SOPC_App_Com_Event event, uint32_t idOrStatus, void* param, uintptr_t appContext)
{
    (void) idOrStatus;
    (void) param;
    (void) appContext;
    if (event == SE_CLOSED_ENDPOINT)
    {
        printf("<Test_Server_Toolkit: closed endpoint event: OK\n");
        SOPC_Atomic_Int_Set(&endpointClosed, 1);
    }
    else if (event == SE_LOCAL_SERVICE_RESPONSE)
    {
        SOPC_EncodeableType* message_type = *((SOPC_EncodeableType**) param);

        if (message_type != &OpcUa_WriteResponse_EncodeableType)
        {
            return;
        }

        OpcUa_WriteResponse* write_response = param;

        bool ok = (write_response->ResponseHeader.ServiceResult == SOPC_GoodGenericStatus);

        for (int32_t i = 0; i < write_response->NoOfResults; ++i)
        {
            ok &= (write_response->Results[i] == SOPC_GoodGenericStatus);
        }

        if (!ok)
        {
            printf("<Test_Server_Toolkit: Error while updating address space\n");
        }

        return;
    }

    else
    {
        printf("<Test_Server_Toolkit: unexpected endpoint event %d : NOK\n", event);
    }
}

void Test_AddressSpaceNotif_Fct(SOPC_App_AddSpace_Event event, void* opParam, SOPC_StatusCode opStatus)
{
    OpcUa_WriteValue* wv = NULL;
    if (event == AS_WRITE_EVENT)
    {
        printf("<Test_Server_Toolkit: address space WRITE event: OK\n");
        wv = (OpcUa_WriteValue*) opParam;
        if (wv != NULL)
        {
            switch (wv->NodeId.IdentifierType)
            {
            case SOPC_IdentifierType_Numeric:
                printf("  NodeId: Namespace %" PRIu16 ", ID %" PRIu32 "\n", wv->NodeId.Namespace,
                       wv->NodeId.Data.Numeric);
                break;
            case SOPC_IdentifierType_String:
                printf("  NodeId: Namespace %" PRIu16 ", ID %s\n", wv->NodeId.Namespace,
                       SOPC_String_GetRawCString(&wv->NodeId.Data.String));
                break;
            default:
                break;
            }
            printf("  AttributeId: %" PRIu32 "\n", wv->AttributeId);
            printf("  Write status: %" PRIX32 "\n", opStatus);
        }
    }
    else
    {
        printf("<Test_Server_Toolkit: unexpected address space event %d : NOK\n", event);
    }
}

#ifdef WITH_EXPAT
static SOPC_AddressSpace* load_nodeset_from_file(const char* filename)
{
    SOPC_ReturnStatus status = SOPC_STATUS_OK;
    SOPC_AddressSpace* space = NULL;
    FILE* fd = fopen(filename, "r");

    if (fd == NULL)
    {
        printf("<Test_Server_Toolkit: Error while opening %s: %s\n", filename, strerror(errno));
        status = SOPC_STATUS_NOK;
    }

    if (status == SOPC_STATUS_OK)
    {
        space = SOPC_UANodeSet_Parse(fd);

        if (space == NULL)
        {
            status = SOPC_STATUS_NOK;
        }
    }

    if (status != SOPC_STATUS_OK)
    {
        printf("<Test_Server_Toolkit: Error while parsing XML address space\n");
    }

    if (fd != NULL)
    {
        fclose(fd);
    }

    if (status == SOPC_STATUS_OK)
    {
        printf("<Test_Server_Toolkit: Loaded address space from %s\n", filename);
        return space;
    }
    else
    {
        SOPC_AddressSpace_Delete(space);
        return NULL;
    }
}
#endif

static time_t parse_build_date(const char* build_date)
{
    struct tm time;
    memset(&time, 0, sizeof(struct tm));

    if (sscanf(build_date, "%4d-%2d-%2d", &time.tm_year, &time.tm_mon, &time.tm_mday) != 3)
    {
        return 0;
    }

    if (time.tm_year < 1900 || time.tm_mon < 0 || time.tm_mon > 11 || time.tm_mday < 0 || time.tm_mday > 31)
    {
        return 0;
    }

    time.tm_year -= 1900;
    time.tm_mon--;

    return mktime(&time);
}

/* The toolkit test servers shall pass the UACTT tests. Hence it shall authenticate
 * (ids and passwords can be changed in the UACTT settings/Server Test/Session):
 *  - anonymous users
 *  - user1:password
 *  - user2:password1
 * Then it shall accept username:password, but return "access denied".
 * Otherwise it shall be "identity token rejected".
 */
static SOPC_ReturnStatus authentication_uactt(SOPC_UserAuthentication_Manager* authn,
                                              const SOPC_ExtensionObject* token,
                                              SOPC_UserAuthentication_Status* authenticated)
{
    (void) (authn);
    assert(NULL != token && NULL != authenticated);

    *authenticated = SOPC_USER_AUTHENTICATION_REJECTED_TOKEN;
    if (&OpcUa_AnonymousIdentityToken_EncodeableType == token->Body.Object.ObjType)
    {
        *authenticated = SOPC_USER_AUTHENTICATION_OK;
    }
    else if (&OpcUa_UserNameIdentityToken_EncodeableType == token->Body.Object.ObjType)
    {
        OpcUa_UserNameIdentityToken* userToken = token->Body.Object.Value;
        SOPC_String* username = &userToken->UserName;
        if (strcmp(SOPC_String_GetRawCString(username), "user1") == 0)
        {
            SOPC_ByteString* pwd = &userToken->Password;
            if (pwd->Length == strlen("password") && memcmp(pwd->Data, "password", strlen("password")) == 0)
            {
                *authenticated = SOPC_USER_AUTHENTICATION_OK;
            }
        }
        else if (strcmp(SOPC_String_GetRawCString(username), "user2") == 0)
        {
            SOPC_ByteString* pwd = &userToken->Password;
            if (pwd->Length == strlen("password1") && memcmp(pwd->Data, "password1", strlen("password1")) == 0)
            {
                *authenticated = SOPC_USER_AUTHENTICATION_OK;
            }
        }
        else if (strcmp(SOPC_String_GetRawCString(username), "username") == 0)
        {
            SOPC_ByteString* pwd = &userToken->Password;
            if (pwd->Length == strlen("password") && memcmp(pwd->Data, "password", strlen("password")) == 0)
            {
                *authenticated = SOPC_USER_AUTHENTICATION_ACCESS_DENIED;
            }
        }
    }

    return SOPC_STATUS_OK;
}

static const SOPC_UserAuthentication_Functions authentication_uactt_functions = {
    .pFuncFree = (SOPC_UserAuthentication_Free_Func) free,
    .pFuncValidateUserIdentity = authentication_uactt};

int main(int argc, char* argv[])
{
    (void) argc;
    (void) argv;

    // Install signal handler to close the server gracefully when server needs to stop
    signal(SIGINT, Test_StopSignal);
    signal(SIGTERM, Test_StopSignal);

    SOPC_ReturnStatus status = SOPC_STATUS_OK;
    uint32_t epConfigIdx = 0;
    SOPC_Endpoint_Config epConfig;
    // Sleep timeout in milliseconds
    const uint32_t sleepTimeout = 500;

    // Secu policy configuration:
    static SOPC_SerializedCertificate* serverCertificate = NULL;
    static SOPC_SerializedAsymmetricKey* serverKey = NULL;
    static SOPC_SerializedCertificate* authCertificate = NULL;
    static SOPC_PKIProvider* pkiProvider = NULL;

    AddressSpaceLoader address_space_loader = AS_LOADER_EMBEDDED;
    SOPC_AddressSpace* address_space = NULL;

    SOPC_SecurityPolicy secuConfig[3];
    SOPC_String_Initialize(&secuConfig[0].securityPolicy);
    SOPC_String_Initialize(&secuConfig[1].securityPolicy);
    SOPC_String_Initialize(&secuConfig[2].securityPolicy);

    // Get Toolkit Configuration
    SOPC_Build_Info build_info = SOPC_ToolkitConfig_GetBuildInfo();
    printf("toolkitVersion: %s\n", build_info.toolkitVersion);
    printf("toolkitSrcCommit: %s\n", build_info.toolkitSrcCommit);
    printf("toolkitDockerId: %s\n", build_info.toolkitDockerId);
    printf("toolkitBuildDate: %s\n", build_info.toolkitBuildDate);

    if (SOPC_STATUS_OK == status)
    {
        status = SOPC_String_AttachFromCstring(&secuConfig[0].securityPolicy, SOPC_SecurityPolicy_None_URI);
        secuConfig[0].securityModes = SOPC_SECURITY_MODE_NONE_MASK;
        if (SOPC_STATUS_OK == status)
        {
            status = SOPC_String_AttachFromCstring(&secuConfig[1].securityPolicy, SOPC_SecurityPolicy_Basic256_URI);
            secuConfig[1].securityModes = SOPC_SECURITY_MODE_SIGN_MASK | SOPC_SECURITY_MODE_SIGNANDENCRYPT_MASK;
        }
        if (SOPC_STATUS_OK == status)
        {
            status =
                SOPC_String_AttachFromCstring(&secuConfig[2].securityPolicy, SOPC_SecurityPolicy_Basic256Sha256_URI);
            secuConfig[2].securityModes = SOPC_SECURITY_MODE_SIGNANDENCRYPT_MASK;
        }
    }

    // Init unique endpoint structure
    epConfig.endpointURL = ENDPOINT_URL;

    if (secuActive != false)
    {
        status = SOPC_KeyManager_SerializedCertificate_CreateFromFile("./server_public/server_2k_cert.der",
                                                                      &serverCertificate);
        epConfig.serverCertificate = serverCertificate;

        if (SOPC_STATUS_OK == status)
        {
            status = SOPC_KeyManager_SerializedAsymmetricKey_CreateFromFile("./server_private/server_2k_key.pem",
                                                                            &serverKey);
            epConfig.serverKey = serverKey;
        }
        if (SOPC_STATUS_OK == status)
        {
            status = SOPC_KeyManager_SerializedCertificate_CreateFromFile("./trusted/cacert.der", &authCertificate);
        }

        if (SOPC_STATUS_OK == status)
        {
            status = SOPC_PKIProviderStack_Create(authCertificate, NULL, &pkiProvider);
            epConfig.pki = pkiProvider;
        }
        if (SOPC_STATUS_OK != status)
        {
            printf("<Test_Server_Toolkit: Failed loading certificates and key (check paths are valid)\n");
        }
        else
        {
            printf("<Test_Server_Toolkit: Certificates and key loaded\n");
        }
    }
    else
    {
        epConfig.serverCertificate = NULL;
        epConfig.serverKey = NULL;
        epConfig.pki = NULL;
    }

    epConfig.secuConfigurations = secuConfig;
    if (secuActive)
    {
        epConfig.nbSecuConfigs = 3;
    }
    else
    {
        epConfig.nbSecuConfigs = 1;
    }

    // Application description configuration
    OpcUa_ApplicationDescription_Initialize(&epConfig.serverDescription);
    SOPC_String_AttachFromCstring(&epConfig.serverDescription.ApplicationUri, APPLICATION_URI);
    SOPC_String_AttachFromCstring(&epConfig.serverDescription.ProductUri, PRODUCT_URI);
    epConfig.serverDescription.ApplicationType = OpcUa_ApplicationType_Server;
    SOPC_String_AttachFromCstring(&epConfig.serverDescription.ApplicationName.Text, "INGOPCS toolkit server example");

    SOPC_UserAuthentication_Manager* authenticationManager = NULL;
    SOPC_UserAuthorization_Manager* authorizationManager = NULL;
    if (SOPC_STATUS_OK == status)
    {
        authenticationManager = calloc(1, sizeof(SOPC_UserAuthentication_Manager));
        authorizationManager = SOPC_UserAuthorization_CreateManager_AllowAll();
        if (NULL == authenticationManager || NULL == authorizationManager)
        {
            status = SOPC_STATUS_OUT_OF_MEMORY;
            printf("<Test_Server_Toolkit: Failed to create the user manager\n");
        }
    }

    if (SOPC_STATUS_OK == status)
    {
        authenticationManager->pFunctions = &authentication_uactt_functions;
        epConfig.authenticationManager = authenticationManager;
        epConfig.authorizationManager = authorizationManager;
    }

    // Init stack configuration
    if (SOPC_STATUS_OK == status)
    {
        status = SOPC_Toolkit_Initialize(Test_ComEvent_FctServer);
        if (SOPC_STATUS_OK != status)
        {
            printf("<Test_Server_Toolkit: Failed initializing\n");
        }
        else
        {
            printf("<Test_Server_Toolkit: initialized\n");
        }
    }

    if (SOPC_STATUS_OK == status)
    {
        status = SOPC_ToolkitConfig_SetLogLevel(SOPC_TOOLKIT_LOG_LEVEL_DEBUG);
    }

    // Define server address space
    if (SOPC_STATUS_OK == status)
    {
#ifdef WITH_EXPAT
        const char* xml_file_path = getenv("TEST_SERVER_XML_ADDRESS_SPACE");

        if (xml_file_path != NULL)
        {
            address_space_loader = AS_LOADER_EXPAT;
        }
#endif

        switch (address_space_loader)
        {
        case AS_LOADER_EMBEDDED:
            address_space = SOPC_Embedded_AddressSpace_Load();
            status = SOPC_STATUS_OK;
            break;
#ifdef WITH_EXPAT
        case AS_LOADER_EXPAT:
            address_space = load_nodeset_from_file(xml_file_path);
            status = (address_space != NULL) ? SOPC_STATUS_OK : SOPC_STATUS_NOK;
            break;
#endif
        }
    }

    if (SOPC_STATUS_OK == status)
    {
        status = SOPC_ToolkitServer_SetAddressSpaceConfig(address_space);

        if (SOPC_STATUS_OK != status)
        {
            printf("<Test_Server_Toolkit: Failed to configure the @ space\n");
        }
        else
        {
            printf("<Test_Server_Toolkit: @ space configured\n");
        }
    }

    // Define address space modification notification callback
    if (SOPC_STATUS_OK == status)
    {
        status = SOPC_ToolkitServer_SetAddressSpaceNotifCb(&Test_AddressSpaceNotif_Fct);
        if (SOPC_STATUS_OK != status)
        {
            printf("<Test_Server_Toolkit: Failed to configure the @ space modification notification callback \n");
        }
        else
        {
            printf("<Test_Server_Toolkit: @ space modification notification callback configured\n");
        }
    }

    // Add endpoint description configuration
    if (SOPC_STATUS_OK == status)
    {
        epConfigIdx = SOPC_ToolkitServer_AddEndpointConfig(&epConfig);
        if (epConfigIdx != 0)
        {
            status = SOPC_Toolkit_Configured();
        }
        else
        {
            status = SOPC_STATUS_NOK;
        }
        if (SOPC_STATUS_OK != status)
        {
            printf("<Test_Server_Toolkit: Failed to configure the endpoint\n");
        }
        else
        {
            printf("<Test_Server_Toolkit: Endpoint configured\n");
        }
    }

    // Asynchronous request to open the endpoint
    if (SOPC_STATUS_OK == status)
    {
        printf("<Test_Server_Toolkit: Opening endpoint... \n");
        SOPC_ToolkitServer_AsyncOpenEndpoint(epConfigIdx);

        RuntimeVariables runtime_vars = {
            .server_uri = PRODUCT_URI,
            .app_namespace_uris = app_namespace_uris,
            .server_state = OpcUa_ServerState_Running,
            .build_info =
                {
                    .product_uri = PRODUCT_URI,
                    .manufacturer_name = "Systerel",
                    .product_name = "S2OPC",
                    .software_version = build_info.toolkitVersion,
                    .build_number = build_info.toolkitSrcCommit,
                    .build_date = parse_build_date(build_info.toolkitBuildDate),
                },
            .service_level = 255,
            .auditing = true,
        };

        if (!set_runtime_variables(epConfigIdx, &runtime_vars))
        {
            printf("<Test_Server_Toolkit: Failed to populate Server object");
            status = SOPC_STATUS_NOK;
        }
    }

    // Run the server until notification that endpoint is closed or stop server signal
    while (SOPC_STATUS_OK == status && stopServer == 0 && SOPC_Atomic_Int_Get(&endpointClosed) == 0)
    {
        SOPC_Sleep(sleepTimeout);
    }

    if (SOPC_STATUS_OK == status && SOPC_Atomic_Int_Get(&endpointClosed) == 0)
    {
        // Asynchronous request to close the endpoint
        SOPC_ToolkitServer_AsyncCloseEndpoint(epConfigIdx);
    }

    // Wait until endpoint is closed or stop server signal
    while (SOPC_STATUS_OK == status && stopServer == 0 && SOPC_Atomic_Int_Get(&endpointClosed) == 0)
    {
        // Retrieve received messages on socket
        SOPC_Sleep(sleepTimeout);
    }

    // Clear the toolkit configuration and stop toolkit threads
    SOPC_Toolkit_Clear();

    // Check that endpoint closed due to stop signal
    if (SOPC_STATUS_OK == status && stopServer == 0)
    {
        status = SOPC_STATUS_NOK;
    }

    if (SOPC_STATUS_OK == status)
    {
        printf("<Test_Server_Toolkit final result: OK\n");
    }
    else
    {
        printf("<Test_Server_Toolkit final result: NOK with status = '%d'\n", status);
    }

    // Deallocate locally allocated data
    SOPC_AddressSpace_Delete(address_space);

    SOPC_String_Clear(&secuConfig[0].securityPolicy);
    SOPC_String_Clear(&secuConfig[1].securityPolicy);
    SOPC_String_Clear(&secuConfig[2].securityPolicy);

    if (secuActive != false)
    {
        SOPC_KeyManager_SerializedCertificate_Delete(serverCertificate);
        SOPC_KeyManager_SerializedAsymmetricKey_Delete(serverKey);
        SOPC_KeyManager_SerializedCertificate_Delete(authCertificate);
        SOPC_PKIProvider_Free(&pkiProvider);
    }

    SOPC_UserAuthentication_FreeManager(&authenticationManager);
    SOPC_UserAuthorization_FreeManager(&authorizationManager);

    return (status == SOPC_STATUS_OK) ? 0 : 1;
}
