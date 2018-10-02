#!/bin/bash

# Licensed to Systerel under one or more contributor license
# agreements. See the NOTICE file distributed with this work
# for additional information regarding copyright ownership.
# Systerel licenses this file to you under the Apache
# License, Version 2.0 (the "License"); you may not use this
# file except in compliance with the License. You may obtain
# a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, either express or implied.  See the License for the
# specific language governing permissions and limitations
# under the License.

touch index.txt
touch index.txt.attr
echo FA > serial.txt  # Current serial number

# CA generation: generate key, generate self signed certificate# (does not work with 4096 key length)
openssl genrsa -aes256 -out cakey.pem 4096
openssl req -config cassl.cnf -new -x509 -key cakey.pem -out cacert.pem -days 36500

# Generate an empty Certificate Revocation List, convert it to DER format for UA stack
openssl ca -config cassl.cnf -gencrl -crldays 36500 -out cacrl.pem
openssl crl -in cacrl.pem -outform der -out cacrl.der

# Generate, for both client and server, and both 2048 and 4096 key lengths, a new key pair and sign them, for the next century
openssl req -config clientssl.cnf -sha256 -nodes -newkey rsa:2048 -keyout client_2k_key.pem -out client_2k.csr
openssl req -config clientssl.cnf -sha256 -nodes -newkey rsa:4096 -keyout client_4k_key.pem -out client_4k.csr
openssl req -config serverssl.cnf -sha256 -nodes -newkey rsa:2048 -keyout server_2k_key.pem -out server_2k.csr
openssl req -config serverssl.cnf -sha256 -nodes -newkey rsa:4096 -keyout server_4k_key.pem -out server_4k.csr
openssl ca -config cassl.cnf -policy signing_policy -days 36500 -in client_2k.csr -out client_2k_cert.pem
openssl ca -config cassl.cnf -policy signing_policy -days 36500 -in client_4k.csr -out client_4k_cert.pem
openssl ca -config cassl.cnf -policy signing_policy -days 36500 -in server_2k.csr -out server_2k_cert.pem
openssl ca -config cassl.cnf -policy signing_policy -days 36500 -in server_4k.csr -out server_4k_cert.pem

# Output certificates in DER format for UA stack
for fradix in ca client_2k_ client_4k_ server_2k_ server_4k_; do
    openssl x509 -in ${fradix}cert.pem -out ${fradix}cert.der -outform der
done

# Output hexlified certificate to include in check_crypto_certificates.c
echo "Server signed public key:"
hexdump -ve '/1 "%02x"' server_2k_cert.der
echo
echo -e "\nCertificate Authority signed public key:"
hexdump -ve '/1 "%02x"' cacert.der
echo