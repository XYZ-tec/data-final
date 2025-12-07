# SkyNet Air Traffic Control (ATC) Simulator

## 📋 Project Overview
**SkyNet** is a console-based Air Traffic Control (ATC) simulation system developed as a Data Structures semester project. The system simulates a busy airspace where aircraft must be managed efficiently to prevent collisions, handle emergencies, and ensure timely landings using **manually implemented data structures**.

##  Features

###  **Module A: Airspace Graph & Radar**
- **Directed Weighted Graph** representation of flight paths
- **20x20 Radar Grid** visualization
- Nodes: Airports (A) and Waypoints
- Real-time position updates on console
- **Dijkstra's Algorithm** for shortest path calculation

###  **Module B: Landing Priority System (Min-Heap)**
- Priority-based landing queue (not FIFO)
- **4 Priority Levels**:
  - **CRITICAL**: Medical Emergency, Engine Failure, Hijack
  - **HIGH**: Low Fuel (< 10%)
  - **MEDIUM**: Commercial Passenger Flight
  - **LOW**: Cargo / Private Jets
- **Emergency Override**: Change priority mid-flight
- Manual Min-Heap implementation (no STL)

###  **Module C: Aircraft Registry (Hash Table)**
- **O(1) lookup** using Flight ID
- Hash Table with chaining collision resolution
- Store aircraft details: Model, Fuel, Origin, Destination, Status
- Instant search functionality

###  **Module D: Flight Log History (AVL Tree)**
- Self-balancing AVL Tree for flight records
- Chronological sorting by timestamp
- **All 4 rotations implemented**: LL, RR, LR, RL
- In-order traversal for chronological reports

###  **Module E: Collision Avoidance**
- Real-time collision detection
- Alert system for occupied nodes
- Prevents two aircraft at same coordinate
- Automatic emergency routing

##  **Data Structures Implemented**

| Data Structure | Purpose | Implementation |
|----------------|---------|----------------|
| **Graph** | Airspace representation | Adjacency List (Directed Weighted) |
| **Min-Heap** | Landing priority queue | Array-based (Manual) |
| **Hash Table** | Aircraft registry | Chaining collision resolution |
| **AVL Tree** | Flight log history | Self-balancing with rotations |
| **2D Array** | Radar visualization | Character grid (20x20) |

##  **Compilation & Execution**

### **Prerequisites**
- C++11 or higher compiler
- Windows OS (for color support) or compatible terminal

### **Compilation**
```bash
g++ -std=c++11 main.cpp -o SkyNet.exe
```

### **Running the Program**
```bash
./SkyNet.exe
```

## 📂 **File Structure**
```
SkyNet_ATC/
│
├── main.cpp              # Main application code
├── SkyNet.h             # Header file with all data structures
├── aircraft_registry.txt # Active flights data file
├── flight_log.txt       # Landed flights history file
└── README.md           # This file
```

##  **User Interface**

### **Main Menu Options**
1. **Display Airspace Visualizer (Radar)** - Shows real-time radar grid
2. **Add New Flight** - Create new aircraft with random starting airport
3. **Declare Emergency** - Update flight priority (Medical, Engine, Hijack, Low Fuel)
4. **Process Landing** - Land highest priority aircraft
5. **Search Flight by ID** - O(1) lookup from Hash Table
6. **Print Flight Log (AVL Tree)** - Chronological flight history
7. **Find Safe Route (Dijkstra)** - Calculate shortest path between nodes
8. **Emergency Route to Nearest Airport** - Automatic emergency routing
9. **Move Flight to New Node** - Manual aircraft movement
10. **Check for Collisions** - Detect potential collisions
11. **Save System Data** - Persist data to files
12. **Load System Data** - Restore from saved files
0. **Exit System** - Graceful shutdown

### **Visual Elements**
- **Color-coded console output** for better readability
- **Radar legend**:
  -  `[A]` - Airport
  -  `[P]` - Plane
  -  `[.]` - Empty Sky
- **Priority color coding**:
  - 🔴 RED: Critical (Priority 1)
  - 🟡 YELLOW: High (Priority 2)
  - 🔵 BLUE: Medium/Low (Priority 3-4)

##  **Sample Data Files**

### **aircraft_registry.txt** (Active Flights)
```
2
PK-701,Boeing737,85,Karachi,Lahore,3,In Flight,2,2,PSW,1
AA-202,AirbusA320,25,Dubai,London,2,EMERGENCY,17,2,KHI,2
```

### **flight_log.txt** (Landed Flights)
```
3
EK-101,Boeing777,15,Dubai,Karachi,2,Landed,10,10,ISB,100
QR-505,AirbusA380,30,Doha,Islamabad,3,Landed,2,17,FSD,200
BA-111,Boeing787,45,London,Lahore,4,Landed,17,17,LHR,300
```

## 🎯 **Technical Requirements**

### **Constraints**
- ✅ **No STL Containers** used for core data structures
- ✅ **Manual implementations** of all required structures
- ✅ **Console-based** interface only
- ✅ **Real-time updates** on radar display
- ✅ **File I/O** for data persistence

### **Algorithms Implemented**
1. **Dijkstra's Algorithm** - Shortest path finding
2. **Heap Operations** - Insert, ExtractMin, DecreaseKey
3. **AVL Tree Rotations** - LL, RR, LR, RL
4. **Hash Function** - Sum of ASCII modulo table size
5. **Collision Detection** - Coordinate-based checking

##  **Project Specifications**

### **Graph Structure**
- **5 Airports**: PSW, LHR, ISB, FSD, KHI
- **8 Waypoints**: WP1-WP8
- **Bidirectional edges** with distance weights
- Automatic distance calculation based on coordinates

### **Performance Requirements**
- Hash Table search: **O(1)** average
- AVL Tree operations: **O(log n)**
- Heap operations: **O(log n)**
- Dijkstra: **O(V²)** with adjacency list
- Collision detection: **O(n²)** worst-case

## 🔧 **Development Details**

### **Error Handling**
- Custom exception classes for different error types
- Input validation for all user inputs
- File operation error handling
- Graceful degradation on errors

### **Memory Management**
- Proper destructors for all data structures
- Manual memory allocation/deallocation
- No memory leaks

## 🧪 **Testing Scenarios**

### **Test 1: Basic Operations**
1. Add flight with normal priority
2. Display radar to verify position
3. Search flight by ID
4. Land flight and check AVL tree update

### **Test 2: Emergency Handling**
1. Declare emergency (Low Fuel)
2. Verify priority change in landing queue
3. Use emergency route to nearest airport
4. Process landing of emergency flight

### **Test 3: Route Planning**
1. Find safe route between two nodes
2. Verify Dijkstra's algorithm output
3. Check step-by-step distance calculation

### **Test 4: Collision Prevention**
1. Try to move two aircraft to same node
2. Verify collision exception is thrown
3. Use collision detection feature

## 📈 **Learning Outcomes**

This project demonstrates practical implementation of:
1. **Graph Theory** with real-world application (airspace)
2. **Priority Queues** for emergency management
3. **Self-balancing Trees** for efficient data storage
4. **Hash Tables** for rapid data retrieval
5. **Algorithm Design** with Dijkstra's shortest path
6. **System Architecture** of a complete ATC simulation

## 👥 **Team Members**
- Azka Faisal
-Misbah Irfan
-Musferah

## **References**
- **Textbook**: Data Structures and Algorithm Analysis in C++ by Mark Allen Weiss
- **Project Manual**: Provided by course instructor
- **C++ Documentation**: cppreference.com

## **License**
This project is developed for educational purposes as part of a university course. All rights reserved by the developers.

---

## 🎓 **Academic Integrity Statement**
This project is submitted as partial fulfillment of the requirements for the Data Structures course. All code is original work developed by the team members, with guidance from course materials and instructor.
---
**Project Status**: ✅ Complete 
