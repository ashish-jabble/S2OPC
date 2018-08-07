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

#include "check_crypto_certificates.h"

#include <stdlib.h>

#include <check.h>

#include "hexlify.h"

// server_2k.der
const char* SRV_CRT =
    "308204263082030ea00302010202010c300d06092a864886f70d01010b0500308188310b3009060355040613024652310c300a06035504080c"
    "03494446310e300c06035504070c055041524953310e300c060355040a0c0553324f5043310e300c060355040b0c0553324f50433111300f06"
    "035504030c0853324f50432043413128302606092a864886f70d010901161973326f70632d737570706f727440737973746572656c2e667230"
    "1e170d3138303932363135303835385a170d3230303931353135303835385a3067310b3009060355040613024652310c300a06035504080c03"
    "494446310e300c06035504070c055041524953310e300c060355040a0c0553324f5043310e300c060355040b0c0553324f5043311a30180603"
    "5504030c1153324f504320546573742053657276657230820122300d06092a864886f70d01010105000382010f003082010a0282010100b245"
    "842f18a25870b4bdcf162ca5b2d48cb531f0c5ba5d509e5051b892fe535d6dbfbafc6f3e430af250468659476cfd8381c57a9dc4063d14ef48"
    "b6ede87b1a15e3560a8a759a4cda397a71034c812142c743d16685b6fe2f2bb3a56570a88f87452121ecb6bce7865e14ef83a5bd436661995a"
    "8a0635d3ba9b22324bd2d06f3cf117418d6e9e2fa3e1d317c77ba00972e622d602d8f1c07a720ba7d064150ee0533f256ccc0bb6fa0447dcac"
    "c974627c54bb544c60cbef5aeb70ebfe8c07acaf64f315088e8461a4af2f9d8846065c416d0c1cc4075e4baa2d7f44c1be2bd1e2d8dec1f979"
    "6b8727cac05f5e931069192fae496298a549bae6df699e2638270203010001a381ba3081b7301d0603551d0e0416041449da0583068c3c894a"
    "c32777d92ba84e8763f722301f0603551d2304183016801431ae8e65336406e4379b518fc3379dce459bb61f300f0603551d130101ff040530"
    "030101ff300b0603551d0f04040302010630290603551d1104223020861375726e3a53324f50433a6c6f63616c686f737482096c6f63616c68"
    "6f7374302c06096086480186f842010d041f161d4f70656e53534c2047656e657261746564204365727469666963617465300d06092a864886"
    "f70d01010b050003820101002cf984139a67c74081d54a8f7d248e485e1425b512fff329fafba8320a75c93acec0caf5c04b04f6f6848be949"
    "c3de393648101fe8d741f081596486411982ee58c3a1586eaf7d1d66287986a51e1012e8d4b78cdb37e1978f51b0901b44076b5d0653d251d4"
    "a1512a1d8493466df59eb3fe48bb958a9c8aed90289434dc29566aaffec818b378830ac5d3857230786a860bc9fd386b62503aeed26b4e1f02"
    "6db052616bb1ac87632ecf50d4c126ff57c292f6aed03fe80e97b657dbc3fe76a74322ab3e7dc7d65e71fb4ffefab8919ad5e0990f306bed8f"
    "7ecccbbe0d95c09d851640c9b0d178a4aca9462ebb5ef52f516554cd71571ad11edc6cb1ebfdca1d";

// cacert.der
const char* CA_CRT =
    "308203f5308202dda003020102020900c8f37b15fa230255300d06092a864886f70d01010b0500308188310b3009060355040613024652310c"
    "300a06035504080c03494446310e300c06035504070c055041524953310e300c060355040a0c0553324f5043310e300c060355040b0c055332"
    "4f50433111300f06035504030c0853324f50432043413128302606092a864886f70d010901161973326f70632d737570706f72744073797374"
    "6572656c2e6672301e170d3138303932363134333830345a170d3230303932353134333830345a308188310b3009060355040613024652310c"
    "300a06035504080c03494446310e300c06035504070c055041524953310e300c060355040a0c0553324f5043310e300c060355040b0c055332"
    "4f50433111300f06035504030c0853324f50432043413128302606092a864886f70d010901161973326f70632d737570706f72744073797374"
    "6572656c2e667230820122300d06092a864886f70d01010105000382010f003082010a02820101009b3ba50f9a27b27370513de979667a3a4b"
    "24b3cc0fbd90fb03c6b19395a7a11eadc6aefeab54cb9c863c9b52eb8a48cd187c02b598f2e8d611ef56c18009701e480f4bcdfba3a339d794"
    "53fb0897b89e20f5ae5381f2a0bda97599c5e4167d243b03caa5b4bc7a05a424e2176ea04b2ef71ac273b20bd7c727e1a716f07e4ec1251deb"
    "c6c98409c8eafa16c05254495a7833bee269aa0fdb4c98e30fef2345afa65d1630e657e6a4c10ab51b9c34a4ddbd0e7090984859bb2565b3fd"
    "10497ede6274ee7968a3bfc503a6f380e292a4c46f2e37e01a89dbd5af6e9af7f9fb34bbda53e62a87a0427823c4866e5877d5c2660d6cd264"
    "9ccbd57265e6b56c1ad2570203010001a360305e301d0603551d0e0416041431ae8e65336406e4379b518fc3379dce459bb61f301f0603551d"
    "2304183016801431ae8e65336406e4379b518fc3379dce459bb61f300f0603551d130101ff040530030101ff300b0603551d0f040403020106"
    "300d06092a864886f70d01010b050003820101002e827d8b88e3480807265069fe93d67ff2712de84a561c035d778ff9cd28e22bc16664570f"
    "017d646d5dd2ef8189873f365b18df9915b48e1e6ababff2409dc821dc37fe668dfa0b9a905af62837023f69764c883298492beadb53a7ebf3"
    "70677b4741f80944a012d3048826f106f4fad1949f9fde83ccfc9ea595f67f7b36315ca3ded52901de82957541e47cfd490dac547149e6b83c"
    "38477c4ca84f0e47741c9a29e7eedeb6b5384d4033f2ffb694b71aff96d7e6e84674ffa222a6676e739ed626a26842f5ed31502cc298b8f488"
    "01ebf53f7f7515b178f2fce2b1fbb088fe201c7accf6bce4dc0561fa36e01e8221301737e71716f866cb1839500bfc91";

SOPC_Certificate* SOPC_UnhexlifyCertificate(const char* hex_data)
{
    size_t der_len = strlen(hex_data) / 2;
    uint8_t* der_data = calloc(der_len, sizeof(uint8_t));
    ck_assert_ptr_nonnull(der_data);

    ck_assert(unhexlify(hex_data, der_data, der_len) == (int) der_len);
    SOPC_Certificate* crt = NULL;
    ck_assert(der_len <= SIZE_MAX);
    ck_assert_uint_eq(SOPC_STATUS_OK, SOPC_KeyManager_Certificate_CreateFromDER(der_data, (uint32_t) der_len, &crt));
    free(der_data);

    return crt;
}