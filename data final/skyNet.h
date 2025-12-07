#pragma once
#ifndef SKYNET_H
#define SKYNET_H

#include <iostream>
#include <string>
#include <fstream>
#include <windows.h>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <sstream>
#include <climits>
#include <cstring>
#include <chrono>

using namespace std;

// ===== COLORS =====
const int COLOR_RESET = 7;
const int COLOR_HEADER = 11;
const int COLOR_SUCCESS = 10;
const int COLOR_WARNING = 14;
const int COLOR_DANGER = 12;
const int COLOR_INFO = 9;
const int COLOR_CRITICAL = 13;
const int MENU = 15;
const int COLOR_SUBTITLE = 8;

void setColor(int color)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

string toString(int value)
{
    stringstream ss;
    ss << value;
    return ss.str();
}

// ===== EXCEPTIONS =====
class AircraftException
{
protected:
    string message;
public:
    AircraftException(string msg) : message(msg) {}
    virtual string what() { return message; }
};

class FlightNotFoundException : public AircraftException
{
public:
    FlightNotFoundException(string id)
        : AircraftException("Flight " + id + " not found in system") {
    }
};

class InvalidInputException : public AircraftException
{
public:
    InvalidInputException(string field)
        : AircraftException("Invalid input for " + field) {
    }
};

class HeapOverflowException : public AircraftException
{
public:
    HeapOverflowException()
        : AircraftException("Landing queue is full") {
    }
};

class FileOperationException : public AircraftException
{
public:
    FileOperationException(string filename)
        : AircraftException("Failed to open file: " + filename) {
    }
};

class EmptyQueueException : public AircraftException
{
public:
    EmptyQueueException()
        : AircraftException("No aircraft in landing queue") {
    }
};

class CollisionException : public AircraftException
{
public:
    CollisionException(string pos)
        : AircraftException("Collision at " + pos) {
    }
};

class NoPathException : public AircraftException
{
public:
    NoPathException(string from, string to)
        : AircraftException("No path from " + from + " to " + to) {
    }
};

// ===== AIRCRAFT =====
struct Aircraft
{
    string flightID;
    string model;
    int fuelLevel;
    string origin;
    string destination;
    int priority;
    string status;
    int x;
    int y;
    long timestamp;
    string currentNode;

    Aircraft() : fuelLevel(0), priority(4), x(0), y(0), timestamp(0), status("In Flight") {}

    Aircraft(string id, string m, int fuel, string orig, string dest, int prio)
        : flightID(id), model(m), fuelLevel(fuel), origin(orig), destination(dest),
        priority(prio), status("In Flight"), x(0), y(0), timestamp(0) {
    }

    void validate()
    {
        if (flightID.empty()) throw InvalidInputException("Flight ID");
        if (model.empty()) throw InvalidInputException("Aircraft Model");
        if (fuelLevel < 0 || fuelLevel > 100) throw InvalidInputException("Fuel Level");
        if (priority < 1 || priority > 4) throw InvalidInputException("Priority");
    }

    string toString() const
    {
        stringstream ss;
        ss << flightID << "," << model << "," << fuelLevel << ","
            << origin << "," << destination << "," << priority << ","
            << status << "," << x << "," << y << "," << currentNode << "," << timestamp;
        return ss.str();
    }

    static Aircraft fromString(const string& data)
    {
        Aircraft a;
        stringstream ss(data);
        string token;

        getline(ss, a.flightID, ',');
        getline(ss, a.model, ',');
        getline(ss, token, ','); a.fuelLevel = stoi(token);
        getline(ss, a.origin, ',');
        getline(ss, a.destination, ',');
        getline(ss, token, ','); a.priority = stoi(token);
        getline(ss, a.status, ',');
        getline(ss, token, ','); a.x = stoi(token);
        getline(ss, token, ','); a.y = stoi(token);
        getline(ss, a.currentNode, ',');
        getline(ss, token); a.timestamp = stol(token);

        return a;
    }
};

// ===== HASH TABLE =====
class HashNode
{
public:
    string key;
    Aircraft value;
    HashNode* next;

    HashNode(string k, Aircraft v) : key(k), value(v), next(NULL) {}
};

class HashTable
{
private:
    static const int TABLE_SIZE = 100;
    HashNode* table[100];
    int itemCount;

    int hashFunction(string key)
    {
        int sum = 0;
        for (size_t i = 0; i < key.length(); i++)
            sum += (int)key[i];
        return sum % TABLE_SIZE;
    }

    void clearChain(HashNode* node)
    {
        while (node != NULL) {
            HashNode* temp = node;
            node = node->next;
            delete temp;
        }
    }

public:
    HashTable() : itemCount(0)
    {
        for (int i = 0; i < TABLE_SIZE; i++) table[i] = NULL;
    }

    void insert(string key, Aircraft value)
    {
        int index = hashFunction(key);
        HashNode* newNode = new HashNode(key, value);
        newNode->next = table[index];
        table[index] = newNode;
        itemCount++;
    }

    Aircraft* search(string key)
    {
        int index = hashFunction(key);
        HashNode* current = table[index];
        while (current != NULL) {
            if (current->key == key) return &(current->value);
            current = current->next;
        }
        return NULL;
    }

    bool update(string key, Aircraft newValue)
    {
        Aircraft* found = search(key);
        if (found != NULL) {
            *found = newValue;
            return true;
        }
        return false;
    }

    bool remove(string key)
    {
        int index = hashFunction(key);
        HashNode* current = table[index];
        HashNode* prev = NULL;

        while (current != NULL) {
            if (current->key == key) {
                if (prev == NULL) table[index] = current->next;
                else prev->next = current->next;
                delete current;
                itemCount--;
                return true;
            }
            prev = current;
            current = current->next;
        }
        return false;
    }

    void saveToFile(string filename)
    {
        ofstream file(filename);
        if (!file.is_open()) throw FileOperationException(filename);

        file << itemCount << "\n";
        for (int i = 0; i < TABLE_SIZE; i++) {
            HashNode* current = table[i];
            while (current != NULL) {
                file << current->value.toString() << "\n";
                current = current->next;
            }
        }
        file.close();
    }

    void loadFromFile(string filename)
    {
        ifstream file(filename);
        if (!file.is_open()) throw FileOperationException(filename);

        for (int i = 0; i < TABLE_SIZE; i++) {
            clearChain(table[i]);
            table[i] = NULL;
        }
        itemCount = 0;

        string line;
        getline(file, line);
        int count = stoi(line);

        for (int i = 0; i < count; i++) {
            getline(file, line);
            if (line.empty()) continue;
            Aircraft aircraft = Aircraft::fromString(line);
            insert(aircraft.flightID, aircraft);
        }
        file.close();
    }

    int getCount() { return itemCount; }

    void getAllAircraft(Aircraft* arr, int& count)
    {
        count = 0;
        for (int i = 0; i < TABLE_SIZE && count < 100; i++) {
            HashNode* current = table[i];
            while (current != NULL && count < 100) {
                arr[count++] = current->value;
                current = current->next;
            }
        }
    }

    ~HashTable()
    {
        for (int i = 0; i < TABLE_SIZE; i++) clearChain(table[i]);
    }
};

// ===== MINHEAP =====
class MinHeap
{
private:
    Aircraft* heap;
    int capacity;
    int size;

    int parent(int i) { return (i - 1) / 2; }
    int leftChild(int i) { return 2 * i + 1; }
    int rightChild(int i) { return 2 * i + 2; }

    void swap(Aircraft& a, Aircraft& b)
    {
        Aircraft temp = a;
        a = b;
        b = temp;
    }

    void heapifyUp(int index)
    {
        while (index > 0 && heap[parent(index)].priority > heap[index].priority) {
            swap(heap[parent(index)], heap[index]);
            index = parent(index);
        }
    }

    void heapifyDown(int index)
    {
        int smallest = index;
        int left = leftChild(index);
        int right = rightChild(index);

        if (left < size && heap[left].priority < heap[smallest].priority) smallest = left;
        if (right < size && heap[right].priority < heap[smallest].priority) smallest = right;

        if (smallest != index) {
            swap(heap[index], heap[smallest]);
            heapifyDown(smallest);
        }
    }

public:
    MinHeap(int cap = 100) : capacity(cap), size(0)
    {
        heap = new Aircraft[capacity];
    }

    ~MinHeap() { delete[] heap; }

    void insert(Aircraft aircraft)
    {
        if (size >= capacity) throw HeapOverflowException();
        heap[size] = aircraft;
        heapifyUp(size);
        size++;
    }

    Aircraft extractMin()
    {
        if (size <= 0) throw EmptyQueueException();
        Aircraft root = heap[0];
        heap[0] = heap[size - 1];
        size--;
        heapifyDown(0);
        return root;
    }

    void decreaseKey(string flightID, int newPriority)
    {
        for (int i = 0; i < size; i++) {
            if (heap[i].flightID == flightID) {
                heap[i].priority = newPriority;
                heapifyUp(i);
                return;
            }
        }
        throw FlightNotFoundException(flightID);
    }

    Aircraft peek()
    {
        if (size > 0) return heap[0];
        throw EmptyQueueException();
    }

    int getSize() { return size; }
};

// ===== AVL TREE =====
class AVLNode
{
public:
    Aircraft data;
    AVLNode* left;
    AVLNode* right;
    int height;

    AVLNode(Aircraft d) : data(d), left(NULL), right(NULL), height(1) {}
};

class AVLTree
{
private:
    AVLNode* root;
    int nodeCount;

    int height(AVLNode* node) { return node ? node->height : 0; }
    int getBalance(AVLNode* node) { return node ? height(node->left) - height(node->right) : 0; }
    int maxVal(int a, int b) { return (a > b) ? a : b; }

    AVLNode* rightRotate(AVLNode* y)
    {
        AVLNode* x = y->left;
        AVLNode* T2 = x->right;
        x->right = y;
        y->left = T2;
        y->height = maxVal(height(y->left), height(y->right)) + 1;
        x->height = maxVal(height(x->left), height(x->right)) + 1;
        return x;
    }

    AVLNode* leftRotate(AVLNode* x)
    {
        AVLNode* y = x->right;
        AVLNode* T2 = y->left;
        y->left = x;
        x->right = T2;
        x->height = maxVal(height(x->left), height(x->right)) + 1;
        y->height = maxVal(height(y->left), height(y->right)) + 1;
        return y;
    }

    AVLNode* insertNode(AVLNode* node, Aircraft data)
    {
        if (node == NULL) {
            nodeCount++;
            return new AVLNode(data);
        }

        if (data.timestamp < node->data.timestamp)
            node->left = insertNode(node->left, data);
        else
            node->right = insertNode(node->right, data);

        node->height = 1 + maxVal(height(node->left), height(node->right));

        int balance = getBalance(node);

        if (balance > 1 && data.timestamp < node->left->data.timestamp)
            return rightRotate(node);

        if (balance < -1 && data.timestamp >= node->right->data.timestamp)
            return leftRotate(node);

        if (balance > 1 && data.timestamp >= node->left->data.timestamp) {
            node->left = leftRotate(node->left);
            return rightRotate(node);
        }

        if (balance < -1 && data.timestamp < node->right->data.timestamp) {
            node->right = rightRotate(node->right);
            return leftRotate(node);
        }

        return node;
    }

    void inOrderTraversal(AVLNode* node)
    {
        if (node != NULL) {
            inOrderTraversal(node->left);
            setColor(COLOR_INFO);
            cout << "  [";

            // Convert timestamp to readable format
            time_t rawtime = (time_t)node->data.timestamp;
            struct tm timeinfo;
            localtime_s(&timeinfo, &rawtime);
            char buffer[80];
            strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeinfo);

            cout << buffer << "] ";
            setColor(MENU);
            cout << node->data.flightID;
            setColor(COLOR_SUBTITLE);
            cout << " -> ";
            if (node->data.status == "Landed") setColor(COLOR_SUCCESS);
            else setColor(COLOR_WARNING);
            cout << node->data.status;
            setColor(COLOR_RESET);
            cout << endl;
            inOrderTraversal(node->right);
        }
    }

    void clearTree(AVLNode* node)
    {
        if (node != NULL) {
            clearTree(node->left);
            clearTree(node->right);
            delete node;
        }
    }

    void storeInOrder(AVLNode* node, Aircraft* flights, int& index)
    {
        if (node != NULL) {
            storeInOrder(node->left, flights, index);
            flights[index++] = node->data;
            storeInOrder(node->right, flights, index);
        }
    }

public:
    AVLTree() : root(NULL), nodeCount(0) {}

    ~AVLTree() { clearTree(root); }

    void insert(Aircraft data)
    {
        root = insertNode(root, data);
    }

    void printLog()
    {
        setColor(COLOR_HEADER);
        cout << "\n|||===========================================================|||\n";
        cout << "|||           FLIGHT LOG (Chronological Order)                |||\n";
        cout << "|||===========================================================|||\n";
        setColor(COLOR_RESET);

        if (root == NULL) {
            setColor(COLOR_WARNING);
            cout << "\n  No flights recorded yet.\n";
            setColor(COLOR_RESET);
        }
        else {
            inOrderTraversal(root);
        }

        setColor(COLOR_SUBTITLE);
        cout << "\n  Total Flights Logged: ";
        setColor(COLOR_INFO);
        cout << nodeCount << endl;
        setColor(COLOR_RESET);
    }

    void saveToFile(string filename)
    {
        ofstream file(filename);
        if (!file.is_open()) throw FileOperationException(filename);

        file << nodeCount << "\n";

        if (nodeCount > 0) {
            Aircraft* flights = new Aircraft[nodeCount];
            int index = 0;
            storeInOrder(root, flights, index);

            for (int i = 0; i < nodeCount; i++)
                file << flights[i].toString() << "\n";

            delete[] flights;
        }
        file.close();
    }

    void loadFromFile(string filename)
    {
        ifstream file(filename);
        if (!file.is_open()) throw FileOperationException(filename);

        clearTree(root);
        root = NULL;
        nodeCount = 0;

        string line;
        getline(file, line);
        int count = stoi(line);

        for (int i = 0; i < count; i++) {
            getline(file, line);
            if (line.empty()) continue;
            Aircraft aircraft = Aircraft::fromString(line);
            insert(aircraft);
        }
        file.close();
    }

    int getCount() { return nodeCount; }
};

// ===== GRAPH =====
class GraphNode
{
public:
    string name;
    int x;
    int y;
    bool isAirport;
    bool occupied;
    string occupiedBy;

    GraphNode(string n, int px, int py, bool airport = false)
        : name(n), x(px), y(py), isAirport(airport), occupied(false) {
    }
};

class Edge
{
public:
    string destination;
    int weight;
    Edge* next;

    Edge(string dest, int w) : destination(dest), weight(w), next(NULL) {}
};

class Graph
{
private:
    static const int MAX_NODES = 50;
    GraphNode* nodes[50];
    Edge* adjList[50];
    int nodeCount;

    int findNodeIndex(string name)
    {
        for (int i = 0; i < nodeCount; i++)
            if (nodes[i]->name == name) return i;
        return -1;
    }

    int minDistance(int dist[], bool visited[])
    {
        int min = INT_MAX;
        int min_index = -1;

        for (int i = 0; i < nodeCount; i++)
            if (!visited[i] && dist[i] <= min) {
                min = dist[i];
                min_index = i;
            }
        return min_index;
    }

    void clearEdges(Edge* edge)
    {
        while (edge != NULL) {
            Edge* temp = edge;
            edge = edge->next;
            delete temp;
        }
    }

public:
    Graph() : nodeCount(0)
    {
        for (int i = 0; i < MAX_NODES; i++) {
            nodes[i] = NULL;
            adjList[i] = NULL;
        }
    }

    ~Graph()
    {
        for (int i = 0; i < nodeCount; i++) {
            delete nodes[i];
            clearEdges(adjList[i]);
        }
    }

    void addNode(string name, int x, int y, bool isAirport = false)
    {
        if (findNodeIndex(name) != -1) return;
        if (nodeCount >= MAX_NODES) return;

        nodes[nodeCount] = new GraphNode(name, x, y, isAirport);
        nodeCount++;
    }

    void addEdge(string src, string dest, int weight)
    {
        int srcIdx = findNodeIndex(src);
        int destIdx = findNodeIndex(dest);

        if (srcIdx == -1 || destIdx == -1) return;

        Edge* newEdge = new Edge(dest, weight);
        newEdge->next = adjList[srcIdx];
        adjList[srcIdx] = newEdge;
    }

    bool isNodeOccupied(string nodeName)
    {
        int idx = findNodeIndex(nodeName);
        return (idx != -1) ? nodes[idx]->occupied : false;
    }

    bool isNodeReachable(string from, string to)
    {
        int fromIdx = findNodeIndex(from);
        if (fromIdx == -1) return false;

        Edge* edge = adjList[fromIdx];
        while (edge != NULL) {
            if (edge->destination == to) return true;
            edge = edge->next;
        }
        return false;
    }

    bool occupyNode(string nodeName, string flightID)
    {
        int idx = findNodeIndex(nodeName);
        if (idx == -1) return false;

        if (nodes[idx]->occupied)
            throw CollisionException("(" + toString(nodes[idx]->x) + ", " + toString(nodes[idx]->y) + ")");

        nodes[idx]->occupied = true;
        nodes[idx]->occupiedBy = flightID;
        return true;
    }

    void freeNode(string nodeName)
    {
        int idx = findNodeIndex(nodeName);
        if (idx != -1) {
            nodes[idx]->occupied = false;
            nodes[idx]->occupiedBy = "";
        }
    }

    // NEW: Get full Dijkstra path as array (for step-by-step movement)
    void getDijkstraPath(string start, string end, string path[], int& pathCount)
    {
        int startIdx = findNodeIndex(start);
        int endIdx = findNodeIndex(end);

        if (startIdx == -1 || endIdx == -1)
            throw InvalidInputException("Node name");

        int dist[50];
        bool visited[50];
        int parent[50];

        for (int i = 0; i < nodeCount; i++) {
            dist[i] = INT_MAX;
            visited[i] = false;
            parent[i] = -1;
        }

        dist[startIdx] = 0;

        for (int count = 0; count < nodeCount - 1; count++) {
            int u = minDistance(dist, visited);
            if (u == -1) break;

            visited[u] = true;

            Edge* edge = adjList[u];
            while (edge != NULL) {
                int v = findNodeIndex(edge->destination);
                if (v != -1 && !visited[v] && dist[u] != INT_MAX &&
                    dist[u] + edge->weight < dist[v]) {
                    dist[v] = dist[u] + edge->weight;
                    parent[v] = u;
                }
                edge = edge->next;
            }
        }

        if (dist[endIdx] == INT_MAX)
            throw NoPathException(start, end);

        // Build path
        pathCount = 0;
        int current = endIdx;
        while (current != -1) {
            path[pathCount++] = nodes[current]->name;
            current = parent[current];
        }

        // Reverse path (currently end->start)
        for (int i = 0; i < pathCount / 2; i++) {
            string temp = path[i];
            path[i] = path[pathCount - 1 - i];
            path[pathCount - 1 - i] = temp;
        }
    }

    void dijkstra(string start, string end)
    {
        int startIdx = findNodeIndex(start);
        int endIdx = findNodeIndex(end);

        if (startIdx == -1 || endIdx == -1)
            throw InvalidInputException("Node name");

        int dist[50];
        bool visited[50];
        int parent[50];
        string pathNodes[50];
        int pathCount = 0;

        for (int i = 0; i < nodeCount; i++) {
            dist[i] = INT_MAX;
            visited[i] = false;
            parent[i] = -1;
        }

        dist[startIdx] = 0;

        for (int count = 0; count < nodeCount - 1; count++) {
            int u = minDistance(dist, visited);
            if (u == -1) break;

            visited[u] = true;

            Edge* edge = adjList[u];
            while (edge != NULL) {
                int v = findNodeIndex(edge->destination);
                if (v != -1 && !visited[v] && dist[u] != INT_MAX &&
                    dist[u] + edge->weight < dist[v]) {
                    dist[v] = dist[u] + edge->weight;
                    parent[v] = u;
                }
                edge = edge->next;
            }
        }

        if (dist[endIdx] == INT_MAX)
            throw NoPathException(start, end);

        setColor(COLOR_HEADER);
        cout << "\n|||===========================================================|||\n";
        cout << "|||           DIJKSTRA'S SHORTEST PATH ALGORITHM              |||\n";
        cout << "|||===========================================================|||\n";
        setColor(COLOR_RESET);

        setColor(COLOR_INFO);
        cout << "  From: ";
        setColor(MENU);
        cout << start;
        setColor(COLOR_INFO);
        cout << "  ->  To: ";
        setColor(MENU);
        cout << end << endl;

        setColor(COLOR_SUCCESS);
        cout << "  Total Distance: ";
        setColor(MENU);
        cout << dist[endIdx] << " km\n";

        setColor(COLOR_INFO);
        cout << "  Route: ";
        setColor(COLOR_RESET);

        int current = endIdx;
        while (current != -1) {
            pathNodes[pathCount++] = nodes[current]->name;
            current = parent[current];
        }

        for (int i = pathCount - 1; i >= 0; i--) {
            setColor(MENU);
            cout << pathNodes[i];
            if (i > 0) {
                setColor(COLOR_SUBTITLE);
                cout << " -> ";
            }
        }
        cout << endl;
        setColor(COLOR_RESET);
    }

    string findNearestAirport(string startNode)
    {
        int startIdx = findNodeIndex(startNode);
        if (startIdx == -1) return "";

        int dist[50];
        bool visited[50];

        for (int i = 0; i < nodeCount; i++) {
            dist[i] = INT_MAX;
            visited[i] = false;
        }

        dist[startIdx] = 0;

        for (int count = 0; count < nodeCount - 1; count++) {
            int u = minDistance(dist, visited);
            if (u == -1) break;

            visited[u] = true;

            Edge* edge = adjList[u];
            while (edge != NULL) {
                int v = findNodeIndex(edge->destination);
                if (v != -1 && !visited[v] && dist[u] != INT_MAX &&
                    dist[u] + edge->weight < dist[v]) {
                    dist[v] = dist[u] + edge->weight;
                }
                edge = edge->next;
            }
        }

        int nearestIdx = -1;
        int minDist = INT_MAX;

        for (int i = 0; i < nodeCount; i++) {
            if (nodes[i]->isAirport && dist[i] < minDist && dist[i] != INT_MAX) {
                minDist = dist[i];
                nearestIdx = i;
            }
        }

        return (nearestIdx != -1) ? nodes[nearestIdx]->name : "";
    }

    GraphNode* getNodeByName(string name)
    {
        int idx = findNodeIndex(name);
        return (idx != -1) ? nodes[idx] : NULL;
    }

    void getAirports(string* airportList, int& count)
    {
        count = 0;
        for (int i = 0; i < nodeCount; i++) {
            if (nodes[i]->isAirport)
                airportList[count++] = nodes[i]->name;
        }
    }

    void checkAllCollisions()
    {
        bool collision = false;
        for (int i = 0; i < nodeCount; i++) {
            if (nodes[i]->occupied) {
                for (int j = i + 1; j < nodeCount; j++) {
                    if (nodes[j]->occupied &&
                        abs(nodes[i]->x - nodes[j]->x) <= 1 &&
                        abs(nodes[i]->y - nodes[j]->y) <= 1) {
                        setColor(COLOR_DANGER);
                        cout << "  WARNING: Collision between " << nodes[i]->occupiedBy
                            << " and " << nodes[j]->occupiedBy << "!\n";
                        setColor(COLOR_RESET);
                        collision = true;
                    }
                }
            }
        }
        if (!collision) {
            setColor(COLOR_SUCCESS);
            cout << "  No collisions detected.\n";
            setColor(COLOR_RESET);
        }
    }
};

#endif