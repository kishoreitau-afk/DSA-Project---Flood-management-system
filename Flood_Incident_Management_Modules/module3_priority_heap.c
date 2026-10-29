#include "common.h"

void swapIncidentPointers(Incident **a, Incident **b)
{
    Incident *t = *a;
    *a = *b;
    *b = t;
}

void siftDown(Incident **heap, int n, int index)
{
    while (1)
    {
        int largest = index;
        int left = 2 * index + 1;
        int right = 2 * index + 2;

        if (left < n && incidentHigherPriority(heap[left], heap[largest]))
            largest = left;
        if (right < n && incidentHigherPriority(heap[right], heap[largest]))
            largest = right;

        if (largest == index)
            break;

        swapIncidentPointers(&heap[index], &heap[largest]);
        index = largest;
    }
}

void viewPriorityDashboard(void)
{
    node current = start;
    Incident **heap;
    int n = 0, i;

    while (current != NULL)
    {
        n++;
        current = current->next;
    }

    if (n == 0)
    {
        printf("No incidents available.\n");
        return;
    }

    heap = (Incident **)malloc(sizeof(Incident *) * n);
    if (heap == NULL)
    {
        printf("Memory allocation failed for priority dashboard.\n");
        return;
    }

    current = start;
    i = 0;
    while (current != NULL)
    {
        heap[i++] = &current->info;
        current = current->next;
    }

    //build max heap => high priority in top//
    for (i = n / 2 - 1; i >= 0; i--)
        siftDown(heap, n, i);

    printf("\n===== PRIORITY DASHBOARD (HIGHEST FIRST) =====\n");
    for (i = n; i > 0; i--)
    {
        printf("\nID: %d | Priority: %d | Tier: %s | Verification: %s | Severity: %d | Affected: %d\n",
               heap[0]->id, heap[0]->priorityScore,
               getTriageTierName(heap[0]->triageTier),
               getEvidenceStatusName(heap[0]->evidenceStatus),
               heap[0]->severity, heap[0]->peopleAffected);

        // Extract max and restore heap order. //
        heap[0] = heap[i - 1];
        siftDown(heap, i - 1, 0);
    }

    free(heap);
}
