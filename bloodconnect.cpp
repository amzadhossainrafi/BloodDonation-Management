/*
 * ============================================================
 *  BLOODCONNECT - Blood Donation Management System (FIXED)
 *  Data Structures Demonstrated:
 *    1. ARRAY          - Donor & Blood Bank Database
 *    2. LINKED LIST    - Donation History Records
 *    3. STACK          - Undo Operations
 *    4. CIRCULAR QUEUE - Donor Waiting List
 *    5. BST            - Fast Donor Search by ID
 *    6. MIN-HEAP       - Emergency Blood Request Priority
 *    7. SORTING        - Bubble Sort & Selection Sort
 * ============================================================
 */

#include <iostream>
#include <string>
#include <iomanip>
#include <ctime>
#include <cstdlib>
using namespace std;

/* ─── Constants ─────────────────────────────────────────── */
#define MAX_DONORS      100
#define MAX_BANKS       20
#define QUEUE_SIZE      10
#define HEAP_SIZE       50

/* ─── Blood Type Priority Mapping ───────────────────────── */
int getBloodTypePriority(string bloodType) {
    if (bloodType == "O-") return 1;
    if (bloodType == "O+") return 2;
    if (bloodType == "A-") return 3;
    if (bloodType == "A+") return 4;
    if (bloodType == "B-") return 5;
    if (bloodType == "B+") return 6;
    if (bloodType == "AB-") return 7;
    if (bloodType == "AB+") return 8;
    return 8;
}

string getPriorityName(int priority) {
    if (priority == 1) return "CRITICAL (O-)";
    if (priority == 2) return "HIGH (O+)";
    if (priority == 3) return "HIGH (A-)";
    if (priority == 4) return "MEDIUM (A+)";
    if (priority == 5) return "MEDIUM (B-)";
    if (priority == 6) return "LOW (B+)";
    if (priority == 7) return "LOW (AB-)";
    return "NORMAL (AB+)";
}

/* ─── Structures ────────────────────────────────────────── */
struct Donor {
    int id;
    string name;
    int age;
    string bloodType;
    string contact;
    int donationCount;
    string lastDonationDate;
    int isEligible;
    
    Donor() {
        id = 0;
        age = 0;
        donationCount = 0;
        isEligible = 1;
        name = "";
        bloodType = "";
        contact = "";
        lastDonationDate = "";
    }
};

struct BloodBank {
    int id;
    string name;
    string location;
    int unitsAplus, unitsAminus, unitsBplus, unitsBminus;
    int unitsOplus, unitsOminus, unitsABplus, unitsABminus;
    int available;
    
    BloodBank() {
        id = 0;
        unitsAplus = unitsAminus = unitsBplus = unitsBminus = 0;
        unitsOplus = unitsOminus = unitsABplus = unitsABminus = 0;
        available = 1;
        name = "";
        location = "";
    }
};

/* ── 2. Linked List node (Donation History) ── */
struct HistoryNode {
    Donor donor;
    string donationDate;
    int units;
    HistoryNode *next;
    
    HistoryNode() {
        next = nullptr;
        donationDate = "";
        units = 0;
    }
};

/* ── 3. Stack node (Undo) ── */
struct StackNode {
    string operation;
    StackNode *next;
    
    StackNode() {
        next = nullptr;
        operation = "";
    }
};

/* ─── Global State ──────────────────────────────────────── */

/* 1. Arrays */
BloodBank bloodBanks[MAX_BANKS];
int bankCount = 0;

Donor donors[MAX_DONORS];
int donorCount = 0;

/* 2. Linked list */
HistoryNode *historyHead = nullptr;

/* 3. Stack */
StackNode *undoStack = nullptr;

/* 4. Circular Queue */
Donor waitingQueue[QUEUE_SIZE];
int front = -1, rear = -1;

/* ─── 5. BST for Donor Search by ID ─────────────────────── */
struct BSTNode {
    int donorIndex;
    BSTNode *left, *right;
    
    BSTNode() {
        donorIndex = -1;
        left = nullptr;
        right = nullptr;
    }
};

BSTNode *bstRoot = nullptr;

/* ─── 6. Min-Heap for Emergency Blood Requests ──────────── */
struct EmergencyRequest {
    int id;
    string patientName;
    string bloodType;
    int priority;
    
    EmergencyRequest() {
        id = 0;
        patientName = "";
        bloodType = "";
        priority = 8;
    }
};

EmergencyRequest heap[HEAP_SIZE];
int heapSize = 0;

/* ─── Function Prototypes ───────────────────────────────── */
void clearInput();
void pushUndo(const string &msg);
int getNextDonorId();

/* BST Functions */
BSTNode* bstInsert(BSTNode *root, int donorIndex);
int bstSearch(BSTNode *root, int id);
BSTNode* bstDelete(BSTNode *root, int id);
void bstInorder(BSTNode *root);
void freeBST(BSTNode *root);

/* Heap Functions */
void heapInsert(EmergencyRequest req);
EmergencyRequest heapExtractMin();
void showPriorityQueue();

/* Blood Bank Functions */
void initBloodBanks();

/* Menu Functions */
void donorManagementMenu();
void donationHistoryMenu();
void undoMenu();
void waitingQueueMenu();
void emergencyMenu();
void searchSortMenu();

/* ───────────────────────────────────────────────────────── */
void clearInput() {
    cin.ignore(1000, '\n');
}

void pushUndo(const string &msg) {
    StackNode *n = new StackNode();
    n->operation = msg;
    n->next = undoStack;
    undoStack = n;
}

int getNextDonorId() {
    return 2000 + donorCount;
}

/* ─── BST Implementation ────────────────────────────────── */
BSTNode* bstInsert(BSTNode *root, int donorIndex) {
    if (root == nullptr) {
        BSTNode *n = new BSTNode();
        n->donorIndex = donorIndex;
        return n;
    }
    if (donors[donorIndex].id < donors[root->donorIndex].id)
        root->left = bstInsert(root->left, donorIndex);
    else if (donors[donorIndex].id > donors[root->donorIndex].id)
        root->right = bstInsert(root->right, donorIndex);
    return root;
}

int bstSearch(BSTNode *root, int id) {
    if (root == nullptr) return -1;
    int rid = donors[root->donorIndex].id;
    if (id == rid) return root->donorIndex;
    if (id < rid) return bstSearch(root->left, id);
    return bstSearch(root->right, id);
}

BSTNode* bstFindMin(BSTNode *root) {
    while (root->left) root = root->left;
    return root;
}

BSTNode* bstDelete(BSTNode *root, int id) {
    if (root == nullptr) return nullptr;
    int rid = donors[root->donorIndex].id;
    if (id < rid)
        root->left = bstDelete(root->left, id);
    else if (id > rid)
        root->right = bstDelete(root->right, id);
    else {
        if (root->left == nullptr) {
            BSTNode *tmp = root->right;
            delete root;
            return tmp;
        } else if (root->right == nullptr) {
            BSTNode *tmp = root->left;
            delete root;
            return tmp;
        }
        BSTNode *succ = bstFindMin(root->right);
        root->donorIndex = succ->donorIndex;
        root->right = bstDelete(root->right, donors[succ->donorIndex].id);
    }
    return root;
}

void bstInorder(BSTNode *root) {
    if (root == nullptr) return;
    bstInorder(root->left);
    Donor *d = &donors[root->donorIndex];
    if (d->isEligible)
        cout << "  " << setw(6) << d->id 
             << " | " << setw(20) << left << d->name 
             << " | " << setw(3) << d->age 
             << " | " << setw(5) << d->bloodType 
             << " | " << setw(12) << d->donationCount << endl;
    bstInorder(root->right);
}

void freeBST(BSTNode *root) {
    if (root == nullptr) return;
    freeBST(root->left);
    freeBST(root->right);
    delete root;
}

/* ─── Min-Heap Priority Queue ──────────────────────────── */
void heapSwap(int a, int b) {
    EmergencyRequest tmp = heap[a];
    heap[a] = heap[b];
    heap[b] = tmp;
}

void heapifyUp(int i) {
    while (i > 0) {
        int parent = (i - 1) / 2;
        if (heap[parent].priority > heap[i].priority) {
            heapSwap(parent, i);
            i = parent;
        } else break;
    }
}

void heapifyDown(int i) {
    int smallest = i;
    int l = 2*i+1, r = 2*i+2;
    if (l < heapSize && heap[l].priority < heap[smallest].priority) smallest = l;
    if (r < heapSize && heap[r].priority < heap[smallest].priority) smallest = r;
    if (smallest != i) {
        heapSwap(i, smallest);
        heapifyDown(smallest);
    }
}

void heapInsert(EmergencyRequest req) {
    if (heapSize >= HEAP_SIZE) {
        cout << "  [!] Emergency queue is full!" << endl;
        return;
    }
    req.id = heapSize + 1;
    heap[heapSize++] = req;
    heapifyUp(heapSize - 1);
    cout << "  [OK] Emergency request for " << req.patientName 
         << " (Blood: " << req.bloodType << " | Priority: " << getPriorityName(req.priority) << ")" << endl;
}

EmergencyRequest heapExtractMin() {
    EmergencyRequest top = heap[0];
    heap[0] = heap[--heapSize];
    heapifyDown(0);
    return top;
}

void showPriorityQueue() {
    cout << "\n========== EMERGENCY REQUESTS (Min-Heap by Priority) ==========" << endl;
    if (heapSize == 0) { 
        cout << "  No emergency requests." << endl; 
        return; 
    }
    
    EmergencyRequest copy[HEAP_SIZE];
    for(int i = 0; i < heapSize; i++) copy[i] = heap[i];
    
    for (int i = 0; i < heapSize - 1; i++) {
        int mi = i;
        for (int j = i+1; j < heapSize; j++)
            if (copy[j].priority < copy[mi].priority) mi = j;
        EmergencyRequest tmp = copy[i]; copy[i] = copy[mi]; copy[mi] = tmp;
    }
    
    cout << "  " << left << setw(8) << "Order" 
         << setw(25) << "Priority" 
         << setw(12) << "Blood Type" 
         << "Patient Name" << endl;
    cout << "  ----------------------------------------------------------------" << endl;
    for (int i = 0; i < heapSize; i++) {
        cout << "  " << setw(8) << (i+1) 
             << setw(25) << getPriorityName(copy[i].priority)
             << setw(12) << copy[i].bloodType 
             << copy[i].patientName << endl;
    }
    cout << "  Total emergency requests: " << heapSize << endl;
}

/* ─── Blood Bank Initialization ─────────────────────────── */
void initBloodBanks() {
    struct InitBank { int id; const char *name; const char *location; };
    InitBank init[] = {
        {101, "City Blood Bank", "Downtown"},
        {102, "Red Cross Center", "Uptown"},
        {103, "LifeSaver Blood Bank", "Midtown"},
        {104, "Community Blood Center", "Westside"},
        {105, "Unity Blood Bank", "Eastside"},
    };
    for (int i = 0; i < 5; i++) {
        bloodBanks[i].id = init[i].id;
        bloodBanks[i].name = init[i].name;
        bloodBanks[i].location = init[i].location;
        bloodBanks[i].available = 1;
        bloodBanks[i].unitsAplus = 10;
        bloodBanks[i].unitsAminus = 5;
        bloodBanks[i].unitsBplus = 8;
        bloodBanks[i].unitsBminus = 3;
        bloodBanks[i].unitsOplus = 15;
        bloodBanks[i].unitsOminus = 4;
        bloodBanks[i].unitsABplus = 6;
        bloodBanks[i].unitsABminus = 2;
    }
    bankCount = 5;
}

void showBloodBanks() {
    cout << "\n========== BLOOD BANK LIST ==========" << endl;
    cout << "  " << left << setw(8) << "ID" 
         << setw(25) << "Name" 
         << setw(20) << "Location" 
         << "Status" << endl;
    cout << "  ---------------------------------------------------------------" << endl;
    for (int i = 0; i < bankCount; i++)
        cout << "  " << setw(8) << bloodBanks[i].id 
             << setw(25) << bloodBanks[i].name 
             << setw(20) << bloodBanks[i].location 
             << (bloodBanks[i].available ? "Available" : "Busy") << endl;
}

/* ─── Donor Functions ───────────────────────────────────── */
void showDonors() {
    cout << "\n========== DONOR LIST ==========" << endl;
    int shown = 0;
    cout << "  " << left << setw(8) << "ID" 
         << setw(22) << "Name" 
         << setw(6) << "Age" 
         << setw(8) << "Blood" 
         << setw(15) << "Contact" 
         << setw(12) << "Donations" 
         << "Status" << endl;
    cout << "  ---------------------------------------------------------------------------------------------" << endl;
    for (int i = 0; i < donorCount; i++) {
        cout << "  " << setw(8) << donors[i].id 
             << setw(22) << donors[i].name 
             << setw(6) << donors[i].age 
             << setw(8) << donors[i].bloodType 
             << setw(15) << donors[i].contact 
             << setw(12) << donors[i].donationCount 
             << (donors[i].isEligible ? "Eligible" : "Not Eligible") << endl;
        shown++;
    }
    if (!shown) cout << "  No donors registered." << endl;
    else cout << "\n  Total donors: " << shown << endl;
}

void addNewDonor() {
    if (donorCount >= MAX_DONORS) {
        cout << "  [!] Donor database is full!" << endl;
        return;
    }

    Donor d;
    d.id = getNextDonorId();
    d.donationCount = 0;
    d.isEligible = 1;

    cout << "\n========== ADD NEW DONOR ==========" << endl;
    cout << "  Name: ";
    clearInput();
    getline(cin, d.name);

    cout << "  Age: ";
    cin >> d.age;
    if (d.age < 18 || d.age > 65) { 
        cout << "  [!] Donor must be between 18-65 years old." << endl; 
        return; 
    }

    cout << "  Blood Type (A+, A-, B+, B-, O+, O-, AB+, AB-): ";
    clearInput();
    getline(cin, d.bloodType);
    
    // Convert to uppercase for consistency
    for (char &c : d.bloodType) c = toupper(c);

    cout << "  Contact Number: ";
    getline(cin, d.contact);

    int idx = donorCount;
    donors[idx] = d;
    donorCount++;

    bstRoot = bstInsert(bstRoot, idx);

    string msg = "ADD Donor: " + d.name + " (ID: " + to_string(d.id) + ", Blood: " + d.bloodType + ")";
    pushUndo(msg);

    cout << "\n  [OK] Donor added! ID: " << d.id << endl;
}

void deleteDonor() {
    int id;
    cout << "\n========== REMOVE DONOR ==========" << endl;
    cout << "  Enter Donor ID: ";
    cin >> id;

    int idx = bstSearch(bstRoot, id);
    if (idx == -1) { 
        cout << "  [!] Donor ID " << id << " not found." << endl; 
        return; 
    }
    if (!donors[idx].isEligible) { 
        cout << "  [!] Donor already removed." << endl; 
        return; 
    }

    donors[idx].isEligible = 0;
    bstRoot = bstDelete(bstRoot, id);

    string msg = "REMOVE Donor: " + donors[idx].name + " (ID: " + to_string(id) + ")";
    pushUndo(msg);

    cout << "  [OK] Donor " << donors[idx].name << " has been removed." << endl;
}

/* ─── Linked List – Donation History ────────────────────── */
void addDonationHistory() {
    int id;
    cout << "\n========== ADD DONATION HISTORY ==========" << endl;
    cout << "  Enter Donor ID: ";
    cin >> id;

    int idx = bstSearch(bstRoot, id);
    if (idx == -1) { 
        cout << "  [!] Donor with ID " << id << " not found." << endl; 
        return; 
    }

    HistoryNode *n = new HistoryNode();
    n->donor = donors[idx];

    cout << "  Donation Date (DD/MM/YYYY): ";
    clearInput();
    getline(cin, n->donationDate);

    cout << "  Units Donated (1 unit = 450ml): ";
    cin >> n->units;
    
    if (n->units <= 0 || n->units > 5) {
        cout << "  [!] Invalid units. Setting to 1 unit." << endl;
        n->units = 1;
    }

    donors[idx].donationCount++;
    donors[idx].lastDonationDate = n->donationDate;

    n->next = historyHead;
    historyHead = n;

    string msg = "DONATION: " + donors[idx].name + " donated " + to_string(n->units) + " units";
    pushUndo(msg);

    cout << "  [OK] Donation recorded for " << donors[idx].name << endl;
}

void showHistory() {
    cout << "\n========== DONATION HISTORY (Linked List) ==========" << endl;
    if (historyHead == nullptr) { 
        cout << "  No donation records yet." << endl; 
        return; 
    }
    HistoryNode *cur = historyHead;
    int cnt = 1;
    while (cur) {
        cout << "\n  --- Donation Record " << cnt++ << " ---" << endl;
        cout << "  Donor   : " << cur->donor.name << " (ID: " << cur->donor.id << ")" << endl;
        cout << "  Blood   : " << cur->donor.bloodType << endl;
        cout << "  Date    : " << cur->donationDate << endl;
        cout << "  Units   : " << cur->units << " unit(s)" << endl;
        cur = cur->next;
    }
}

/* ─── Stack – Undo ──────────────────────────────────────── */
void showUndoHistory() {
    cout << "\n========== UNDO HISTORY (Stack) ==========" << endl;
    if (!undoStack) { 
        cout << "  Stack is empty." << endl; 
        return; 
    }
    StackNode *cur = undoStack;
    int i = 1;
    while (cur && i <= 10) {
        cout << "  " << i++ << ". " << cur->operation << endl;
        cur = cur->next;
    }
}

void undoLast() {
    if (!undoStack) { 
        cout << "  [!] Nothing to undo." << endl; 
        return; 
    }
    cout << "  [UNDO] " << undoStack->operation << endl;
    StackNode *tmp = undoStack;
    undoStack = undoStack->next;
    delete tmp;
}

/* ─── Circular Queue – Waiting List ─────────────────────── */
bool isQueueFull() { 
    return (front == 0 && rear == QUEUE_SIZE-1) || ((rear+1) % QUEUE_SIZE == front); 
}

bool isQueueEmpty() { 
    return front == -1; 
}

void enqueue(Donor d) {
    if (isQueueFull()) { 
        cout << "  [!] Waiting queue is full!" << endl; 
        return; 
    }
    if (isQueueEmpty()) { 
        front = rear = 0; 
    } else {
        rear = (rear+1) % QUEUE_SIZE;
    }
    waitingQueue[rear] = d;
}

void addToWaitingQueue() {
    int id;
    cout << "\n========== ADD TO WAITING QUEUE ==========" << endl;
    cout << "  Enter Donor ID: ";
    cin >> id;

    int idx = bstSearch(bstRoot, id);
    if (idx == -1) { 
        cout << "  [!] Donor not found." << endl; 
        return; 
    }
    if (!donors[idx].isEligible) {
        cout << "  [!] Donor is not eligible." << endl;
        return;
    }

    enqueue(donors[idx]);
    cout << "  [OK] " << donors[idx].name << " added to waiting queue." << endl;
}

void showWaitingQueue() {
    cout << "\n========== DONOR WAITING QUEUE (Circular Queue) ==========" << endl;
    if (isQueueEmpty()) { 
        cout << "  Queue is empty." << endl; 
        return; 
    }
    cout << "  " << left << setw(8) << "Position" 
         << setw(22) << "Name" 
         << setw(10) << "Blood Type" 
         << "Contact" << endl;
    cout << "  ----------------------------------------------------------------" << endl;
    int i = front, pos = 1;
    for (;;) {
        cout << "  " << setw(8) << pos++ 
             << setw(22) << waitingQueue[i].name 
             << setw(10) << waitingQueue[i].bloodType 
             << waitingQueue[i].contact << endl;
        if (i == rear) break;
        i = (i+1) % QUEUE_SIZE;
    }
    cout << "  Total waiting donors: " << ((rear - front + QUEUE_SIZE) % QUEUE_SIZE + 1) << endl;
}

void processNextDonor() {
    if (isQueueEmpty()) { 
        cout << "  [!] No donors in queue." << endl; 
        return; 
    }
    cout << "\n========== PROCESSING NEXT DONOR ==========" << endl;
    cout << "  Name    : " << waitingQueue[front].name << " (ID: " << waitingQueue[front].id << ")" << endl;
    cout << "  Blood   : " << waitingQueue[front].bloodType << endl;
    cout << "  Contact : " << waitingQueue[front].contact << endl;
    
    if (front == rear) { 
        front = rear = -1; 
    } else {
        front = (front+1) % QUEUE_SIZE;
    }
    cout << "  [OK] Donor sent for donation process." << endl;
}

/* ─── Emergency Request Functions ───────────────────────── */
void addEmergencyRequest() {
    EmergencyRequest req;
    
    cout << "\n========== ADD EMERGENCY BLOOD REQUEST ==========" << endl;
    cout << "  Patient Name: ";
    clearInput();
    getline(cin, req.patientName);
    
    cout << "  Required Blood Type (O-, O+, A-, A+, B-, B+, AB-, AB+): ";
    getline(cin, req.bloodType);
    for (char &c : req.bloodType) c = toupper(c);
    
    req.priority = getBloodTypePriority(req.bloodType);
    
    heapInsert(req);
}

void serveHighestEmergency() {
    if (heapSize == 0) { 
        cout << "  [!] No emergency requests." << endl; 
        return; 
    }
    EmergencyRequest req = heapExtractMin();
    cout << "\n========== SERVING HIGHEST PRIORITY REQUEST ==========" << endl;
    cout << "  Patient    : " << req.patientName << endl;
    cout << "  Blood Type : " << req.bloodType << endl;
    cout << "  Priority   : " << getPriorityName(req.priority) << endl;
    cout << "  [OK] Blood request dispatched to nearest blood bank." << endl;
}

/* ─── Sorting Functions ─────────────────────────────────── */
void bubbleSortByName() {
    Donor temp[MAX_DONORS];
    int n = 0;
    for (int i = 0; i < donorCount; i++)
        if (donors[i].isEligible) temp[n++] = donors[i];

    for (int i = 0; i < n-1; i++)
        for (int j = 0; j < n-i-1; j++)
            if (temp[j].name > temp[j+1].name) {
                Donor t = temp[j]; temp[j] = temp[j+1]; temp[j+1] = t;
            }

    cout << "\n========== DONORS SORTED BY NAME (Bubble Sort) ==========" << endl;
    cout << "  " << left << setw(8) << "ID" 
         << setw(22) << "Name" 
         << setw(8) << "Blood" 
         << setw(6) << "Age" 
         << "Donations" << endl;
    cout << "  ----------------------------------------------------------------" << endl;
    for (int i = 0; i < n; i++)
        cout << "  " << setw(8) << temp[i].id 
             << setw(22) << temp[i].name 
             << setw(8) << temp[i].bloodType 
             << setw(6) << temp[i].age 
             << temp[i].donationCount << endl;
}

void selectionSortByBloodType() {
    Donor temp[MAX_DONORS];
    int n = 0;
    for (int i = 0; i < donorCount; i++)
        if (donors[i].isEligible) temp[n++] = donors[i];

    for (int i = 0; i < n-1; i++) {
        int mi = i;
        for (int j = i+1; j < n; j++)
            if (getBloodTypePriority(temp[j].bloodType) < getBloodTypePriority(temp[mi].bloodType)) 
                mi = j;
        Donor t = temp[i]; temp[i] = temp[mi]; temp[mi] = t;
    }

    cout << "\n========== DONORS SORTED BY BLOOD TYPE PRIORITY (Selection Sort) ==========" << endl;
    cout << "  " << left << setw(10) << "Priority" 
         << setw(10) << "Blood" 
         << setw(22) << "Name" 
         << "Contact" << endl;
    cout << "  -------------------------------------------------------------------------" << endl;
    for (int i = 0; i < n; i++)
        cout << "  " << setw(10) << getBloodTypePriority(temp[i].bloodType)
             << setw(10) << temp[i].bloodType 
             << setw(22) << temp[i].name 
             << temp[i].contact << endl;
}

/* ─── Search Functions ──────────────────────────────────── */
void searchByID() {
    int id;
    cout << "\n========== SEARCH BY ID (BST) ==========" << endl;
    cout << "  Enter Donor ID: ";
    cin >> id;

    int idx = bstSearch(bstRoot, id);
    if (idx == -1) { 
        cout << "  [!] No donor with ID " << id << "." << endl; 
        return; 
    }

    Donor *d = &donors[idx];
    cout << "\n  +------------------+-------------------------+" << endl;
    cout << "  | ID          : " << setw(20) << left << d->id << "|" << endl;
    cout << "  | Name        : " << setw(20) << d->name << "|" << endl;
    cout << "  | Age         : " << setw(20) << d->age << "|" << endl;
    cout << "  | Blood Type  : " << setw(20) << d->bloodType << "|" << endl;
    cout << "  | Contact     : " << setw(20) << d->contact << "|" << endl;
    cout << "  | Donations   : " << setw(20) << d->donationCount << "|" << endl;
    cout << "  | Last Donation: " << setw(19) << d->lastDonationDate << "|" << endl;
    cout << "  | Status      : " << setw(20) << (d->isEligible ? "Eligible" : "Not Eligible") << "|" << endl;
    cout << "  +------------------+-------------------------+" << endl;
}

void searchByBloodType() {
    string bloodType;
    cout << "\n========== SEARCH BY BLOOD TYPE ==========" << endl;
    cout << "  Enter Blood Type (A+, A-, B+, B-, O+, O-, AB+, AB-): ";
    clearInput();
    getline(cin, bloodType);
    for (char &c : bloodType) c = toupper(c);

    int found = 0;
    cout << "\n  Donors with blood type " << bloodType << ":" << endl;
    cout << "  -------------------------------------------------" << endl;
    cout << "  " << left << setw(8) << "ID" << setw(22) << "Name" 
         << setw(6) << "Age" << "Contact" << endl;
    for (int i = 0; i < donorCount; i++) {
        if (donors[i].isEligible && donors[i].bloodType == bloodType) {
            cout << "  " << setw(8) << donors[i].id 
                 << setw(22) << donors[i].name 
                 << setw(6) << donors[i].age 
                 << donors[i].contact << endl;
            found = 1;
        }
    }
    if (!found) cout << "  No donors found with blood type " << bloodType << "." << endl;
}

void showBSTInorder() {
    cout << "\n========== BST INORDER TRAVERSAL (Donors sorted by ID) ==========" << endl;
    if (!bstRoot) { 
        cout << "  BST is empty." << endl; 
        return; 
    }
    cout << "  " << left << setw(8) << "ID" 
         << setw(22) << "Name" 
         << setw(6) << "Age" 
         << setw(8) << "Blood" 
         << "Donations" << endl;
    cout << "  ---------------------------------------------------------------" << endl;
    bstInorder(bstRoot);
}

/* ─── Sub-Menus ─────────────────────────────────────────── */
void donorManagementMenu() {
    int choice;
    do {
        cout << "\n========== DONOR MANAGEMENT ==========" << endl;
        cout << "  1. View All Blood Banks" << endl;
        cout << "  2. View All Donors" << endl;
        cout << "  3. Add New Donor" << endl;
        cout << "  4. Remove Donor" << endl;
        cout << "  0. Back to Main Menu" << endl;
        cout << "=======================================" << endl;
        cout << "  Choice: ";
        cin >> choice;

        switch(choice) {
            case 1: showBloodBanks(); break;
            case 2: showDonors(); break;
            case 3: addNewDonor(); break;
            case 4: deleteDonor(); break;
            case 0: cout << "  Returning to main menu..." << endl; break;
            default: cout << "  [!] Invalid choice!" << endl;
        }
    } while(choice != 0);
}

void donationHistoryMenu() {
    int choice;
    do {
        cout << "\n========== DONATION HISTORY (Linked List) ==========" << endl;
        cout << "  1. View Donation History" << endl;
        cout << "  2. Add Donation Record" << endl;
        cout << "  0. Back to Main Menu" << endl;
        cout << "====================================================" << endl;
        cout << "  Choice: ";
        cin >> choice;

        switch(choice) {
            case 1: showHistory(); break;
            case 2: addDonationHistory(); break;
            case 0: cout << "  Returning to main menu..." << endl; break;
            default: cout << "  [!] Invalid choice!" << endl;
        }
    } while(choice != 0);
}

void undoMenu() {
    int choice;
    do {
        cout << "\n========== UNDO OPERATIONS (Stack) ==========" << endl;
        cout << "  1. View Undo History" << endl;
        cout << "  2. Undo Last Operation" << endl;
        cout << "  0. Back to Main Menu" << endl;
        cout << "=============================================" << endl;
        cout << "  Choice: ";
        cin >> choice;

        switch(choice) {
            case 1: showUndoHistory(); break;
            case 2: undoLast(); break;
            case 0: cout << "  Returning to main menu..." << endl; break;
            default: cout << "  [!] Invalid choice!" << endl;
        }
    } while(choice != 0);
}

void waitingQueueMenu() {
    int choice;
    do {
        cout << "\n========== DONOR WAITING QUEUE (Circular Queue) ==========" << endl;
        cout << "  1. Add Donor to Waiting Queue" << endl;
        cout << "  2. View Waiting Queue" << endl;
        cout << "  3. Process Next Donor (Dequeue)" << endl;
        cout << "  0. Back to Main Menu" << endl;
        cout << "===========================================================" << endl;
        cout << "  Choice: ";
        cin >> choice;

        switch(choice) {
            case 1: addToWaitingQueue(); break;
            case 2: showWaitingQueue(); break;
            case 3: processNextDonor(); break;
            case 0: cout << "  Returning to main menu..." << endl; break;
            default: cout << "  [!] Invalid choice!" << endl;
        }
    } while(choice != 0);
}

void emergencyMenu() {
    int choice;
    do {
        cout << "\n========== EMERGENCY BLOOD REQUESTS (Min-Heap) ==========" << endl;
        cout << "  1. Add Emergency Blood Request" << endl;
        cout << "  2. View All Emergency Requests" << endl;
        cout << "  3. Serve Highest Priority Request" << endl;
        cout << "  0. Back to Main Menu" << endl;
        cout << "=========================================================" << endl;
        cout << "  Choice: ";
        cin >> choice;

        switch(choice) {
            case 1: addEmergencyRequest(); break;
            case 2: showPriorityQueue(); break;
            case 3: serveHighestEmergency(); break;
            case 0: cout << "  Returning to main menu..." << endl; break;
            default: cout << "  [!] Invalid choice!" << endl;
        }
    } while(choice != 0);
}

void searchSortMenu() {
    int choice;
    do {
        cout << "\n========== SEARCH & SORT ==========" << endl;
        cout << "  1. Search Donor by ID (BST)" << endl;
        cout << "  2. Search Donor by Blood Type" << endl;
        cout << "  3. BST Inorder Traversal (Sorted IDs)" << endl;
        cout << "  4. Sort Donors by Name (Bubble Sort)" << endl;
        cout << "  5. Sort Donors by Blood Type (Selection Sort)" << endl;
        cout << "  0. Back to Main Menu" << endl;
        cout << "===================================" << endl;
        cout << "  Choice: ";
        cin >> choice;

        switch(choice) {
            case 1: searchByID(); break;
            case 2: searchByBloodType(); break;
            case 3: showBSTInorder(); break;
            case 4: bubbleSortByName(); break;
            case 5: selectionSortByBloodType(); break;
            case 0: cout << "  Returning to main menu..." << endl; break;
            default: cout << "  [!] Invalid choice!" << endl;
        }
    } while(choice != 0);
}

/* ─── Cleanup ───────────────────────────────────────────── */
void cleanup() {
    freeBST(bstRoot);
    bstRoot = nullptr;

    HistoryNode *h = historyHead;
    while (h) { 
        HistoryNode *t = h; 
        h = h->next; 
        delete t; 
    }
    historyHead = nullptr;

    StackNode *s = undoStack;
    while (s) { 
        StackNode *t = s; 
        s = s->next; 
        delete t; 
    }
    undoStack = nullptr;
}

/* ─── Main Menu ─────────────────────────────────────────── */
int main() {
    initBloodBanks();

    cout << "\n";
    cout << "  =====================================================" << endl;
    cout << "         🩸 BLOODCONNECT - Blood Donation System       " << endl;
    cout << "  =====================================================" << endl;
    cout << "  Data Structures Used:" << endl;
    cout << "    1. ARRAY          - Donor & Blood Bank Database" << endl;
    cout << "    2. LINKED LIST    - Donation History Records" << endl;
    cout << "    3. STACK          - Undo Operation Log" << endl;
    cout << "    4. CIRCULAR QUEUE - Donor Waiting List" << endl;
    cout << "    5. BST            - Fast Donor Lookup by ID" << endl;
    cout << "    6. MIN-HEAP       - Emergency Blood Request Priority" << endl;
    cout << "    7. SORTING        - Bubble Sort & Selection Sort" << endl;
    cout << "  =====================================================" << endl;

    int choice;
    do {
        cout << "\n========== MAIN MENU ==========" << endl;
        cout << "  1. Donor Management (Array)" << endl;
        cout << "  2. Donation History (Linked List)" << endl;
        cout << "  3. Undo Operations (Stack)" << endl;
        cout << "  4. Donor Waiting Queue (Circular Queue)" << endl;
        cout << "  5. Emergency Requests (Min-Heap)" << endl;
        cout << "  6. Search & Sort" << endl;
        cout << "  0. Exit" << endl;
        cout << "===============================" << endl;
        cout << "  Choice: ";
        cin >> choice;

        switch(choice) {
            case 1: donorManagementMenu(); break;
            case 2: donationHistoryMenu(); break;
            case 3: undoMenu(); break;
            case 4: waitingQueueMenu(); break;
            case 5: emergencyMenu(); break;
            case 6: searchSortMenu(); break;
            case 0: cout << "\n  Thank you for using BloodConnect! 🩸\n\n" << endl; break;
            default: cout << "  [!] Invalid option. Try again." << endl;
        }
    } while(choice != 0);

    cleanup();
    return 0;
}