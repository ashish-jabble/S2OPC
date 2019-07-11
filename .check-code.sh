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


# Script to check properties on code of the S2OPC project:
# - check absence of use of functions and includes which guarantee compliance with some CERT rules
# - run C sources compilation with Clang compiler using specific options to guarantee some CERT rules
# - run C sources analysis using Clang tidy tool using specific options to guarantee some CERT rules
# - run C sources automatic formatting using Clang format tool to normalize code presentation
#
# OPTIONS:
# - if first argument provided is "advanced", the clang-tidy tool will be used with the default options in addition to CERT specific analyses. It could allow to detect more programming errors but could also contains false positives.

ISADVANCED=$1

BSRC=bsrc
CSRC=csrc
TST=tests

EXITCODE=0
LOGPATH=$(pwd)/pre-build-check.log

# Redirect all output and errors to log file
echo "Pre-build-check log" > $LOGPATH

#### Check contributor list ####
echo "Contributors list verification" | tee -a $LOGPATH
ifs_save=$IFS
IFS=$'\n'
LIST_CONTRIB=(`git log --format="%an <%ae>" | sort -u`)

for contrib in ${LIST_CONTRIB[*]}
do
    grep $contrib Contributors.md &> /dev/null
    if [[ $? != 0 ]]; then
        echo "ERROR: contributor not declared: $contrib" | tee -a $LOGPATH
        EXITCODE=1
    fi
done

IFS=$ifs_save

#### Check absence of functions / includes ####

CHECK_ABSENCE="(restrict|fgets|fgetws|getc|putc|getwc|putwc|fsetpos|rand|readlink|vfork|putenv|lstat|setuid|setgid|getuid|getgid|seteuid|geteuid|fork|pthread_kill|pthread_cancel|pthread_exit|signal.h)"

echo "Checking specific functions or headers not used in code" | tee -a $LOGPATH
find $CSRC -name "*.c" -or -name "*.h" | xargs grep -wiEc $CHECK_ABSENCE | grep -Ec ":[^0]+" | xargs test 0 -eq
if [[ $? != 0 ]]; then
    echo "ERROR: checking absence of functions or headers: $CHECK_ABSENCE" | tee -a $LOGPATH
    find $CSRC -name "*.c" -or -name "*.h" | xargs grep -nwiE $CHECK_ABSENCE | tee -a $LOGPATH
    EXITCODE=1
fi

CHECK_STD_MEM_ALLOC_ABSENCE="(\bfree\b\(|\bmalloc\b\(|\bcalloc\b\(|\brealloc\b\(|=.*\bfree\b|=.*\bmalloc\b|=.*\bcalloc\b|=.*\brealloc\b)"
EXLUDE_STD_MEM_IMPLEM="*\/p_mem_alloc.c"
EXCLUDED_TESTS="*\/client_subscription\/*"

find $CSRC -not -path $EXLUDE_STD_MEM_IMPLEM -name "*.c" | xargs grep -E $CHECK_STD_MEM_ALLOC_ABSENCE | grep -Ec ":[^0]+" | xargs test 0 -eq
if [[ $? != 0 ]]; then
    echo "ERROR: checking absence of std library use for memory allocation in tookit" | tee -a $LOGPATH
    find $CSRC -not -path $EXLUDE_STD_MEM_IMPLEM -name "*.c" | xargs grep -nE $CHECK_STD_MEM_ALLOC_ABSENCE | tee -a $LOGPATH
    EXITCODE=1
fi
find $TST -not -path $EXCLUDED_TESTS -name "*.c" | xargs grep -E $CHECK_STD_MEM_ALLOC_ABSENCE | grep -Ec ":[^0]+" | xargs test 0 -eq
if [[ $? != 0 ]]; then
    echo "ERROR: checking absence of std library use for memory allocation in tests" | tee -a $LOGPATH
    find $TST -not -path $EXCLUDED_TESTS -name "*.c" | xargs grep -nE $CHECK_STD_MEM_ALLOC_ABSENCE | tee -a $LOGPATH
    EXITCODE=1
fi

#### Clang static analyzer ####
echo "Compilation with Clang static analyzer" | tee -a $LOGPATH
rm -fr build-analyzer && ./.run-clang-static-analyzer.sh 2>&1 | tee -a $LOGPATH
if [[ ${PIPESTATUS[0]} != 0 ]]; then
    EXITCODE=1
else
    rm -fr build-analyzer
fi

#### Analyze C sources with clang-tidy ####

echo "Checking specific CERT rules using clang-tidy tool" | tee -a $LOGPATH
# CERT rules to verify
if [[ -z $ISADVANCED || $ISADVANCED != "advanced" ]]; then
    # remove default rules
    REMOVE_DEFAULT_RULES="-*,"
else
    # do not remove default rules
    echo "clang-tidy tool: keep default rules for advanced analysis of code" | tee -a $LOGPATH
    REMOVE_DEFAULT_RULES=""
fi

CERT_RULES=cert-flp30-c,cert-fio38-c,cert-env33-c,cert-err34-c,cert-msc30-c
# Define include directories
SRC_DIRS=(`find $CSRC -not -path "*windows*" -not -path "*freertos*" -type d`)
SRC_INCL=${SRC_DIRS[@]/#/-I}
CLANG_TIDY_LOG=clang_tidy.log
# Run clang-tidy removing default checks (-*) and adding CERT rules verification
find $CSRC -not -path "*windows*" -not -path "*freertos*" -not -path "*uanodeset_expat*" -name "*.c" -exec clang-tidy {} -checks=$REMOVE_DEFAULT_RULES$CERT_RULES -- $SRC_INCL \; &> $CLANG_TIDY_LOG
# Check if resulting log contains error or warnings
grep -wiEc "(error|warning)" $CLANG_TIDY_LOG | xargs test 0 -eq
if [[ $? != 0 ]]; then
    echo "ERROR: checking CERT rules $CERT_RULES with clang-tidy: see log $CLANG_TIDY_LOG" | tee -a $LOGPATH
    # Note: for default checks the scan-build tool can be use to build the project (scan-build ./build.sh).
    #       It generates an HTML report providing diagnostics of the warning
    EXITCODE=1
else
    \rm $CLANG_TIDY_LOG
fi

#### Format C sources with clang-format ####

echo "Clang automatic formatting check" | tee -a $LOGPATH
./.format.sh >> $LOGPATH
ALREADY_FORMAT=`git ls-files -m $BSRC $CSRC $TST | grep -v bgenc`

if [[ -z $ALREADY_FORMAT ]]; then
    echo "C source code formatting already done" | tee -a $LOGPATH
else
    echo "ERROR: C source code code formatting not done or not committed" | tee -a $LOGPATH
    EXITCODE=1
fi

if [[ $EXITCODE -eq 0 ]]; then
    echo "Completed with SUCCESS" | tee -a $LOGPATH
else
    echo "Completed with ERRORS" | tee -a $LOGPATH
fi

#### Check license in files ####

echo "License in files verification" | tee -a $LOGPATH
./.license-check.sh >> $LOGPATH

if [[ $? != 0 ]]; then
    echo "ERROR: license in files verification failed, see $LOGPATH" | tee -a $LOGPATH
    EXITCODE=1
fi

#### Check final result:

if [[ $EXITCODE -eq 0 ]]; then
    echo "Completed with SUCCESS" | tee -a $LOGPATH
else
    echo "Completed with ERRORS, see $LOGPATH" | tee -a $LOGPATH
fi

exit $EXITCODE
