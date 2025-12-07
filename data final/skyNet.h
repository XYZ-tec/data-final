#pragma once
#ifndef SKYNET_H
#define SKYNET_H
#include <iostream>
#include <string>
#include <fstream>
#include <windows.h>
#include <cstdlib>
#include <ctime>
using namespace std;

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

class AircraftException
{
protected:
    string message;
public:
    AircraftException(string msg) : message(msg) {}
    virtual string what() 
    {
        return message;
    }
};

class FlightNotFoundException : public AircraftException
{
public:
    FlightNotFoundException(string id)
        : AircraftException("Flight " + id + " not found in system") {}
};

class InvalidInputException : public AircraftException
{
public:
    InvalidInputException(string field)
        : AircraftException("Invalid input for " + field) {}
};

class HeapOverflowException : public AircraftException {
public:
    HeapOverflowException()
        : AircraftException("Landing queue is full - cannot accept more flights") {}
};

class FileOperationException : public AircraftException {
public:
    FileOperationException(string filename)
        : AircraftException("Failed to open file: " + filename) {}
};

class EmptyQueueException : public AircraftException {
public:
    EmptyQueueException()
        : AircraftException("No aircraft in landing queue") {}
};

class CollisionException : public AircraftException {
public:
    CollisionException(string pos)
        : AircraftException("Collision detected at position " + pos) {}
};

// ==================== AIRCRAFT STRUCTURE ====================
struct Aircraft {
    string flightID;
    string model;
    int fuelLevel;
    string origin;
    string destination;
    int priority;
    string status;
    int x, y;
    long timestamp;
    string currentNode;

    Aircraft() : fuelLevel(0), priority(4), x(0), y(0), timestamp(0), currentNode("") {}

    Aircraft(string id, string m, int fuel, string orig, string dest, int prio)
        : flightID(id), model(m), fuelLevel(fuel), origin(orig),destination(dest), priority(prio), status("In Flight"), x(0), y(0), currentNode(""), timestamp(0) {}

    void validate()
    {
        if (flightID.empty())
            throw InvalidInputException("Flight ID");
        if (model.empty())
            throw InvalidInputException("Aircraft Model");
        if (fuelLevel < 0 || fuelLevel > 100)
            throw InvalidInputException("Fuel Level (must be 0-100)");
        if (priority < 1 || priority > 4)
            throw InvalidInputException("Priority (must be 1-4)");
    }
};

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
    HashNode* table[TABLE_SIZE];
    int itemCount;

    int hashFunction(string key) 
    {
        int sum = 0;
        for (int i = 0; i < key.length(); i++) 
        {
            sum += (int)key[i];
        }
        return sum % TABLE_SIZE;
    }

    void clearChain(HashNode* node) 
    {
        while (node != NULL)
        {
            HashNode* temp = node;
            node = node->next;
            delete temp;
        }
    }

public:
    HashTable() : itemCount(0) 
    {
        for (int i = 0; i < TABLE_SIZE; i++) 
        {
            table[i] = NULL;
        }
    }

    void insert(string key, Aircraft value)
    {
        int index = hashFunction(key);
        HashNode* newNode = new HashNode(key, value);
        if (table[index] == NULL)
        {
            table[index] = newNode;
        }
        else
        {
            newNode->next = table[index];
            table[index] = newNode;
        }
        itemCount++;
    }

    Aircraft* search(string key)
    {
        int index = hashFunction(key);
        HashNode* current = table[index];
        while (current != NULL)
        {
            if (current->key == key)
            {
                return &(current->value);
            }
            current = current->next;
        }
        return NULL;
    }

    bool update(string key, Aircraft newValue)
    {
        Aircraft* found = search(key);
        if (found != NULL)
        {
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

        while (current != NULL) 
        {
            if (current->key == key) 
            {
                if (prev == NULL)
                {
                    table[index] = current->next;
                }
                else 
                {
                    prev->next = current->next;
                }
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
        ofstream file(filename.c_str());
        if (!file.is_open()) 
        {
            throw FileOperationException(filename);
        }
        file << itemCount << "\n";
        for (int i = 0; i < TABLE_SIZE; i++) 
        {
            HashNode* current = table[i];
            while (current != NULL) 
            {
                file << current->value.flightID << "\n";
                file << current->value.model << "\n";
                file << current->value.fuelLevel << "\n";
                file << current->value.origin << "\n";
                file << current->value.destination << "\n";
                file << current->value.priority << "\n";
                file << current->value.status << "\n";
                file << current->value.x << "\n";
                file << current->value.y << "\n";
                file << current->value.currentNode << "\n";
                current = current->next;
            }
        }
        file.close();
    }

    void loadFromFile(string filename)
    {
        ifstream file(filename.c_str());
        if (!file.is_open()) 
        {
            throw FileOperationException(filename);
        }
        int count;
        file >> count;
        file.ignore();
        for (int i = 0; i < count; i++) 
        {
            Aircraft aircraft;
            getline(file, aircraft.flightID);
            getline(file, aircraft.model);
            file >> aircraft.fuelLevel;
            file.ignore();
            getline(file, aircraft.origin);
            getline(file, aircraft.destination);
            file >> aircraft.priority;
            file.ignore();
            getline(file, aircraft.status);
            file >> aircraft.x;
            file >> aircraft.y;
            file.ignore();
            getline(file, aircraft.currentNode);

            insert(aircraft.flightID, aircraft);
        }
        file.close();
    }
    int getCount() 
    {
        return itemCount; 
    }

    void getAllAircraft(Aircraft* arr, int& count) 
    {
        count = 0;
        for (int i = 0; i < TABLE_SIZE; i++) 
        {
            HashNode* current = table[i];
            while (current != NULL && count < TABLE_SIZE) 
            {
                arr[count++] = current->value;
                current = current->next;
            }
        }
    }

    ~HashTable() 
    {
        for (int i = 0; i < TABLE_SIZE; i++) 
        {
            clearChain(table[i]);
        }
    }
};

class MinHeap
{
private:
    Aircraft* heap;
    int capacity;
    int size;
    int parent(int i) 
    {
        return (i - 1) / 2; 
    }
    int leftChild(int i)
    {
        return 2 * i + 1;
    }
    int rightChild(int i)
    {
        return 2 * i + 2;
    }

    void swap(Aircraft& a, Aircraft& b)
    {
        Aircraft temp = a;
        a = b;
        b = temp;
    }

    void heapifyUp(int index) 
    {
        while (index > 0 && heap[parent(index)].priority > heap[index].priority) 
        {
            swap(heap[parent(index)], heap[index]);
            index = parent(index);
        }
    }

    void heapifyDown(int index) 
    {
        int smallest = index;
        int left = leftChild(index);
        int right = rightChild(index);

        if (left < size && heap[left].priority < heap[smallest].priority) 
        {
            smallest = left;
        }
        if (right < size && heap[right].priority < heap[smallest].priority) 
        {
            smallest = right;
        }

        if (smallest != index) 
        {
            swap(heap[index], heap[smallest]);
            heapifyDown(smallest);
        }
    }

public:
    MinHeap(int cap = 100) : capacity(cap), size(0) 
    {
        heap = new Aircraft[capacity];
    }
    void insert(Aircraft aircraft) 
    {
        if (size >= capacity) 
        {
            throw HeapOverflowException();
        }
        heap[size] = aircraft;
        heapifyUp(size);
        size++;
    }

    Aircraft extractMin()
    {
        if (size <= 0) 
        {
            throw EmptyQueueException();
        }
        Aircraft root = heap[0];
        heap[0] = heap[size - 1];
        size--;
        heapifyDown(0);
        return root;
    }

    void decreaseKey(string flightID, int newPriority) 
    {
        for (int i = 0; i < size; i++) 
        {
            if (heap[i].flightID == flightID)
            {
                heap[i].priority = newPriority;
                heapifyUp(i);
                return;
            }
        }
        throw FlightNotFoundException(flightID);
    }

    Aircraft peek()
    {
        if (size > 0) 
            return heap[0];
        throw EmptyQueueException();
    }


    bool contains(string flightID) 
    {
        for (int i = 0; i < size; i++)
        {
            if (heap[i].flightID == flightID)
            {
                return true;
            }
        }
        return false;
    }

    int getSize() 
    {
        return size;
    }
    bool isEmpty() 
    {
        return size == 0; 
    }

    ~MinHeap() 
    {
        delete[] heap;
    }
};

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
    int avlMax(int a, int b)
    {
        return (a > b) ? a : b;
    }

    int height(AVLNode* node)
    {
        return node ? node->height : 0;
    }

    int getBalance(AVLNode* node)
    {
        return node ? height(node->left) - height(node->right) : 0;
    }

    AVLNode* rightRotate(AVLNode* y)
    {
        AVLNode* x = y->left;
        AVLNode* T2 = x->right;
        x->right = y;
        y->left = T2;
        y->height = max(height(y->left), height(y->right)) + 1;
        x->height = max(height(x->left), height(x->right)) + 1;
        return x;
    }

    AVLNode* leftRotate(AVLNode* x)
    {
        AVLNode* y = x->right;
        AVLNode* T2 = y->left;
        y->left = x;
        x->right = T2;
        x->height = max(height(x->left), height(x->right)) + 1;
        y->height = max(height(y->left), height(y->right)) + 1;
        return y;
    }

    AVLNode* insertNode(AVLNode* node, Aircraft data) 
    {
        if (node == NULL)
        {
            nodeCount++;
            return new AVLNode(data);
        }
        if (data.timestamp < node->data.timestamp)
            node->left = insertNode(node->left, data);
        else
            node->right = insertNode(node->right, data);
        node->height = 1 + max(height(node->left), height(node->right));
        int balance = getBalance(node);
        if (balance > 1 && data.timestamp < node->left->data.timestamp)
            return rightRotate(node);

        if (balance < -1 && data.timestamp >= node->right->data.timestamp)
            return leftRotate(node);

        if (balance > 1 && data.timestamp >= node->left->data.timestamp) 
        {
            node->left = leftRotate(node->left);
            return rightRotate(node);
        }

        if (balance < -1 && data.timestamp < node->right->data.timestamp)
        {
            node->right = rightRotate(node->right);
            return leftRotate(node);
        }

        return node;
    }

    void inOrderTraversal(AVLNode* node)
    {
        if (node != NULL)
        {
            inOrderTraversal(node->left);
            setColor(COLOR_INFO);
            cout << "  Flight: ";
            setColor(MENU);
            cout << node->data.flightID;
            setColor(COLOR_SUBTITLE);
            cout << " | Status: ";
            if (node->data.status == "Landed")
            {
                setColor(COLOR_SUCCESS);
            }
            else 
            {
                setColor(COLOR_WARNING);
            }
            cout << node->data.status;
            setColor(COLOR_SUBTITLE);
            cout << " | Timestamp: ";
            setColor(COLOR_RESET);
            cout << node->data.timestamp << endl;
            inOrderTraversal(node->right);
        }
    }

    void saveToFileHelper(AVLNode* node, ofstream& file) 
    {
        if (node != NULL) 
        {
            saveToFileHelper(node->left, file);
            file << node->data.flightID << "\n";
            file << node->data.status << "\n";
            file << node->data.priority << "\n";
            file << node->data.timestamp << "\n";
            saveToFileHelper(node->right, file);
        }
    }

public:
    AVLTree() : root(NULL), nodeCount(0) {}

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

        if (root == NULL)
        {
            setColor(COLOR_WARNING);
            cout << "\n  No flights recorded yet.\n";
            setColor(COLOR_RESET);
        }
        else
        {
            inOrderTraversal(root);
        }
        setColor(COLOR_SUBTITLE);
        cout << "\nTotal Flights Logged: ";
        setColor(COLOR_INFO);
        cout << nodeCount << endl;
        setColor(COLOR_RESET);
    }

    void saveToFile(string filename) 
    {
        ofstream file(filename.c_str());
        if (!file.is_open()) 
        {
            throw FileOperationException(filename);
        }
        file << nodeCount << "\n";
        saveToFileHelper(root, file);
        file.close();
    }

    int getCount()
    {
        return nodeCount;
    }
};
class GraphNode 
{
public:
    string name;
    int x, y;
    bool isAirport;
    bool occupied;
    string occupiedBy;
    GraphNode* next;

    GraphNode(string n, int px, int py, bool airport = false)
        : name(n), x(px), y(py), isAirport(airport), occupied(false), occupiedBy(""), next(NULL) {}
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
    GraphNode* nodes[MAX_NODES];
    Edge* adjList[MAX_NODES];
    int nodeCount;
    int findNodeIndex(string name) 
    {
        for (int i = 0; i < nodeCount; i++) 
        {
            if (nodes[i]->name == name) 
                return i;
        }
        return -1;
    }

    int minDistance(int dist[], bool visited[]) 
    {
        int min = 999999;
        int min_index = -1;

        for (int i = 0; i < nodeCount; i++)
        {
            if (!visited[i] && dist[i] <= min)
            {
                min = dist[i];
                min_index = i;
            }
        }
        return min_index;
    }

public:
    Graph() : nodeCount(0)
    {
        for (int i = 0; i < MAX_NODES; i++) 
        {
            nodes[i] = NULL;
            adjList[i] = NULL;
        }
    }

    void addNode(string name, int x, int y, bool isAirport = false) 
    {
        if (nodeCount >= MAX_NODES)
            return;
        nodes[nodeCount] = new GraphNode(name, x, y, isAirport);
        nodeCount++;
    }

    void addEdge(string src, string dest, int weight)
    {
        int srcIdx = findNodeIndex(src);
        if (srcIdx == -1) 
            return;
        Edge* newEdge = new Edge(dest, weight);
        newEdge->next = adjList[srcIdx];
        adjList[srcIdx] = newEdge;
    }

    bool isNodeOccupied(string nodeName)
    {
        int idx = findNodeIndex(nodeName);
        if (idx == -1) 
            return false;
        return nodes[idx]->occupied;
    }
    bool occupyNode(string nodeName, string flightID)
    {
        int idx = findNodeIndex(nodeName);
        if (idx == -1) 
            return false;
        if (nodes[idx]->occupied) 
        {
            string pos = "";
            // Manual int to string conversion
            int xVal = nodes[idx]->x;
            int yVal = nodes[idx]->y;
            char xStr[10], yStr[10];
            int xi = 0, yi = 0;

            do 
            {
                xStr[xi++] = (xVal % 10) + '0'; 
                xVal /= 10; 
            } while (xVal > 0);
            do
            {
                yStr[yi++] = (yVal % 10) + '0'; 
                yVal /= 10; 
            } while (yVal > 0);

            pos += "(";
            for (int i = xi - 1; i >= 0; i--)
                pos += xStr[i];
            pos += ", ";
            for (int i = yi - 1; i >= 0; i--)
                pos += yStr[i];
            pos += ")";
            throw CollisionException(pos);
        }

        nodes[idx]->occupied = true;
        nodes[idx]->occupiedBy = flightID;
        return true;
    }

    void freeNode(string nodeName)
    {
        int idx = findNodeIndex(nodeName);
        if (idx != -1)
        {
            nodes[idx]->occupied = false;
            nodes[idx]->occupiedBy = "";
        }
    }
    void dijkstra(string start, string end) 
    {
        int startIdx = findNodeIndex(start);
        int endIdx = findNodeIndex(end);
        if (startIdx == -1 || endIdx == -1) 
        {
            throw InvalidInputException("Node name");
        }
        int dist[MAX_NODES];
        bool visited[MAX_NODES];
        int parent[MAX_NODES];
        for (int i = 0; i < nodeCount; i++) 
        {
            dist[i] = 999999;
            visited[i] = false;
            parent[i] = -1;
        }
        dist[startIdx] = 0;
        for (int count = 0; count < nodeCount - 1; count++)
        {
            int u = minDistance(dist, visited);
            if (u == -1) 
                break;
            visited[u] = true;
            Edge* edge = adjList[u];
            while (edge != NULL) 
            {
                int v = findNodeIndex(edge->destination);
                if (v != -1 && !visited[v] && dist[u] + edge->weight < dist[v]) 
                {
                    dist[v] = dist[u] + edge->weight;
                    parent[v] = u;
                }
                edge = edge->next;
            }
        }

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
        int path[MAX_NODES];
        int pathLen = 0;
        int current = endIdx;
        while (current != -1)
        {
            path[pathLen++] = current;
            current = parent[current];
        }

        for (int i = pathLen - 1; i >= 0; i--) 
        {
            setColor(MENU);
            cout << nodes[path[i]]->name;
            if (i > 0) 
            {
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
        if (startIdx == -1) 
            return "";

        int dist[MAX_NODES];
        bool visited[MAX_NODES];
        for (int i = 0; i < nodeCount; i++) 
        {
            dist[i] = 999999;
            visited[i] = false;
        }
        dist[startIdx] = 0;
        for (int count = 0; count < nodeCount - 1; count++)
        {
            int u = minDistance(dist, visited);
            if (u == -1)
                break;
            visited[u] = true;
            Edge* edge = adjList[u];
            while (edge != NULL) 
            {
                int v = findNodeIndex(edge->destination);
                if (v != -1 && !visited[v] && dist[u] + edge->weight < dist[v]) 
                {
                    dist[v] = dist[u] + edge->weight;
                }
                edge = edge->next;
            }
        }
        int nearestIdx = -1;
        int minDist = 999999;
        for (int i = 0; i < nodeCount; i++) 
        {
            if (nodes[i]->isAirport && dist[i] < minDist) 
            {
                minDist = dist[i];
                nearestIdx = i;
            }
        }
        if (nearestIdx != -1) 
        {
            return nodes[nearestIdx]->name;
        }
        return "";
    }

    GraphNode* getNode(int index)
    {
        if (index >= 0 && index < nodeCount)
            return nodes[index];
        return NULL;
    }

    GraphNode* getNodeByName(string name) 
    {
        int idx = findNodeIndex(name);
        if (idx != -1)
            return nodes[idx];
        return NULL;
    }

    int getNodeCount() 
    {
        return nodeCount; 
    }

    void getAirports(string* airportList, int& count)
    {
        count = 0;
        for (int i = 0; i < nodeCount; i++) 
        {
            if (nodes[i]->isAirport) 
            {
                airportList[count++] = nodes[i]->name;
            }
        }
    }
};

#endif 