/* Licensed to Systerel under one or more contributor license
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

#ifndef SOPC_COMMON_H_
#define SOPC_COMMON_H_

#include "sopc_enums.h"

#include <inttypes.h>
#include "sopc_log_manager.h"

/**
 * \brief structure containing the file system log configuration
 */
typedef struct SOPC_LogSystem_File_Configuration
{
    const char* logDirPath; /**< path of the log directory */
    uint32_t logMaxBytes;   /**< max bytes per log file */
    uint16_t logMaxFiles;   /**< max number of log files */
} SOPC_LogSystem_File_Configuration;

/**
 * \brief log system discriminant
 */
typedef enum SOPC_Log_System
{
    SOPC_LOG_SYSTEM_FILE /**< file system logger */
} SOPC_Log_System;

/**
 * \brief Configuration of the log system
 */
typedef union SOPC_Log_SystemConfiguration {
    SOPC_LogSystem_File_Configuration fileSystemLogConfig; /**< log file system configuration */
} SOPC_Log_SystemConfiguration;

/**
 * \brief logs configuration
 *
 * the user can choose between multiple log system using the logSystem enumerate
 * and give the proper configuration in the logSysConfig union.
 */
typedef struct SOPC_Log_Configuration
{
    SOPC_Log_Level logLevel;                   /**< default log level */
    SOPC_Log_System logSystem;                 /**< discriminant for the log system configuration */
    SOPC_Log_SystemConfiguration logSysConfig; /**< log system configuration */
} SOPC_Log_Configuration;

/**
 * \brief return Common module initialization status
 *
 * \return initialization status
 */
bool SOPC_Common_IsInitialized(void);

/**
 * \brief Initalizes Common module
 *
 * \param logConfiguration [in] log configuration
 *
 * \return SOPC_STATUS_OK if initialization is a success
 *         SOPC_STATUS_INVALID_PARAMETERS if logConfiguration is invalid
 *         SOPC_STATUS_NOK else
 */
SOPC_ReturnStatus SOPC_Common_Initialize(SOPC_Log_Configuration logConfiguration);

/**
 * \brief clear common library (including logs)
 */
void SOPC_Common_Clear(void);

/**
 * \brief set general log level
 *
 * \param level [in] target log level
 * \return SOPC_STATUS_OK
 */
SOPC_ReturnStatus SOPC_Common_SetLogLevel(SOPC_Log_Level level)

    /**
     * \brief retrieve the default log configuration
     *
     * The configuration can be used directly, or modified as the user desires.
     *
     * \return the default log configuration
     */
    SOPC_Log_Configuration SOPC_Common_GetDefaultLogConfiguration(void)

#endif /* SOPC_COMMON_H_ */
