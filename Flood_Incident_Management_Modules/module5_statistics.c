#include "common.h"

void viewStatistics(void)
{
    node current = start;
    int total = 0, unresolved = 0, resolved = 0, highPriority = 0;
    int severityCount[6] = {0};
    int floodTypeCount[5] = {0};
    int districtCount = 0;
    char districts[100][50];
    int districtTotals[100] = {0};
    int i, found;

    while (current != NULL)
    {
        Incident *x = &current->info;
        total++;

        if (x->resolutionStatus == 2)
            resolved++;
        else
            unresolved++;

        if (x->priorityScore >= 70)
            highPriority++;

        if (x->severity >= 1 && x->severity <= 5)
            severityCount[x->severity]++;

        if (x->floodType >= 1 && x->floodType <= 4)
            floodTypeCount[x->floodType]++;

        found = -1;
        for (i = 0; i < districtCount; i++)
        {
            if (strcmp(districts[i], x->district) == 0)
            {
                found = i;
                break;
            }
        }

        if (found >= 0)
            districtTotals[found]++;
        else if (districtCount < 100)
        {
            snprintf(districts[districtCount], sizeof(districts[districtCount]),
                     "%.49s", x->district);
            districtTotals[districtCount] = 1;
            districtCount++;
        }

        current = current->next;
    }

    printf("\n===== INCIDENT STATISTICS =====\n");
    printf("Total incidents       : %d\n", total);
    printf("Unresolved incidents  : %d\n", unresolved);
    printf("Resolved incidents    : %d\n", resolved);
    printf("High-priority (>=70)  : %d\n", highPriority);

    printf("\nIncidents by severity:\n");
    for (i = 1; i <= 5; i++)
        printf("Severity %d: %d\n", i, severityCount[i]);

    printf("\nIncidents by flood type:\n");
    printf("River Overflow    : %d\n", floodTypeCount[1]);
    printf("Urban Flooding    : %d\n", floodTypeCount[2]);
    printf("Flash Flood       : %d\n", floodTypeCount[3]);
    printf("Drainage Overflow : %d\n", floodTypeCount[4]);

    printf("\nIncidents by district:\n");
    if (districtCount == 0)
        printf("No incidents recorded.\n");
    for (i = 0; i < districtCount; i++)
        printf("%s: %d\n", districts[i], districtTotals[i]);
}

void displayAllIncidents(void)
{
    node current = start;

    if (current == NULL)
    {
        printf("No incidents recorded.\n");
        return;
    }

    printf("\n===== ALL INCIDENTS =====\n");
    while (current != NULL)
    {
        Incident *x = &current->info;
        printf("ID: %d | Title: %s | District: %s | Severity: %d | "
               "Priority: %d | Status: %s\n",
               x->id, x->title, x->district, x->severity, x->priorityScore,
               x->resolutionStatus == 2 ? "Resolved" :
               x->resolutionStatus == 1 ? "Rescue in progress" :
               x->verificationStatus == 1 ? "Verified" : "Reported");
        current = current->next;
    }
}

void freeIncidentHistory(void)
{
    node current = start;

    while (current != NULL)
    {
        Histnode h = current->info.historyHead;
        while (h != NULL)
        {
            Histnode nextHistory = h->next;
            free(h);
            h = nextHistory;
        }
        current->info.historyHead = NULL;
        current->info.historyTail = NULL;
        current = current->next;
    }
}
