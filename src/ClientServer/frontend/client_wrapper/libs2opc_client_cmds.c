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

#include "libs2opc_client_cmds.h"
#include "libs2opc_client_cmds_internal_api.h"

#include "sopc_array.h"
#include "sopc_builtintypes.h"
#include "sopc_logger.h"
#include "sopc_macros.h"
#include "sopc_mutexes.h"
#include "sopc_toolkit_config.h"
#include "sopc_types.h"

#define SKIP_S2OPC_DEFINITIONS
#include "libs2opc_client.h"
#include "libs2opc_client_common.h"
#include "sopc_mem_alloc.h"
#include "sopc_types.h"

#include <assert.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Timeout for requests */
#define SYNCHRONOUS_REQUEST_TIMEOUT 10000
/* Max number of simultaneous connections */
#define MAX_SIMULTANEOUS_CONNECTIONS 200
/* Max number of subscribed items per connection */
#define MAX_SUBSCRIBED_ITEMS 200
/* Max BrowseNext requests iteration number */
uint32_t CfgMaxBrowseNextRequests = 50;
/* Max references per node (BrowseRequest) */
uint32_t CfgMaxReferencesPerNode = 0;

/* Connection global timeout */
#define TIMEOUT_MS 10000
/* Secure Channel lifetime */
#define SC_LIFETIME_MS 3600000
/* Default publish period */
#define PUBLISH_PERIOD_MS 500
/* Default max keep alive count */
#define MAX_KEEP_ALIVE_COUNT 3
/* Lifetime Count of subscriptions */
#define MAX_LIFETIME_COUNT 10
/* Number of targeted publish token */
#define PUBLISH_N_TOKEN 3

typedef struct
{
    char* endpoint_url;
    char* policyId;
    char* username;
    char* password;
    char* publish_period_str;
    int64_t publish_period;
    char* token_target_str;
    uint16_t token_target;
    int node_ids_size;
    char** node_ids;
    bool disable_certificate_verification;
    char* n_max_keepalive_str;
    uint32_t n_max_keepalive;
} cmd_line_options_t;

typedef struct
{
    Mutex mutex; /* protect this context */
    Condition condition;

    SOPC_DataValue* values;
    int32_t nbElements;
    SOPC_StatusCode status;
    bool finish;
} ReadContext;

static SOPC_ReturnStatus SOPC_ReadContext_Initialization(ReadContext* ctx)
{
    SOPC_ReturnStatus status = SOPC_STATUS_NOK;
    if (NULL != ctx)
    {
        status = Mutex_Initialization(&ctx->mutex);
        if (SOPC_STATUS_OK == status)
        {
            status = Condition_Init(&ctx->condition);
            ctx->values = NULL;
            ctx->nbElements = 0;
            ctx->status = SOPC_STATUS_NOK;
            ctx->finish = false;
        }
    }
    return status;
}

typedef struct
{
    Mutex mutex; /* protect this context */
    Condition condition;

    SOPC_StatusCode* writeResults;
    int32_t nbElements;
    SOPC_StatusCode status;
    bool finish;
} WriteContext;

static SOPC_ReturnStatus SOPC_WriteContext_Initialization(WriteContext* ctx)
{
    SOPC_ReturnStatus status = SOPC_STATUS_NOK;
    if (NULL != ctx)
    {
        status = Mutex_Initialization(&ctx->mutex);
        if (SOPC_STATUS_OK == status)
        {
            status = Condition_Init(&ctx->condition);
            ctx->writeResults = NULL;
            ctx->nbElements = 0;
            ctx->status = SOPC_STATUS_NOK;
            ctx->finish = false;
        }
    }
    return status;
}

typedef struct
{
    Mutex mutex; /* protect this context */
    Condition condition;

    SOPC_StatusCode* statusCodes;
    SOPC_Array** browseResults;
    SOPC_ByteString** continuationPoints;
    int32_t nbElements;
    SOPC_StatusCode status;
    bool finish;
} BrowseContext;

static SOPC_ReturnStatus SOPC_BrowseContext_Initialization(BrowseContext* ctx)
{
    SOPC_ReturnStatus status = SOPC_STATUS_NOK;
    if (NULL != ctx)
    {
        status = Mutex_Initialization(&ctx->mutex);
        if (SOPC_STATUS_OK == status)
        {
            status = Condition_Init(&ctx->condition);
            ctx->statusCodes = NULL;
            ctx->browseResults = NULL;
            ctx->continuationPoints = NULL;
            ctx->nbElements = 0;
            ctx->status = SOPC_STATUS_NOK;
            ctx->finish = false;
        }
    }
    return status;
}

typedef struct
{
    Mutex mutex; /* protect this context */
    Condition condition;

    int32_t nbElements; /* (input) the number of result elements expected in results received */
    SOPC_ClientHelper_CallMethodResult* results; /* (output) results filled using service results received */

    SOPC_StatusCode status; /* (output) service status or service results treatment status in case of failure */

    bool finish; /* (output) flag indicating the service treatment state */
} CallMethodContext;

static SOPC_ReturnStatus SOPC_CallMethodContext_Initialization(CallMethodContext* ctx)
{
    SOPC_ReturnStatus status = SOPC_STATUS_NOK;
    if (NULL != ctx)
    {
        status = Mutex_Initialization(&ctx->mutex);
        if (SOPC_STATUS_OK == status)
        {
            status = Condition_Init(&ctx->condition);
            ctx->nbElements = 0;
            ctx->results = NULL;
            ctx->finish = false;
        }
    }
    return status;
}

static void SOPC_CallMethodContext_Clear(CallMethodContext* ctx)
{
    SOPC_ReturnStatus statusMutex = Condition_Clear(&ctx->condition);
    assert(SOPC_STATUS_OK == statusMutex);
    statusMutex = Mutex_Clear(&ctx->mutex);
    assert(SOPC_STATUS_OK == statusMutex);
}

typedef struct
{
    Mutex mutex; /* protect this context */
    Condition condition;

    SOPC_ClientHelper_GetEndpointsResult* endpoints;
    SOPC_StatusCode status;
    bool finish;
} GetEndpointsContext;

static SOPC_ReturnStatus SOPC_GetEndpointsContext_Initialization(GetEndpointsContext* ctx)
{
    SOPC_ReturnStatus status = SOPC_STATUS_NOK;
    if (NULL != ctx)
    {
        status = Mutex_Initialization(&ctx->mutex);
        if (SOPC_STATUS_OK == status)
        {
            status = Condition_Init(&ctx->condition);
            ctx->endpoints = NULL;
            ctx->status = SOPC_STATUS_NOK;
            ctx->finish = false;
        }
    }
    return status;
}

/* Callbacks */
static void default_disconnect_callback(const SOPC_LibSub_ConnectionId c_id);
static void SOPC_ClientHelper_GenericCallback(SOPC_LibSub_ConnectionId c_id,
                                              SOPC_LibSub_ApplicativeEvent event,
                                              SOPC_StatusCode status,
                                              const void* response,
                                              uintptr_t responseContext);
/* static functions */

static int32_t ConnectHelper_CreateConfiguration(SOPC_LibSub_ConnectionCfg* cfg_con,
                                                 const char* endpointUrl,
                                                 SOPC_ClientHelper_Security* security,
                                                 OpcUa_GetEndpointsResponse* expectedEndpoints);
static SOPC_ReturnStatus ReadHelper_Initialize(SOPC_ReturnStatus status,
                                               size_t nbElements,
                                               OpcUa_ReadValueId* nodesToRead,
                                               SOPC_ClientHelper_ReadValue* readValues);
static SOPC_ReturnStatus WriteHelper_InitializeValues(size_t nbElements,
                                                      SOPC_ReturnStatus status,
                                                      OpcUa_WriteValue* nodesToWrite,
                                                      SOPC_ClientHelper_WriteValue* writeValues);
static SOPC_ReturnStatus BrowseHelper_InitializeContinuationPoints(size_t nbElements,
                                                                   SOPC_ReturnStatus status,
                                                                   SOPC_ByteString** continuationPointsArray);
static SOPC_ReturnStatus BrowseHelper_InitializeNodesToBrowse(size_t nbElements,
                                                              SOPC_ReturnStatus status,
                                                              OpcUa_BrowseDescription* nodesToBrowse,
                                                              SOPC_ClientHelper_BrowseRequest* browseRequests);
static SOPC_ReturnStatus BrowseHelper_InitializeBrowseResults(size_t nbElements,
                                                              SOPC_ReturnStatus status,
                                                              SOPC_Array** browseResultsListArray);

static void GenericCallback_GetEndpoints(const SOPC_StatusCode requestStatus,
                                         const void* response,
                                         uintptr_t responseContext);
static void GenericCallbackHelper_Read(SOPC_StatusCode status, const void* response, uintptr_t responseContext);
static void GenericCallbackHelper_Write(SOPC_StatusCode status, const void* response, uintptr_t responseContext);
static void GenericCallbackHelper_Browse(SOPC_StatusCode status, const void* response, uintptr_t responseContext);
static void GenericCallbackHelper_BrowseNext(SOPC_StatusCode status, const void* response, uintptr_t responseContext);
static bool ContainsContinuationPoints(SOPC_ByteString** continuationPointsArray, size_t nbElements);
static SOPC_ReturnStatus BrowseNext(int32_t connectionId,
                                    SOPC_StatusCode* statusCodes,
                                    SOPC_Array** browseResultsListArray,
                                    size_t nbElements,
                                    SOPC_ByteString** continuationPoints);
static void GenericCallbackHelper_CallMethod(SOPC_StatusCode status, const void* response, uintptr_t responseContext);

/* Functions */

static void SOPC_ClientHelper_Logger(const SOPC_Log_Level log_level, SOPC_LibSub_CstString text)
{
    switch (log_level)
    {
    case SOPC_LOG_LEVEL_ERROR:
        SOPC_Logger_TraceError(SOPC_LOG_MODULE_CLIENTSERVER, "%s", text);
        break;
    case SOPC_LOG_LEVEL_WARNING:
        SOPC_Logger_TraceWarning(SOPC_LOG_MODULE_CLIENTSERVER, "%s", text);
        break;
    case SOPC_LOG_LEVEL_INFO:
        SOPC_Logger_TraceInfo(SOPC_LOG_MODULE_CLIENTSERVER, "%s", text);
        break;
    case SOPC_LOG_LEVEL_DEBUG:
        SOPC_Logger_TraceDebug(SOPC_LOG_MODULE_CLIENTSERVER, "%s", text);
        break;
    default:
        assert(false);
    }
}

// Return 0 if succeeded
int32_t SOPC_ClientHelper_Initialize(const SOPC_ClientHelper_DisconnectCbk disconnect_callback)
{
    SOPC_LibSub_StaticCfg cfg_cli = {
        .host_log_callback = SOPC_ClientHelper_Logger,
        .disconnect_callback = disconnect_callback != NULL ? disconnect_callback : default_disconnect_callback,
        .toolkit_logger = {
            .level = 0,
            .log_path = NULL,
            .maxBytes = 0,
            .maxFiles = 0}}; // .toolkit_logger only used by LibSub code (libs2opc_client.c), initialize to 0 values.

    SOPC_ReturnStatus status = SOPC_ClientCommon_Initialize(&cfg_cli, GenericCallback_GetEndpoints);

    if (SOPC_STATUS_OK != status)
    {
        Helpers_Log(SOPC_LOG_LEVEL_ERROR, "Could not initialize library.");
        return -2;
    }

    return 0;
}

void SOPC_ClientHelper_Finalize(void)
{
    Helpers_Log(SOPC_LOG_LEVEL_INFO, "Closing the Toolkit.");
    SOPC_ClientCommon_Clear();
    Helpers_Log(SOPC_LOG_LEVEL_INFO, "Toolkit closed.");
}

int32_t SOPC_ClientHelper_GetEndpoints(const char* endpointUrl, SOPC_ClientHelper_GetEndpointsResult** result)
{
    SOPC_ReturnStatus status = SOPC_STATUS_OK;
    SOPC_ReturnStatus statusMutex = SOPC_STATUS_OK;
    GetEndpointsContext* ctx = NULL;
    int32_t res = 0;

    if (NULL == endpointUrl)
    {
        return -1;
    }
    else if (NULL == result)
    {
        return -2;
    }

    /* allocate context */
    if (SOPC_STATUS_OK == status)
    {
        ctx = SOPC_Calloc(1, sizeof(GetEndpointsContext));
        if (NULL == ctx)
        {
            status = SOPC_STATUS_OUT_OF_MEMORY;
        }
    }

    /* Initialize context */
    if (SOPC_STATUS_OK == status)
    {
        SOPC_GetEndpointsContext_Initialization(ctx);
    }

    /* wait for the request result */
    if (SOPC_STATUS_OK == status)
    {
        /* Prepare the synchronous context */
        statusMutex = Mutex_Lock(&ctx->mutex);
        assert(SOPC_STATUS_OK == statusMutex);

        status = SOPC_ClientCommon_AsyncSendGetEndpointsRequest(endpointUrl, (uintptr_t) ctx);

        /* Wait for the response */
        while (SOPC_STATUS_OK == status && !ctx->finish)
        {
            status = Mutex_UnlockAndTimedWaitCond(&ctx->condition, &ctx->mutex, SYNCHRONOUS_REQUEST_TIMEOUT);
            assert(SOPC_STATUS_OK == status || SOPC_STATUS_TIMEOUT == status);
        }
        if (SOPC_STATUS_OK == status)
        {
            status = ctx->status;
        }

        /* Free the context */
        statusMutex = Mutex_Unlock(&ctx->mutex);
        assert(SOPC_STATUS_OK == statusMutex);
        statusMutex = Condition_Clear(&ctx->condition);
        assert(SOPC_STATUS_OK == statusMutex);
        statusMutex = Mutex_Clear(&ctx->mutex);
        assert(SOPC_STATUS_OK == statusMutex);
    }

    if (SOPC_STATUS_OK == status)
    {
        *result = ctx->endpoints;
    }

    /* request was not successful*/
    if (SOPC_STATUS_OK != status)
    {
        res = -100;
    }

    SOPC_Free(ctx);

    return res;
}

void SOPC_ClientHelper_GetEndpointsResult_Free(SOPC_ClientHelper_GetEndpointsResult** getEpResult)
{
    if (NULL == getEpResult || NULL == *getEpResult)
    {
        return;
    }
    SOPC_ClientHelper_GetEndpointsResult* result = *getEpResult;

    if (NULL != result->endpoints)
    {
        for (int32_t i = 0; i < result->nbOfEndpoints; i++)
        {
            SOPC_Free(result->endpoints[i].endpointUrl);
            SOPC_Free(result->endpoints[i].security_policyUri);
            SOPC_Free(result->endpoints[i].serverCertificate);
            SOPC_Free(result->endpoints[i].transportProfileUri);
            if (NULL != result->endpoints[i].userIdentityTokens)
            {
                for (int32_t j = 0; j < result->endpoints[i].nbOfUserIdentityTokens; j++)
                {
                    SOPC_Free(result->endpoints[i].userIdentityTokens[j].policyId);
                    SOPC_Free(result->endpoints[i].userIdentityTokens[j].issuedTokenType);
                    SOPC_Free(result->endpoints[i].userIdentityTokens[j].issuerEndpointUrl);
                    SOPC_Free(result->endpoints[i].userIdentityTokens[j].securityPolicyUri);
                }
                SOPC_Free(result->endpoints[i].userIdentityTokens);
            }
        }
        SOPC_Free(result->endpoints);
    }
    SOPC_Free(result);
    *getEpResult = NULL;
}

static int32_t ConnectHelper_CreateConfiguration(SOPC_LibSub_ConnectionCfg* cfg_con,
                                                 const char* endpointUrl,
                                                 SOPC_ClientHelper_Security* security,
                                                 OpcUa_GetEndpointsResponse* expectedEndpoints)
{
    bool security_none = false;
    const char* cert_auth = security->path_cert_auth;
    const char* ca_crl = security->path_crl;
    const char* cert_srv = security->path_cert_srv;
    const char* cert_cli = security->path_cert_cli;
    const char* key_cli = security->path_key_cli;

    if (NULL == cfg_con)
    {
        return -1;
    }

    switch (security->security_mode)
    {
    case OpcUa_MessageSecurityMode_None:
        if (strncmp(security->security_policy, SOPC_SecurityPolicy_None_URI,
                    strlen(SOPC_SecurityPolicy_None_URI) + 1) != 0)
        {
            return -11;
        }
        security_none = true;
        // Certificates will not be used for SC (but PKI might be used for user encryption)
        cert_cli = NULL;
        key_cli = NULL;
        // Keep server certificate cert_srv since for user encryption need
        // it might be used as trusted issued certificate in PKI and for the encryption
        break;
    case OpcUa_MessageSecurityMode_Sign:
        break;
    case OpcUa_MessageSecurityMode_SignAndEncrypt:
        break;
    default:
        return -12;
    }

    if (!security_none && NULL == cert_srv)
    {
        return -15;
    }

    if (!security_none && NULL == cert_cli)
    {
        return -16;
    }
    if (!security_none && NULL == key_cli)
    {
        return -17;
    }
    if (NULL == security->policyId)
    {
        return -18;
    }
    if (!security_none && (NULL == cert_auth || NULL == ca_crl))
    {
        Helpers_Log(SOPC_LOG_LEVEL_WARNING,
                    "No CA (or mandatory CRL) provided, server certificate will be accepted only if it is self-signed");
    }

    cfg_con->server_url = endpointUrl;
    cfg_con->security_policy = security->security_policy;
    cfg_con->security_mode = security->security_mode;
    cfg_con->disable_certificate_verification = false;
    cfg_con->path_cert_auth = cert_auth;
    cfg_con->path_cert_srv = cert_srv;
    cfg_con->path_cert_cli = cert_cli;
    cfg_con->path_key_cli = key_cli;
    cfg_con->path_crl = ca_crl;
    cfg_con->policyId = security->policyId;
    cfg_con->username = security->username;
    cfg_con->password = security->password;
    cfg_con->publish_period_ms = PUBLISH_PERIOD_MS;
    cfg_con->n_max_keepalive = MAX_KEEP_ALIVE_COUNT;
    cfg_con->n_max_lifetime = MAX_LIFETIME_COUNT;
    cfg_con->data_change_callback = NULL;
    cfg_con->timeout_ms = TIMEOUT_MS;
    cfg_con->sc_lifetime = SC_LIFETIME_MS;
    cfg_con->token_target = PUBLISH_N_TOKEN;
    cfg_con->generic_response_callback = SOPC_ClientHelper_GenericCallback;
    cfg_con->expected_endpoints = expectedEndpoints;

    return 0;
}

// Return configuration Id > 0 if succeeded, -<n> with <n> argument number (starting from 1) if invalid argument
// detected or '-100' if configuration failed
int32_t SOPC_ClientHelper_CreateConfiguration(const char* endpointUrl,
                                              SOPC_ClientHelper_Security* security,
                                              OpcUa_GetEndpointsResponse* expectedEndpoints)
{
    SOPC_ReturnStatus status = SOPC_STATUS_OK;
    if (NULL == endpointUrl)
    {
        return -1;
    }

    if (NULL == security->security_policy)
    {
        return -11;
    }

    SOPC_LibSub_ConnectionCfg cfg_con;
    int32_t res = ConnectHelper_CreateConfiguration(&cfg_con, endpointUrl, security, expectedEndpoints);

    if (0 != res)
    {
        return res;
    }

    SOPC_LibSub_ConfigurationId cfg_id = 0;

    Helpers_Log(SOPC_LOG_LEVEL_INFO, "Configure connection to \"%s\"", cfg_con.server_url);

    status = SOPC_ClientCommon_ConfigureConnection(&cfg_con, &cfg_id);

    if (SOPC_STATUS_OK == status)
    {
        Helpers_Log(SOPC_LOG_LEVEL_INFO, "Configured.");
    }
    else
    {
        Helpers_Log(SOPC_LOG_LEVEL_ERROR, "Could not configure connection.");
        return -100;
    }

    assert(cfg_id > 0);
    assert(cfg_id <= INT32_MAX);
    return (int32_t) cfg_id;
}

// Return connection Id > 0 if succeeded
// or '0' if connection failed
int32_t SOPC_ClientHelper_CreateConnection(int32_t cfg_id)
{
    SOPC_ReturnStatus status = SOPC_STATUS_OK;
    SOPC_LibSub_ConnectionId con_id = 0;

    if (0 >= cfg_id)
    {
        Helpers_Log(SOPC_LOG_LEVEL_ERROR, "Configuration id %d is invalid.", cfg_id);
        return -1;
    }

    status = SOPC_ClientCommon_Connect((SOPC_LibSub_ConfigurationId) cfg_id, &con_id);
    if (SOPC_STATUS_OK == status)
    {
        Helpers_Log(SOPC_LOG_LEVEL_INFO, "Connected.");
    }
    else
    {
        Helpers_Log(SOPC_LOG_LEVEL_ERROR, "Could not connect with given configuration id.");
        return -100;
    }

    assert(con_id > 0);
    assert(con_id <= INT32_MAX);
    return (int32_t) con_id;
}

static void GenericCallback_GetEndpoints(const SOPC_StatusCode requestStatus,
                                         const void* response,
                                         uintptr_t responseContext)
{
    SOPC_StatusCode status = SOPC_STATUS_OK;
    SOPC_StatusCode statusMutex = SOPC_STATUS_OK;

    GetEndpointsContext* ctx = (GetEndpointsContext*) responseContext;
    const OpcUa_GetEndpointsResponse* getEndpointsResp = (const OpcUa_GetEndpointsResponse*) response;
    (void) getEndpointsResp;

    statusMutex = Mutex_Lock(&ctx->mutex);
    assert(SOPC_STATUS_OK == statusMutex);

    if (SOPC_STATUS_OK != requestStatus)
    {
        status = requestStatus;
    }

    /* convert getEndpointsResp to SOPC_ClientHelper_GetEndpointsResult */
    if (SOPC_STATUS_OK == status && 0 != getEndpointsResp->NoOfEndpoints)
    {
        ctx->endpoints = SOPC_Calloc(1, sizeof(SOPC_ClientHelper_GetEndpointsResult));

        if (NULL == ctx->endpoints)
        {
            status = SOPC_STATUS_OUT_OF_MEMORY;
        }

        if (SOPC_STATUS_OK == status)
        {
            ctx->endpoints->nbOfEndpoints = getEndpointsResp->NoOfEndpoints;

            SOPC_ClientHelper_EndpointDescription* endpoints =
                SOPC_Calloc((size_t) getEndpointsResp->NoOfEndpoints, sizeof(SOPC_ClientHelper_EndpointDescription));
            if (NULL == endpoints)
            {
                status = SOPC_STATUS_OUT_OF_MEMORY;
            }

            if (SOPC_STATUS_OK == status)
            {
                ctx->endpoints->endpoints = endpoints;

                OpcUa_EndpointDescription* descriptions = getEndpointsResp->Endpoints;
                for (int32_t i = 0; i < getEndpointsResp->NoOfEndpoints && SOPC_STATUS_OK == status; i++)
                {
                    /* convert OpcUa_EndpointDescription to SOPC_ClientHelper_EndpointDescription */
                    endpoints[i].endpointUrl = SOPC_String_GetCString(&descriptions[i].EndpointUrl);
                    endpoints[i].security_mode = (int32_t) descriptions[i].SecurityMode;
                    endpoints[i].security_policyUri = SOPC_String_GetCString(&descriptions[i].SecurityPolicyUri);
                    endpoints[i].nbOfUserIdentityTokens = descriptions[i].NoOfUserIdentityTokens;
                    endpoints[i].transportProfileUri = SOPC_String_GetCString(&descriptions[i].TransportProfileUri);
                    endpoints[i].securityLevel = descriptions[i].SecurityLevel;
                    endpoints[i].serverCertificateNbBytes = descriptions[i].ServerCertificate.Length;
                    endpoints[i].serverCertificate = descriptions[i].ServerCertificate.Data;
                    // avoid data cleared after callback call
                    descriptions[i].ServerCertificate.DoNotClear = true;

                    SOPC_ClientHelper_UserIdentityToken* userIds = SOPC_Calloc(
                        (size_t) descriptions[i].NoOfUserIdentityTokens, sizeof(SOPC_ClientHelper_UserIdentityToken));
                    if (NULL == userIds)
                    {
                        status = SOPC_STATUS_OUT_OF_MEMORY;
                    }

                    if (SOPC_STATUS_OK == status)
                    {
                        endpoints[i].userIdentityTokens = userIds;

                        OpcUa_UserTokenPolicy* tokensPolicies = descriptions[i].UserIdentityTokens;
                        for (int32_t j = 0; j < descriptions[i].NoOfUserIdentityTokens; j++)
                        {
                            /* convert OpcUa_UserTokenPolicy to SOPC_ClientHelper_UserIdentityToken */
                            userIds[j].policyId = SOPC_String_GetCString(&tokensPolicies[j].PolicyId);
                            userIds[j].tokenType = tokensPolicies[j].TokenType;
                            userIds[j].issuedTokenType = SOPC_String_GetCString(&tokensPolicies[j].IssuedTokenType);
                            userIds[j].issuerEndpointUrl = SOPC_String_GetCString(&tokensPolicies[j].IssuerEndpointUrl);
                            userIds[j].securityPolicyUri = SOPC_String_GetCString(&tokensPolicies[j].SecurityPolicyUri);
                        }
                    }
                }
            }
        }
    }

    /* clean up allocations if an error occured */
    if (SOPC_STATUS_OK != status)
    {
        if (NULL != ctx->endpoints)
        {
            if (NULL != ctx->endpoints->endpoints)
            {
                for (int32_t i = 0; i < ctx->endpoints->nbOfEndpoints; i++)
                {
                    SOPC_Free(ctx->endpoints->endpoints[i].endpointUrl);
                    SOPC_Free(ctx->endpoints->endpoints[i].security_policyUri);
                    SOPC_Free(ctx->endpoints->endpoints[i].transportProfileUri);
                    if (NULL != ctx->endpoints->endpoints[i].userIdentityTokens)
                    {
                        for (int32_t j = 0; j < ctx->endpoints->endpoints[i].nbOfUserIdentityTokens; j++)
                        {
                            SOPC_Free(ctx->endpoints->endpoints[i].userIdentityTokens[j].policyId);
                            SOPC_Free(ctx->endpoints->endpoints[i].userIdentityTokens[j].issuedTokenType);
                            SOPC_Free(ctx->endpoints->endpoints[i].userIdentityTokens[j].issuerEndpointUrl);
                            SOPC_Free(ctx->endpoints->endpoints[i].userIdentityTokens[j].securityPolicyUri);
                        }
                        SOPC_Free(ctx->endpoints->endpoints[i].userIdentityTokens);
                    }
                }
                SOPC_Free(ctx->endpoints->endpoints);
            }
            SOPC_Free(ctx->endpoints);
        }
    }

    /* transmit request status in ctx->status and avoid the status being the mutex and condition results */
    ctx->status = status;
    ctx->finish = true;

    statusMutex = Mutex_Unlock(&ctx->mutex);
    assert(SOPC_STATUS_OK == statusMutex);
    /* Signal that the response is available */
    statusMutex = Condition_SignalAll(&ctx->condition);
    assert(SOPC_STATUS_OK == statusMutex);
}

static void GenericCallbackHelper_Read(SOPC_StatusCode status, const void* response, uintptr_t responseContext)
{
    ReadContext* ctx = (ReadContext*) responseContext;
    const OpcUa_ReadResponse* readResp = (const OpcUa_ReadResponse*) response;

    ctx->status = Mutex_Lock(&ctx->mutex);
    assert(SOPC_STATUS_OK == ctx->status);

    ctx->status = status;
    if (ctx->nbElements != readResp->NoOfResults)
    {
        Helpers_Log(SOPC_LOG_LEVEL_ERROR, "Invalid number of elements in ReadResponse.");
        ctx->status = SOPC_STATUS_NOK;
    }
    if (SOPC_STATUS_OK == ctx->status)
    {
        for (int32_t i = 0; i < readResp->NoOfResults; i++)
        {
            // Move each DataValue from response to request context
            ctx->values[i] = readResp->Results[i];
            // Tag variant as moved: see SOPC_Variant_Move
            readResp->Results[i].Value.DoNotClear = true;
        }
    }
    ctx->finish = true;

    ctx->status = Mutex_Unlock(&ctx->mutex);
    assert(SOPC_STATUS_OK == ctx->status);
    /* Signal that the response is available */
    status = Condition_SignalAll(&ctx->condition);
    assert(SOPC_STATUS_OK == status);
}

static void GenericCallbackHelper_Write(SOPC_StatusCode status, const void* response, uintptr_t responseContext)
{
    WriteContext* ctx = (WriteContext*) responseContext;
    const OpcUa_WriteResponse* writeResp = (const OpcUa_WriteResponse*) response;

    SOPC_ReturnStatus statusMutex = Mutex_Lock(&ctx->mutex);
    assert(SOPC_STATUS_OK == statusMutex);

    ctx->status = status;
    if (ctx->nbElements != writeResp->NoOfResults)
    {
        Helpers_Log(SOPC_LOG_LEVEL_ERROR, "Invalid number of elements in WriteResponse.");
        ctx->status = SOPC_STATUS_NOK;
    }
    if (SOPC_STATUS_OK == ctx->status)
    {
        for (int32_t i = 0; i < writeResp->NoOfResults; i++)
        {
            if (NULL != ctx->writeResults)
            {
                ctx->writeResults[i] = writeResp->Results[i];
            }
            else
            {
                ctx->status = SOPC_STATUS_NOK;
            }
        }
    }
    ctx->finish = true;

    statusMutex = Mutex_Unlock(&ctx->mutex);
    assert(SOPC_STATUS_OK == statusMutex);
    /* Signal that the response is available */
    statusMutex = Condition_SignalAll(&ctx->condition);
    assert(SOPC_STATUS_OK == statusMutex);
}

static void GenericCallbackHelper_Browse(SOPC_StatusCode status, const void* response, uintptr_t responseContext)
{
    BrowseContext* ctx = (BrowseContext*) responseContext;
    const OpcUa_BrowseResponse* browseResp = (const OpcUa_BrowseResponse*) response;

    SOPC_ReturnStatus statusMutex = Mutex_Lock(&ctx->mutex);
    assert(SOPC_STATUS_OK == statusMutex);

    ctx->status = status;
    if (ctx->nbElements != browseResp->NoOfResults)
    {
        Helpers_Log(SOPC_LOG_LEVEL_ERROR, "Invalid number of elements in BrowseResponse.");
        ctx->status = SOPC_STATUS_NOK;
    }
    if (SOPC_STATUS_OK == ctx->status)
    {
        for (int32_t i = 0; i < browseResp->NoOfResults && SOPC_STATUS_OK == ctx->status; i++)
        {
            ctx->statusCodes[i] = browseResp->Results[i].StatusCode;
            ctx->status = SOPC_ByteString_Copy(ctx->continuationPoints[i], &browseResp->Results[i].ContinuationPoint);
            for (int32_t j = 0; j < browseResp->Results[i].NoOfReferences && SOPC_STATUS_OK == ctx->status; j++)
            {
                SOPC_ClientHelper_BrowseResultReference resultReference;
                OpcUa_ReferenceDescription* reference = &browseResp->Results[i].References[j];

                resultReference.referenceTypeId = SOPC_NodeId_ToCString(&reference->ReferenceTypeId);
                if (NULL == resultReference.referenceTypeId)
                {
                    ctx->status = SOPC_STATUS_OUT_OF_MEMORY;
                }
                resultReference.nodeId = SOPC_NodeId_ToCString(&reference->NodeId.NodeId);
                if (NULL == resultReference.nodeId)
                {
                    ctx->status = SOPC_STATUS_OUT_OF_MEMORY;
                }
                resultReference.browseName = SOPC_String_GetCString(&reference->BrowseName.Name);
                if (NULL == resultReference.browseName)
                {
                    ctx->status = SOPC_STATUS_OUT_OF_MEMORY;
                }
                resultReference.displayName = SOPC_String_GetCString(&reference->DisplayName.defaultText);
                if (NULL == resultReference.displayName)
                {
                    ctx->status = SOPC_STATUS_OUT_OF_MEMORY;
                }
                resultReference.isForward = reference->IsForward;
                resultReference.nodeClass = reference->NodeClass;
                bool result = SOPC_Array_Append(ctx->browseResults[i], resultReference);
                if (!result)
                {
                    ctx->status = SOPC_STATUS_OUT_OF_MEMORY;
                }
            }
        }
    }
    ctx->finish = true;

    statusMutex = Mutex_Unlock(&ctx->mutex);
    assert(SOPC_STATUS_OK == statusMutex);
    /* Signal that the response is available */
    statusMutex = Condition_SignalAll(&ctx->condition);
    assert(SOPC_STATUS_OK == statusMutex);
}

static void GenericCallbackHelper_BrowseNext(SOPC_StatusCode status, const void* response, uintptr_t responseContext)
{
    BrowseContext* ctx = (BrowseContext*) responseContext;
    const OpcUa_BrowseNextResponse* browseNextResp = (const OpcUa_BrowseNextResponse*) response;

    SOPC_ReturnStatus statusMutex = Mutex_Lock(&ctx->mutex);
    assert(SOPC_STATUS_OK == statusMutex);

    ctx->status = status;
    if (ctx->nbElements < browseNextResp->NoOfResults)
    {
        Helpers_Log(SOPC_LOG_LEVEL_ERROR, "Invalid number of elements in BrowseNextResponse.");
        ctx->status = SOPC_STATUS_NOK;
    }
    if (SOPC_STATUS_OK == ctx->status)
    {
        int32_t index = 0;
        for (int32_t i = 0; i < browseNextResp->NoOfResults && index < ctx->nbElements; i++)
        {
            /* we look for continuation points */
            /* with every consecutive request, we have the same number or less continuation points */
            /* some browse next requests are done, some need to continue */
            /* for instance if we have 3 request A,B & C, and B is done, */
            /* in the request context, B continuation point will be of length 0 */
            /* the browseNext answer will contain A & C results */
            /* we look for the first non 0 length continuation point in the context to find the index of A (index=0) */
            /* next loop iteration, we do the same and find the index of C (index=2) */
            /* these variable `index` allows us to fill a general context containing all browse requests results */
            bool found = false;
            while (index < ctx->nbElements && !found)
            {
                if (0 < ctx->continuationPoints[index]->Length)
                {
                    found = true;
                }
                else
                {
                    index++;
                }
            }

            ctx->statusCodes[index] = browseNextResp->Results[i].StatusCode;

            SOPC_ByteString_Delete(ctx->continuationPoints[index]);
            ctx->continuationPoints[index] = SOPC_ByteString_Create();
            ctx->status =
                SOPC_ByteString_Copy(ctx->continuationPoints[index], &browseNextResp->Results[i].ContinuationPoint);
            for (int32_t j = 0; j < browseNextResp->Results[i].NoOfReferences && SOPC_STATUS_OK == ctx->status; j++)
            {
                SOPC_ClientHelper_BrowseResultReference resultReference;
                OpcUa_ReferenceDescription* reference = &browseNextResp->Results[i].References[j];

                resultReference.referenceTypeId = SOPC_NodeId_ToCString(&reference->ReferenceTypeId);
                if (NULL == resultReference.referenceTypeId)
                {
                    ctx->status = SOPC_STATUS_OUT_OF_MEMORY;
                }
                resultReference.nodeId = SOPC_NodeId_ToCString(&reference->NodeId.NodeId);
                if (NULL == resultReference.nodeId)
                {
                    ctx->status = SOPC_STATUS_OUT_OF_MEMORY;
                }
                resultReference.browseName = SOPC_String_GetCString(&reference->BrowseName.Name);
                if (NULL == resultReference.browseName)
                {
                    ctx->status = SOPC_STATUS_OUT_OF_MEMORY;
                }
                resultReference.displayName = SOPC_String_GetCString(&reference->DisplayName.defaultText);
                if (NULL == resultReference.displayName)
                {
                    ctx->status = SOPC_STATUS_OUT_OF_MEMORY;
                }
                resultReference.isForward = reference->IsForward;
                resultReference.nodeClass = reference->NodeClass;
                bool result = SOPC_Array_Append(ctx->browseResults[index], resultReference);
                if (!result)
                {
                    ctx->status = SOPC_STATUS_OUT_OF_MEMORY;
                }
            }

            index++;
        }
    }
    ctx->finish = true;

    statusMutex = Mutex_Unlock(&ctx->mutex);
    assert(SOPC_STATUS_OK == statusMutex);
    /* Signal that the response is available */
    status = Condition_SignalAll(&ctx->condition);
}

static void GenericCallbackHelper_CallMethod(SOPC_StatusCode status, const void* response, uintptr_t responseContext)
{
    CallMethodContext* ctx = (CallMethodContext*) responseContext;
    const OpcUa_CallResponse* callRes = response;

    SOPC_ReturnStatus mutStatus = Mutex_Lock(&ctx->mutex);
    assert(SOPC_STATUS_OK == mutStatus);

    SOPC_ReturnStatus localStatus = status;

    if (SOPC_STATUS_OK == localStatus && ctx->nbElements < callRes->NoOfResults)
    {
        Helpers_Log(SOPC_LOG_LEVEL_ERROR, "Invalid number of elements in CallResponse.");
        localStatus = SOPC_STATUS_NOK;
    }

    for (int32_t i = 0; SOPC_STATUS_OK == localStatus && i < ctx->nbElements; i++)
    {
        SOPC_ClientHelper_CallMethodResult* cres = &ctx->results[i];
        OpcUa_CallMethodResult* res = &callRes->Results[i];
        cres->nbOfOutputParams = res->NoOfOutputArguments;
        cres->outputParams = res->OutputArguments;
        cres->status = res->StatusCode;
        // Clear values in original response to avoid deallocation
        res->NoOfOutputArguments = 0;
        res->OutputArguments = NULL;
    }
    ctx->finish = true;

    ctx->status = localStatus;

    mutStatus = Mutex_Unlock(&ctx->mutex);
    assert(SOPC_STATUS_OK == mutStatus);
    /* Signal that the response is available */
    mutStatus = Condition_SignalAll(&ctx->condition);
    assert(SOPC_STATUS_OK == mutStatus);
}

void SOPC_ClientHelper_GenericCallback(SOPC_LibSub_ConnectionId c_id,
                                       SOPC_LibSub_ApplicativeEvent event,
                                       SOPC_StatusCode status,
                                       const void* response,
                                       uintptr_t responseContext)
{
    // unused
    (void) c_id;

    if (SOPC_LibSub_ApplicativeEvent_Response != event)
    {
        return;
    }

    SOPC_EncodeableType* pEncType = *(SOPC_EncodeableType* const*) response;

    if (pEncType == &OpcUa_ReadResponse_EncodeableType)
    {
        GenericCallbackHelper_Read(status, response, responseContext);
    }
    else if (pEncType == &OpcUa_WriteResponse_EncodeableType)
    {
        GenericCallbackHelper_Write(status, response, responseContext);
    }
    else if (pEncType == &OpcUa_BrowseResponse_EncodeableType)
    {
        GenericCallbackHelper_Browse(status, response, responseContext);
    }
    else if (pEncType == &OpcUa_BrowseNextResponse_EncodeableType)
    {
        GenericCallbackHelper_BrowseNext(status, response, responseContext);
    }
    else if (pEncType == &OpcUa_CallResponse_EncodeableType)
    {
        GenericCallbackHelper_CallMethod(status, response, responseContext);
    }
}

static SOPC_ReturnStatus ReadHelper_Initialize(SOPC_ReturnStatus status,
                                               size_t nbElements,
                                               OpcUa_ReadValueId* nodesToRead,
                                               SOPC_ClientHelper_ReadValue* readValues)
{
    if (SOPC_STATUS_OK == status)
    {
        size_t i = 0;
        for (i = 0; i < nbElements && SOPC_STATUS_OK == status; i++)
        {
            OpcUa_ReadValueId_Initialize(&nodesToRead[i]);
            nodesToRead[i].AttributeId = readValues[i].attributeId;
            if (NULL == readValues[i].indexRange)
            {
                nodesToRead[i].IndexRange.Length = 0;
                nodesToRead[i].IndexRange.DoNotClear = true;
                nodesToRead[i].IndexRange.Data = NULL;
            }
            else
            {
                status = SOPC_String_CopyFromCString(&nodesToRead[i].IndexRange, readValues[i].indexRange);
            }
            if (SOPC_STATUS_OK == status)
            {
                // create an instance of NodeId
                SOPC_NodeId* nodeId = SOPC_NodeId_FromCString(readValues[i].nodeId, (int) strlen(readValues[i].nodeId));
                if (NULL == nodeId)
                {
                    status = SOPC_STATUS_OUT_OF_MEMORY;
                }
                if (SOPC_STATUS_OK == status)
                {
                    status = SOPC_NodeId_Copy(&nodesToRead[i].NodeId, nodeId);
                    SOPC_NodeId_Clear(nodeId);
                    SOPC_Free(nodeId);
                }
            }
        }
    }
    return status;
}

int32_t SOPC_ClientHelper_Read(int32_t connectionId,
                               SOPC_ClientHelper_ReadValue* readValues,
                               size_t nbElements,
                               SOPC_DataValue* values)
{
    SOPC_ReturnStatus status = SOPC_STATUS_OK;

    if (connectionId <= 0)
    {
        return -1;
    }
    else if (NULL == readValues || nbElements < 1 || nbElements > INT32_MAX)
    {
        return -2;
    }
    else if (NULL == values)
    {
        return -3;
    }

    for (size_t i = 0; i < nbElements; i++)
    {
        if (NULL == readValues[i].nodeId)
        {
            return -(4 + ((int32_t) i));
        }
    }

    OpcUa_ReadRequest* request = (OpcUa_ReadRequest*) SOPC_Malloc(sizeof(OpcUa_ReadRequest));
    if (NULL == request)
    {
        status = SOPC_STATUS_OUT_OF_MEMORY;
    }

    ReadContext* ctx = (ReadContext*) SOPC_Malloc(sizeof(ReadContext));

    if (NULL == ctx)
    {
        status = SOPC_STATUS_OUT_OF_MEMORY;
    }

    if (SOPC_STATUS_OK == status)
    {
        OpcUa_ReadRequest_Initialize(request);
        request->MaxAge = 0; /* Not manage by S2OPC */
        request->TimestampsToReturn = OpcUa_TimestampsToReturn_Both;
        request->NoOfNodesToRead = (int32_t) nbElements; // check is done before
    }

    /* Set NodesToRead. This is deallocated by toolkit
       when call SOPC_LibSub_AsyncSendRequestOnSession */
    OpcUa_ReadValueId* nodesToRead = SOPC_Calloc((size_t) nbElements, sizeof(OpcUa_ReadValueId));
    if (NULL == nodesToRead)
    {
        status = SOPC_STATUS_OUT_OF_MEMORY;
    }

    status = ReadHelper_Initialize(status, nbElements, nodesToRead, readValues);
    /* set context */
    if (SOPC_STATUS_OK == status)
    {
        status = SOPC_ReadContext_Initialization(ctx);
        if (NULL != ctx && NULL != request)
        {
            ctx->values = values;
            ctx->nbElements = request->NoOfNodesToRead;
            ctx->status = SOPC_STATUS_NOK;
            ctx->finish = false;
        }
        else
        {
            status = SOPC_STATUS_NOK;
        }
    }

    /* send request */
    SOPC_ReturnStatus statusMutex = SOPC_STATUS_OK;
    if (SOPC_STATUS_OK == status)
    {
        request->NodesToRead = nodesToRead;
        /* Prepare the synchronous context */
        statusMutex = Mutex_Lock(&ctx->mutex);
        assert(SOPC_STATUS_OK == statusMutex);

        status = SOPC_ClientCommon_AsyncSendRequestOnSession((SOPC_LibSub_ConnectionId) connectionId, request,
                                                             (uintptr_t) ctx);

        if (SOPC_STATUS_OK == status)
        {
            // Memory now managed by toolkit
            nodesToRead = NULL;
            request = NULL;
        }

        /* Wait for the response */
        while (SOPC_STATUS_OK == status && !ctx->finish)
        {
            status = Mutex_UnlockAndTimedWaitCond(&ctx->condition, &ctx->mutex, SYNCHRONOUS_REQUEST_TIMEOUT);
            assert(SOPC_STATUS_OK == status || SOPC_STATUS_TIMEOUT == status);
        }
        if (SOPC_STATUS_OK == status)
        {
            status = ctx->status;
        }

        /* Free the context */
        statusMutex = Mutex_Unlock(&ctx->mutex);
        assert(SOPC_STATUS_OK == statusMutex);
        statusMutex = Condition_Clear(&ctx->condition);
        assert(SOPC_STATUS_OK == statusMutex);
        statusMutex = Mutex_Clear(&ctx->mutex);
        assert(SOPC_STATUS_OK == statusMutex);
    }

    if (SOPC_STATUS_OK != status)
    {
        SOPC_Free(request);
        if (NULL != nodesToRead)
        {
            for (size_t i = 0; i < nbElements; i++)
            {
                SOPC_NodeId_Clear(&nodesToRead[i].NodeId);
            }
        }
        SOPC_Free(nodesToRead);
    }
    SOPC_Free(ctx);

    if (SOPC_STATUS_OK == status)
    {
        return 0;
    }
    else
    {
        return -100;
    }
}

void SOPC_ClientHelper_ReadResults_Free(size_t nbElements, SOPC_DataValue* values)
{
    if (0 == nbElements || NULL == values)
    {
        return;
    }
    for (size_t i = 0; i < nbElements; i++)
    {
        SOPC_DataValue_Clear(&values[i]);
    }
}

int32_t SOPC_ClientHelper_CreateSubscription(int32_t connectionId, SOPC_ClientHelper_DataChangeCbk callback)
{
    int32_t res = 0;
    SOPC_ReturnStatus status = SOPC_STATUS_OK;

    if (connectionId <= 0)
    {
        return -1;
    }
    else if (NULL == callback)
    {
        return -2;
    }

    status = SOPC_ClientCommon_CreateSubscription((SOPC_LibSub_ConnectionId) connectionId, callback);
    if (SOPC_STATUS_OK != status)
    {
        return -100;
    }

    return res;
}

int32_t SOPC_ClientHelper_AddMonitoredItems(int32_t connectionId,
                                            char** nodeIds,
                                            size_t nbNodeIds,
                                            SOPC_StatusCode* results)
{
    int32_t result = 0;
    OpcUa_CreateMonitoredItemsResponse response;
    SOPC_EncodeableObject_Initialize(&OpcUa_CreateMonitoredItemsResponse_EncodeableType, &response);
    SOPC_ReturnStatus status = SOPC_STATUS_OK;
    if (connectionId <= 0)
    {
        return -1;
    }
    else if (NULL == nodeIds || nbNodeIds <= 0 || nbNodeIds > INT32_MAX)
    {
        return -2;
    }
    else
    {
        for (size_t i = 0; i < nbNodeIds; i++)
        {
            if (NULL == nodeIds[i])
            {
                return -3 - (int32_t) i;
            }
        }
    }

    SOPC_LibSub_AttributeId* lAttrIds = SOPC_Calloc(nbNodeIds, sizeof(SOPC_LibSub_AttributeId));
    if (NULL == lAttrIds)
    {
        status = SOPC_STATUS_OUT_OF_MEMORY;
    }
    else
    {
        for (size_t i = 0; i < nbNodeIds; ++i)
        {
            lAttrIds[i] = SOPC_LibSub_AttributeId_Value;
        }
    }

    SOPC_LibSub_DataId* lDataId = SOPC_Calloc(nbNodeIds, sizeof(SOPC_LibSub_DataId));
    if (NULL == lDataId)
    {
        status = SOPC_STATUS_OUT_OF_MEMORY;
    }

    if (SOPC_STATUS_OK == status)
    {
        status =
            SOPC_ClientCommon_AddToSubscription((SOPC_LibSub_ConnectionId) connectionId, (const char* const*) nodeIds,
                                                lAttrIds, (int32_t) nbNodeIds, lDataId, &response);
    }
    if (SOPC_STATUS_OK == status && response.NoOfResults != (int32_t) nbNodeIds)
    {
        status = SOPC_STATUS_NOK;
    }
    for (size_t i = 0; SOPC_STATUS_OK == status && i < nbNodeIds && ((int32_t) i) < response.NoOfResults; ++i)
    {
        SOPC_StatusCode ResultStatus = response.Results[i].StatusCode;
        if (NULL != results)
        {
            results[i] = ResultStatus;
        }
        if (SOPC_IsGoodStatus(ResultStatus))
        {
            Helpers_Log(SOPC_LOG_LEVEL_INFO, "Created MonIt for \"%s\" with data_id %" PRIu32 ".", nodeIds[i],
                        lDataId[i]);
        }
        else
        {
            // Set result to 1 if one or several MI creation failed
            result = 1;
            Helpers_Log(SOPC_LOG_LEVEL_WARNING, "Failed to create MonIt for \"%s\" with data_id %" PRIu32 ".",
                        nodeIds[i], lDataId[i]);
        }
    }

    SOPC_EncodeableObject_Clear(&OpcUa_CreateMonitoredItemsResponse_EncodeableType, &response);
    SOPC_Free(lAttrIds);
    SOPC_Free(lDataId);

    if (SOPC_STATUS_OK != status)
    {
        Helpers_Log(SOPC_LOG_LEVEL_ERROR, "Could not create monitored items.");
        return -100;
    }

    return result;
}

int32_t SOPC_ClientHelper_Unsubscribe(int32_t connectionId)
{
    SOPC_ReturnStatus status = SOPC_STATUS_OK;

    if (0 >= connectionId)
    {
        return -1;
    }

    status = SOPC_ClientCommon_DeleteSubscription((SOPC_LibSub_ConnectionId) connectionId);

    if (SOPC_STATUS_OK != status)
    {
        return -100;
    }

    return 0;
}

int32_t SOPC_ClientHelper_Disconnect(int32_t connectionId)
{
    if (connectionId <= 0)
    {
        return -1;
    }

    Helpers_Log(SOPC_LOG_LEVEL_INFO, "Closing the connection %" PRIi32, connectionId);
    SOPC_ReturnStatus status = SOPC_ClientCommon_Disconnect((SOPC_LibSub_ConnectionId) connectionId);

    if (SOPC_STATUS_INVALID_STATE == status)
    {
        /* toolkit not initialized */
        return -2;
    }
    else if (SOPC_STATUS_INVALID_PARAMETERS == status || SOPC_STATUS_NOK == status)
    {
        /* connection already closed or not existing */
        return -3;
    }
    else if (SOPC_STATUS_OK != status)
    {
        /* operation failed */
        return -100;
    }

    return 0;
}

static void default_disconnect_callback(const uint32_t c_id)
{
    Helpers_Log(SOPC_LOG_LEVEL_INFO, "Client %" PRIu32 " disconnected.", c_id);
}

static SOPC_ReturnStatus WriteHelper_InitializeValues(size_t nbElements,
                                                      SOPC_ReturnStatus status,
                                                      OpcUa_WriteValue* nodesToWrite,
                                                      SOPC_ClientHelper_WriteValue* writeValues)
{
    for (size_t i = 0; i < nbElements && SOPC_STATUS_OK == status; i++)
    {
        OpcUa_WriteValue_Initialize(&nodesToWrite[i]);
        nodesToWrite[i].AttributeId = 13; // Value AttributeId
        status = SOPC_DataValue_Copy(&nodesToWrite[i].Value, writeValues[i].value);
        if (SOPC_STATUS_OK == status)
        {
            if (NULL == writeValues[i].indexRange)
            {
                nodesToWrite[i].IndexRange.Length = 0;
                nodesToWrite[i].IndexRange.DoNotClear = true;
                nodesToWrite[i].IndexRange.Data = NULL;
            }
            else
            {
                status = SOPC_String_CopyFromCString(&nodesToWrite[i].IndexRange, writeValues[i].indexRange);
            }
            if (SOPC_STATUS_OK == status)
            {
                // create an instance of NodeId
                SOPC_NodeId* nodeId =
                    SOPC_NodeId_FromCString(writeValues[i].nodeId, (int) strlen(writeValues[i].nodeId));
                if (NULL == nodeId)
                {
                    Helpers_Log(SOPC_LOG_LEVEL_INFO, "nodeId NULL");
                }
                status = SOPC_NodeId_Copy(&nodesToWrite[i].NodeId, nodeId);
                SOPC_NodeId_Clear(nodeId);
                SOPC_Free(nodeId);
            }
        }
    }
    return status;
}

int32_t SOPC_ClientHelper_Write(int32_t connectionId,
                                SOPC_ClientHelper_WriteValue* writeValues,
                                size_t nbElements,
                                SOPC_StatusCode* writeResults)
{
    SOPC_ReturnStatus status = SOPC_STATUS_OK;
    if (connectionId <= 0)
    {
        return -1;
    }
    if (NULL == writeValues || nbElements < 1 || nbElements > INT32_MAX)
    {
        return -2;
    }
    if (NULL == writeResults)
    {
        return -3;
    }
    OpcUa_WriteRequest* request = (OpcUa_WriteRequest*) SOPC_Malloc(sizeof(OpcUa_WriteRequest));
    if (NULL == request)
    {
        status = SOPC_STATUS_OUT_OF_MEMORY;
    }
    WriteContext* ctx = (WriteContext*) SOPC_Malloc(sizeof(WriteContext));
    if (NULL == ctx)
    {
        status = SOPC_STATUS_OUT_OF_MEMORY;
    }

    if (SOPC_STATUS_OK == status)
    {
        OpcUa_WriteRequest_Initialize(request);
        request->NoOfNodesToWrite = (int32_t) nbElements;
    }

    OpcUa_WriteValue* nodesToWrite = SOPC_Calloc(nbElements, sizeof(OpcUa_WriteValue));
    if (NULL == nodesToWrite)
    {
        status = SOPC_STATUS_OUT_OF_MEMORY;
    }

    if (SOPC_STATUS_OK == status)
    {
        status = WriteHelper_InitializeValues(nbElements, status, nodesToWrite, writeValues);
    }

    if (SOPC_STATUS_OK == status)
    {
        status = SOPC_WriteContext_Initialization(ctx);
    }

    if (SOPC_STATUS_OK == status)
    {
        /* Set context */
        ctx->nbElements = request->NoOfNodesToWrite;
        ctx->writeResults =
            (SOPC_StatusCode*) SOPC_Malloc(sizeof(SOPC_StatusCode) * (size_t) request->NoOfNodesToWrite);
        if (NULL == ctx->writeResults)
        {
            status = SOPC_STATUS_OUT_OF_MEMORY;
        }
        ctx->status = SOPC_STATUS_NOK;
        ctx->finish = false;
    }

    if (SOPC_STATUS_OK == status)
    {
        request->NodesToWrite = nodesToWrite;
        /* Prepare the synchronous context */
        SOPC_ReturnStatus statusMutex = Mutex_Lock(&ctx->mutex);
        assert(SOPC_STATUS_OK == statusMutex);

        status = SOPC_ClientCommon_AsyncSendRequestOnSession((SOPC_LibSub_ConnectionId) connectionId, request,
                                                             (uintptr_t) ctx);

        if (SOPC_STATUS_OK == status)
        {
            // Memory now managed by toolkit
            nodesToWrite = NULL;
            request = NULL;
        }

        /* Wait for the response */
        while (SOPC_STATUS_OK == status && !ctx->finish)
        {
            status = Mutex_UnlockAndTimedWaitCond(&ctx->condition, &ctx->mutex, SYNCHRONOUS_REQUEST_TIMEOUT);
            assert(SOPC_STATUS_OK == status || SOPC_STATUS_TIMEOUT == status);
        }
        if (SOPC_STATUS_OK == status)
        {
            status = ctx->status;
        }

        /* fill write results */
        for (int i = 0; i < ctx->nbElements; i++)
        {
            writeResults[i] = ctx->writeResults[i];
        }

        /* Free the context */
        statusMutex = Mutex_Unlock(&ctx->mutex);
        assert(SOPC_STATUS_OK == statusMutex);
        statusMutex = Condition_Clear(&ctx->condition);
        assert(SOPC_STATUS_OK == statusMutex);
        statusMutex = Mutex_Clear(&ctx->mutex);
        assert(SOPC_STATUS_OK == statusMutex);
        SOPC_Free(ctx->writeResults);
    }

    if (SOPC_STATUS_OK != status)
    {
        SOPC_Free(request);
        SOPC_Free(nodesToWrite);
    }
    SOPC_Free(ctx);

    if (SOPC_STATUS_OK == status)
    {
        return 0;
    }
    else
    {
        return -100;
    }
}

static SOPC_ReturnStatus BrowseHelper_InitializeContinuationPoints(size_t nbElements,
                                                                   SOPC_ReturnStatus status,
                                                                   SOPC_ByteString** continuationPointsArray)
{
    size_t i = 0;

    if (NULL == continuationPointsArray)
    {
        status = SOPC_STATUS_INVALID_PARAMETERS;
    }

    for (; i < nbElements && SOPC_STATUS_OK == status; i++)
    {
        continuationPointsArray[i] = SOPC_ByteString_Create();
        if (NULL == continuationPointsArray[i])
        {
            status = SOPC_STATUS_OUT_OF_MEMORY;
        }
    }
    if (SOPC_STATUS_OK != status)
    {
        for (size_t j = 0; j < i; j++)
        {
            SOPC_ByteString_Delete(continuationPointsArray[j]);
            continuationPointsArray[j] = NULL;
        }
    }
    return status;
}

static SOPC_ReturnStatus BrowseHelper_InitializeNodesToBrowse(size_t nbElements,
                                                              SOPC_ReturnStatus status,
                                                              OpcUa_BrowseDescription* nodesToBrowse,
                                                              SOPC_ClientHelper_BrowseRequest* browseRequests)
{
    for (size_t i = 0; i < nbElements && SOPC_STATUS_OK == status; i++)
    {
        OpcUa_BrowseDescription_Initialize(&nodesToBrowse[i]);
        // create an instance of NodeId
        SOPC_NodeId* nodeId = SOPC_NodeId_FromCString(browseRequests[i].nodeId, (int) strlen(browseRequests[i].nodeId));
        if (NULL == nodeId)
        {
            Helpers_Log(SOPC_LOG_LEVEL_INFO, "nodeId NULL");
        }
        status = SOPC_NodeId_Copy(&nodesToBrowse[i].NodeId, nodeId);
        SOPC_NodeId_Clear(nodeId);
        SOPC_Free(nodeId);
        if (SOPC_STATUS_OK == status)
        {
            // create an instance of NodeId
            SOPC_NodeId* refNodeId = SOPC_NodeId_FromCString(browseRequests[i].referenceTypeId,
                                                             (int) strlen(browseRequests[i].referenceTypeId));
            if (NULL == refNodeId)
            {
                Helpers_Log(SOPC_LOG_LEVEL_INFO, "refNodeId NULL");
            }
            else
            {
                status = SOPC_NodeId_Copy(&nodesToBrowse[i].ReferenceTypeId, refNodeId);
                SOPC_NodeId_Clear(refNodeId);
                SOPC_Free(refNodeId);
            }
        }
        if (SOPC_STATUS_OK == status)
        {
            nodesToBrowse[i].BrowseDirection = (OpcUa_BrowseDirection) browseRequests[i].direction;
            nodesToBrowse[i].IncludeSubtypes = browseRequests[i].includeSubtypes;
            nodesToBrowse[i].NodeClassMask = 0; // All NodeClasses are returned
            nodesToBrowse[i].ResultMask = 0x3f; // All fields returned
        }
    }
    return status;
}

static SOPC_ReturnStatus BrowseHelper_InitializeBrowseResults(size_t nbElements,
                                                              SOPC_ReturnStatus status,
                                                              SOPC_Array** browseResultsListArray)
{
    size_t i = 0;

    if (NULL == browseResultsListArray)
    {
        status = SOPC_STATUS_INVALID_PARAMETERS;
    }

    for (; i < nbElements && SOPC_STATUS_OK == status; i++)
    {
        browseResultsListArray[i] = SOPC_Array_Create(sizeof(SOPC_ClientHelper_BrowseResultReference), 10, SOPC_Free);
        if (NULL == browseResultsListArray[i])
        {
            status = SOPC_STATUS_OUT_OF_MEMORY;
        }
    }
    if (SOPC_STATUS_OK != status)
    {
        for (size_t j = 0; j < i; j++)
        {
            SOPC_Array_Delete(browseResultsListArray[j]);
            browseResultsListArray[j] = NULL;
        }
    }
    return status;
}

void SOPC_ClientHelper_BrowseResultReference_Move(SOPC_ClientHelper_BrowseResultReference* dest,
                                                  SOPC_ClientHelper_BrowseResultReference* src)
{
    if (NULL == dest || NULL == src)
    {
        return;
    }

    *dest = *src;
    memset(src, 0, sizeof(SOPC_ClientHelper_BrowseResultReference));
}

void SOPC_ClientHelper_BrowseResultReference_Clear(SOPC_ClientHelper_BrowseResultReference* brr)
{
    if (NULL == brr)
    {
        return;
    }
    SOPC_Free(brr->browseName);
    SOPC_Free(brr->displayName);
    SOPC_Free(brr->nodeId);
    SOPC_Free(brr->referenceTypeId);
    memset(brr, 0, sizeof(SOPC_ClientHelper_BrowseResultReference));
}

static void SOPC_ClientHelper_BrowseResult_Clear(SOPC_ClientHelper_BrowseResult* br)
{
    if (NULL == br)
    {
        return;
    }
    for (int32_t i = 0; i < br->nbOfReferences; i++)
    {
        SOPC_ClientHelper_BrowseResultReference_Clear(&br->references[i]);
    }
    SOPC_Free(br->references);
}

void SOPC_ClientHelper_BrowseResults_Clear(size_t nbElements, SOPC_ClientHelper_BrowseResult* results)
{
    if (NULL == results || 0 == nbElements)
    {
        return;
    }
    for (size_t i = 0; i < nbElements; i++)
    {
        SOPC_ClientHelper_BrowseResult_Clear(&results[i]);
    }
}

int32_t SOPC_ClientHelper_Browse(int32_t connectionId,
                                 SOPC_ClientHelper_BrowseRequest* browseRequests,
                                 size_t nbElements,
                                 SOPC_ClientHelper_BrowseResult* browseResults)
{
    SOPC_ReturnStatus status = SOPC_STATUS_OK;
    if (connectionId <= 0)
    {
        return -1;
    }
    if (NULL == browseRequests || nbElements < 1 || nbElements > INT32_MAX)
    {
        return -2;
    }
    if (NULL == browseResults)
    {
        return -3;
    }
    OpcUa_BrowseRequest* request = (OpcUa_BrowseRequest*) SOPC_Calloc(1, sizeof(OpcUa_BrowseRequest));
    if (NULL == request)
    {
        status = SOPC_STATUS_OUT_OF_MEMORY;
    }

    BrowseContext* ctx = (BrowseContext*) SOPC_Calloc(1, sizeof(BrowseContext));
    if (NULL == ctx)
    {
        status = SOPC_STATUS_OUT_OF_MEMORY;
    }

    /* Convert browseRequests to an actual request */
    OpcUa_BrowseDescription* nodesToBrowse = SOPC_Calloc(nbElements, sizeof(OpcUa_BrowseDescription));
    if (NULL == nodesToBrowse)
    {
        status = SOPC_STATUS_OUT_OF_MEMORY;
    }

    /* Create array of singly linked lists, containing browse results */
    SOPC_Array** browseResultsListArray = (SOPC_Array**) SOPC_Calloc(nbElements, sizeof(SOPC_Array*));
    if (NULL == browseResultsListArray)
    {
        status = SOPC_STATUS_OUT_OF_MEMORY;
    }

    /* create an array of status code to store request results */
    SOPC_StatusCode* statusCodes = SOPC_Calloc(nbElements, sizeof(SOPC_StatusCode));
    if (NULL == statusCodes)
    {
        status = SOPC_STATUS_OUT_OF_MEMORY;
    }

    if (SOPC_STATUS_OK == status)
    {
        status = BrowseHelper_InitializeBrowseResults(nbElements, status, browseResultsListArray);
    }

    /* create array of continuationPoints */
    SOPC_ByteString** continuationPointsArray = (SOPC_ByteString**) SOPC_Calloc(nbElements, sizeof(SOPC_ByteString*));
    if (NULL == continuationPointsArray)
    {
        status = SOPC_STATUS_OUT_OF_MEMORY;
    }

    if (SOPC_STATUS_OK == status)
    {
        status = BrowseHelper_InitializeContinuationPoints(nbElements, status, continuationPointsArray);
    }

    if (SOPC_STATUS_OK == status)
    {
        status = BrowseHelper_InitializeNodesToBrowse(nbElements, status, nodesToBrowse, browseRequests);
    }

    if (SOPC_STATUS_OK == status)
    {
        OpcUa_BrowseRequest_Initialize(request);
        OpcUa_ViewDescription_Initialize(&request->View);
        request->NodesToBrowse = nodesToBrowse;
        request->NoOfNodesToBrowse = (int32_t) nbElements;
        request->RequestedMaxReferencesPerNode = CfgMaxReferencesPerNode;
    }

    /* Create context */
    if (SOPC_STATUS_OK == status)
    {
        status = SOPC_BrowseContext_Initialization(ctx);
    }

    /* fill context */
    if (SOPC_STATUS_OK == status)
    {
        ctx->nbElements = (int32_t) nbElements;
        ctx->statusCodes = statusCodes;
        ctx->browseResults = browseResultsListArray;
        ctx->continuationPoints = continuationPointsArray;
    }

    /* Send Browse Request */
    if (SOPC_STATUS_OK == status)
    {
        /* Prepare the synchronous context */
        SOPC_ReturnStatus statusMutex = Mutex_Lock(&ctx->mutex);
        assert(SOPC_STATUS_OK == statusMutex);

        status = SOPC_ClientCommon_AsyncSendRequestOnSession((SOPC_LibSub_ConnectionId) connectionId, request,
                                                             (uintptr_t) ctx);

        if (SOPC_STATUS_OK == status)
        {
            // Memory now managed by toolkit
            nodesToBrowse = NULL;
            request = NULL;
        }

        /* Wait for the response */
        while (SOPC_STATUS_OK == status && !ctx->finish)
        {
            status = Mutex_UnlockAndTimedWaitCond(&ctx->condition, &ctx->mutex, SYNCHRONOUS_REQUEST_TIMEOUT);
            assert(SOPC_STATUS_OK == status || SOPC_STATUS_TIMEOUT == status);
        }
        if (SOPC_STATUS_OK == status)
        {
            status = ctx->status;
        }

        /* Free the context */
        statusMutex = Mutex_Unlock(&ctx->mutex);
        assert(SOPC_STATUS_OK == statusMutex);
        statusMutex = Condition_Clear(&ctx->condition);
        assert(SOPC_STATUS_OK == statusMutex);
        statusMutex = Mutex_Clear(&ctx->mutex);
        assert(SOPC_STATUS_OK == statusMutex);
    }

    /* Send Browse Next Request until there are no continuation or limit is reached */
    bool containsContinuationPoint = ContainsContinuationPoints(continuationPointsArray, nbElements);
    bool maxRequestNumberReached = false;

    uint32_t j = 0;
    for (; j < CfgMaxBrowseNextRequests && containsContinuationPoint && SOPC_STATUS_OK == status; j++)
    {
        status = BrowseNext(connectionId, statusCodes, browseResultsListArray, nbElements, continuationPointsArray);
        containsContinuationPoint = ContainsContinuationPoints(continuationPointsArray, nbElements);
    }
    if (j >= CfgMaxBrowseNextRequests && ContainsContinuationPoints(continuationPointsArray, nbElements))
    {
        status = SOPC_STATUS_NOK;
        maxRequestNumberReached = true;
    }

    /* process browseResultsListArray */
    if (SOPC_STATUS_OK == status)
    {
        for (size_t i = 0; i < nbElements; i++)
        {
            browseResults[i].statusCode = statusCodes[i];
            browseResults[i].nbOfReferences = (int32_t) SOPC_Array_Size(browseResultsListArray[i]);
            browseResults[i].references = SOPC_Array_Into_Raw(browseResultsListArray[i]);
        }
    }
    else if (NULL != browseResultsListArray)
    {
        for (size_t i = 0; i < nbElements; i++)
        {
            SOPC_Array_Delete(browseResultsListArray[i]);
        }
    }

    /* Free memory */
    if (SOPC_STATUS_OK != status)
    {
        SOPC_Free(nodesToBrowse);
        SOPC_Free(request);
    }
    SOPC_Free(statusCodes);
    SOPC_Free(browseResultsListArray);

    if (NULL != continuationPointsArray)
    {
        for (size_t i = 0; i < nbElements; i++)
        {
            SOPC_ByteString_Delete(continuationPointsArray[i]);
        }
    }
    SOPC_Free(continuationPointsArray);

    SOPC_Free(ctx);

    if (true == maxRequestNumberReached)
    {
        return -4;
    }
    if (SOPC_STATUS_OK == status)
    {
        return 0;
    }
    else
    {
        return -100;
    }
}

static SOPC_ReturnStatus BrowseNext(int32_t connectionId,
                                    SOPC_StatusCode* statusCodes,
                                    SOPC_Array** browseResultsListArray,
                                    size_t nbElements,
                                    SOPC_ByteString** continuationPoints)
{
    SOPC_ReturnStatus status = SOPC_STATUS_OK;

    if (0 > connectionId || NULL == statusCodes || NULL == browseResultsListArray || 1 > nbElements ||
        NULL == continuationPoints)
    {
        return SOPC_STATUS_INVALID_PARAMETERS;
    }

    /* allocate memory */
    OpcUa_BrowseNextRequest* request = (OpcUa_BrowseNextRequest*) SOPC_Calloc(1, sizeof(OpcUa_BrowseNextRequest));
    if (NULL == request)
    {
        status = SOPC_STATUS_OUT_OF_MEMORY;
    }

    BrowseContext* ctx = (BrowseContext*) SOPC_Calloc(1, sizeof(BrowseContext));
    if (NULL == ctx)
    {
        status = SOPC_STATUS_OUT_OF_MEMORY;
    }

    SOPC_ByteString* nextContinuationPoints = NULL;

    if (SOPC_STATUS_OK == status)
    {
        nextContinuationPoints = SOPC_Calloc(nbElements, sizeof(SOPC_ByteString));

        if (NULL == nextContinuationPoints)
        {
            status = SOPC_STATUS_OUT_OF_MEMORY;
        }
    }

    int32_t count = 0;
    if (SOPC_STATUS_OK == status)
    {
        for (int32_t i = 0; i < (int32_t) nbElements; i++)
        {
            if (NULL != continuationPoints[i] && 0 < continuationPoints[i]->Length)
            {
                SOPC_ByteString_Copy(&nextContinuationPoints[count], continuationPoints[i]);
                count++;
            }
        }
    }

    /* craft request */
    if (SOPC_STATUS_OK == status)
    {
        OpcUa_BrowseNextRequest_Initialize(request);
        request->ReleaseContinuationPoints = false;
        request->NoOfContinuationPoints = count;
        request->ContinuationPoints = nextContinuationPoints;
    }

    /* create context */
    if (SOPC_STATUS_OK == status)
    {
        status = SOPC_BrowseContext_Initialization(ctx);
    }

    /* fill context */
    if (SOPC_STATUS_OK == status)
    {
        ctx->nbElements = (int32_t) nbElements;
        ctx->statusCodes = statusCodes;
        ctx->browseResults = browseResultsListArray;
        ctx->continuationPoints = continuationPoints;
    }
    /* send request */
    if (SOPC_STATUS_OK == status)
    {
        /* Prepare the synchronous context */
        SOPC_ReturnStatus statusMutex = Mutex_Lock(&ctx->mutex);
        assert(SOPC_STATUS_OK == statusMutex);

        status = SOPC_ClientCommon_AsyncSendRequestOnSession((SOPC_LibSub_ConnectionId) connectionId, request,
                                                             (uintptr_t) ctx);

        if (SOPC_STATUS_OK == status)
        {
            // Memory now managed by toolkit
            nextContinuationPoints = NULL;
            request = NULL;
        }

        /* Wait for the response */
        while (SOPC_STATUS_OK == status && !ctx->finish)
        {
            status = Mutex_UnlockAndTimedWaitCond(&ctx->condition, &ctx->mutex, SYNCHRONOUS_REQUEST_TIMEOUT);
            assert(SOPC_STATUS_OK == status || SOPC_STATUS_TIMEOUT == status);
        }
        if (SOPC_STATUS_OK == status)
        {
            status = ctx->status;
        }

        /* Free the context */
        statusMutex = Mutex_Unlock(&ctx->mutex);
        assert(SOPC_STATUS_OK == statusMutex);
        statusMutex = Condition_Clear(&ctx->condition);
        assert(SOPC_STATUS_OK == statusMutex);
        statusMutex = Mutex_Clear(&ctx->mutex);
        assert(SOPC_STATUS_OK == statusMutex);
    }

    /* if status == SOPC_STATUS_OK, another thread has ownership of the memory
     * else we need to free it */
    if (SOPC_STATUS_OK != status)
    {
        for (int32_t i = 0; i < count && NULL != nextContinuationPoints; i++)
        {
            SOPC_ByteString_Clear(&nextContinuationPoints[i]);
        }
        SOPC_Free(nextContinuationPoints);
        SOPC_Free(request);
    }

    /* free memory */
    SOPC_Free(ctx);

    return status;
}

static bool ContainsContinuationPoints(SOPC_ByteString** continuationPointsArray, size_t nbElements)
{
    bool result = false;

    if (NULL == continuationPointsArray)
    {
        return false;
    }

    for (size_t i = 0; i < nbElements && !result; i++)
    {
        result = result || (NULL != continuationPointsArray[i] && 0 < continuationPointsArray[i]->Length);
    }

    return result;
}

int32_t SOPC_ClientHelper_CallMethod(int32_t connectionId,
                                     SOPC_ClientHelper_CallMethodRequest* callRequests,
                                     size_t nbOfElements,
                                     SOPC_ClientHelper_CallMethodResult* callResults)
{
    SOPC_ReturnStatus status = SOPC_STATUS_OK;

    if (0 > connectionId)
    {
        return -1;
    }

    if (nbOfElements <= 0 || nbOfElements > INT32_MAX || NULL == callRequests || NULL == callResults)
    {
        return -2;
    }

    CallMethodContext ctx;
    OpcUa_CallRequest* callReqs = SOPC_Malloc(sizeof(OpcUa_CallRequest));
    OpcUa_CallMethodRequest* lrs = SOPC_Calloc(nbOfElements, sizeof(OpcUa_CallMethodRequest));
    if (NULL == callReqs || NULL == lrs)
    {
        SOPC_Free(callReqs);
        SOPC_Free(lrs);
        return -3;
    }

    OpcUa_CallRequest_Initialize(callReqs);
    callReqs->MethodsToCall = lrs;
    callReqs->NoOfMethodsToCall = (int32_t) nbOfElements;

    for (size_t i = 0; SOPC_STATUS_OK == status && i < nbOfElements; i++)
    {
        SOPC_ClientHelper_CallMethodRequest* creq = &callRequests[i];
        OpcUa_CallMethodRequest* req = &lrs[i];

        OpcUa_CallMethodRequest_Initialize(req);

        if (NULL == creq->objectNodeId || NULL == creq->methodNodeId || creq->nbOfInputParams < 0 ||
            (creq->nbOfInputParams > 0 && NULL == creq->inputParams))
        {
            status = SOPC_STATUS_INVALID_PARAMETERS;
        }

        if (SOPC_STATUS_OK == status)
        {
            status = SOPC_NodeId_InitializeFromCString(&req->ObjectId, creq->objectNodeId,
                                                       (int32_t) strlen(creq->objectNodeId));
        }
        if (SOPC_STATUS_OK == status)
        {
            status = SOPC_NodeId_InitializeFromCString(&req->MethodId, creq->methodNodeId,
                                                       (int32_t) strlen(creq->methodNodeId));
        }

        if (SOPC_STATUS_OK == status)
        {
            req->InputArguments = SOPC_Calloc((size_t) creq->nbOfInputParams, sizeof(SOPC_Variant));
            if (NULL == req->InputArguments)
            {
                status = SOPC_STATUS_OUT_OF_MEMORY;
            }
            else
            {
                for (int32_t j = 0; j < creq->nbOfInputParams; j++)
                {
                    SOPC_Variant_Move(&req->InputArguments[j], &creq->inputParams[j]);
                }
                req->NoOfInputArguments = creq->nbOfInputParams;
                // Free input param content
                SOPC_Free(creq->inputParams);
                creq->inputParams = NULL;
                creq->nbOfInputParams = 0;
            }
        }
    }

    if (SOPC_STATUS_OK == status)
    {
        SOPC_CallMethodContext_Initialization(&ctx);
        ctx.nbElements = (int32_t) nbOfElements;
        ctx.results = callResults;
    }

    /* wait for the request result */
    if (SOPC_STATUS_OK == status)
    {
        /* Prepare the synchronous context */
        SOPC_ReturnStatus statusMutex = Mutex_Lock(&ctx.mutex);
        assert(SOPC_STATUS_OK == statusMutex);

        status = SOPC_ClientCommon_AsyncSendRequestOnSession((SOPC_LibSub_ConnectionId) connectionId, (void*) callReqs,
                                                             (uintptr_t) &ctx);

        if (SOPC_STATUS_OK == status)
        {
            // Memory now managed by toolkit
            callReqs = NULL;
        }

        /* Wait for the response */
        while (SOPC_STATUS_OK == status && !ctx.finish)
        {
            status = Mutex_UnlockAndTimedWaitCond(&ctx.condition, &ctx.mutex, SYNCHRONOUS_REQUEST_TIMEOUT);
            assert(SOPC_STATUS_OK == status || SOPC_STATUS_TIMEOUT == status);
        }
        // Note: ctx.status and generical callback signature should be a ReturnStatus and not a StatusCode
        if (SOPC_STATUS_OK == status)
        {
            status = ctx.status;
        }

        /* Clear the context */
        statusMutex = Mutex_Unlock(&ctx.mutex);
        assert(SOPC_STATUS_OK == statusMutex);
        SOPC_CallMethodContext_Clear(&ctx);
    }

    if (SOPC_STATUS_OK != status)
    {
        OpcUa_CallRequest_Clear(callReqs);
        SOPC_Free(callReqs);
        return -100;
    }

    return 0;
}

void SOPC_ClientHelper_CallMethodResults_Clear(size_t nbElements, SOPC_ClientHelper_CallMethodResult* results)
{
    if (0 == nbElements || NULL == results)
    {
        return;
    }
    for (size_t i = 0; i < nbElements; i++)
    {
        for (int32_t j = 0; j < results[i].nbOfOutputParams && NULL != results[i].outputParams; j++)
        {
            SOPC_Variant_Clear(&results[i].outputParams[j]);
        }
        SOPC_Free(results[i].outputParams);
        memset(&results[i], 0, sizeof(SOPC_ClientHelper_CallMethodResult));
    }
}

SOPC_ReturnStatus SOPC_ClientHelper_SetLocaleIds(size_t nbLocales, char** localeIds)
{
    return SOPC_ClientCommon_SetLocaleIds(nbLocales, localeIds);
}

SOPC_ReturnStatus SOPC_ClientHelper_SetApplicationDescription(const char* applicationUri,
                                                              const char* productUri,
                                                              const char* defaultAppName,
                                                              const char* defaultAppNameLocale,
                                                              OpcUa_ApplicationType applicationType)
{
    return SOPC_ClientCommon_SetApplicationDescription(applicationUri, productUri, defaultAppName, defaultAppNameLocale,
                                                       applicationType);
}
