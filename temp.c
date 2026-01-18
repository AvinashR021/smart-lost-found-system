#include <windows.h>
#include <stdio.h>
#include <string.h>

#define MAX_ITEMS 200

struct Item {
    int id;
    char name[30];
    char category[20];
    char location[20];
    char status[10];   // lost, found, returned
    char contact[50];
};

struct Item items[MAX_ITEMS];
int itemCount = 0;
int nextID = 1001;

/* GUI Controls */
HWND hName, hCategory, hLocation, hStatus, hContact;
HWND hSearchName, hSearchLocation;

/* ================= FILE HANDLING ================= */

void saveItems() {
    FILE *fp = fopen("items.txt", "w");
    if (!fp) return;
    for (int i = 0; i < itemCount; i++) {
        fprintf(fp, "%d %s %s %s %s %s\n",
            items[i].id,
            items[i].name,
            items[i].category,
            items[i].location,
            items[i].status,
            items[i].contact);
    }
    fclose(fp);
}

void loadItems() {
    FILE *fp = fopen("items.txt", "r");
    if (!fp) return;

    while (fscanf(fp, "%d %s %s %s %s %s",
        &items[itemCount].id,
        items[itemCount].name,
        items[itemCount].category,
        items[itemCount].location,
        items[itemCount].status,
        items[itemCount].contact) != EOF) {

        if (items[itemCount].id >= nextID)
            nextID = items[itemCount].id + 1;

        itemCount++;
    }
    fclose(fp);
}

/* ================= ADD ITEM ================= */

void addItem(HWND hwnd) {
    char name[30], category[20], location[20], status[10], contact[50];

    GetWindowText(hName, name, 30);
    GetWindowText(hCategory, category, 20);
    GetWindowText(hLocation, location, 20);
    GetWindowText(hStatus, status, 10);
    GetWindowText(hContact, contact, 50);

    if (strlen(name) == 0 || strlen(location) == 0 || strlen(status) == 0) {
        MessageBox(hwnd, "Please fill required fields!", "Error", MB_OK);
        return;
    }

    items[itemCount].id = nextID++;
    strcpy(items[itemCount].name, name);
    strcpy(items[itemCount].category, category);
    strcpy(items[itemCount].location, location);
    strcpy(items[itemCount].status, status);
    strcpy(items[itemCount].contact, contact);
    itemCount++;

    /* Notification Logic */
    if (strcmp(status, "found") == 0) {
        for (int i = 0; i < itemCount - 1; i++) {
            if (strcmp(items[i].name, name) == 0 &&
                strcmp(items[i].location, location) == 0 &&
                strcmp(items[i].status, "lost") == 0) {

                char msg[200];
                sprintf(msg,
                    "MATCH FOUND!\nLost Item ID: %d\nOwner Contact: %s",
                    items[i].id, items[i].contact);

                MessageBox(hwnd, msg, "NOTIFICATION", MB_OK);
            }
        }
    }

    saveItems();
    MessageBox(hwnd, "Item added successfully!", "Success", MB_OK);
}

/* ================= SEARCH & CLAIM ================= */

void searchAndClaim(HWND hwnd) {
    char name[30], location[20];
    GetWindowText(hSearchName, name, 30);
    GetWindowText(hSearchLocation, location, 20);

    for (int i = 0; i < itemCount; i++) {
        if (strcmp(items[i].name, name) == 0 &&
            strcmp(items[i].location, location) == 0 &&
            strcmp(items[i].status, "lost") == 0) {

            char msg[300];
            sprintf(msg,
                "Item Found!\nID: %d\nCategory: %s\nContact: %s\n\nMark as Returned?",
                items[i].id, items[i].category, items[i].contact);

            if (MessageBox(hwnd, msg, "Claim Item", MB_YESNO) == IDYES) {
                strcpy(items[i].status, "returned");
                saveItems();
                MessageBox(hwnd, "Item marked as RETURNED", "Success", MB_OK);
            }
            return;
        }
    }
    MessageBox(hwnd, "Item not found", "Info", MB_OK);
}

/* ================= DISPLAY ITEMS ================= */

void displayItems(HWND hwnd) {
    char buffer[2000] = "";
    for (int i = 0; i < itemCount; i++) {
        char line[200];
        sprintf(line,
            "ID:%d  Name:%s  Location:%s  Status:%s\n",
            items[i].id,
            items[i].name,
            items[i].location,
            items[i].status);
        strcat(buffer, line);
    }
    MessageBox(hwnd, buffer, "All Items", MB_OK);
}

/* ================= WINDOW PROCEDURE ================= */

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_COMMAND:
        switch (LOWORD(wp)) {
        case 1: addItem(hwnd); break;
        case 2: searchAndClaim(hwnd); break;
        case 3: displayItems(hwnd); break;
        case 4: PostQuitMessage(0); break;
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    }
    return DefWindowProc(hwnd, msg, wp, lp);
}

/* ================= MAIN ================= */

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nCmdShow) {

    loadItems();

    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInst;
    wc.lpszClassName = "LostFoundApp";
    RegisterClass(&wc);

    HWND hwnd = CreateWindow("LostFoundApp",
        "Smart Lost & Found System",
        WS_OVERLAPPEDWINDOW,
        100, 100, 700, 500,
        NULL, NULL, hInst, NULL);

    /* ADD ITEM INPUTS */
    CreateWindow("STATIC", "Name", WS_VISIBLE | WS_CHILD, 20, 20, 60, 20, hwnd, NULL, hInst, NULL);
    hName = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER, 100, 20, 150, 20, hwnd, NULL, hInst, NULL);

    CreateWindow("STATIC", "Category", WS_VISIBLE | WS_CHILD, 20, 50, 60, 20, hwnd, NULL, hInst, NULL);
    hCategory = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER, 100, 50, 150, 20, hwnd, NULL, hInst, NULL);

    CreateWindow("STATIC", "Location", WS_VISIBLE | WS_CHILD, 20, 80, 60, 20, hwnd, NULL, hInst, NULL);
    hLocation = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER, 100, 80, 150, 20, hwnd, NULL, hInst, NULL);

    CreateWindow("STATIC", "Status", WS_VISIBLE | WS_CHILD, 20, 110, 60, 20, hwnd, NULL, hInst, NULL);
    hStatus = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER, 100, 110, 150, 20, hwnd, NULL, hInst, NULL);

    CreateWindow("STATIC", "Contact", WS_VISIBLE | WS_CHILD, 20, 140, 60, 20, hwnd, NULL, hInst, NULL);
    hContact = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER, 100, 140, 150, 20, hwnd, NULL, hInst, NULL);

    CreateWindow("BUTTON", "Add Item", WS_VISIBLE | WS_CHILD, 300, 30, 120, 30, hwnd, (HMENU)1, hInst, NULL);

    /* SEARCH */
    CreateWindow("STATIC", "Search Name", WS_VISIBLE | WS_CHILD, 20, 200, 80, 20, hwnd, NULL, hInst, NULL);
    hSearchName = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER, 120, 200, 150, 20, hwnd, NULL, hInst, NULL);

    CreateWindow("STATIC", "Search Location", WS_VISIBLE | WS_CHILD, 20, 230, 100, 20, hwnd, NULL, hInst, NULL);
    hSearchLocation = CreateWindow("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER, 120, 230, 150, 20, hwnd, NULL, hInst, NULL);

    CreateWindow("BUTTON", "Search & Claim", WS_VISIBLE | WS_CHILD, 300, 210, 150, 30, hwnd, (HMENU)2, hInst, NULL);

    CreateWindow("BUTTON", "Display All", WS_VISIBLE | WS_CHILD, 300, 260, 150, 30, hwnd, (HMENU)3, hInst, NULL);
    CreateWindow("BUTTON", "Exit", WS_VISIBLE | WS_CHILD, 300, 310, 150, 30, hwnd, (HMENU)4, hInst, NULL);

    ShowWindow(hwnd, nCmdShow);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}
