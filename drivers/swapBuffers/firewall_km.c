#pragma warning (disable : 4201)

#define NDIS_SUPPORT_NDIS6 1
#define INITGUID

#include <ndis.h>
#include <wdm.h>
#include <fwpsk.h>
#include <guiddef.h>
#include <fwpmk.h>

#include <wdm.h>
#include <ip2string.h>

#define UINT_MAX 0xFFFFFFFF
#define PROTOCOL_TCP 6
#define PROTOCOL_UDP 17
#define PROTOCOL_ICMP 1


///
/// **************************************************************************************************
/// *                           NETWORK FILTER CALLOUTS                                              *
/// **************************************************************************************************
///

void GetNetwork5TupleIndexesForLayer(
    _In_ UINT16 layerId,
    _Out_ UINT* appId,
    _Out_ UINT* localAddressIndex,
    _Out_ UINT* remoteAddressIndex,
    _Out_ UINT* localPortIndex,
    _Out_ UINT* remotePortIndex,
    _Out_ UINT* protocolIndex,
    _Out_ UINT* icmpIndex

)
{
    switch (layerId)
    {
    case FWPS_LAYER_ALE_AUTH_CONNECT_V4:
        *appId = FWPS_FIELD_ALE_AUTH_CONNECT_V4_ALE_APP_ID;
        *localAddressIndex = FWPS_FIELD_ALE_AUTH_CONNECT_V4_IP_LOCAL_ADDRESS;
        *remoteAddressIndex = FWPS_FIELD_ALE_AUTH_CONNECT_V4_IP_REMOTE_ADDRESS;
        *localPortIndex = FWPS_FIELD_ALE_AUTH_CONNECT_V4_IP_LOCAL_PORT;
        *remotePortIndex = FWPS_FIELD_ALE_AUTH_CONNECT_V4_IP_REMOTE_PORT;
        *protocolIndex = FWPS_FIELD_ALE_AUTH_CONNECT_V4_IP_PROTOCOL;
        *icmpIndex = FWPS_FIELD_ALE_AUTH_CONNECT_V4_ICMP_TYPE;
        break;
    case FWPS_LAYER_ALE_AUTH_CONNECT_V6:
        *appId = FWPS_FIELD_ALE_AUTH_CONNECT_V6_ALE_APP_ID;
        *localAddressIndex = FWPS_FIELD_ALE_AUTH_CONNECT_V6_IP_LOCAL_ADDRESS;
        *remoteAddressIndex = FWPS_FIELD_ALE_AUTH_CONNECT_V6_IP_REMOTE_ADDRESS;
        *localPortIndex = FWPS_FIELD_ALE_AUTH_CONNECT_V6_IP_LOCAL_PORT;
        *remotePortIndex = FWPS_FIELD_ALE_AUTH_CONNECT_V6_IP_REMOTE_PORT;
        *protocolIndex = FWPS_FIELD_ALE_AUTH_CONNECT_V6_IP_PROTOCOL;
        *icmpIndex = FWPS_FIELD_ALE_AUTH_CONNECT_V6_ICMP_TYPE;
        break;
    case FWPS_LAYER_ALE_AUTH_RECV_ACCEPT_V4:
        *appId = FWPS_FIELD_ALE_AUTH_RECV_ACCEPT_V4_ALE_APP_ID;
        *localAddressIndex = FWPS_FIELD_ALE_AUTH_RECV_ACCEPT_V4_IP_LOCAL_ADDRESS;
        *remoteAddressIndex = FWPS_FIELD_ALE_AUTH_RECV_ACCEPT_V4_IP_REMOTE_ADDRESS;
        *localPortIndex = FWPS_FIELD_ALE_AUTH_RECV_ACCEPT_V4_IP_LOCAL_PORT;
        *remotePortIndex = FWPS_FIELD_ALE_AUTH_RECV_ACCEPT_V4_IP_REMOTE_PORT;
        *protocolIndex = FWPS_FIELD_ALE_AUTH_RECV_ACCEPT_V4_IP_PROTOCOL;
        *icmpIndex = FWPS_FIELD_ALE_AUTH_RECV_ACCEPT_V4_ICMP_TYPE;
        break;
    case FWPS_LAYER_ALE_AUTH_RECV_ACCEPT_V6:
        *appId = FWPS_FIELD_ALE_AUTH_RECV_ACCEPT_V6_ALE_APP_ID;
        *localAddressIndex = FWPS_FIELD_ALE_AUTH_RECV_ACCEPT_V6_IP_LOCAL_ADDRESS;
        *remoteAddressIndex = FWPS_FIELD_ALE_AUTH_RECV_ACCEPT_V6_IP_REMOTE_ADDRESS;
        *localPortIndex = FWPS_FIELD_ALE_AUTH_RECV_ACCEPT_V6_IP_LOCAL_PORT;
        *remotePortIndex = FWPS_FIELD_ALE_AUTH_RECV_ACCEPT_V6_IP_REMOTE_PORT;
        *protocolIndex = FWPS_FIELD_ALE_AUTH_RECV_ACCEPT_V6_IP_PROTOCOL;
        *icmpIndex = FWPS_FIELD_ALE_AUTH_RECV_ACCEPT_V6_ICMP_TYPE;
        break;
    default:
        *appId = UINT_MAX;
        *localAddressIndex = UINT_MAX;
        *remoteAddressIndex = UINT_MAX;
        *localPortIndex = UINT_MAX;
        *remotePortIndex = UINT_MAX;
        *protocolIndex = UINT_MAX;
        *icmpIndex = UINT_MAX;
    }
}

void NTAPI
DefaultClassifyFn(
    _In_ const FWPS_INCOMING_VALUES0* inFixedValues,
    _In_ const FWPS_INCOMING_METADATA_VALUES0* inMetaValues,
    _Inout_opt_ void* layerData,
    _In_opt_ const void* classifyContext,
    _In_ const FWPS_FILTER3* filter,
    _In_ UINT64 flowContext,
    _Inout_ FWPS_CLASSIFY_OUT0* classifyOut
)
{
    UNREFERENCED_PARAMETER(inFixedValues);
    UNREFERENCED_PARAMETER(inMetaValues);
    UNREFERENCED_PARAMETER(layerData);
    UNREFERENCED_PARAMETER(classifyContext);
    UNREFERENCED_PARAMETER(filter);
    UNREFERENCED_PARAMETER(flowContext);
    UNREFERENCED_PARAMETER(classifyOut);

    UINT appId = 0;
    UINT localAddressIndex = 0;
    UINT remoteAddressIndex = 0;
    UINT localPortIndex = 0;
    UINT remotePortIndex = 0;
    UINT protocolIndex = 0;
    UINT icmpIndex = 0;

    GetNetwork5TupleIndexesForLayer(inFixedValues->layerId,
        &appId,
        &localAddressIndex,
        &remoteAddressIndex,
        &localPortIndex,
        &remotePortIndex,
        &protocolIndex,
        &icmpIndex);

    FWPS_INCOMING_VALUE* localAddress = &inFixedValues->incomingValue[localAddressIndex];
    FWPS_INCOMING_VALUE* localPort = &inFixedValues->incomingValue[localPortIndex];
    FWPS_INCOMING_VALUE* remoteAddress = &inFixedValues->incomingValue[remoteAddressIndex];
    FWPS_INCOMING_VALUE* remotePort = &inFixedValues->incomingValue[remotePortIndex];

    /* Todo read docs and extract actual values :D be careful of ipv4/ipv6 */
    UNREFERENCED_PARAMETER(localAddress);
    UNREFERENCED_PARAMETER(localPort);
    UNREFERENCED_PARAMETER(remoteAddress);
    UNREFERENCED_PARAMETER(remotePort);

    /* To format an IPV4 address you can use something similar with the following snippet. Be carefult of IRQL and the ipv4/ipv6 :) This is for ipv4*/
    // {
    //     NT_ASSERT(localAddress->value.type == FWP_UINT32);
    //     NT_ASSERT(localPort->value.type == FWP_UINT16);
    // 
    //     struct in_addr ipAddress = { 0 };
    // 
    //     WCHAR ipAddressBuffer[100] = { 0 };
    //     ULONG ipAddressBufferSize = ARRAYSIZE(ipAddressBuffer);
    // 
    //     ipAddress.S_un.S_addr = RtlUlongByteSwap(localAddress->value.int32);
    //     RtlIpv4AddressToStringExW(&ipAddress, localPort->value.int16, &ipAddressBuffer[0], &ipAddressBufferSize);
    // 
    //     __debugbreak();
    // }
}

NTSTATUS NTAPI
DefaultNotifyFn(
    _In_ FWPS_CALLOUT_NOTIFY_TYPE notifyType,
    _In_ const GUID* filterKey,
    _Inout_ FWPS_FILTER3* filter
)
{
    UNREFERENCED_PARAMETER(notifyType);
    UNREFERENCED_PARAMETER(filterKey);
    UNREFERENCED_PARAMETER(filter);

    return STATUS_SUCCESS;
}

void NTAPI
DefaultDeleteFn(
    _In_ UINT16 layerId,
    _In_ UINT32 calloutId,
    _In_ UINT64 flowContext
)
{
    UNREFERENCED_PARAMETER(layerId);
    UNREFERENCED_PARAMETER(calloutId);
    UNREFERENCED_PARAMETER(flowContext);
}

///
/// **************************************************************************************************
/// *                           GLOBAL DATA AREA                                                     *
/// **************************************************************************************************
///

static PDEVICE_OBJECT gNetworkDeviceObject = NULL;
static HANDLE gFilterEngine = NULL;

/* {927E39F6 - E5F3 - 4ED6 - B4E0 - E63D0A01F704} */
#define gFwpProviderKey { 0x927e39f6, 0xe5f3, 0x4ed6, { 0xb4, 0xe0, 0xe6, 0x3d, 0xa, 0x1, 0xf7, 0x4 } }

static FWPM_PROVIDER gFwpProvider =
{
    .providerKey = gFwpProviderKey,
    .displayData.name = L"Dsmk - Network Filter Provider",
    .displayData.description = L"Used to filter network connections",
    .flags = 0,
    .providerData = {0},
    .serviceName = NULL
};
static FWPM_SUBLAYER gFwpFilterSubLayer =
{
    .subLayerKey = { 0x97317c16, 0x25f6, 0x4b9d, { 0x9d, 0x4c, 0xec, 0x96, 0x89, 0xc6, 0xf5, 0xf } },
    .displayData.name = L"Dsmk - Network Filter Sublayer",
    .displayData.description = L"Used to filter network connections",
    .flags = 0,
    .providerKey = &gFwpProvider.providerKey,
    .providerData = {0},
    .weight = 0x100
};


/// Similar data with this must be added to complete the homework :) 

// This must be unique!
#define gAleAuthConnectV4CalloutKeyGuid { 0x18b4d00e, 0x1540, 0x45f5, { 0xa1, 0xfb, 0x4f, 0x8d, 0xd7, 0xe9, 0x6e, 0x00 } }

static FWPM_CALLOUT gAleAuthConnectV4ManagementCallout =
{
    .calloutKey = gAleAuthConnectV4CalloutKeyGuid,
    .displayData.name = L"Dsmk - Callout for FWPM_LAYER_ALE_AUTH_CONNECT_V4",
    .displayData.description = L"This filtering layer allows for authorizing connect requests for outgoing TCP connections.",
    .flags = 0,
    .providerKey = &gFwpProvider.providerKey,
    .providerData = {0},
    .applicableLayer = {0},
    .calloutId = 0
};
static FWPS_CALLOUT gAleAuthConnectV4StateCallout =
{
    .calloutKey = gAleAuthConnectV4CalloutKeyGuid,
    .flags = 0,
    .classifyFn = DefaultClassifyFn,
    .flowDeleteFn = DefaultDeleteFn,
    .notifyFn = DefaultNotifyFn,
};
static UINT32 gAleAuthConnectV4StateCalloutID = 0;
static FWPM_FILTER gAleAuthConnectV4Filter =
{
    .filterKey = {0},
    .displayData.name = L"Dsmk - Filter for FWPM_LAYER_ALE_AUTH_CONNECT_V4",
    .displayData.description = L"This filtering layer allows for authorizing connect requests for outgoing TCP connections.",
    .flags = FWPM_FILTER_FLAG_NONE,
    .providerKey = &gFwpProvider.providerKey,
    .providerData = {0},
    .layerKey = {0},
    .subLayerKey = {0},
    .weight.type = FWP_EMPTY,
    .weight.uint64 = 0,
    .numFilterConditions = 0,
    .filterCondition = NULL,
    .action = {0},
    .providerContextKey = {0},
    .reserved = NULL,
    .filterId = 0,
    .effectiveWeight = {0}
};

///
/// **************************************************************************************************
/// *                           NETWORK FILTER REGISTRATION REGION                                   *
/// **************************************************************************************************
///


VOID
DriverUnregisterNetworkFilter()
{
    /* Unregister the FWPM_LAYER_ALE_AUTH_CONNECT_V4 callout. */
    if (gAleAuthConnectV4Filter.filterId != 0)
    {
        FwpmFilterDeleteById(gFilterEngine, gAleAuthConnectV4Filter.filterId);
        RtlZeroMemory(&gAleAuthConnectV4Filter, sizeof(gAleAuthConnectV4Filter));
    }
    if (gAleAuthConnectV4StateCalloutID != 0)
    {
        FwpsCalloutUnregisterById(gAleAuthConnectV4StateCalloutID);
        gAleAuthConnectV4StateCalloutID = 0;
    }
    if (gAleAuthConnectV4ManagementCallout.calloutId != 0)
    {
        FwpmCalloutDeleteById(gFilterEngine, gAleAuthConnectV4ManagementCallout.calloutId);
        RtlZeroMemory(&gAleAuthConnectV4ManagementCallout, sizeof(gAleAuthConnectV4ManagementCallout));
    }

    /* TODO: unregister for FWPM_LAYER_ALE_AUTH_RECV_ACCEPT_V4  */
    /* TODO: unregister for FWPM_LAYER_ALE_AUTH_CONNECT_V6      */
    /* TODO: unregister for FWPM_LAYER_ALE_AUTH_RECV_ACCEPT_V6  */

    /* Delete the sublayer. */
    if (&gFwpProvider.providerKey == gFwpFilterSubLayer.providerKey)
    {
        FwpmSubLayerDeleteByKey(gFilterEngine, &gFwpFilterSubLayer.subLayerKey);
        RtlZeroMemory(&gFwpFilterSubLayer, sizeof(gFwpFilterSubLayer));
    }

    /* Delete the provider. */
    if (gFwpProvider.displayData.name != NULL)
    {
        FwpmProviderDeleteByKey(gFilterEngine, &gFwpProvider.providerKey);
        RtlZeroMemory(&gFwpProvider, sizeof(gFwpProvider));
    }

    /* Close the engine. */
    if (NULL != gFilterEngine)
    {
        FwpmEngineClose(gFilterEngine);
        gFilterEngine = NULL;
    }
}

NTSTATUS
DriverRegisterNetworkFilter()
{
    NTSTATUS status = STATUS_UNSUCCESSFUL;

    /* The FwpmEngineOpen0 function opens a session to the filter engine. */
    status = FwpmEngineOpen(NULL,
        RPC_C_AUTHN_WINNT,
        NULL,
        NULL,
        &gFilterEngine);
    if (!NT_SUCCESS(status))
    {
        gFilterEngine = NULL;
        goto CleanUp;
    }

    /* Add the provider. */
    status = FwpmProviderAdd(gFilterEngine, &gFwpProvider, NULL);
    if (!NT_SUCCESS(status))
    {
        RtlZeroMemory(&gFwpProvider, sizeof(gFwpProvider));
        goto CleanUp;
    }

    /* Add the sublayer. */
    status = FwpmSubLayerAdd(gFilterEngine, &gFwpFilterSubLayer, NULL);
    if (!NT_SUCCESS(status))
    {
        RtlZeroMemory(&gFwpFilterSubLayer, sizeof(gFwpFilterSubLayer));
        goto CleanUp;
    }

    /* Register to FWPM_LAYER_ALE_AUTH_CONNECT_V4. */
    gAleAuthConnectV4ManagementCallout.applicableLayer = FWPM_LAYER_ALE_AUTH_CONNECT_V4;
    status = FwpmCalloutAdd(gFilterEngine, &gAleAuthConnectV4ManagementCallout, NULL, &gAleAuthConnectV4ManagementCallout.calloutId);
    if (!NT_SUCCESS(status))
    {
        RtlZeroMemory(&gAleAuthConnectV4ManagementCallout, sizeof(gAleAuthConnectV4ManagementCallout));
        goto CleanUp;
    }

    status = FwpsCalloutRegister(gNetworkDeviceObject, &gAleAuthConnectV4StateCallout, &gAleAuthConnectV4StateCalloutID);
    if (!NT_SUCCESS(status))
    {
        RtlZeroMemory(&gAleAuthConnectV4StateCallout, sizeof(gAleAuthConnectV4StateCallout));
        gAleAuthConnectV4StateCalloutID = 0;

        goto CleanUp;
    }

    gAleAuthConnectV4Filter.action.calloutKey = gAleAuthConnectV4ManagementCallout.calloutKey;
    gAleAuthConnectV4Filter.action.type = FWP_ACTION_CALLOUT_INSPECTION;
    gAleAuthConnectV4Filter.layerKey = gAleAuthConnectV4ManagementCallout.applicableLayer;
    gAleAuthConnectV4Filter.subLayerKey = gFwpFilterSubLayer.subLayerKey;
    status = FwpmFilterAdd(gFilterEngine, &gAleAuthConnectV4Filter, NULL, &gAleAuthConnectV4Filter.filterId);
    if (!NT_SUCCESS(status))
    {
        RtlZeroMemory(&gAleAuthConnectV4Filter, sizeof(gAleAuthConnectV4Filter));
        goto CleanUp;
    }

    /* TODO - register for FWPM_LAYER_ALE_AUTH_RECV_ACCEPT_V4 */
    /* TODO - don't forget cleanup in DriverUnregisterNetworkFilter for this as well :) */

    /* For IPv6 filtering: */
        /* TODO - register for FWPM_LAYER_ALE_AUTH_CONNECT_V6 */
        /* TODO - register for FWPM_LAYER_ALE_AUTH_RECV_ACCEPT_V6 */
        /* TODO - don't forget cleanup in DriverUnregisterNetworkFilter for those as well :) */

CleanUp:
    if (!NT_SUCCESS(status))
    {
        DriverUnregisterNetworkFilter();
    }
    return status;
}

///
/// **************************************************************************************************
/// *                           DRIVER LOAD - UNLOAD REGION                                          *
/// **************************************************************************************************
///

VOID
DriverUnload(
    _In_ PDRIVER_OBJECT DriverObject
)
{
    UNREFERENCED_PARAMETER(DriverObject);

    /* Unregister the network filter. */
    DriverUnregisterNetworkFilter();

    /* We no longer need the device object. */
    if (NULL != gNetworkDeviceObject)
    {
        IoDeleteDevice(gNetworkDeviceObject);
        gNetworkDeviceObject = NULL;
    }
}

NTSTATUS
DriverEntry(
    _In_ PDRIVER_OBJECT DriverObject,
    _In_ PUNICODE_STRING RegistryPath
)
{
    UNREFERENCED_PARAMETER(RegistryPath);

    NTSTATUS status = STATUS_UNSUCCESSFUL;

    /* Set unload routine. */
    DriverObject->DriverUnload = DriverUnload;

    /* Create the device for being associated with network filter. */
    status = IoCreateDevice(DriverObject,
        0,
        NULL,
        FILE_DEVICE_NETWORK,
        FILE_DEVICE_SECURE_OPEN,
        FALSE,
        &gNetworkDeviceObject);
    if (!NT_SUCCESS(status))
    {
        gNetworkDeviceObject = NULL;
        return status;
    }

    /* Register the network filter callouts. */
    status = DriverRegisterNetworkFilter();
    if (!NT_SUCCESS(status))
    {
        IoDeleteDevice(gNetworkDeviceObject);
        gNetworkDeviceObject = NULL;

        return status;
    }

    /* All good. */
    return STATUS_SUCCESS;
}