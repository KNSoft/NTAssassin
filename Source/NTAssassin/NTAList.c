#include "include\NTAssassin\NTAssassin.h"

VOID NTAPI Data_ListInit(_Out_ PDATA_LIST DataList) {
    DataList->First = DataList->Last = NULL;
    DataList->Length = 0;
    if (DataList->Lock) {
        RtlInitializeCriticalSection(&DataList->Reserved);
    }
}

BOOL NTAPI Data_ListPushBack(_In_ PDATA_LIST DataList, PVOID NodeValue) {
    PDATA_LIST_NODE pNode = Mem_Alloc(sizeof(DATA_LIST_NODE));
    if (pNode) {
        if (DataList->Lock) {
            RtlEnterCriticalSection(&DataList->Reserved);
        }
        pNode->Flink = NULL;
        pNode->Blink = DataList->Last;
        pNode->Value = NodeValue;
        if (DataList->Last) {
            DataList->Last->Flink = pNode;
        } else {
            DataList->First = pNode;
        }
        DataList->Last = pNode;
        DataList->Length++;
        if (DataList->Lock) {
            RtlLeaveCriticalSection(&DataList->Reserved);
        }
        return TRUE;
    } else {
        return FALSE;
    }
}

BOOL NTAPI Data_ListPushFront(_In_ PDATA_LIST DataList, PVOID NodeValue) {
    PDATA_LIST_NODE pNode = Mem_Alloc(sizeof(DATA_LIST_NODE));
    if (pNode) {
        if (DataList->Lock) {
            RtlEnterCriticalSection(&DataList->Reserved);
        }
        pNode->Flink = DataList->First;
        pNode->Blink = NULL;
        pNode->Value = NodeValue;
        if (DataList->First) {
            DataList->First->Blink = pNode;
        } else {
            DataList->Last = pNode;
        }
        DataList->First = pNode;
        DataList->Length++;
        if (DataList->Lock) {
            RtlLeaveCriticalSection(&DataList->Reserved);
        }
        return TRUE;
    } else {
        return FALSE;
    }
}

_Success_(return != FALSE)
BOOL NTAPI Data_ListPopBack(_In_ PDATA_LIST DataList, _Out_opt_ PVOID * NodeValue) {
    PDATA_LIST_NODE pNodeTemp;
    if (DataList->Last) {
        if (DataList->Lock) {
            RtlEnterCriticalSection(&DataList->Reserved);
        }
        if (NodeValue) {
            *NodeValue = DataList->Last->Value;
        }
        pNodeTemp = DataList->Last;
        DataList->Last = pNodeTemp->Blink;
        DataList->Last->Flink = NULL;
        Mem_Free(pNodeTemp);
        DataList->Length--;
        if (DataList->Lock) {
            RtlLeaveCriticalSection(&DataList->Reserved);
        }
        return TRUE;
    } else {
        return FALSE;
    }
}

_Success_(return != FALSE)
BOOL NTAPI Data_ListPopFront(_In_ PDATA_LIST DataList, _Out_opt_ PVOID * NodeValue) {
    PDATA_LIST_NODE pNodeTemp;
    if (DataList->First) {
        if (DataList->Lock) {
            RtlEnterCriticalSection(&DataList->Reserved);
        }
        if (NodeValue) {
            *NodeValue = DataList->First->Value;
        }
        pNodeTemp = DataList->First;
        DataList->First = pNodeTemp->Flink;
        DataList->First->Blink = NULL;
        Mem_Free(pNodeTemp);
        DataList->Length--;
        if (DataList->Lock) {
            RtlLeaveCriticalSection(&DataList->Reserved);
        }
        return TRUE;
    } else {
        return FALSE;
    }
}

BOOL NTAPI Data_ListInsertBefore(_In_ PDATA_LIST DataList, _In_ PDATA_LIST_NODE DestNode, PVOID NodeValue) {
    PDATA_LIST_NODE pNode = Mem_Alloc(sizeof(DATA_LIST_NODE));
    if (DestNode && pNode) {
        if (DataList->Lock) {
            RtlEnterCriticalSection(&DataList->Reserved);
        }
        pNode->Blink = DestNode->Blink;
        pNode->Flink = DestNode;
        if (pNode->Blink) {
            pNode->Blink->Flink = pNode;
        } else {
            DestNode->Blink = DataList->First = pNode;
        }
        DestNode->Blink = pNode;
        DataList->Length++;
        if (DataList->Lock) {
            RtlLeaveCriticalSection(&DataList->Reserved);
        }
        return TRUE;
    } else {
        return FALSE;
    }
}

BOOL NTAPI Data_ListInsertAfter(_In_ PDATA_LIST DataList, _In_ PDATA_LIST_NODE DestNode, PVOID NodeValue) {
    PDATA_LIST_NODE pNode = Mem_Alloc(sizeof(DATA_LIST_NODE));
    if (DestNode && pNode) {
        if (DataList->Lock) {
            RtlEnterCriticalSection(&DataList->Reserved);
        }
        pNode->Blink = DestNode;
        pNode->Flink = DestNode->Flink;
        if (pNode->Flink) {
            pNode->Flink->Blink = pNode;
        } else {
            DestNode->Flink = DataList->Last = pNode;
        }
        DestNode->Flink = pNode;
        DataList->Length++;
        if (DataList->Lock) {
            RtlLeaveCriticalSection(&DataList->Reserved);
        }
        return TRUE;
    } else {
        return FALSE;
    }
}

BOOL NTAPI Data_ListRemove(_In_ PDATA_LIST DataList, _In_ PDATA_LIST_NODE Node) {
    if (Node) {
        if (DataList->Lock) {
            RtlEnterCriticalSection(&DataList->Reserved);
        }
        if (Node->Blink) {
            Node->Blink->Flink = Node->Flink;
        } else {
            DataList->First = Node->Flink;
        }
        if (Node->Flink) {
            Node->Flink->Blink = Node->Blink;
        } else {
            DataList->Last = Node->Blink;
        }
        Mem_Free(Node);
        DataList->Length--;
        if (DataList->Lock) {
            RtlLeaveCriticalSection(&DataList->Reserved);
        }
        return TRUE;
    } else {
        return FALSE;
    }
}

VOID NTAPI Data_ListReset(_In_ PDATA_LIST DataList, BOOL FreeValuePtr) {
    PDATA_LIST_NODE pNode, pNodeNext;
    if (DataList->Lock) {
        RtlEnterCriticalSection(&DataList->Reserved);
    }
    pNode = DataList->First;
    while (pNode) {
        if (FreeValuePtr && pNode->Value) {
            Mem_Free(pNode->Value);
        }
        pNodeNext = pNode->Flink;
        Mem_Free(pNode);
        pNode = pNodeNext;
    }
    DataList->First = DataList->Last = NULL;
    DataList->Length = 0;
    if (DataList->Lock) {
        RtlLeaveCriticalSection(&DataList->Reserved);
    }
}