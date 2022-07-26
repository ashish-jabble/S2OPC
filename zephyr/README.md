# HowTo : Develop a  ZEPHYR S2OPC application (PubSub + Server)

## Prerequisite
It is mandatory to have first ZEPHYR and S2OPC installed and configured properly.
See [WIKI](https://trac.aix.systerel.fr/ingopcs/wiki/ZephyrGetStarted)

## Configuration
### ZEPHYR configuration

The configuration is provided in `s2opc/zephyr/Kconfig`. The default configuration is provided for a PubSub + OPC UA server application, and already meets expected constraints (except network configuration : `SOPC_ETH_ADDRESS`,  `SOPC_ENDPOINT_ADDRESS`, ...)
Note: This configuration is based on a Hardware design with a single Ethernet interface. In case several interfaces exist, this may have to be reworked.

### S2OPC configuration
The configuration is provided in `s2opc/zephyr/CMakelist.txt`. The default configuration is provided for a PubSub + OPC UA server application with constant address space (RAM-optimized). Note that when using the constant Address Space, only the DataValue of the nodes can be modified (not the META DATA like status or timestamps)

## Using S2OPC

Adding S2OPC in an external ZEPHYR project requires to:
- include s2opc module in ZEPHYR workspace. If not already included, this can be done by adding the following lines to `west.yml`:

```yaml
    - name: s2opc
      revision: zephyr
      path: modules/lib/s2opc
      url: https://gitlab.com/systerel/S2OPC.git
```

- check that s2opc is up to date (`west update s2opc`)
- In `prj.conf`, ensure to have `CONFIG_S2OPC=y`
- Check the specific configuration in `$ZEPHYR_BASE/modules/lib/s2opc/zephyr/Kconfig` and modify the `prj.conf` accordingly to project needs
- In `CMakeLists.txt`, add the s2opc build options (e.g. `add_definitions (-DWITH_STATIC_SECURITY_DATA=1)`, `add_definitions (-DWITH_USER_ASSERT=1)`, ...). Refer to the [WIKI](https://gitlab.com/systerel/S2OPC/-/wikis/compilation) for all possible options.

## Examples

Several examples are provided. 
Common files to several examples are locarted in subfolder `zephyr_common_src`

- `static_security_data` provides a hard-coded example of Pubsub symmetric keys
- `pubsub_config_static` provides a hard-coded example of PubSub configuration
- `network_init` provides a example of network configuration (tested on STM32H7 and IMX1064RT)
- `cache` provides a basic cache for PubSub communication
- `tls_config` provides the MdebTLS required parameters for its integration into S2OPC
- `server` provides a basic OPC server implementation

Note: a 'cache' can be simply seen as a basic dictionnary containing both variables to be read by "Pub" and writton to by "Sub".

### Building
Enter the chosen test folder.
Edit the `Makefile` to set-up local parameters and constants (BOARD, ZEPHYR_BASE)
Simply type `make` in current folder

### ZEPHYR S2OPC PubSub demo

`zephyr_pubsub` provides an example of S2OPC PubSub demo over Zephyr.

The demo runs a cache-based S2OPC PubSub (Not based on an OPC address space).

### ZEPHYR S2OPC Client demo

`zephyr_client` provides an example of S2OPC Client demo over Zephyr.

The demo runs a cache-based S2OPC Client (Not based on an OPC address space).

### ZEPHYR S2OPC PtP demo

`zephyr_ptp` provides an example of PtP clock synchronisation on Zephyr.

The demo requires an hardware PtP-capable device to run successfully. It has been tested on STM NUCLEO-144 series.
This demo is interactive and allows the user to start/stop specific clock-related measurement tests.

### ZEPHYR S2OPC server demo

`zephyr_server` provides an example of S2OPC server.

The addresss space can be manually edited:
- update the `xml/pubsub_server.xml` file
- start `make regen` command to regenerate the `test_address_space.c` file

## Main design
### Code design - Server
The main entry is in `pubsub_test_server.c`
Hereunder is the typical sequence to create and start an OPC server:

- `SOPC_Assert_Set_UserCallback`
  - Defines the behavior in case of assertion failure.
See also S2OPC configuration:
     -	`WITH_USER_ASSERT`
     -	`WITH_NO_ASSERT`
     -	`WITH_MINIMAL_FOOTPRINT`
See also `sopc_assert.h`
- `Network_Initialize`
    - See details in `network_init.c`.
This is an example of implementation of network configuration for a project with a single ETHERNET interface. It might have to be adapted depending on actual application needs
- `tls_threading_initialize`
    - Initialize MBEDTLS library. 
- `Server_Initialize`
    - Initialize the OPC server. Provides the logging callback mechanism. This examples just prints out logs onto the console, but it can be reworked depending on project requirements.
- `SOPC_S2OPC_Config_Initialize`
    - Initialize an empty configuration
- `Server_CreateServerConfig`
    - Creates the configuration. Provides examples for static security data (typically when no file system is present) or dynamic (reading keys from files) 
- `Server_LoadAddressSpace`
    - Set-up the address space using generated file `test_address_space.c`.
The file is generated using the xml file `pubsub_server_perf_zephyr.xml` and python tool `s2opc/scripts/generate-s2opc-address-space.py`
- `SOPC_ToolkitServer_AddEndpointConfig`
    - Create the endpoint.
- `SOPC_Toolkit_Configured`
    - Set the Server as configured.
- `SOPC_ToolkitServer_AsyncOpenEndpoint`
    - Start the server

The server Address Space is manager by a dedicated component that ensures its integrity. Thus, no direct access to any content is possible. The user application must send requests to the S2OPC core and wait for the answer.
See `Server_LocalWriteSingleNode` for an example of how the server can update a DataValue of a specific node. 

### Code design � PubSub
The PubSub can simply be seen as a periodic event to send/receive some couples (NodeId / DataValue). So as to integrate the PubSub in a user application, 2 callbacks have to be provided:
-	The �GetSource� callback. It is called by Publisher when a message has to be sent, and the call must provide all the DataValue required for message emission.
-	The �SetTarget� callback. It is called by �Subscriber� when a message has been received, and requests the user application to store the received values.

The PubSub is thus uncorrelated from the server address space. Of course, the �GetSource� and �SetTarget� can be defined to read/write into the address space, however, this method is discouraged, in particular in the case of high frequency messages. The reason is that accessing the address space is not direct (The address space is managed by �B� layer and all exchanges use queued requests). This is typically inefficient when the Publishing interval of the PubSub is low. See example in `pubsub.c`, with `CONFIG_SOPC_PUBSUB_USES_CACHE` equal to 0.
Otherwise, one simple possibility is to use a `SOPC_Dict` object (hashed map with O(1) lookup time) to store/read values in those both events. See example in `pubsub.c`, with `CONFIG_SOPC_PUBSUB_USES_CACHE` equal to 1.
Of course, an application is free to use any other methods in "GetSource"/"SetTarget" events.

Hereunder is the typical sequence to create and start the PubSub:
- `SOPC_PubSubConfig_ParseXML` Or `SOPC_PubSubConfig_GetStatic` (see `PubSub_Configure`)
- Choose where to read/write variables (see `PubSub_Configure`)
- (If using Cache)
    - `Cache_Initialize`
- Initialize SKS
    - `SOPC_LocalSKS_init` (with Filesystem)
    - `SOPC_LocalSKS_init_static` (without filesystem)
- Start Subscriber
    - `SOPC_SubScheduler_Start`
- Start Publisher
    - `SOPC_PubScheduler_Start`
- Stop Subscriber
    - `SOPC_SubScheduler_Stop`
    - `SOPC_SubTargetVariableConfig_Delete`
- Stop Publisher
    - `SOPC_PubScheduler_Stop`
    - `SOPC_PubSourceVariableConfig_Delete`
- Clear PubSub
    - `SOPC_PubSubConfiguration_Delete`
- (If using Cache)
    -   `Cache_Clear()`

All unlisted features are for demo purpose, and thus are not relevant in typical sequence.

## Attention points
### Address Space
When using the Cache (read/write), it is mandatory to enclose uses by calls to `Cache_Lock` and `Cache_Unlock` to ensure integrity of content. Thus, any user-defined tasks that accesses the cache  will prevent the PubSub events to occure in time if the cache locking time is not the shortest possible. Typically, if a value received on the Subscriber must be repeated onto the OPC server, it is important to first read the `DataValue` into a local variable, then release the cache, and then update the OPC server using the local variable.
Example:

```c
SOPC_DataValue* dvCopy = SOPC_Malloc(sizeof(*dvCopy));
SOPC_DataValue_Initialize(dvCopy);
SOPC_ASSERT(NULL != dvCopy);
SOPC_NodeId* pNid = SOPC_NodeId_FromCString(SERVER_STATUS_SUB_VAR_INFO, strlen(SERVER_STATUS_SUB_VAR_INFO));

Cache_Lock();
SOPC_DataValue* dv = Cache_Get(pNid);
if (NULL != dv){
     // Copy to avoid locking Cache for too long
     SOPC_DataValue_Copy(dvCopy, dv);
}
Cache_Unlock();

Server_LocalWriteSingleNode(pNid, dvCopy);

SOPC_DataValue_Clear (dvCopy);
SOPC_Free(dvCopy);
SOPC_NodeId_Clear(pNid);
SOPC_Free(pNid);

```

### Configuration tuning
The ZEPHYR HAL for S2OPC provides an instrumentation of stacks and memory allocation. These instrumentations should be disabled in a project release but can be used during development to ensure optimization of different resources allocations.

`CONFIG_SOPC_HELPER_IMPL_INSTRUM`
- 0 (default): Release build without instrumentation.
- 1 Instrumentation is active. This allows :
    -	Dumping current stacks usages (see `SOPC_Thread_GetAllThreadsInfo`)
    -	Checking memory leaks (see `SOPC_MemAlloc_Nb_Allocs`)


