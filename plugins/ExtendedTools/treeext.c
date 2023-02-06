/*
 * Process Hacker Extended Tools -
 *   process and network tree support
 *
 * Copyright (C) 2011 wj32
 *
 * This file is part of Process Hacker.
 *
 * Process Hacker is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Process Hacker is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Process Hacker.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "exttools.h"
#define CINTERFACE
#define COBJMACROS
#include <netfw.h>

LONG EtpProcessTreeNewSortFunction(
    _In_ PVOID Node1,
    _In_ PVOID Node2,
    _In_ ULONG SubId,
    _In_ PVOID Context
    );

LONG EtpNetworkTreeNewSortFunction(
    _In_ PVOID Node1,
    _In_ PVOID Node2,
    _In_ ULONG SubId,
    _In_ PVOID Context
    );

typedef struct _COLUMN_INFO
{
    ULONG SubId;
    PWSTR Text;
    ULONG Width;
    ULONG Alignment;
    ULONG TextFlags;
    BOOLEAN SortDescending;
} COLUMN_INFO, *PCOLUMN_INFO;

static ULONG ProcessTreeListSortColumn;
static PH_SORT_ORDER ProcessTreeListSortOrder;

static GUID IID_INetFwMgr_I = { 0xf7898af5, 0xcac4, 0x4632, { 0xa2, 0xec, 0xda, 0x06, 0xe5, 0x11, 0x1a, 0xf2 } };
static GUID CLSID_NetFwMgr_I = { 0x304ce942, 0x6e39, 0x40d8, { 0x94, 0x3a, 0xb9, 0x13, 0xc4, 0x0c, 0x9c, 0xd4 } };

VOID EtpAddTreeNewColumn(
    _In_ PPH_PLUGIN_TREENEW_INFORMATION TreeNewInfo,
    _In_ ULONG SubId,
    _In_ PWSTR Text,
    _In_ ULONG Width,
    _In_ ULONG Alignment,
    _In_ ULONG TextFlags,
    _In_ BOOLEAN SortDescending,
    _In_ PPH_PLUGIN_TREENEW_SORT_FUNCTION SortFunction
    )
{
    PH_TREENEW_COLUMN column;

    memset(&column, 0, sizeof(PH_TREENEW_COLUMN));
    column.SortDescending = SortDescending;
    column.Text = Text;
    column.Width = Width;
    column.Alignment = Alignment;
    column.TextFlags = TextFlags;

    PhPluginAddTreeNewColumn(
        PluginInstance,
        TreeNewInfo->CmData,
        &column,
        SubId,
        NULL,
        SortFunction
        );
}

VOID EtProcessTreeNewInitializing(
    _In_ PVOID Parameter
    )
{
    static COLUMN_INFO columns[] =
    {
        { ETPRTNC_DISKREADS, L"Disk Reads", 70, PH_ALIGN_RIGHT, DT_RIGHT, TRUE },
        { ETPRTNC_DISKWRITES, L"Disk Writes", 70, PH_ALIGN_RIGHT, DT_RIGHT, TRUE },
        { ETPRTNC_DISKREADBYTES, L"Disk Read Bytes", 70, PH_ALIGN_RIGHT, DT_RIGHT, TRUE },
        { ETPRTNC_DISKWRITEBYTES, L"Disk Write Bytes", 70, PH_ALIGN_RIGHT, DT_RIGHT, TRUE },
        { ETPRTNC_DISKTOTALBYTES, L"Disk Total Bytes", 70, PH_ALIGN_RIGHT, DT_RIGHT, TRUE },
        { ETPRTNC_DISKREADSDELTA, L"Disk Reads Delta", 70, PH_ALIGN_RIGHT, DT_RIGHT, TRUE },
        { ETPRTNC_DISKWRITESDELTA, L"Disk Writes Delta", 70, PH_ALIGN_RIGHT, DT_RIGHT, TRUE },
        { ETPRTNC_DISKREADBYTESDELTA, L"Disk Read Bytes Delta", 70, PH_ALIGN_RIGHT, DT_RIGHT, TRUE },
        { ETPRTNC_DISKWRITEBYTESDELTA, L"Disk Write Bytes Delta", 70, PH_ALIGN_RIGHT, DT_RIGHT, TRUE },
        { ETPRTNC_DISKTOTALBYTESDELTA, L"Disk Total Bytes Delta", 70, PH_ALIGN_RIGHT, DT_RIGHT, TRUE },
        { ETPRTNC_NETWORKRECEIVES, L"Network Receives", 70, PH_ALIGN_RIGHT, DT_RIGHT, TRUE },
        { ETPRTNC_NETWORKSENDS, L"Network Sends", 70, PH_ALIGN_RIGHT, DT_RIGHT, TRUE },
        { ETPRTNC_NETWORKRECEIVEBYTES, L"Network Receive Bytes", 70, PH_ALIGN_RIGHT, DT_RIGHT, TRUE },
        { ETPRTNC_NETWORKSENDBYTES, L"Network Send Bytes", 70, PH_ALIGN_RIGHT, DT_RIGHT, TRUE },
        { ETPRTNC_NETWORKTOTALBYTES, L"Network Total Bytes", 70, PH_ALIGN_RIGHT, DT_RIGHT, TRUE },
        { ETPRTNC_NETWORKRECEIVESDELTA, L"Network Receives Delta", 70, PH_ALIGN_RIGHT, DT_RIGHT, TRUE },
        { ETPRTNC_NETWORKSENDSDELTA, L"Network Sends Delta", 70, PH_ALIGN_RIGHT, DT_RIGHT, TRUE },
        { ETPRTNC_NETWORKRECEIVEBYTESDELTA, L"Network Receive Bytes Delta", 70, PH_ALIGN_RIGHT, DT_RIGHT, TRUE },
        { ETPRTNC_NETWORKSENDBYTESDELTA, L"Network Send Bytes Delta", 70, PH_ALIGN_RIGHT, DT_RIGHT, TRUE },
        { ETPRTNC_NETWORKTOTALBYTESDELTA, L"Network Total Bytes Delta", 70, PH_ALIGN_RIGHT, DT_RIGHT, TRUE },
        { ETPRTNC_HARDFAULTS, L"Hard Faults", 70, PH_ALIGN_RIGHT, DT_RIGHT, TRUE },
        { ETPRTNC_HARDFAULTSDELTA, L"Hard Faults Delta", 70, PH_ALIGN_RIGHT, DT_RIGHT, TRUE },
        { ETPRTNC_PEAKTHREADS, L"Peak Threads", 45, PH_ALIGN_RIGHT, DT_RIGHT, TRUE },
        { ETPRTNC_GPU, L"GPU", 45, PH_ALIGN_RIGHT, DT_RIGHT, TRUE },
        { ETPRTNC_GPUDEDICATEDBYTES, L"GPU Dedicated Bytes", 70, PH_ALIGN_RIGHT, DT_RIGHT, TRUE },
        { ETPRTNC_GPUSHAREDBYTES, L"GPU Shared Bytes", 70, PH_ALIGN_RIGHT, DT_RIGHT, TRUE },
        { ETPRTNC_DISKREADRATE, L"Disk Read Rate", 70, PH_ALIGN_RIGHT, DT_RIGHT, TRUE },
        { ETPRTNC_DISKWRITERATE, L"Disk Write Rate", 70, PH_ALIGN_RIGHT, DT_RIGHT, TRUE },
        { ETPRTNC_DISKTOTALRATE, L"Disk Total Rate", 70, PH_ALIGN_RIGHT, DT_RIGHT, TRUE },
        { ETPRTNC_NETWORKRECEIVERATE, L"Network Receive Rate", 70, PH_ALIGN_RIGHT, DT_RIGHT, TRUE },
        { ETPRTNC_NETWORKSENDRATE, L"Network Send Rate", 70, PH_ALIGN_RIGHT, DT_RIGHT, TRUE },
        { ETPRTNC_NETWORKTOTALRATE, L"Network Total Rate", 70, PH_ALIGN_RIGHT, DT_RIGHT, TRUE }
    };

    PPH_PLUGIN_TREENEW_INFORMATION treeNewInfo = Parameter;
    ULONG i;

    for (i = 0; i < sizeof(columns) / sizeof(COLUMN_INFO); i++)
    {
        EtpAddTreeNewColumn(treeNewInfo, columns[i].SubId, columns[i].Text, columns[i].Width, columns[i].Alignment,
            columns[i].TextFlags, columns[i].SortDescending, EtpProcessTreeNewSortFunction);
    }

    PhPluginEnableTreeNewNotify(PluginInstance, treeNewInfo->CmData);
}

static FLOAT EtpCalculateInclusiveGpuUsage(
    _In_ PPH_PROCESS_NODE ProcessNode
    )
{
    FLOAT gpuUsage;
    ULONG i;

    gpuUsage = EtGetProcessBlock(ProcessNode->ProcessItem)->GpuNodeUsage;

    for (i = 0; i < ProcessNode->Children->Count; i++)
    {
        gpuUsage += EtpCalculateInclusiveGpuUsage(ProcessNode->Children->Items[i]);
    }

    return gpuUsage;
}

VOID EtProcessTreeNewMessage(
    _In_ PVOID Parameter
    )
{
    PPH_PLUGIN_TREENEW_MESSAGE message = Parameter;
    PPH_PROCESS_NODE processNode;
    PET_PROCESS_BLOCK block;

    if (message->Message == TreeNewGetCellText)
    {
        PPH_TREENEW_GET_CELL_TEXT getCellText = message->Parameter1;
        PPH_STRING text;

        processNode = (PPH_PROCESS_NODE)getCellText->Node;
        block = EtGetProcessBlock(processNode->ProcessItem);

        PhAcquireQueuedLockExclusive(&block->TextCacheLock);

        if (block->TextCacheValid[message->SubId])
        {
            if (block->TextCache[message->SubId])
                getCellText->Text = block->TextCache[message->SubId]->sr;
        }
        else
        {
            text = NULL;

            switch (message->SubId)
            {
            case ETPRTNC_DISKREADS:
                if (block->DiskReadCount != 0)
                    text = PhFormatUInt64(block->DiskReadCount, TRUE);
                break;
            case ETPRTNC_DISKWRITES:
                if (block->DiskWriteCount != 0)
                    text = PhFormatUInt64(block->DiskWriteCount, TRUE);
                break;
            case ETPRTNC_DISKREADBYTES:
                if (block->DiskReadRaw != 0)
                    text = PhFormatSize(block->DiskReadRaw, -1);
                break;
            case ETPRTNC_DISKWRITEBYTES:
                if (block->DiskWriteRaw != 0)
                    text = PhFormatSize(block->DiskWriteRaw, -1);
                break;
            case ETPRTNC_DISKTOTALBYTES:
                if (block->DiskReadRaw + block->DiskWriteRaw != 0)
                    text = PhFormatSize(block->DiskReadRaw + block->DiskWriteRaw, -1);
                break;
            case ETPRTNC_DISKREADSDELTA:
                if (block->DiskReadDelta.Delta != 0)
                    text = PhFormatUInt64(block->DiskReadDelta.Delta, TRUE);
                break;
            case ETPRTNC_DISKWRITESDELTA:
                if (block->DiskWriteDelta.Delta != 0)
                    text = PhFormatUInt64(block->DiskWriteDelta.Delta, TRUE);
                break;
            case ETPRTNC_DISKREADBYTESDELTA:
                if (block->DiskReadRawDelta.Delta != 0)
                    text = PhFormatSize(block->DiskReadRawDelta.Delta, -1);
                break;
            case ETPRTNC_DISKWRITEBYTESDELTA:
                if (block->DiskWriteRawDelta.Delta != 0)
                    text = PhFormatSize(block->DiskWriteRawDelta.Delta, -1);
                break;
            case ETPRTNC_DISKTOTALBYTESDELTA:
                if (block->DiskReadRawDelta.Delta + block->DiskWriteRawDelta.Delta != 0)
                    text = PhFormatSize(block->DiskReadRawDelta.Delta + block->DiskWriteRawDelta.Delta, -1);
                break;
            case ETPRTNC_NETWORKRECEIVES:
                if (block->NetworkReceiveCount != 0)
                    text = PhFormatUInt64(block->NetworkReceiveCount, TRUE);
                break;
            case ETPRTNC_NETWORKSENDS:
                if (block->NetworkSendCount != 0)
                    text = PhFormatUInt64(block->NetworkSendCount, TRUE);
                break;
            case ETPRTNC_NETWORKRECEIVEBYTES:
                if (block->NetworkReceiveRaw != 0)
                    text = PhFormatSize(block->NetworkReceiveRaw, -1);
                break;
            case ETPRTNC_NETWORKSENDBYTES:
                if (block->NetworkSendRaw != 0)
                    text = PhFormatSize(block->NetworkSendRaw, -1);
                break;
            case ETPRTNC_NETWORKTOTALBYTES:
                if (block->NetworkReceiveRaw + block->NetworkSendRaw != 0)
                    text = PhFormatSize(block->NetworkReceiveRaw + block->NetworkSendRaw, -1);
                break;
            case ETPRTNC_NETWORKRECEIVESDELTA:
                if (block->NetworkReceiveDelta.Delta != 0)
                    text = PhFormatUInt64(block->NetworkReceiveDelta.Delta, TRUE);
                break;
            case ETPRTNC_NETWORKSENDSDELTA:
                if (block->NetworkSendDelta.Delta != 0)
                    text = PhFormatUInt64(block->NetworkSendDelta.Delta, TRUE);
                break;
            case ETPRTNC_NETWORKRECEIVEBYTESDELTA:
                if (block->NetworkReceiveRawDelta.Delta != 0)
                    text = PhFormatSize(block->NetworkReceiveRawDelta.Delta, -1);
                break;
            case ETPRTNC_NETWORKSENDBYTESDELTA:
                if (block->NetworkSendRawDelta.Delta != 0)
                    text = PhFormatSize(block->NetworkSendRawDelta.Delta, -1);
                break;
            case ETPRTNC_NETWORKTOTALBYTESDELTA:
                if (block->NetworkReceiveRawDelta.Delta + block->NetworkSendRawDelta.Delta != 0)
                    text = PhFormatSize(block->NetworkReceiveRawDelta.Delta + block->NetworkSendRawDelta.Delta, -1);
                break;
            case ETPRTNC_HARDFAULTS:
                text = PhFormatUInt64(block->HardFaultsDelta.Value, TRUE);
                break;
            case ETPRTNC_HARDFAULTSDELTA:
                if (block->HardFaultsDelta.Delta != 0)
                    text = PhFormatUInt64(block->HardFaultsDelta.Delta, TRUE);
                break;
            case ETPRTNC_PEAKTHREADS:
                text = PhFormatUInt64(block->ProcessItem->PeakNumberOfThreads, TRUE);
                break;
            case ETPRTNC_GPU:
                {
                    FLOAT gpuUsage;

                    if (!PhGetIntegerSetting(L"PropagateCpuUsage") || processNode->Node.Expanded || ProcessTreeListSortOrder != NoSortOrder)
                    {
                        gpuUsage = block->GpuNodeUsage * 100;
                    }
                    else
                    {
                        gpuUsage = EtpCalculateInclusiveGpuUsage(processNode) * 100;
                    }

                    if (gpuUsage >= 0.01)
                    {
                        PH_FORMAT format;

                        PhInitFormatF(&format, gpuUsage, 2);
                        text = PhFormat(&format, 1, 0);
                    }
                }
                break;
            case ETPRTNC_GPUDEDICATEDBYTES:
                if (block->GpuDedicatedUsage != 0)
                    text = PhFormatSize(block->GpuDedicatedUsage, -1);
                break;
            case ETPRTNC_GPUSHAREDBYTES:
                if (block->GpuSharedUsage != 0)
                    text = PhFormatSize(block->GpuSharedUsage, -1);
                break;
            case ETPRTNC_DISKREADRATE:
                if (block->DiskReadRawDelta.Delta != 0)
                    EtFormatRate(block->DiskReadRawDelta.Delta, &text, NULL);
                break;
            case ETPRTNC_DISKWRITERATE:
                if (block->DiskWriteRawDelta.Delta != 0)
                    EtFormatRate(block->DiskWriteRawDelta.Delta, &text, NULL);
                break;
            case ETPRTNC_DISKTOTALRATE:
                if (block->DiskReadRawDelta.Delta + block->DiskWriteRawDelta.Delta != 0)
                    EtFormatRate(block->DiskReadRawDelta.Delta + block->DiskWriteRawDelta.Delta, &text, NULL);
                break;
            case ETPRTNC_NETWORKRECEIVERATE:
                if (block->NetworkReceiveRawDelta.Delta != 0)
                    EtFormatRate(block->NetworkReceiveRawDelta.Delta, &text, NULL);
                break;
            case ETPRTNC_NETWORKSENDRATE:
                if (block->NetworkSendRawDelta.Delta != 0)
                    EtFormatRate(block->NetworkSendRawDelta.Delta, &text, NULL);
                break;
            case ETPRTNC_NETWORKTOTALRATE:
                if (block->NetworkReceiveRawDelta.Delta + block->NetworkSendRawDelta.Delta != 0)
                    EtFormatRate(block->NetworkReceiveRawDelta.Delta + block->NetworkSendRawDelta.Delta, &text, NULL);
                break;
            }

            if (text)
            {
                getCellText->Text = text->sr;
            }

            PhMoveReference(&block->TextCache[message->SubId], text);
            block->TextCacheValid[message->SubId] = TRUE;
        }

        PhReleaseQueuedLockExclusive(&block->TextCacheLock);
    }
    else if (message->Message == TreeNewSortChanged)
    {
        TreeNew_GetSort(message->TreeNewHandle, &ProcessTreeListSortColumn, &ProcessTreeListSortOrder);
    }
    else if (message->Message == TreeNewNodeExpanding)
    {
        processNode = message->Parameter1;
        block = EtGetProcessBlock(processNode->ProcessItem);

        if (PhGetIntegerSetting(L"PropagateCpuUsage"))
            block->TextCacheValid[ETPRTNC_GPU] = FALSE;
    }
}

LONG EtpProcessTreeNewSortFunction(
    _In_ PVOID Node1,
    _In_ PVOID Node2,
    _In_ ULONG SubId,
    _In_ PVOID Context
    )
{
    LONG result;
    PPH_PROCESS_NODE node1 = Node1;
    PPH_PROCESS_NODE node2 = Node2;
    PET_PROCESS_BLOCK block1;
    PET_PROCESS_BLOCK block2;

    block1 = EtGetProcessBlock(node1->ProcessItem);
    block2 = EtGetProcessBlock(node2->ProcessItem);

    result = 0;

    switch (SubId)
    {
    case ETPRTNC_DISKREADS:
        result = uint64cmp(block1->DiskReadCount, block2->DiskReadCount);
        break;
    case ETPRTNC_DISKWRITES:
        result = uint64cmp(block1->DiskWriteCount, block2->DiskWriteCount);
        break;
    case ETPRTNC_DISKREADBYTES:
        result = uint64cmp(block1->DiskReadRaw, block2->DiskReadRaw);
        break;
    case ETPRTNC_DISKWRITEBYTES:
        result = uint64cmp(block1->DiskWriteRaw, block2->DiskWriteRaw);
        break;
    case ETPRTNC_DISKTOTALBYTES:
        result = uint64cmp(block1->DiskReadRaw + block1->DiskWriteRaw, block2->DiskReadRaw + block2->DiskWriteRaw);
        break;
    case ETPRTNC_DISKREADSDELTA:
        result = uint64cmp(block1->DiskReadDelta.Delta, block2->DiskReadDelta.Delta);
        break;
    case ETPRTNC_DISKWRITESDELTA:
        result = uint64cmp(block1->DiskWriteDelta.Delta, block2->DiskWriteDelta.Delta);
        break;
    case ETPRTNC_DISKREADBYTESDELTA:
        result = uint64cmp(block1->DiskReadRawDelta.Delta, block2->DiskReadRawDelta.Delta);
        break;
    case ETPRTNC_DISKWRITEBYTESDELTA:
        result = uint64cmp(block1->DiskWriteRawDelta.Delta, block2->DiskWriteRawDelta.Delta);
        break;
    case ETPRTNC_DISKTOTALBYTESDELTA:
        result = uint64cmp(block1->DiskReadRawDelta.Delta + block1->DiskWriteRawDelta.Delta, block2->DiskReadRawDelta.Delta + block2->DiskWriteRawDelta.Delta);
        break;
    case ETPRTNC_NETWORKRECEIVES:
        result = uint64cmp(block1->NetworkReceiveCount, block2->NetworkReceiveCount);
        break;
    case ETPRTNC_NETWORKSENDS:
        result = uint64cmp(block1->NetworkSendCount, block2->NetworkSendCount);
        break;
    case ETPRTNC_NETWORKRECEIVEBYTES:
        result = uint64cmp(block1->NetworkReceiveRaw, block2->NetworkReceiveRaw);
        break;
    case ETPRTNC_NETWORKSENDBYTES:
        result = uint64cmp(block1->NetworkSendRaw, block2->NetworkSendRaw);
        break;
    case ETPRTNC_NETWORKTOTALBYTES:
        result = uint64cmp(block1->NetworkReceiveRaw + block1->NetworkSendRaw, block2->NetworkReceiveRaw + block2->NetworkSendRaw);
        break;
    case ETPRTNC_NETWORKRECEIVESDELTA:
        result = uint64cmp(block1->NetworkReceiveDelta.Delta, block2->NetworkReceiveDelta.Delta);
        break;
    case ETPRTNC_NETWORKSENDSDELTA:
        result = uint64cmp(block1->NetworkSendDelta.Delta, block2->NetworkSendDelta.Delta);
        break;
    case ETPRTNC_NETWORKRECEIVEBYTESDELTA:
        result = uint64cmp(block1->NetworkReceiveRawDelta.Delta, block2->NetworkReceiveRawDelta.Delta);
        break;
    case ETPRTNC_NETWORKSENDBYTESDELTA:
        result = uint64cmp(block1->NetworkSendRawDelta.Delta, block2->NetworkSendRawDelta.Delta);
        break;
    case ETPRTNC_NETWORKTOTALBYTESDELTA:
        result = uint64cmp(block1->NetworkReceiveRawDelta.Delta + block1->NetworkSendRawDelta.Delta, block2->NetworkReceiveRawDelta.Delta + block2->NetworkSendRawDelta.Delta);
        break;
    case ETPRTNC_HARDFAULTS:
        result = uintcmp(block1->HardFaultsDelta.Value, block2->HardFaultsDelta.Value);
        break;
    case ETPRTNC_HARDFAULTSDELTA:
        result = uintcmp(block1->HardFaultsDelta.Delta, block2->HardFaultsDelta.Delta);
        break;
    case ETPRTNC_PEAKTHREADS:
        result = uintcmp(block1->ProcessItem->PeakNumberOfThreads, block2->ProcessItem->PeakNumberOfThreads);
        break;
    case ETPRTNC_GPU:
        result = singlecmp(block1->GpuNodeUsage, block2->GpuNodeUsage);
        break;
    case ETPRTNC_GPUDEDICATEDBYTES:
        result = uint64cmp(block1->GpuDedicatedUsage, block2->GpuDedicatedUsage);
        break;
    case ETPRTNC_GPUSHAREDBYTES:
        result = uint64cmp(block1->GpuSharedUsage, block2->GpuSharedUsage);
        break;
    case ETPRTNC_DISKREADRATE:
        result = uint64cmp(block1->DiskReadRawDelta.Delta, block2->DiskReadRawDelta.Delta);
        break;
    case ETPRTNC_DISKWRITERATE:
        result = uint64cmp(block1->DiskWriteRawDelta.Delta, block2->DiskWriteRawDelta.Delta);
        break;
    case ETPRTNC_DISKTOTALRATE:
        result = uint64cmp(block1->DiskReadRawDelta.Delta + block1->DiskWriteRawDelta.Delta, block2->DiskReadRawDelta.Delta + block2->DiskWriteRawDelta.Delta);
        break;
    case ETPRTNC_NETWORKRECEIVERATE:
        result = uint64cmp(block1->NetworkReceiveRawDelta.Delta, block2->NetworkReceiveRawDelta.Delta);
        break;
    case ETPRTNC_NETWORKSENDRATE:
        result = uint64cmp(block1->NetworkSendRawDelta.Delta, block2->NetworkSendRawDelta.Delta);
        break;
    case ETPRTNC_NETWORKTOTALRATE:
        result = uint64cmp(block1->NetworkReceiveRawDelta.Delta + block1->NetworkSendRawDelta.Delta, block2->NetworkReceiveRawDelta.Delta + block2->NetworkSendRawDelta.Delta);
        break;
    }

    return result;
}

VOID EtNetworkTreeNewInitializing(
    _In_ PVOID Parameter
    )
{
    static COLUMN_INFO columns[] =
    {
        { ETNETNC_RECEIVES, L"Receives", 70, PH_ALIGN_RIGHT, DT_RIGHT, TRUE },
        { ETNETNC_SENDS, L"Sends", 70, PH_ALIGN_RIGHT, DT_RIGHT, TRUE },
        { ETNETNC_RECEIVEBYTES, L"Receive Bytes", 70, PH_ALIGN_RIGHT, DT_RIGHT, TRUE },
        { ETNETNC_SENDBYTES, L"Send Bytes", 70, PH_ALIGN_RIGHT, DT_RIGHT, TRUE },
        { ETNETNC_TOTALBYTES, L"Total Bytes", 70, PH_ALIGN_RIGHT, DT_RIGHT, TRUE },
        { ETNETNC_RECEIVESDELTA, L"Receives Delta", 70, PH_ALIGN_RIGHT, DT_RIGHT, TRUE },
        { ETNETNC_SENDSDELTA, L"Sends Delta", 70, PH_ALIGN_RIGHT, DT_RIGHT, TRUE },
        { ETNETNC_RECEIVEBYTESDELTA, L"Receive Bytes Delta", 70, PH_ALIGN_RIGHT, DT_RIGHT, TRUE },
        { ETNETNC_SENDBYTESDELTA, L"Send Bytes Delta", 70, PH_ALIGN_RIGHT, DT_RIGHT, TRUE },
        { ETNETNC_TOTALBYTESDELTA, L"Total Bytes Delta", 70, PH_ALIGN_RIGHT, DT_RIGHT, TRUE },
        { ETNETNC_FIREWALLSTATUS, L"Firewall Status", 170, PH_ALIGN_LEFT, 0, FALSE },
        { ETNETNC_RECEIVERATE, L"Receive Rate", 70, PH_ALIGN_RIGHT, DT_RIGHT, TRUE },
        { ETNETNC_SENDRATE, L"Send Rate", 70, PH_ALIGN_RIGHT, DT_RIGHT, TRUE },
        { ETNETNC_TOTALRATE, L"Total Rate", 70, PH_ALIGN_RIGHT, DT_RIGHT, TRUE }
    };

    PPH_PLUGIN_TREENEW_INFORMATION treeNewInfo = Parameter;
    ULONG i;

    for (i = 0; i < sizeof(columns) / sizeof(COLUMN_INFO); i++)
    {
        EtpAddTreeNewColumn(treeNewInfo, columns[i].SubId, columns[i].Text, columns[i].Width, columns[i].Alignment,
            columns[i].TextFlags, columns[i].SortDescending, EtpNetworkTreeNewSortFunction);
    }
}

VOID EtpUpdateFirewallStatus(
    _Inout_ PET_NETWORK_BLOCK Block
    )
{
    if (!Block->FirewallStatusValid)
    {
        Block->FirewallStatus = EtQueryFirewallStatus(Block->NetworkItem);
        Block->FirewallStatusValid = TRUE;
    }
}

VOID EtNetworkTreeNewMessage(
    _In_ PVOID Parameter
    )
{
    PPH_PLUGIN_TREENEW_MESSAGE message = Parameter;

    if (message->Message == TreeNewGetCellText)
    {
        PPH_TREENEW_GET_CELL_TEXT getCellText = message->Parameter1;
        PPH_NETWORK_NODE networkNode = (PPH_NETWORK_NODE)getCellText->Node;
        PET_NETWORK_BLOCK block;
        PPH_STRING text;

        block = EtGetNetworkBlock(networkNode->NetworkItem);

        PhAcquireQueuedLockExclusive(&block->TextCacheLock);

        if (block->TextCacheValid[message->SubId])
        {
            if (block->TextCache[message->SubId])
                getCellText->Text = block->TextCache[message->SubId]->sr;
        }
        else
        {
            text = NULL;

            switch (message->SubId)
            {
            case ETNETNC_RECEIVES:
                if (block->ReceiveCount != 0)
                    text = PhFormatUInt64(block->ReceiveCount, TRUE);
                break;
            case ETNETNC_SENDS:
                if (block->SendCount != 0)
                    text = PhFormatUInt64(block->SendCount, TRUE);
                break;
            case ETNETNC_RECEIVEBYTES:
                if (block->ReceiveRaw != 0)
                    text = PhFormatSize(block->ReceiveRaw, -1);
                break;
            case ETNETNC_SENDBYTES:
                if (block->SendRaw != 0)
                    text = PhFormatSize(block->SendRaw, -1);
                break;
            case ETNETNC_TOTALBYTES:
                if (block->ReceiveRaw + block->SendRaw != 0)
                    text = PhFormatSize(block->ReceiveRaw + block->SendRaw, -1);
                break;
            case ETNETNC_RECEIVESDELTA:
                if (block->ReceiveDelta.Delta != 0)
                    text = PhFormatUInt64(block->ReceiveDelta.Delta, TRUE);
                break;
            case ETNETNC_SENDSDELTA:
                if (block->SendDelta.Delta != 0)
                    text = PhFormatUInt64(block->SendDelta.Delta, TRUE);
                break;
            case ETNETNC_RECEIVEBYTESDELTA:
                if (block->ReceiveRawDelta.Delta != 0)
                    text = PhFormatSize(block->ReceiveRawDelta.Delta, -1);
                break;
            case ETNETNC_SENDBYTESDELTA:
                if (block->SendRawDelta.Delta != 0)
                    text = PhFormatSize(block->SendRawDelta.Delta, -1);
                break;
            case ETNETNC_TOTALBYTESDELTA:
                if (block->ReceiveRawDelta.Delta + block->SendRawDelta.Delta != 0)
                    text = PhFormatSize(block->ReceiveRawDelta.Delta + block->SendRawDelta.Delta, -1);
                break;
            case ETNETNC_FIREWALLSTATUS:
                {
                    static PPH_STRING strings[FirewallMaximumStatus];
                    static PH_INITONCE initOnce = PH_INITONCE_INIT;

                    if (PhBeginInitOnce(&initOnce))
                    {
                        strings[FirewallUnknownStatus] = NULL;
                        strings[FirewallAllowedNotRestricted] = PhCreateString(L"Allowed, not restricted");
                        strings[FirewallAllowedRestricted] = PhCreateString(L"Allowed, restricted");
                        strings[FirewallNotAllowedNotRestricted] = PhCreateString(L"Not allowed, not restricted");
                        strings[FirewallNotAllowedRestricted] = PhCreateString(L"Not allowed, restricted");
                        PhEndInitOnce(&initOnce);
                    }

                    EtpUpdateFirewallStatus(block);

                    if (block->FirewallStatus < FirewallMaximumStatus)
                        PhSetReference(&text, strings[block->FirewallStatus]);
                }
                break;
            case ETNETNC_RECEIVERATE:
                if (block->ReceiveRawDelta.Delta != 0)
                    EtFormatRate(block->ReceiveRawDelta.Delta, &text, NULL);
                break;
            case ETNETNC_SENDRATE:
                if (block->SendRawDelta.Delta != 0)
                    EtFormatRate(block->SendRawDelta.Delta, &text, NULL);
                break;
            case ETNETNC_TOTALRATE:
                if (block->ReceiveRawDelta.Delta + block->SendRawDelta.Delta != 0)
                    EtFormatRate(block->ReceiveRawDelta.Delta + block->SendRawDelta.Delta, &text, NULL);
                break;
            }

            if (text)
            {
                getCellText->Text = text->sr;
            }

            PhMoveReference(&block->TextCache[message->SubId], text);
            block->TextCacheValid[message->SubId] = TRUE;
        }

        PhReleaseQueuedLockExclusive(&block->TextCacheLock);
    }
}

LONG EtpNetworkTreeNewSortFunction(
    _In_ PVOID Node1,
    _In_ PVOID Node2,
    _In_ ULONG SubId,
    _In_ PVOID Context
    )
{
    LONG result;
    PPH_NETWORK_NODE node1 = Node1;
    PPH_NETWORK_NODE node2 = Node2;
    PET_NETWORK_BLOCK block1;
    PET_NETWORK_BLOCK block2;

    block1 = EtGetNetworkBlock(node1->NetworkItem);
    block2 = EtGetNetworkBlock(node2->NetworkItem);

    result = 0;

    switch (SubId)
    {
    case ETNETNC_RECEIVES:
        result = uint64cmp(block1->ReceiveCount, block2->ReceiveCount);
        break;
    case ETNETNC_SENDS:
        result = uint64cmp(block1->SendCount, block2->SendCount);
        break;
    case ETNETNC_RECEIVEBYTES:
        result = uint64cmp(block1->ReceiveRaw, block2->ReceiveRaw);
        break;
    case ETNETNC_SENDBYTES:
        result = uint64cmp(block1->SendRaw, block2->SendRaw);
        break;
    case ETNETNC_TOTALBYTES:
        result = uint64cmp(block1->ReceiveRaw + block1->SendRaw, block2->ReceiveRaw + block2->SendRaw);
        break;
    case ETNETNC_RECEIVESDELTA:
        result = uint64cmp(block1->ReceiveDelta.Delta, block2->ReceiveDelta.Delta);
        break;
    case ETNETNC_SENDSDELTA:
        result = uint64cmp(block1->SendDelta.Delta, block2->SendDelta.Delta);
        break;
    case ETNETNC_RECEIVEBYTESDELTA:
        result = uint64cmp(block1->ReceiveRawDelta.Delta, block2->ReceiveRawDelta.Delta);
        break;
    case ETNETNC_SENDBYTESDELTA:
        result = uint64cmp(block1->SendRawDelta.Delta, block2->SendRawDelta.Delta);
        break;
    case ETNETNC_TOTALBYTESDELTA:
        result = uint64cmp(block1->ReceiveRawDelta.Delta + block1->SendRawDelta.Delta, block2->ReceiveRawDelta.Delta + block2->SendRawDelta.Delta);
        break;
    case ETNETNC_FIREWALLSTATUS:
        EtpUpdateFirewallStatus(block1);
        EtpUpdateFirewallStatus(block2);
        result = intcmp(block1->FirewallStatus, block2->FirewallStatus);
        break;
    case ETNETNC_RECEIVERATE:
        result = uint64cmp(block1->ReceiveRawDelta.Delta, block2->ReceiveRawDelta.Delta);
        break;
    case ETNETNC_SENDRATE:
        result = uint64cmp(block1->SendRawDelta.Delta, block2->SendRawDelta.Delta);
        break;
    case ETNETNC_TOTALRATE:
        result = uint64cmp(block1->ReceiveRawDelta.Delta + block1->SendRawDelta.Delta, block2->ReceiveRawDelta.Delta + block2->SendRawDelta.Delta);
        break;
    }

    return result;
}

ET_FIREWALL_STATUS EtQueryFirewallStatus(
    _In_ PPH_NETWORK_ITEM NetworkItem
    )
{
    static INetFwMgr* manager = NULL;
    ET_FIREWALL_STATUS result;
    PPH_PROCESS_ITEM processItem;
    BSTR imageFileNameBStr;
    BSTR localAddressBStr;
    VARIANT allowed;
    VARIANT restricted;

    if (!manager)
    {
        if (!SUCCEEDED(CoCreateInstance(&CLSID_NetFwMgr_I, NULL, CLSCTX_INPROC_SERVER, &IID_INetFwMgr_I, &manager)))
            return FirewallUnknownStatus;

        if (!manager)
            return FirewallUnknownStatus;
    }

    processItem = PhReferenceProcessItem(NetworkItem->ProcessId);

    if (!processItem)
        return FirewallUnknownStatus;

    if (!processItem->FileName)
    {
        PhDereferenceObject(processItem);
        return FirewallUnknownStatus;
    }

    result = FirewallUnknownStatus;

    if (imageFileNameBStr = SysAllocStringLen(processItem->FileName->Buffer, (ULONG)processItem->FileName->Length / sizeof(WCHAR)))
    {
        localAddressBStr = NULL;

        if (!PhIsNullIpAddress(&NetworkItem->LocalEndpoint.Address))
            localAddressBStr = SysAllocString(NetworkItem->LocalAddressString);

        if (SUCCEEDED(INetFwMgr_IsPortAllowed(
            manager,
            imageFileNameBStr,
            (NetworkItem->ProtocolType & PH_IPV6_NETWORK_TYPE) ? NET_FW_IP_VERSION_V6 : NET_FW_IP_VERSION_V4,
            NetworkItem->LocalEndpoint.Port,
            localAddressBStr,
            (NetworkItem->ProtocolType & PH_UDP_PROTOCOL_TYPE) ? NET_FW_IP_PROTOCOL_UDP : NET_FW_IP_PROTOCOL_TCP,
            &allowed,
            &restricted
            )))
        {
            if (allowed.boolVal)
            {
                if (restricted.boolVal)
                    result = FirewallAllowedRestricted;
                else
                    result = FirewallAllowedNotRestricted;
            }
            else
            {
                if (restricted.boolVal)
                    result = FirewallNotAllowedRestricted;
                else
                    result = FirewallNotAllowedNotRestricted;
            }
        }

        if (localAddressBStr)
            SysFreeString(localAddressBStr);

        SysFreeString(imageFileNameBStr);
    }

    PhDereferenceObject(processItem);

    return result;
}
