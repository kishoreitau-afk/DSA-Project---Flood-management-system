#include "common.h"
node start = NULL, last = NULL, temp = NULL, prev = NULL, new_node = NULL;
void create()
{
    start = NULL;
    last = NULL;
}

int isEmpty()
{
    return (start == NULL);
}

int generateID()
{
    static int id = 0;
    return ++id;
}

void getDetails(Incident *i)
{
    
    printf("Enter Incident Title: ");
    scanf(" %99[^\n]", i->title);

    printf("Enter Description: ");
    scanf(" %299[^\n]", i->description);

    printf("Enter District: ");
    scanf(" %49[^\n]", i->district);

    printf("Enter Location: ");
    scanf(" %149[^\n]", i->location);

    printf("\nFlood Type:\n");
    printf("1. River Overflow\n");
    printf("2. Urban Flooding\n");
    printf("3. Flash Flood\n");
    printf("4. Drainage Overflow\n");
    printf("Enter Flood Type: ");
    scanf("%d", &i->floodType);

    printf("\nWater Level:\n");
    printf("1. Below Knee\n");
    printf("2. Knee Deep\n");
    printf("3. Waist Deep\n");
    printf("4. Above Waist\n");
    printf("5. Unknown\n");
    printf("Enter Water Level: ");
    scanf("%d", &i->waterLevel);

    printf("Enter Severity (1-5): ");
    scanf("%d", &i->severity);

    printf("Enter Number of People Affected: ");
    scanf("%d", &i->peopleAffected);

    printf("Enter Urgency (1-5): ");
    scanf("%d", &i->urgency);

    printf("Enter Reported Date and Time: ");
    scanf(" %29[^\n]", i->reportedAt);

    i->verificationStatus = 0;
    i->resolutionStatus = 0;
    i->triageTier = 0;
    i->evidenceStatus = 0;
    i->immediateThreat = 0;

    strcpy(i->resolutionNotes, "-");
}

int validateIncidentData(Incident x)
{
    if (strlen(x.title) == 0) {
        printf("Error: Incident title is required.\n");
        return 0;
    }

    if (strlen(x.description) == 0) {
        printf("Error: Incident description is required.\n");
        return 0;
    }

    if (strlen(x.district) == 0) {
        printf("Error: District is required.\n");
        return 0;
    }

    if (strlen(x.location) == 0) {
        printf("Error: Incident location is required.\n");
        return 0;
    }

    if (x.floodType < 1 || x.floodType > 4) {
        printf("Error: Invalid flood type.\n");
        return 0;
    }

    if (x.waterLevel < 1 || x.waterLevel > 5) {
        printf("Error: Invalid water level.\n");
        return 0;
    }

    if (x.severity < 1 || x.severity > 5) {
        printf("Error: Severity must be between 1 and 5.\n");
        return 0;
    }

    if (x.peopleAffected < 0) {
        printf("Error: Number of people cannot be negative.\n");
        return 0;
    }

    if (x.urgency < 1 || x.urgency > 5) {
        printf("Error: Urgency must be between 1 and 5.\n");
        return 0;
    }

    return 1;
}

int  insertIncident(Incident i)
{
    new_node = (node)malloc(sizeof(list_node_rec));

    if (new_node == NULL)
    {
        printf("Memory allocation failed\n");
        return 0;
    }

    new_node->info = i;
    new_node->next = NULL;

    if(isEmpty())
    {
        start = new_node;
        last = new_node;
        return 1;
    }
    else
    {
        last->next = new_node;
        last = new_node;
        return 1;
    }
    return 0;
}

int Priorityscore(Incident *i)
{

    float S = i->severity / 5.0;
    float A = i->peopleAffected / 1000.0;
    float U = i->urgency / 5.0;

    // Limit affected population score//
    if (A > 1.0)
        A = 1.0;

    // Population matters, but cannot dominate severity, urgency or water conditions. //
    float W = (i->waterLevel == 5 ? 0.5f : i->waterLevel / 4.0f);
    int ps = (int)(35 * S + 25 * U + 25 * W + 15 * A + 0.5);

    return ps;

}

void reportIncident(Incident *i, HashTable H)
{

    getDetails(i); 

    if (!validateIncidentData(*i))
    {
        printf("Incident report rejected.\n");
        return;
    }

    i->id = generateID();


    i->priorityScore = Priorityscore(i);


    i->historyHead = NULL;
    i->historyTail = NULL;
    

    if (insert(i->id, H) == NULL)
    {
        printf("Hash table insertion failed.\n");
        return;
    }

    if (insertIncident(*i))
    {
        printf("Incident reported successfully!\n");
        printf("Incident ID: %d\n", i->id);
        printf("Priority Score: %d\n", i->priorityScore);
        printf("Verification: Unverified | Triage: Needs assessment\n");
    }
    else
    {

        deleteHash(i->id, H);
        printf("Incident could not be saved to linked list.\n");
    
    }
}

void freeIncidentList()
{
    node temp;

    while (start != NULL)
    {
        temp = start;
        start = start->next;

        Histnode h = temp->info.historyHead;
        while (h != NULL)
        {
            Histnode nextHistory = h->next;
            free(h);
            h = nextHistory;
        }

        free(temp);
    }

    last = NULL;
}
