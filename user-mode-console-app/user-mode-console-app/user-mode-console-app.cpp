//
//   Copyright (C) 2018 BitDefender S.R.L.
//   Author(s)    : Radu PORTASE(rportase@bitdefender.com)
//
#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>
#include <Ws2tcpip.h>
#include <fwpmu.h>
#include <malloc.h>


#pragma comment (lib, "Fwpuclnt.lib")
#pragma comment (lib, "Ws2_32.lib")

#define OPT_INAVLID	0
#define OPT_LIST	1
#define OPT_ADD		2
#define OPT_DELETE	3
#define OPT_HELP	128

void PrintHelp(char* ExecutableName)
{
    printf("%s help|list|add|del\n", ExecutableName);
    printf("\n");
    printf("	help\n");
    printf("	list\n");
    printf("	add <process>|* <domain>|*\n");
    printf("	del <filter-id>\n");
}

DWORD AddRule(HANDLE hEngine, wchar_t* Process, wchar_t* Domain)
{
    DWORD error = 0;
    FWPM_FILTER filter = { 0 };
    FWPM_FILTER_CONDITION conditions[4] = { 0 };
    FWP_V4_ADDR_AND_MASK addrAndMask = { 0 };
    FWP_BYTE_BLOB* pAppId = NULL;
    ADDRINFOW getAddrHints = { 0 };
    ADDRINFOW* plIpAddrInfo = NULL;
    struct sockaddr_in* pSockAddr = NULL;
    DWORD ipAddress = 0;

    /* Translate the full file name to the app id.*/
    /* Example: "C:\Program Files\Mozilla Firefox\firefox.exe" to <unique id> */
    error = FwpmGetAppIdFromFileName(Process, &pAppId);
    if (ERROR_SUCCESS != error)
    {
        printf("FwpmGetAppIdFromFileName returned: %d!\n", error);
        wprintf(L"Application name: %s\n", Process);
        goto cleanup;
    }

    /* Translate the domain to the IP address. */
    /* Example: ro.wikipedia.org to 185.15.59.224. */
    memset(&getAddrHints, 0, sizeof(ADDRINFOW));
    getAddrHints.ai_family = AF_INET;
    error = GetAddrInfoW(Domain,
        NULL,
        &getAddrHints,
        &plIpAddrInfo);
    if (0 != error || AF_INET != plIpAddrInfo->ai_family)
    {
        printf("GetAddrInfoW returned: %d!\n", error);
        wprintf(L"Domain name:%s\n", Domain);
        goto cleanup;
    }

    /* We need to convert from network to host byte ordering. */
    pSockAddr = (struct sockaddr_in*)plIpAddrInfo->ai_addr;
    ipAddress = ntohl(pSockAddr->sin_addr.S_un.S_addr);
    printf("IP: %d.%d.%d.%d\n",
        ipAddress >> 24,
        (ipAddress & 0x00FF0000) >> 16,
        (ipAddress & 0x0000FF00) >> 8,
        (ipAddress & 0x000000FF));

    /* We'll create a blocking condition. */
    /* First rule is to block by APP ID - we want to deny firefox.exe access. */
    /* We converted firefox.exe to APP ID above - so the first condition is for the process to match the app id.*/
    ZeroMemory(&conditions[0], sizeof(FWPM_FILTER_CONDITION));
    conditions[0].fieldKey = FWPM_CONDITION_ALE_APP_ID;
    conditions[0].matchType = FWP_MATCH_EQUAL;
    conditions[0].conditionValue.type = FWP_BYTE_BLOB_TYPE;
    conditions[0].conditionValue.byteBlob = pAppId;

    printf("Domain %ws not blocked. This is your homework :).\n", Domain);

    /* Now we have the blocking condition (firefox will not be allowed access to wikipedia) */
    /* Now we need to add a filter to the connect layer to describe these two conditions. */
    /* The display name will be accessible by domain (so in our example "wikipedia") - you can use list to enumerate and find it.*/
    ZeroMemory(&filter, sizeof(FWPM_FILTER));
    filter.displayData.name = Domain;
    filter.displayData.description = NULL;
    filter.layerKey = FWPM_LAYER_ALE_AUTH_CONNECT_V4;
    filter.weight.type = FWP_EMPTY;
    filter.numFilterConditions = 1;
    filter.filterCondition = conditions;
    filter.action.type = FWP_ACTION_BLOCK;

    error = FwpmFilterAdd(hEngine, &filter, NULL, NULL);
    if (ERROR_SUCCESS != error)
    {
        printf("FwpmFilterAdd returned: %d!\n", error);
    }
cleanup:
    if (NULL != pAppId)
    {
        FwpmFreeMemory((void**)&pAppId);
    }
    if (NULL != plIpAddrInfo)
    {
        FreeAddrInfoW(plIpAddrInfo);
        plIpAddrInfo = NULL;
    }
    return error;
}

DWORD DeleteRule(HANDLE hEngine, UINT64 FilterId)
{
    /* Use list method do find the id. The id is the left hand side number. */
    /* 68104: Allow inbound UDP traffic to fdphost port 3702 */
    /* 68612: Wi-Fi Direct ASP Coordination Protocol (UDP-Out) */
    /* 68444: @FirewallAPI.dll,-80201 */
    /* 68094: DhcpFirewallPolicy */
    /* 68594: Network Discovery (SSDP-In) */
    /* 67979: Microsoft Edge (mDNS-In) */

    DWORD error = FwpmFilterDeleteById(hEngine, FilterId);
    if (ERROR_SUCCESS != error)
    {
        printf("FwpmFilterDeleteById returned: %d!\n", error);
    }

    return error;
}

DWORD ListRules(HANDLE hEngine)
{
    DWORD error = ERROR_SUCCESS;
    HANDLE hEnum = NULL;
    FWPM_FILTER** plEntries = NULL;
    UINT32 entriesReturned = 0;

    /* Enumerates all existing firewall rules on the machine. */
    error = FwpmFilterCreateEnumHandle(hEngine,
        NULL,
        &hEnum);
    if (ERROR_SUCCESS != error)
    {
        printf("FwpmFilterCreateEnumHandler returned: %d!\n", error);
        goto cleanup;
    }

    for (;;)
    {
        error = FwpmFilterEnum(hEngine,
            hEnum,
            1,
            &plEntries,
            &entriesReturned);
        if (ERROR_SUCCESS != error)
        {
            printf("FwpmFilterEnum returned: %d!\n", error);
            goto cleanup;
        }

        if (entriesReturned < 1 || NULL == plEntries)
        {
            break;
        }
        wprintf(L"%llu: %s\n", plEntries[0]->filterId, plEntries[0]->displayData.name);
        FwpmFreeMemory((void**)&plEntries);
    }

cleanup:
    if (NULL != hEnum)
    {
        FwpmFilterDestroyEnumHandle(hEngine, hEnum);
        hEnum = NULL;
    }
    return error;
}

int __cdecl main(int argc, char** argv)
{
    DWORD error, option;
    WORD wWsaVersion;
    WSADATA wsaData;
    HANDLE hEngine;
    BOOL wsaInitialized;

    error = ERROR_SUCCESS;
    option = OPT_INAVLID;
    hEngine = NULL;
    wWsaVersion = MAKEWORD(2, 2);
    wsaInitialized = FALSE;

    // Get option
    do
    {
        if (argc < 2 || 0 == strcmp("help", argv[1]))
        {
            option = OPT_HELP;
            break;
        }
        if (0 == strcmp("list", argv[1]))
        {
            option = OPT_LIST;
            break;
        }
        if (0 == strcmp("add", argv[1]))
        {
            option = OPT_ADD;
            break;
        }
        if (0 == strcmp("del", argv[1]))
        {
            option = OPT_DELETE;
            break;
        }
    } while (0);

    if (OPT_INAVLID == option)
    {
        printf("Unknown option: %s!\n", argv[1]);
        PrintHelp(argv[0]);
        return 1;
    }

    // Check input
    if (OPT_ADD == option && argc != 4)
    {
        printf("Not enough arguments for add!\n");
        return 1;
    }
    if (OPT_DELETE == option && argc != 3)
    {
        printf("Not enough arguments for delete!\n");
        return 1;
    }

    // Initialize WSA environment
    error = WSAStartup(wWsaVersion, &wsaData);
    if (0 != error)
    {
        printf("failed WSAStartup: %d!\n", error);
        goto cleanup;
    }
    wsaInitialized = TRUE;

    // Open engine
    error = FwpmEngineOpen(
        NULL,
        RPC_C_AUTHN_DEFAULT,
        NULL,
        NULL,
        &hEngine
    );

    if (ERROR_SUCCESS != error)
    {
        printf("Failed opening engine: %d!\n", error);
        goto cleanup;
    }

    switch (option)
    {
    case OPT_HELP:
    {
        PrintHelp(argv[0]);
        break;
    }
    case OPT_ADD:
    {
        wchar_t* process, * domain;
        size_t lenProcess, lenDomain;

        lenProcess = strlen(argv[2]);
        lenDomain = strlen(argv[3]);
        process = (wchar_t*)malloc((lenProcess + 1) * sizeof(wchar_t*));
        domain = (wchar_t*)malloc((lenDomain + 1) * sizeof(wchar_t*));
        mbstowcs(process, argv[2], lenProcess);
        process[lenProcess] = L'\x00';
        mbstowcs(domain, argv[3], lenDomain);
        domain[lenDomain] = L'\x00';

        error = AddRule(hEngine, process, domain);
        if (error)
        {
            printf("An error has occurred!\n");
        }

        free(domain);
        free(process);
        break;
    }
    case OPT_DELETE:
    {
        UINT64 filterId;

        filterId = atoll(argv[2]);
        error = DeleteRule(hEngine, filterId);
        if (error)
        {
            printf("An error has occured!\n");
        }

        break;
    }
    case OPT_LIST:
    {
        error = ListRules(hEngine);
        if (error)
        {
            printf("An error has occured!\n");
        }
        break;
    }
    default:
        printf("Command not yet implemented!\n");
    }

cleanup:
    if (NULL != hEngine)
    {
        FwpmEngineClose(hEngine);
        hEngine = NULL;
    }

    if (wsaInitialized)
    {
        WSACleanup();
    }

    return error;
}