#include "..\..\include\filter\network.h"

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

/* So this is a mapper */
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

/* This callout notifies me about things ("something is happening now") */
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

/* This callback is called when adding a new filter (FwpmFilterAdd )
and when a filter is deleted (FwpmFilterDelete )
[From the lecture]
*/
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

/* What's this one doing if the Notify also notifies me of deleting the device? */
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

/* Provider = container of WFP defined policies
Logical Grouping: It organizes sets of rules (filters) under a specific entity. 
For example, the Windows Firewall and built-in IPsec templates are considered
distinct providers.
[MSDN]
*/
static FWPM_PROVIDER gFwpProvider =
{
    .providerKey = gFwpProviderKey,
    .displayData.name = L"Dsmk - Network Filter Provider",
    .displayData.description = L"Used to filter network connections",
    .flags = 0,
    .providerData = {0},
    .serviceName = NULL
};

/*
* Sub-layer
*** Sub-component of layer used in filter arbitration
*** Has a weight (relative priority to other sublayers)
*** Usually each filtering app registers its own sublayer
[From the Lecture]
A sublayer is a way to group filters together so their arbitration in a single sublayer can be predicted. 
For example, firewall rules all belong to the same sublayer, and Windows Service Hardening rules are in a separate sublayer.
[From zerotnetworks.com]
*/
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

/// Similar data with this must be added to complete the homework :) Yeeeeey

// These must be unique!
#define gAleAuthConnectV4CalloutKeyGuid { 0x18b4d00e, 0x1540, 0x45f5, { 0xa1, 0xfb, 0x4f, 0x8d, 0xd7, 0xe9, 0x6e, 0x00 } }
#define gAleAuthConnectV6CalloutKeyGuid { 0x23ae61e8, 0x6cbe, 0x4674, { 0xa4, 0x4e, 0x1c, 0x7c, 0x83, 0x1, 0x17, 0x17 } } // {23AE61E8-6CBE-4674-A44E-1C7C83011717}
#define gAleAuthReceiveAcceptV4CalloutKeyGuid { 0xc2739011, 0x9ec3, 0x44b9, { 0x86, 0x7e, 0x7c, 0xdf, 0xe, 0xb7, 0xc, 0x62 } } // {C2739011-9EC3-44B9-867E-7CDF0EB70C62}
#define gAleAuthReceiveAcceptV6CalloutKeyGuid { 0x2eaebe40, 0xb706, 0x4b70, { 0xbe, 0xcd, 0xf5, 0x8, 0xeb, 0xcd, 0xc, 0xd3 } } // {2EAEBE40-B706-4B70-BECD-F508EBCD0CD3}

/* The FWPM_CALLOUT0 structure stores the state associated with a callout 
Ok so for callouts I want I need structures storing state :)
*/
/* With .field I (designatedly) initialise exactly the fields I want. Interesting C, interesting */
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
static FWPM_CALLOUT gAleAuthReceiveAcceptV4ManagementCallout =
{
    .calloutKey = gAleAuthReceiveAcceptV4CalloutKeyGuid,
    .displayData.name = L"Dsmk - Callout for FWPM_LAYER_ALE_RECV_ACCEPT_V4",
    .displayData.description = L"This filtering layer allows for authorizing connect requests for incoming TCP connections.",
    .flags = 0,
    .providerKey = &gFwpProvider.providerKey,
    .providerData = {0},
    .applicableLayer = {0},
    .calloutId = 0
};
static FWPM_CALLOUT gAleAuthConnectV6ManagementCallout =
{
    .calloutKey = gAleAuthConnectV6CalloutKeyGuid,
    .displayData.name = L"Dsmk - Callout for FWPM_LAYER_ALE_AUTH_CONNECT_V6",
    .displayData.description = L"This filtering layer allows for authorizing connect requests for outgoing TCP connections.",
    .flags = 0,
    .providerKey = &gFwpProvider.providerKey,
    .providerData = {0},
    .applicableLayer = {0},
    .calloutId = 0
};
static FWPM_CALLOUT gAleAuthReceiveAcceptV6ManagementCallout =
{
    .calloutKey = gAleAuthReceiveAcceptV6CalloutKeyGuid,
    .displayData.name = L"Dsmk - Callout for FWPM_LAYER_ALE_RECV_ACCEPT_V6",
    .displayData.description = L"This filtering layer allows for authorizing connect requests for incoming TCP connections.",
    .flags = 0,
    .providerKey = &gFwpProvider.providerKey,
    .providerData = {0},
    .applicableLayer = {0},
    .calloutId = 0
};

/*
* The FWPS_CALLOUT3 structure defines the data that is required for a callout driver to register a callout with the filter engine.
[MSDN]
WHY?? So I have a structure in FWPM for callouts with the states, and another structure for callouts in FWPS to register to BFE?
WTF (What-The-Filter) Microsoft?
*/
static FWPS_CALLOUT gAleAuthConnectV4StateCallout =
{
    .calloutKey = gAleAuthConnectV4CalloutKeyGuid,
    .flags = 0,
    .classifyFn = DefaultClassifyFn,
    .flowDeleteFn = DefaultDeleteFn,
    .notifyFn = DefaultNotifyFn,
};
static FWPS_CALLOUT gAleAuthReceiveAcceptV4StateCallout =
{
    .calloutKey = gAleAuthReceiveAcceptV4CalloutKeyGuid,
    .flags = 0,
    .classifyFn = DefaultClassifyFn,
    .flowDeleteFn = DefaultDeleteFn,
    .notifyFn = DefaultNotifyFn,
};
static FWPS_CALLOUT gAleAuthConnectV6StateCallout =
{
    .calloutKey = gAleAuthConnectV6CalloutKeyGuid,
    .flags = 0,
    .classifyFn = DefaultClassifyFn,
    .flowDeleteFn = DefaultDeleteFn,
    .notifyFn = DefaultNotifyFn,
};
static FWPS_CALLOUT gAleAuthReceiveAcceptV6StateCallout =
{
    .calloutKey = gAleAuthReceiveAcceptV6CalloutKeyGuid,
    .flags = 0,
    .classifyFn = DefaultClassifyFn,
    .flowDeleteFn = DefaultDeleteFn,
    .notifyFn = DefaultNotifyFn,
};

static UINT32 gAleAuthConnectV4StateCalloutID = 0;
static UINT32 gAleAuthReceiveAcceptV4StateCalloutID = 0;
static UINT32 gAleAuthConnectV6StateCalloutID = 0;
static UINT32 gAleAuthReceiveAcceptV6StateCalloutID = 0;

/* Finally the sweet things.. almost. Structure for Filter
The FWPM_FILTER0 structure stores the state associated with a filter.
[MSDN]
*/
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
static FWPM_FILTER gAleAuthReceiveAcceptV4Filter =
{
    .filterKey = {0},
    .displayData.name = L"Dsmk - Filter for FWPM_LAYER_ALE_AUTH_RECV_ACCEPT_V4",
    .displayData.description = L"This filtering layer allows for authorizing connect requests for incoming TCP connections.",
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
static FWPM_FILTER gAleAuthConnectV6Filter =
{
    .filterKey = {0},
    .displayData.name = L"Dsmk - Filter for FWPM_LAYER_ALE_AUTH_CONNECT_V6",
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
static FWPM_FILTER gAleAuthReceiveAcceptV6Filter =
{
    .filterKey = {0},
    .displayData.name = L"Dsmk - Filter for FWPM_LAYER_ALE_AUTH_RECV_ACCEPT_V6",
    .displayData.description = L"This filtering layer allows for authorizing connect requests for incoming TCP connections.",
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

/*
* First logical step: Register my network filter
* I don't start building a house by destroying it (unregister) :>
* Registers my filters: gAleAuth{Connect|ReceiveAccept}V{4|6}Filter
*/
NTSTATUS
DriverRegisterNetworkFilter()
{
    NTSTATUS status = STATUS_UNSUCCESSFUL;

    /* The FwpmEngineOpen0 function opens a session to the filter engine. */
    status = FwpmEngineOpen(
        NULL,
        RPC_C_AUTHN_WINNT,
        NULL,
        NULL,
        &gFilterEngine
    );
    if (!NT_SUCCESS(status))
    {
        gFilterEngine = NULL;
        goto CleanUp;
    }

    /* Add the provider. The one and only */
    status = FwpmProviderAdd(gFilterEngine, &gFwpProvider, NULL);
    if (!NT_SUCCESS(status))
    {
        RtlZeroMemory(&gFwpProvider, sizeof(gFwpProvider));
        goto CleanUp;
    }

    /* Add the sublayer. The one and only */
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

    /* TODO - Completed now - register for FWPM_LAYER_ALE_AUTH_RECV_ACCEPT_V4 */
    gAleAuthReceiveAcceptV4ManagementCallout.applicableLayer = FWPM_LAYER_ALE_AUTH_RECV_ACCEPT_V4;
    status = FwpmCalloutAdd(gFilterEngine, &gAleAuthReceiveAcceptV4ManagementCallout, NULL, &gAleAuthReceiveAcceptV4ManagementCallout.calloutId);
    if (NT_NOT_SUCCESS(status))
    {
        RtlZeroMemory(&gAleAuthReceiveAcceptV4ManagementCallout, sizeof(gAleAuthReceiveAcceptV4ManagementCallout));
        goto CleanUp;
    }

    status = FwpsCalloutRegister(gNetworkDeviceObject, &gAleAuthReceiveAcceptV4StateCallout, &gAleAuthReceiveAcceptV4StateCalloutID);
    if (NT_NOT_SUCCESS(status))
    {
        RtlZeroMemory(&gAleAuthReceiveAcceptV4StateCallout, sizeof(gAleAuthReceiveAcceptV4StateCallout));
        gAleAuthReceiveAcceptV4StateCalloutID = 0;

        goto CleanUp;
    }

    gAleAuthReceiveAcceptV4Filter.action.calloutKey = gAleAuthReceiveAcceptV4ManagementCallout.calloutKey;
    gAleAuthReceiveAcceptV4Filter.action.type = FWP_ACTION_CALLOUT_INSPECTION;
    gAleAuthReceiveAcceptV4Filter.layerKey = gAleAuthReceiveAcceptV4ManagementCallout.applicableLayer;
    gAleAuthReceiveAcceptV4Filter.subLayerKey = gFwpFilterSubLayer.subLayerKey;
    status = FwpmFilterAdd(gFilterEngine, &gAleAuthReceiveAcceptV4Filter, NULL, &gAleAuthReceiveAcceptV4Filter.filterId);
    if (!NT_SUCCESS(status))
    {
        RtlZeroMemory(&gAleAuthReceiveAcceptV4Filter, sizeof(gAleAuthReceiveAcceptV4Filter));
        goto CleanUp;
    }

    /* For IPv6 filtering:  (TODO LIST) */
        /* [BIFAT] TODO - register for FWPM_LAYER_ALE_AUTH_CONNECT_V6 */
        /* [BIFAT] TODO - register for FWPM_LAYER_ALE_AUTH_RECV_ACCEPT_V6 */
        /* TODO - don't forget cleanup in DriverUnregisterNetworkFilter for those as well :) */

    /* TODO - Completed now - register for FWPM_LAYER_ALE_AUTH_CONNECT_V6 */
    gAleAuthConnectV6ManagementCallout.applicableLayer = FWPM_LAYER_ALE_AUTH_CONNECT_V6;
    status = FwpmCalloutAdd(gFilterEngine, &gAleAuthConnectV6ManagementCallout, NULL, &gAleAuthConnectV6ManagementCallout.calloutId);
    if (!NT_SUCCESS(status))
    {
        RtlZeroMemory(&gAleAuthConnectV6ManagementCallout, sizeof(gAleAuthConnectV6ManagementCallout));
        goto CleanUp;
    }

    status = FwpsCalloutRegister(gNetworkDeviceObject, &gAleAuthConnectV6StateCallout, &gAleAuthConnectV6StateCalloutID);
    if (!NT_SUCCESS(status))
    {
        RtlZeroMemory(&gAleAuthConnectV6StateCallout, sizeof(gAleAuthConnectV6StateCallout));
        gAleAuthConnectV6StateCalloutID = 0;

        goto CleanUp;
    }

    gAleAuthConnectV6Filter.action.calloutKey = gAleAuthConnectV6ManagementCallout.calloutKey;
    gAleAuthConnectV6Filter.action.type = FWP_ACTION_CALLOUT_INSPECTION;
    gAleAuthConnectV6Filter.layerKey = gAleAuthConnectV6ManagementCallout.applicableLayer;
    gAleAuthConnectV6Filter.subLayerKey = gFwpFilterSubLayer.subLayerKey;
    status = FwpmFilterAdd(gFilterEngine, &gAleAuthConnectV6Filter, NULL, &gAleAuthConnectV6Filter.filterId);
    if (!NT_SUCCESS(status))
    {
        RtlZeroMemory(&gAleAuthConnectV6Filter, sizeof(gAleAuthConnectV6Filter));
        goto CleanUp;
    }

    /* TODO - Completed now - register for FWPM_LAYER_ALE_AUTH_RECV_ACCEPT_V6 */
    gAleAuthReceiveAcceptV6ManagementCallout.applicableLayer = FWPM_LAYER_ALE_AUTH_RECV_ACCEPT_V6;
    status = FwpmCalloutAdd(gFilterEngine, &gAleAuthReceiveAcceptV6ManagementCallout, NULL, &gAleAuthReceiveAcceptV6ManagementCallout.calloutId);
    if (NT_NOT_SUCCESS(status))
    {
        RtlZeroMemory(&gAleAuthReceiveAcceptV6ManagementCallout, sizeof(gAleAuthReceiveAcceptV6ManagementCallout));
        goto CleanUp;
    }

    status = FwpsCalloutRegister(gNetworkDeviceObject, &gAleAuthReceiveAcceptV6StateCallout, &gAleAuthReceiveAcceptV6StateCalloutID);
    if (NT_NOT_SUCCESS(status))
    {
        RtlZeroMemory(&gAleAuthReceiveAcceptV6StateCallout, sizeof(gAleAuthReceiveAcceptV6StateCallout));
        gAleAuthReceiveAcceptV6StateCalloutID = 0;

        goto CleanUp;
    }

    gAleAuthReceiveAcceptV6Filter.action.calloutKey = gAleAuthReceiveAcceptV6ManagementCallout.calloutKey;
    gAleAuthReceiveAcceptV6Filter.action.type = FWP_ACTION_CALLOUT_INSPECTION;
    gAleAuthReceiveAcceptV6Filter.layerKey = gAleAuthReceiveAcceptV6ManagementCallout.applicableLayer;
    gAleAuthReceiveAcceptV6Filter.subLayerKey = gFwpFilterSubLayer.subLayerKey;
    status = FwpmFilterAdd(gFilterEngine, &gAleAuthReceiveAcceptV6Filter, NULL, &gAleAuthReceiveAcceptV6Filter.filterId);
    if (!NT_SUCCESS(status))
    {
        RtlZeroMemory(&gAleAuthReceiveAcceptV6Filter, sizeof(gAleAuthReceiveAcceptV6Filter));
        goto CleanUp;
    }

CleanUp:
    if (!NT_SUCCESS(status))
    {
        DriverUnregisterNetworkFilter();
    }
    return status;
}


/*
* Unregisters my filters: gAleAuth{Connect|ReceiveAccept}V{4|6}Filter
*/
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

    /* [BIFAT] TODO: unregister for FWPM_LAYER_ALE_AUTH_RECV_ACCEPT_V4  */
    if (gAleAuthReceiveAcceptV4Filter.filterId != 0)
    {
        FwpmFilterDeleteById(gFilterEngine, gAleAuthReceiveAcceptV4Filter.filterId);
        RtlZeroMemory(&gAleAuthReceiveAcceptV4Filter, sizeof(gAleAuthReceiveAcceptV4Filter));
    }
    if (gAleAuthReceiveAcceptV4StateCalloutID != 0)
    {
        FwpsCalloutUnregisterById(gAleAuthReceiveAcceptV4StateCalloutID);
        gAleAuthReceiveAcceptV4StateCalloutID = 0;
    }
    if (gAleAuthReceiveAcceptV4ManagementCallout.calloutId != 0)
    {
        FwpmCalloutDeleteById(gFilterEngine, gAleAuthReceiveAcceptV4ManagementCallout.calloutId);
        RtlZeroMemory(&gAleAuthReceiveAcceptV4ManagementCallout, sizeof(gAleAuthReceiveAcceptV4ManagementCallout));
    }

    /* [BIFAT] TODO: unregister for FWPM_LAYER_ALE_AUTH_CONNECT_V6      */
    if (gAleAuthConnectV6Filter.filterId != 0)
    {
        FwpmFilterDeleteById(gFilterEngine, gAleAuthConnectV6Filter.filterId);
        RtlZeroMemory(&gAleAuthConnectV6Filter, sizeof(gAleAuthConnectV6Filter));
    }
    if (gAleAuthConnectV6StateCalloutID != 0)
    {
        FwpsCalloutUnregisterById(gAleAuthConnectV6StateCalloutID);
        gAleAuthConnectV6StateCalloutID = 0;
    }
    if (gAleAuthConnectV6ManagementCallout.calloutId != 0)
    {
        FwpmCalloutDeleteById(gFilterEngine, gAleAuthConnectV6ManagementCallout.calloutId);
        RtlZeroMemory(&gAleAuthConnectV6ManagementCallout, sizeof(gAleAuthConnectV6ManagementCallout));
    }

    /* [BIFAT] TODO: unregister for FWPM_LAYER_ALE_AUTH_RECV_ACCEPT_V6  */
    if (gAleAuthReceiveAcceptV6Filter.filterId != 0)
    {
        FwpmFilterDeleteById(gFilterEngine, gAleAuthReceiveAcceptV6Filter.filterId);
        RtlZeroMemory(&gAleAuthReceiveAcceptV6Filter, sizeof(gAleAuthReceiveAcceptV6Filter));
    }
    if (gAleAuthReceiveAcceptV6StateCalloutID != 0)
    {
        FwpsCalloutUnregisterById(gAleAuthReceiveAcceptV6StateCalloutID);
        gAleAuthReceiveAcceptV6StateCalloutID = 0;
    }
    if (gAleAuthReceiveAcceptV6ManagementCallout.calloutId != 0)
    {
        FwpmCalloutDeleteById(gFilterEngine, gAleAuthReceiveAcceptV6ManagementCallout.calloutId);
        RtlZeroMemory(&gAleAuthReceiveAcceptV6ManagementCallout, sizeof(gAleAuthReceiveAcceptV6ManagementCallout));
    }

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
