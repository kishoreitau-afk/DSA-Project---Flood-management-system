#include "common.h"

int NextPrime(int size)
{
    int i, j, prime;

    if (size < 2)
        size = 2;  //min prime number = 2//

    for (i = size; ; i++) {
        prime = 1;

        for (j = 2; j * j <= i; j++) {
            if (i % j == 0) {
                prime = 0;
                break;
            }
        }

        if (prime)
            return i;
    }
}

HashTable initialize(int size)
{

    HashTable H;

    H = (HashTable)malloc(sizeof(HashTbl));
    if(H == NULL)
    {
        printf("Fatal error\n");
        return NULL;
    }

    H->Tablesize = NextPrime(size);

    H->Thelists = (List*)malloc(sizeof(List) * H->Tablesize);

    if(H->Thelists == NULL)
    {
        printf("Fatal error in HashTable linked list\n");
        free(H);
        return NULL;
    }

    for(int i=0;i<H->Tablesize;i++)
    {
        H->Thelists[i] = (Hnode)malloc(sizeof(hash_node_rec));
        
        if(H->Thelists[i] == NULL)
        {
            while(i > 0)
                free(H->Thelists[--i]);

            free(H->Thelists);
            free(H);
            
            return NULL;
        }
        
        H->Thelists[i]->next = NULL;
    }

    return H;

}

int hash(int key , int size)
{
    int index = key%size;
    return index;
}

Hnode Find(int key , HashTable H)
{
    Hnode n;
    List L;

    if (H == NULL)
        return NULL;

    L = H->Thelists[hash(key , H->Tablesize)];
    n = L->next;
    while(n != NULL)
    {
        if(n->incidentID == key)
        {
            return n;
        }
        n = n->next;
    }
    return NULL;
}

HashTable insert(int key , HashTable H)
{

    Hnode n ,newNode;
    List L;

    if (H == NULL)
        return NULL;
    
    L = H->Thelists[hash(key , H->Tablesize)];
    n = Find(key , H);
    if(n!=NULL)
    {
        return NULL;
    }
    newNode = (Hnode)malloc(sizeof(hash_node_rec));
    
    if (newNode == NULL)
        return NULL;

    newNode->incidentID = key;
    
    newNode->next = L->next;
    L->next = newNode;

    return H;

}

int deleteHash(int key, HashTable H)
{
    Hnode n, prev;
    List L;

    if (H == NULL)
        return 0;

    L = H->Thelists[hash(key, H->Tablesize)];
    prev = L;
    n = L->next;

    while (n != NULL)
    {
        if (n->incidentID == key)
        {
            prev->next = n->next;
            free(n);
            return 1;
        }

        prev = n;
        n = n->next;
    }

    return 0;
}

void searchIncidentByHash(HashTable H)
{
    int id;
    Hnode found;
    Incident *incident;

    printf("Enter incident ID to search: ");
    if (scanf("%d", &id) != 1)
    {
        printf("Invalid ID.\n");
        return;
    }

    found = Find(id, H);
    if (found == NULL)
    {
        printf("Incident ID %d not found.\n", id);
        return;
    }

    incident = findIncidentByID(id);
    if (incident == NULL)
    {
        printf("Hash entry exists, but incident record is missing.\n");
        return;
    }

    printf("\n===== INCIDENT DETAILS =====\n");
    printf("ID: %d\nTitle: %s\nDescription: %s\nDistrict: %s\nLocation: %s\n",
           incident->id, incident->title, incident->description,
           incident->district, incident->location);
    printf("Flood Type: %d\nWater Level: %d\nSeverity: %d\n",
           incident->floodType, incident->waterLevel, incident->severity);
    printf("People Affected: %d\nUrgency: %d\nPriority Score: %d\n",
           incident->peopleAffected, incident->urgency, incident->priorityScore);
    printf("Verification: %s\n",
           incident->verificationStatus ? "Verified" : "Not verified");
    printf("Evidence Status: %s\n", getEvidenceStatusName(incident->evidenceStatus));
    printf("Life-Safety Tier: %s\n", getTriageTierName(incident->triageTier));
    printf("Resolution: %s\n",
           incident->resolutionStatus == 2 ? "Resolved" :
           incident->resolutionStatus == 1 ? "Rescue in progress" : "Pending");
}


void destroyHashTable(HashTable H)
{
    Hnode temp, nextNode;

    if (H == NULL)
        return;

    for (int i = 0; i < H->Tablesize; i++)
    {
        temp = H->Thelists[i];

        while (temp != NULL)
        {
            nextNode = temp->next;
            free(temp);
            temp = nextNode;
        }
    }

    free(H->Thelists);
    free(H);
}
