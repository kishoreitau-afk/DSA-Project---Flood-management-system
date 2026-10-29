#include "common.h"

//For official login user cant be respond for the rescue operation//
#define OFFICIAL_USERNAME "official"
#define OFFICIAL_PASSWORD "Flood@2026!"

static void addReportedIncident(HashTable H)
{
    int oldID = last == NULL ? -1 : last->info.id;
    Incident newIncident;

    reportIncident(&newIncident, H);
    if (last != NULL && last->info.id != oldID)
    {
        Incident *incident = &last->info;
        enqueueIncident(incident->id);
        appendHistoryForIncident(incident, REPORTED, "Incident reported by citizen");
    }
}

static const char *publicStatus(const Incident *incident)
{
    if (incident == NULL) return "Unknown";
    if (incident->resolutionStatus == 2) return "Resolved";
    if (incident->resolutionStatus == 1) return "Rescue in progress";
    if (incident->verificationStatus == 1) return "Verified; awaiting rescue";
    return "Received; awaiting official review";
}

static void trackPublicIncident(void)
{
    int id;
    Incident *incident;

    printf("Enter your incident ID: ");
    if (scanf("%d", &id) != 1)
    {
        printf("Invalid incident ID.\n");
        return;
    }

    incident = findIncidentByID(id);
    if (incident == NULL)
    {
        printf("Incident not found. Check the ID and try again.\n");
        return;
    }

    printf("\n===== PUBLIC INCIDENT TRACKING =====\n");
    printf("Incident ID: %d\n", incident->id);
    printf("Title: %s\n", incident->title);
    printf("District: %s\n", incident->district);
    printf("Location: %s\n", incident->location);
    printf("Current status: %s\n", publicStatus(incident));
    printf("Please contact local emergency services if anyone is in immediate danger.\n");
}

static int officialLogin(void)
{
    char username[64], password[128];

    printf("\n===== OFFICIAL LOGIN =====\n");
    printf("Username: ");
    if (scanf("%63s", username) != 1) return 0;
    printf("Password (demo input is visible): ");
    if (scanf("%127s", password) != 1) return 0;

    if (strcmp(username, OFFICIAL_USERNAME) == 0 &&
        strcmp(password, OFFICIAL_PASSWORD) == 0)
    {
        printf("Official login successful.\n");
        return 1;
    }

    printf("Access denied. Invalid official credentials.\n");
    return 0;
}

static void citizenMenu(HashTable H)
{
    int choice;
    do
    {
        printf("\n========== CITIZEN PORTAL (TERMINAL) ==========\n");
        printf("1. Report a flood incident\n");
        printf("2. Track incident status by ID\n");
        printf("0. Return to role selection\n");
        printf("Enter choice: ");

        if (scanf("%d", &choice) != 1)
        {
            printf("Invalid input. Returning to role selection.\n");
            return;
        }

        switch (choice)
        {
            case 1: addReportedIncident(H); break;
            case 2: trackPublicIncident(); break;
            case 0: printf("Returning to role selection...\n"); break;
            default: printf("Invalid menu choice.\n");
        }
    } while (choice != 0);
}

static void officialMenu(HashTable H)
{
    int choice, id;
    Incident *incident;

    do
    {
        printf("\n========== OFFICIAL CONTROL ROOM ==========\n");
        printf("1. View all incidents\n");
        printf("2. Find incident by ID (Hash Table)\n");
        printf("3. Verify evidence / assign triage / update rescue status\n");
        printf("4. View priority dashboard (Max-Heap)\n");
        printf("5. View statistics and reports\n");
        printf("6. Display incident status history\n");
        printf("7. Display pending rescue queue\n");
        printf("8. Process next incident by life-first priority\n");
        printf("0. Log out\n");
        printf("Enter choice: ");

        if (scanf("%d", &choice) != 1)
        {
            printf("Invalid input. Logging out.\n");
            return;
        }

        switch (choice)
        {
            case 1:
                displayAllIncidents();
                break;
            case 2:
                searchIncidentByHash(H);
                break;
            case 3:
                printf("Enter incident ID: ");
                if (scanf("%d", &id) != 1)
                {
                    printf("Invalid ID.\n");
                    break;
                }
                incident = findIncidentByID(id);
                if (incident == NULL)
                    printf("Incident not found.\n");
                else
                    updateResolutionStatus(incident);
                break;
            case 4:
                viewPriorityDashboard();
                break;
            case 5:
                viewStatistics();
                break;
            case 6:
                printf("Enter incident ID for history: ");
                if (scanf("%d", &id) != 1)
                    printf("Invalid ID.\n");
                else
                    displayIncidentHistory(findIncidentByID(id));
                break;
            case 7:
                displayPendingQueue();
                break;
            case 8:
                processNextIncident();
                break;
            case 0:
                printf("Official logged out.\n");
                break;
            default:
                printf("Invalid menu choice.\n");
        }
    } while (choice != 0);
}

int main(void)
{
    HashTable H;
    int role;

    create();
    cq = NULL;
    initializeQueue(1000);
    if (cq == NULL)
    {
        printf("Queue initialization failed.\n");
        return 1;
    }

    H = initialize(101);
    if (H == NULL)
    {
        freeQueue();
        printf("Hash table initialization failed.\n");
        return 1;
    }

    do
    {
        printf("\n====== FLOOD INCIDENT MANAGEMENT SYSTEM ======\n");
        printf("1. Citizen portal (report / track only)\n");
        printf("2. Official control room (authorized operations)\n");
        printf("0. Exit system\n");
        printf("Select role: ");

        if (scanf("%d", &role) != 1)
        {
            printf("Invalid input. Exiting system.\n");
            break;
        }

        switch (role)
        {
            case 1:
                citizenMenu(H);
                break;
            case 2:
                if (officialLogin()) officialMenu(H);
                break;
            case 0:
                printf("Exiting system...\n");
                break;
            default:
                printf("Invalid role selection.\n");
        }
    } while (role != 0);

    freeIncidentList();
    destroyHashTable(H);
    freeQueue();
    freeHistory();
    return 0;
}
