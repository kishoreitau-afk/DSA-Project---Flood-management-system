#ifndef FLOOD_INCIDENT_COMMON_H
#define FLOOD_INCIDENT_COMMON_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef enum { REPORTED, VERIFIED, RESCUE_IN_PROGRESS, RESOLVED } Incidentstatus;
typedef struct { Incidentstatus status; char timestamp[20]; char note[300]; } Incident_hist_info;
typedef struct HistoryNode_Rec { Incident_hist_info info; struct HistoryNode_Rec *next; } HistoryNode_Rec;
typedef HistoryNode_Rec *Histnode;

typedef struct {
    int id;
    char title[100], description[300], district[50], location[150];
    int floodType, waterLevel, severity, peopleAffected, urgency, priorityScore;
    int verificationStatus, resolutionStatus;
    int triageTier; /* 0=needs triage, 1=immediate life threat, 2=high risk, 3=mass evacuation, 4=monitor */
    int evidenceStatus; /* 0=unverified, 1=corroborated, 2=officially verified, 3=disputed */
    int immediateThreat; /* dispatcher-confirmed life threat */
    char reportedAt[30], resolutionNotes[200];
    Histnode historyHead, historyTail;
} Incident;
typedef struct list_node_rec { Incident info; struct list_node_rec *next; } list_node_rec;
typedef list_node_rec *node;

typedef struct hash_node_rec { int incidentID; struct hash_node_rec *next; } hash_node_rec;
typedef hash_node_rec *Hnode;
typedef Hnode List;
typedef struct HashTbl { int Tablesize; List *Thelists; } HashTbl;
typedef HashTbl *HashTable;

typedef struct { int *cq_arr; int front, rear, size, capacity; } queue_rec;
typedef queue_rec *cqueue;

extern node start, last, temp, prev, new_node;
extern cqueue cq;
extern Histnode hist_temp, hist_prev, hist_start, hist_last, hist_new_node;

void create(void); int isEmpty(void); int generateID(void);
void getDetails(Incident *i); int validateIncidentData(Incident x);
int insertIncident(Incident i); int Priorityscore(Incident *i); void reportIncident(Incident *i, HashTable H);
void freeIncidentList(void);
int NextPrime(int size); HashTable initialize(int size); int hash(int key, int size); void destroyHashTable(HashTable H);
Hnode Find(int key, HashTable H); HashTable insert(int key, HashTable H); int deleteHash(int key, HashTable H);
void searchIncidentByHash(HashTable H);
void swapIncidentPointers(Incident **a, Incident **b); void siftDown(Incident **heap, int n, int index); void viewPriorityDashboard(void);
void initializeQueue(int max); int isQueueEmpty(void); int isQueueFull(void); void enqueueIncident(int id); int dequeueIncident(int *id); void displayPendingQueue(void); void freeQueue(void);
void createHistoryNode(void); int ishistoryEmpty(void); int appendHistory(Incident_hist_info i); const char *getStatusName(Incidentstatus status); void displayIncientHistory(void); void freeHistory(void);
Incident *findIncidentByID(int id); int appendHistoryForIncident(Incident *incident, Incidentstatus status, const char *note); void displayIncidentHistory(Incident *incident); void updateResolutionStatus(Incident *incident); void processNextIncident(void);
int incidentHigherPriority(const Incident *a, const Incident *b); const char *getTriageTierName(int tier); const char *getEvidenceStatusName(int status);
void viewStatistics(void); void displayAllIncidents(void); void freeIncidentHistory(void);
#endif
