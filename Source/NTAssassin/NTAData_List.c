#include "include\NTAssassin\NTAData_List.h"

#include "include\NTAssassin\NTAMem.h"

VOID NTAPI Data_ListInit(_Out_ PDATA_LIST List)
{
    List->First = List->Last = NULL;
    List->Length = 0;
    List->Lock = (RTL_SRWLOCK)RTL_SRWLOCK_INIT;
}

BOOL NTAPI Data_ListPushBack(_In_ PDATA_LIST List, PVOID NodeValue)
{
    PDATA_LIST_NODE pNode = Mem_Alloc(sizeof(DATA_LIST_NODE));
    if (!pNode) {
        return FALSE;
    }

    RtlAcquireSRWLockExclusive(&List->Lock);
    pNode->Flink = NULL;
    pNode->Blink = List->Last;
    pNode->Value = NodeValue;
    if (List->Last) {
        List->Last->Flink = pNode;
    } else {
        List->First = pNode;
    }
    List->Last = pNode;
    List->Length++;
    RtlReleaseSRWLockExclusive(&List->Lock);
    return TRUE;
}

BOOL NTAPI Data_ListPushFront(_In_ PDATA_LIST List, PVOID NodeValue)
{
    PDATA_LIST_NODE pNode = Mem_Alloc(sizeof(DATA_LIST_NODE));
    if (!pNode) {
        return FALSE;
    }

    RtlAcquireSRWLockExclusive(&List->Lock);
    pNode->Flink = List->First;
    pNode->Blink = NULL;
    pNode->Value = NodeValue;
    if (List->First) {
        List->First->Blink = pNode;
    } else {
        List->Last = pNode;
    }
    List->First = pNode;
    List->Length++;
    RtlReleaseSRWLockExclusive(&List->Lock);
    return TRUE;
}

_Success_(return != FALSE)
BOOL NTAPI Data_ListPopBack(_In_ PDATA_LIST List, _Out_opt_ PVOID * NodeValue)
{
    PDATA_LIST_NODE pNodeTemp;
    if (!List->Last) {
        return FALSE;
    }

    RtlAcquireSRWLockExclusive(&List->Lock);
    if (NodeValue) {
        *NodeValue = List->Last->Value;
    }
    pNodeTemp = List->Last;
    List->Last = pNodeTemp->Blink;
    List->Last->Flink = NULL;
    Mem_Free(pNodeTemp);
    List->Length--;
    RtlReleaseSRWLockExclusive(&List->Lock);
    return TRUE;
}

_Success_(return != FALSE)
BOOL NTAPI Data_ListPopFront(_In_ PDATA_LIST List, _Out_opt_ PVOID * NodeValue)
{
    PDATA_LIST_NODE pNodeTemp;
    if (!List->First) {
        return FALSE;
    }

    RtlAcquireSRWLockExclusive(&List->Lock);
    if (NodeValue) {
        *NodeValue = List->First->Value;
    }
    pNodeTemp = List->First;
    List->First = pNodeTemp->Flink;
    List->First->Blink = NULL;
    Mem_Free(pNodeTemp);
    List->Length--;
    RtlReleaseSRWLockExclusive(&List->Lock);
    return TRUE;
}

BOOL NTAPI Data_ListInsertBefore(_In_ PDATA_LIST List, _In_ PDATA_LIST_NODE DestNode, PVOID NodeValue)
{
    PDATA_LIST_NODE pNode = Mem_Alloc(sizeof(DATA_LIST_NODE));
    if (!pNode) {
        return FALSE;
    }

    RtlAcquireSRWLockExclusive(&List->Lock);
    pNode->Blink = DestNode->Blink;
    pNode->Flink = DestNode;
    if (pNode->Blink) {
        pNode->Blink->Flink = pNode;
    } else {
        DestNode->Blink = List->First = pNode;
    }
    DestNode->Blink = pNode;
    List->Length++;
    RtlReleaseSRWLockExclusive(&List->Lock);
    return TRUE;
}

BOOL NTAPI Data_ListInsertAfter(_In_ PDATA_LIST List, _In_ PDATA_LIST_NODE DestNode, PVOID NodeValue)
{
    PDATA_LIST_NODE pNode = Mem_Alloc(sizeof(DATA_LIST_NODE));
    if (!pNode) {
        return FALSE;
    }

    RtlAcquireSRWLockExclusive(&List->Lock);
    pNode->Blink = DestNode;
    pNode->Flink = DestNode->Flink;
    if (pNode->Flink) {
        pNode->Flink->Blink = pNode;
    } else {
        DestNode->Flink = List->Last = pNode;
    }
    DestNode->Flink = pNode;
    List->Length++;
    RtlReleaseSRWLockExclusive(&List->Lock);
    return TRUE;
}

BOOL NTAPI Data_ListRemove(_In_ PDATA_LIST List, _In_ PDATA_LIST_NODE Node)
{
    RtlAcquireSRWLockExclusive(&List->Lock);
    if (Node->Blink) {
        Node->Blink->Flink = Node->Flink;
    } else {
        List->First = Node->Flink;
    }
    if (Node->Flink) {
        Node->Flink->Blink = Node->Blink;
    } else {
        List->Last = Node->Blink;
    }
    Mem_Free(Node);
    List->Length--;
    RtlReleaseSRWLockExclusive(&List->Lock);
    return TRUE;
}

VOID NTAPI Data_ListReset(_In_ PDATA_LIST List, BOOL FreeValuePtr)
{
    PDATA_LIST_NODE pNode, pNodeNext;
    RtlAcquireSRWLockExclusive(&List->Lock);
    pNode = List->First;
    while (pNode) {
        if (FreeValuePtr && pNode->Value) {
            Mem_Free(pNode->Value);
        }
        pNodeNext = pNode->Flink;
        Mem_Free(pNode);
        pNode = pNodeNext;
    }
    List->First = List->Last = NULL;
    List->Length = 0;
    RtlReleaseSRWLockExclusive(&List->Lock);
}
