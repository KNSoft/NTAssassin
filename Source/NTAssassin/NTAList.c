#include "NTAssassin.h"

VOID NTAPI Data_ListInit(PDATA_LIST DataList) {
    DataList->First = DataList->Last = NULL;
    DataList->Length = 0;
    if (DataList->Lock)
        RtlInitializeCriticalSection(&DataList->Reserved);
}

BOOL NTAPI Data_ListPushBack(PDATA_LIST DataList, PVOID NodeValue) {
    PDATA_LIST_NODE lpNode = Mem_Alloc(sizeof(DATA_LIST_NODE));
    if (lpNode) {
        if (DataList->Lock)
            RtlEnterCriticalSection(&DataList->Reserved);
        lpNode->Flink = NULL;
        lpNode->Blink = DataList->Last;
        lpNode->Value = NodeValue;
        if (DataList->Last)
            DataList->Last->Flink = lpNode;
        else
            DataList->First = lpNode;
        DataList->Last = lpNode;
        DataList->Length++;
        if (DataList->Lock)
            RtlLeaveCriticalSection(&DataList->Reserved);
        return TRUE;
    } else
        return FALSE;
}

BOOL NTAPI Data_ListPushFront(PDATA_LIST DataList, PVOID NodeValue) {
    PDATA_LIST_NODE lpNode = Mem_Alloc(sizeof(DATA_LIST_NODE));
    if (lpNode) {
        if (DataList->Lock)
            RtlEnterCriticalSection(&DataList->Reserved);
        lpNode->Flink = DataList->First;
        lpNode->Blink = NULL;
        lpNode->Value = NodeValue;
        if (DataList->First)
            DataList->First->Blink = lpNode;
        else
            DataList->Last = lpNode;
        DataList->First = lpNode;
        DataList->Length++;
        if (DataList->Lock)
            RtlLeaveCriticalSection(&DataList->Reserved);
        return TRUE;
    } else
        return FALSE;
}

BOOL NTAPI Data_ListPopBack(PDATA_LIST DataList, PVOID* NodeValuePointer) {
    PDATA_LIST_NODE lpNodeTemp;
    if (DataList->Last) {
        if (DataList->Lock)
            RtlEnterCriticalSection(&DataList->Reserved);
        if (NodeValuePointer)
            *NodeValuePointer = DataList->Last->Value;
        lpNodeTemp = DataList->Last;
        DataList->Last = lpNodeTemp->Blink;
        DataList->Last->Flink = NULL;
        Mem_Free(lpNodeTemp);
        DataList->Length--;
        if (DataList->Lock)
            RtlLeaveCriticalSection(&DataList->Reserved);
        return TRUE;
    } else
        return FALSE;
}

BOOL NTAPI Data_ListPopFront(PDATA_LIST DataList, PVOID* NodeValuePointer) {
    PDATA_LIST_NODE lpNodeTemp;
    if (DataList->First) {
        if (DataList->Lock)
            RtlEnterCriticalSection(&DataList->Reserved);
        if (NodeValuePointer)
            *NodeValuePointer = DataList->First->Value;
        lpNodeTemp = DataList->First;
        DataList->First = lpNodeTemp->Flink;
        DataList->First->Blink = NULL;
        Mem_Free(lpNodeTemp);
        DataList->Length--;
        if (DataList->Lock)
            RtlLeaveCriticalSection(&DataList->Reserved);
        return TRUE;
    } else
        return FALSE;
}

BOOL NTAPI Data_ListInsertBefore(PDATA_LIST DataList, PDATA_LIST_NODE DestNode, PVOID NodeValue) {
    PDATA_LIST_NODE lpNode = Mem_Alloc(sizeof(DATA_LIST_NODE));
    if (DestNode && lpNode) {
        if (DataList->Lock)
            RtlEnterCriticalSection(&DataList->Reserved);
        lpNode->Blink = DestNode->Blink;
        lpNode->Flink = DestNode;
        if (lpNode->Blink)
            lpNode->Blink->Flink = lpNode;
        else
            DestNode->Blink = DataList->First = lpNode;
        DestNode->Blink = lpNode;
        DataList->Length++;
        if (DataList->Lock)
            RtlLeaveCriticalSection(&DataList->Reserved);
        return TRUE;
    } else
        return FALSE;
}

BOOL NTAPI Data_ListInsertAfter(PDATA_LIST DataList, PDATA_LIST_NODE DestNode, PVOID NodeValue) {
    PDATA_LIST_NODE lpNode = Mem_Alloc(sizeof(DATA_LIST_NODE));
    if (DestNode && lpNode) {
        if (DataList->Lock)
            RtlEnterCriticalSection(&DataList->Reserved);
        lpNode->Blink = DestNode;
        lpNode->Flink = DestNode->Flink;
        if (lpNode->Flink)
            lpNode->Flink->Blink = lpNode;
        else
            DestNode->Flink = DataList->Last = lpNode;
        DestNode->Flink = lpNode;
        DataList->Length++;
        if (DataList->Lock)
            RtlLeaveCriticalSection(&DataList->Reserved);
        return TRUE;
    } else
        return FALSE;
}

BOOL NTAPI Data_ListRemove(PDATA_LIST DataList, PDATA_LIST_NODE Node) {
    if (Node) {
        if (DataList->Lock)
            RtlEnterCriticalSection(&DataList->Reserved);
        if (Node->Blink)
            Node->Blink->Flink = Node->Flink;
        else
            DataList->First = Node->Flink;
        if (Node->Flink)
            Node->Flink->Blink = Node->Blink;
        else
            DataList->Last = Node->Blink;
        Mem_Free(Node);
        DataList->Length--;
        if (DataList->Lock)
            RtlLeaveCriticalSection(&DataList->Reserved);
        return TRUE;
    } else
        return FALSE;
}

VOID NTAPI Data_ListReset(PDATA_LIST DataList, BOOL FreeValuePtr) {
    PDATA_LIST_NODE lpNode, lpNodeNext;
    if (DataList->Lock)
        RtlEnterCriticalSection(&DataList->Reserved);
    lpNode = DataList->First;
    while (lpNode) {
        if (FreeValuePtr && lpNode->Value)
            Mem_Free(lpNode->Value);
        lpNodeNext = lpNode->Flink;
        Mem_Free(lpNode);
        lpNode = lpNodeNext;
    }
    DataList->First = DataList->Last = NULL;
    DataList->Length = 0;
    if (DataList->Lock)
        RtlLeaveCriticalSection(&DataList->Reserved);
}