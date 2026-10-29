#include "common.h"
cqueue cq = NULL;
Histnode hist_temp = NULL, hist_prev = NULL, hist_start = NULL, hist_last = NULL, hist_new_node = NULL;


void initializeQueue(int max)
{
    cq = (cqueue)malloc(sizeof(queue_rec));
    if (cq == NULL)
    {
        printf("Queue allocation failed.\\n");
        return;
    }

    cq->cq_arr = (int*)malloc(sizeof(int) * max);
    if (cq->cq_arr == NULL)
    {
        free(cq);
        cq = NULL;
        printf("Queue array allocation failed.\\n");
        return;
    }

    cq->front = -1;
    cq->rear = -1;
    cq->size = 0;
    cq->capacity = max;
}

int isQueueEmpty()
{
    return (cq == NULL || cq->size == 0);
}

int isQueueFull()
{
    return (cq == NULL || cq->size == cq->capacity);
}

void enqueueIncident(int id)
{
    if(isQueueFull())
    {
        printf("Queue is full\n");
        return;
    }
    cq->rear = (cq->rear + 1) % cq->capacity;
    cq->cq_arr[cq->rear] = id;
    if(cq->front == -1)
    {
        cq->front = 0;
    }
    cq->size += 1;
}

/* The circular queue stores IDs, but dispatch selects by the same life-first
   ordering as the priority dashboard rather than blindly using arrival order. */
int dequeueIncident(int *id)
{
    if (isQueueEmpty())
    {
        printf("Queue is empty\n");
        return 0;
    }

    int bestOffset = 0;
    Incident *best = findIncidentByID(cq->cq_arr[cq->front]);
    for (int offset = 1; offset < cq->size; offset++)
    {
        int pos = (cq->front + offset) % cq->capacity;
        Incident *candidate = findIncidentByID(cq->cq_arr[pos]);
        if (candidate != NULL && (best == NULL || incidentHigherPriority(candidate, best)))
        {
            best = candidate;
            bestOffset = offset;
        }
    }

    int selectedPos = (cq->front + bestOffset) % cq->capacity;
    *id = cq->cq_arr[selectedPos];
    /* Rebuild the circular queue in logical order, omitting the selected ID. */
    int write = 0;
    int *remaining = (int *)malloc(sizeof(int) * cq->capacity);
    if (remaining == NULL)
    {
        /* Safe fallback: remove the front item if temporary allocation fails. */
        *id = cq->cq_arr[cq->front];
        cq->front = (cq->front + 1) % cq->capacity;
        cq->size--;
        if (cq->size == 0) cq->front = cq->rear = -1;
        return 1;
    }
    for (int offset = 0; offset < cq->size; offset++)
    {
        if (offset == bestOffset) continue;
        remaining[write++] = cq->cq_arr[(cq->front + offset) % cq->capacity];
    }
    for (int i = 0; i < write; i++) cq->cq_arr[i] = remaining[i];
    free(remaining);
    cq->size = write;
    cq->front = write ? 0 : -1;
    cq->rear = write ? write - 1 : -1;
    return 1;
}

static int triageRank(const Incident *i)
{
    if (i->triageTier == 1 || i->immediateThreat) return 5;
    /* High self-reported danger is sent for rapid triage, not treated as verified. */
    if (i->triageTier == 0 && i->urgency == 5 && (i->waterLevel >= 3 || i->severity >= 4)) return 4;
    if (i->triageTier == 2) return 4;
    if (i->triageTier == 3) return 3;
    if (i->triageTier == 4) return 2;
    return 1;
}

int incidentHigherPriority(const Incident *a, const Incident *b)
{
    if (a == NULL) return 0;
    if (b == NULL) return 1;
    int ar = triageRank(a), br = triageRank(b);
    if (ar != br) return ar > br;
    if (a->priorityScore != b->priorityScore) return a->priorityScore > b->priorityScore;
    /* Older reports win ties to reduce indefinite waiting. */
    return a->id < b->id;
}

const char *getTriageTierName(int tier)
{
    switch (tier) {
        case 1: return "Immediate life threat";
        case 2: return "High risk";
        case 3: return "Mass evacuation";
        case 4: return "Monitor";
        default: return "Needs assessment";
    }
}

const char *getEvidenceStatusName(int status)
{
    switch (status) {
        case 1: return "Corroborated";
        case 2: return "Officially verified";
        case 3: return "Disputed";
        default: return "Unverified";
    }
}

void displayPendingQueue()
{
    if (isQueueEmpty())
    {
        printf("Queue is empty\n");
        return;
    }
    Incident **items = (Incident **)malloc(sizeof(Incident *) * cq->size);
    if (items == NULL)
    {
        printf("Queue order unavailable (memory allocation failed).\n");
        return;
    }
    int count = 0;
    for (int offset = 0; offset < cq->size; offset++)
    {
        Incident *item = findIncidentByID(cq->cq_arr[(cq->front + offset) % cq->capacity]);
        if (item != NULL && item->resolutionStatus != 2) items[count++] = item;
    }
    for (int i = 1; i < count; i++)
    {
        Incident *key = items[i];
        int j = i - 1;
        while (j >= 0 && incidentHigherPriority(key, items[j]))
        {
            items[j + 1] = items[j];
            j--;
        }
        items[j + 1] = key;
    }
    printf("Pending incidents ordered by life-safety triage (not arrival order): ");
    for (int i = 0; i < count; i++) printf("%s%d", i ? " " : "", items[i]->id);
    printf("\n");
    free(items);
}

void freeQueue(void)
{
    if (cq != NULL)
    {
        free(cq->cq_arr);
        free(cq);
        cq = NULL;
    }
}

void createHistoryNode()
{
    hist_start = NULL;
    hist_last = NULL;
}

int ishistoryEmpty()
{
    return(hist_start == NULL);
}

int  appendHistory(Incident_hist_info i)
{
    hist_new_node = (Histnode)malloc(sizeof(HistoryNode_Rec));

    if (hist_new_node == NULL)
    {
        printf("Memory allocation failed\n");
        return 0;
    }

    hist_new_node->info = i;
    hist_new_node->next = NULL;

    if(ishistoryEmpty())
    {
        hist_start = hist_new_node;
        hist_last = hist_new_node;
        return 1;
    }
    else
    {
        hist_last->next = hist_new_node;
        hist_last = hist_new_node;
        return 1;
    }
    return 0;
}

const char *getStatusName(Incidentstatus status)
{
    switch (status)
    {
        case REPORTED:
            return "Reported";

        case VERIFIED:
            return "Verified";

        case RESCUE_IN_PROGRESS:
            return "Rescue in progress";

        case RESOLVED:
            return "Resolved";

        default:
            return "Unknown";
    }
}

void displayIncientHistory()
{
    if(ishistoryEmpty())
    {
        printf("List is empty\n");
        return;
    }
    hist_temp = hist_start;
    while(hist_temp != NULL)
    {

        printf("Status:  %s\n",getStatusName(hist_temp->info.status));
        printf("Timestamp : %s\n",hist_temp->info.timestamp);
        printf("Note: %s\n",hist_temp->info.note);
        printf("-------------------------\n");
        hist_temp = hist_temp->next;
    }
}

void freeHistory()
{

    Histnode current = hist_start;

    while (current != NULL)
    {
        Histnode next = current->next;
        free(current);
        current = next;
    }

    hist_start = NULL;
    hist_last = NULL; 

}

Incident *findIncidentByID(int id)
{
    node current = start;

    while (current != NULL)
    {
        if (current->info.id == id)
        {
            return &(current->info);
        }

        current = current->next;
    }

    return NULL;
}

int appendHistoryForIncident(
    Incident *incident,
    Incidentstatus status,
    const char *note)
{
    Histnode newNode;
    time_t now;
    struct tm *timeInfo;

    if (incident == NULL)
        return 0;

    newNode = (Histnode)malloc(sizeof(HistoryNode_Rec));

    if (newNode == NULL)
    {
        printf("Memory allocation failed.\n");
        return 0;
    }

    newNode->info.status = status;

    // Store the current date and time //
    now = time(NULL);
    timeInfo = localtime(&now);

    if (timeInfo != NULL)
        strftime(newNode->info.timestamp,
                 sizeof(newNode->info.timestamp),
                 "%Y-%m-%d %H:%M:%S",
                 timeInfo);
    else
        strcpy(newNode->info.timestamp, "Unknown");

    // Store the status update note //
    snprintf(newNode->info.note,
             sizeof(newNode->info.note),
             "%s",
             note != NULL ? note : "");

    newNode->next = NULL;

    // Insert the first history node //
    if (incident->historyHead == NULL)
    {
        incident->historyHead = newNode;
        incident->historyTail = newNode;
    }
    else
    {
        // Append to the end of the history list //
        incident->historyTail->next = newNode;
        incident->historyTail = newNode;
    }

    return 1;
}

void displayIncidentHistory(Incident *incident)
{
    Histnode current;

    if (incident == NULL)
    {
        printf("Incident not found.\n");
        return;
    }

    current = incident->historyHead;

    if (current == NULL)
    {
        printf("No history available for Incident ID: %d\n",
               incident->id);
        return;
    }

    printf("\n===== INCIDENT HISTORY =====\n");
    printf("Incident ID: %d\n", incident->id);

    while (current != NULL)
    {
        printf("\nStatus: %s\n",
               getStatusName(current->info.status));

        printf("Timestamp: %s\n",
               current->info.timestamp);

        printf("Note: %s\n",
               current->info.note);

        printf("----------------------------\n");

        current = current->next;
    }
}

void updateResolutionStatus(Incident *incident)
{
    int choice;
    Incidentstatus newStatus;
    char note[300];

    if (incident == NULL)
    {
        printf("Incident not found.\n");
        return;
    }

    printf("\n===== UPDATE INCIDENT STATUS =====\n");
    printf("Incident ID: %d\n", incident->id);

    printf("1. Verify Incident\n");
    printf("2. Start Rescue Operations\n");
    printf("3. Mark as Resolved\n");
    printf("Enter your choice: ");

    if (scanf("%d", &choice) != 1)
    {
        printf("Invalid input.\n");
        return;
    }

    switch (choice)
    {
        case 1:
            if (incident->verificationStatus == 1)
            {
                printf("Incident is already verified.\n");
                return;
            }
            printf("Evidence status (1=Corroborated, 2=Officially verified, 3=Disputed): ");
            if (scanf("%d", &incident->evidenceStatus) != 1 || incident->evidenceStatus < 1 || incident->evidenceStatus > 3)
            {
                incident->evidenceStatus = 0;
                printf("Invalid evidence status.\n");
                return;
            }
            printf("Life-safety tier (1=Immediate life threat, 2=High risk, 3=Mass evacuation, 4=Monitor): ");
            if (scanf("%d", &incident->triageTier) != 1 || incident->triageTier < 1 || incident->triageTier > 4)
            {
                incident->triageTier = 0;
                incident->evidenceStatus = 0;
                printf("Invalid triage tier.\n");
                return;
            }
            incident->immediateThreat = (incident->triageTier == 1 && incident->evidenceStatus != 3);
            if (incident->evidenceStatus == 3) incident->triageTier = 0;
            newStatus = incident->evidenceStatus == 3 ? REPORTED : VERIFIED;
            break;

        case 2:
            if (incident->verificationStatus != 1 || incident->evidenceStatus == 3)
            {
                printf("A disputed or unverified incident cannot be dispatched as confirmed. Reassess urgently if a life threat is suspected.\n");
                return;
            }

            if (incident->resolutionStatus != 0)
            {
                printf("Rescue cannot be started in the current status.\n");
                return;
            }

            newStatus = RESCUE_IN_PROGRESS;
            break;

        case 3:
            if (incident->resolutionStatus != 1)
            {
                printf("Rescue must be in progress before resolution.\n");
                return;
            }

            newStatus = RESOLVED;
            break;

        default:
            printf("Invalid choice.\n");
            return;
    }

    printf("Enter status update note: ");

    if (scanf(" %299[^\n]", note) != 1)
    {
        printf("Invalid note.\n");
        return;
    }

    // Save history first. Update status only if this succeeds. //
    if (!appendHistoryForIncident(incident, newStatus, note))
    {
        printf("Status update failed. History was not saved.\n");
        return;
    }

    // Update the incident's current status //
    if (newStatus == VERIFIED)
        incident->verificationStatus = 1;
    else if (newStatus == REPORTED)
        incident->verificationStatus = 0;

    else if (newStatus == RESCUE_IN_PROGRESS)
        incident->resolutionStatus = 1;

    else if (newStatus == RESOLVED)
        incident->resolutionStatus = 2;

    snprintf(incident->resolutionNotes,
             sizeof(incident->resolutionNotes),
             "%.199s",
             note);

    printf("\nStatus updated successfully!\n");
    printf("New Status: %s\n", getStatusName(newStatus));
    printf("Evidence: %s | Triage: %s\n", getEvidenceStatusName(incident->evidenceStatus), getTriageTierName(incident->triageTier));
}

void processNextIncident(void)
{
    int id;
    Incident *incident;

    if (isQueueEmpty())
    {
        printf("No pending incidents to process.\n");
        return;
    }

    // Remove the next incident ID from the queue //
    if (!dequeueIncident(&id))
        return;

    // Find the incident in Module 1 linked list //
    incident = findIncidentByID(id);

    if (incident == NULL)
    {
        printf("Incident ID %d not found. Skipping it.\n", id);
        return;
    }

    printf("\n===== PROCESSING INCIDENT =====\n");
    printf("Incident ID: %d\n", incident->id);
    printf("Title: %s\n", incident->title);
    printf("District: %s\n", incident->district);
    printf("Location: %s\n", incident->location);
    printf("Severity: %d\n", incident->severity);
    printf("Triage: %s | Evidence: %s\n", getTriageTierName(incident->triageTier), getEvidenceStatusName(incident->evidenceStatus));

    if (incident->resolutionStatus == 2)
    {
        printf("This incident is already resolved.\n");
        return;
    }

    // Update its status and record the change in history //
    updateResolutionStatus(incident);

    // Requeue it if -> still unresolved //
    if (incident->resolutionStatus != 2)
    {
        enqueueIncident(incident->id);
        printf("Incident %d remains pending. It will be selected using life-safety triage and risk, not arrival order.\n",
               incident->id);
    }
    else
    {
        printf("Incident %d has been resolved.\n", incident->id);
    }
}
