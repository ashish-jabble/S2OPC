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

#include "sopc_helper_uri.h"

#include <stdint.h>
#include <string.h>

#include "sopc_helper_string.h"
#include "sopc_toolkit_constants.h"

bool SOPC_Helper_URI_ParseTcpUaUri(const char* uri, size_t* hostnameLength, size_t* portIdx, size_t* portLength)
{
    bool result = false;
    size_t idx = 0;
    bool isPort = false;
    bool endOfPort = false;
    bool hasPort = false;
    bool hasName = false;
    bool invalid = false;
    bool startIPv6 = false;
    if (uri != NULL && hostnameLength != NULL && portLength != NULL)
    {
        *hostnameLength = 0;
        *portIdx = 0;
        *portLength = 0;
        if (strlen(uri) + 4 > SOPC_TCP_UA_MAX_URL_LENGTH)
        {
            // Encoded value shall be less than 4096 bytes
        }
        else if (strlen(uri) > 10 && SOPC_strncmp_ignore_case(uri, (const char*) "opc.tcp://", 10) == 0)
        {
            // search for a ':' defining port for given IP
            // search for a '/' defining endpoint name for given IP => at least 1 char after it (len - 1)
            for (idx = 10; idx < strlen(uri) - 1; idx++)
            {
                if (false != isPort && false == endOfPort)
                {
                    if (uri[idx] >= '0' && uri[idx] <= '9')
                    {
                        if (false == hasPort)
                        {
                            // port definition
                            hasPort = true;
                            *portIdx = idx;
                        }
                    }
                    else if (uri[idx] == '/' && false == invalid)
                    {
                        // Name of the endpoint after port, invalid otherwise
                        if (false == hasPort)
                        {
                            invalid = true;
                        }
                        else
                        {
                            *portLength = idx - *portIdx;
                            hasName = true;
                            endOfPort = true; // End of port definition
                        }
                    }
                    else
                    {
                        if (false == hasPort || false == hasName)
                        {
                            // unexpected character: we do not expect a endpoint name
                            invalid = true;
                        }
                    }
                }
                else
                {
                    if (false == endOfPort)
                    {
                        // Treatment before the port parsing
                        if (uri[idx] == ':' && false == startIPv6)
                        {
                            *hostnameLength = idx - 10;
                            isPort = true;
                        }
                        else if (uri[idx] == '[')
                        {
                            startIPv6 = true;
                        }
                        else if (uri[idx] == ']')
                        {
                            if (false == startIPv6)
                            {
                                invalid = true;
                            }
                            else
                            {
                                startIPv6 = false;
                            }
                        }
                    }
                    else if (hasPort)
                    {
                        // Treatment after the port parsing
                        // TODO: check absence of forbidden characters
                    }
                }
            }

            if (hasPort != false && false == invalid)
            {
                result = true;
                if (*portLength == 0)
                {
                    // No endpoint name after port provided
                    *portLength = idx - *portIdx + 1;
                }
            }
        }
    }

    return result;
}
