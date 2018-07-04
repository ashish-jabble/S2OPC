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

/** \file
 *
 * \brief This PKIProvider does not verify certificates.
 */

#ifndef SOPC_PKI_PERMISSIVE_H_
#define SOPC_PKI_PERMISSIVE_H_

#include "sopc_pki.h"

/**
 * \brief           Creates a PKI Provider without security.
 *
 * \warning         Using this PKI is considered harmful for the security of the connection.
 *
 * \return          SOPC_STATUS_OK when successful, SOPC_STATUS_INVALID_PARAMETERS when parameters are NULL,
 *                  and SOPC_STATUS_NOK when there was an error.
 */
SOPC_ReturnStatus SOPC_PKIPermissive_Create(SOPC_PKIProvider** ppPKI);

/**
 * \brief           Frees a pki created with SOPC_PKIPermissive_Create().
 *
 * \note            Prefer the use of SOPC_PKIProvider_Free(), which also clears the pointer.
 *
 * \param pPKI      A valid pointer to the pki to free.
 */
void SOPC_PKIPermissive_Free(SOPC_PKIProvider* pPKI);

#endif /* SOPC_PKI_PERMISSIVE_H_ */
