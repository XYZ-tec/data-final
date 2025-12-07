#include "SkyNet.h"
#include <cmath>
#include <chrono>
int readInt(const string& prompt, int minVal, int maxVal)
{
    int value;
    while (true) {
        cout << prompt;
        if (cin >> value) {
            if (value >= minVal && value <= maxVal) {
                cin.ignore(1000, '\n');
                return value;
            }
            else {
                setColor(COLOR_WARNING);
                cout << "  Please enter a value between " << minVal << " and " << maxVal << ".\n";
                setColor(COLOR_RESET);
                cin.clear();
                cin.ignore(1000, '\n');
            }
        }
        else {
            cin.clear();
            cin.ignore(1000, '\n');
            setColor(COLOR_DANGER);
            cout << "  Invalid input. Please enter a number.\n";
            setColor(COLOR_RESET);
        }
    }
}

string readString(const string& prompt)
{
    string value;
    cout << prompt;
    cin >> value;
    cin.ignore(1000, '\n');
    return value;
}

class SkyNetATC
{
private:
    static const int GRID_SIZE = 20;
    char radar[GRID_SIZE][GRID_SIZE];
    HashTable aircraftRegistry;
    MinHeap landingQueue;
    AVLTree flightLog;
    Graph airspace;

    void initializeRadar()
    {
        for (int i = 0; i < GRID_SIZE; i++)
            for (int j = 0; j < GRID_SIZE; j++)
                radar[i][j] = '.';
    }

    void initializeAirspace()
    {
        // Airports
        airspace.addNode("PSW", 2, 2, true);
        airspace.addNode("LHR", 17, 17, true);
        airspace.addNode("ISB", 10, 10, true);
        airspace.addNode("FSD", 2, 17, true);
        airspace.addNode("KHI", 17, 2, true);

        // Waypoints
        airspace.addNode("WP1", 5, 5);
        airspace.addNode("WP2", 15, 5);
        airspace.addNode("WP3", 5, 15);
        airspace.addNode("WP4", 15, 15);
        airspace.addNode("WP5", 10, 2);
        airspace.addNode("WP6", 10, 18);
        airspace.addNode("WP7", 2, 10);
        airspace.addNode("WP8", 18, 10);

        // Connect airports to waypoints
        airspace.addEdge("PSW", "WP1", 30);
        airspace.addEdge("WP1", "PSW", 30);
        airspace.addEdge("PSW", "WP7", 25);
        airspace.addEdge("WP7", "PSW", 25);
        airspace.addEdge("LHR", "WP4", 35);
        airspace.addEdge("WP4", "LHR", 35);
        airspace.addEdge("LHR", "WP6", 30);
        airspace.addEdge("WP6", "LHR", 30);
        airspace.addEdge("ISB", "WP1", 40);
        airspace.addEdge("WP1", "ISB", 40);
        airspace.addEdge("ISB", "WP2", 45);
        airspace.addEdge("WP2", "ISB", 45);
        airspace.addEdge("ISB", "WP3", 40);
        airspace.addEdge("WP3", "ISB", 40);
        airspace.addEdge("ISB", "WP4", 35);
        airspace.addEdge("WP4", "ISB", 35);
        airspace.addEdge("FSD", "WP3", 30);
        airspace.addEdge("WP3", "FSD", 30);
        airspace.addEdge("FSD", "WP6", 35);
        airspace.addEdge("WP6", "FSD", 35);
        airspace.addEdge("KHI", "WP2", 30);
        airspace.addEdge("WP2", "KHI", 30);
        airspace.addEdge("KHI", "WP5", 25);
        airspace.addEdge("WP5", "KHI", 25);

        // Connect waypoints
        airspace.addEdge("WP1", "WP2", 50);
        airspace.addEdge("WP2", "WP1", 50);
        airspace.addEdge("WP1", "WP3", 45);
        airspace.addEdge("WP3", "WP1", 45);
        airspace.addEdge("WP2", "WP4", 55);
        airspace.addEdge("WP4", "WP2", 55);
        airspace.addEdge("WP3", "WP4", 50);
        airspace.addEdge("WP4", "WP3", 50);
        airspace.addEdge("WP5", "WP1", 35);
        airspace.addEdge("WP1", "WP5", 35);
        airspace.addEdge("WP5", "WP2", 40);
        airspace.addEdge("WP2", "WP5", 40);
        airspace.addEdge("WP6", "WP3", 40);
        airspace.addEdge("WP3", "WP6", 40);
        airspace.addEdge("WP6", "WP4", 45);
        airspace.addEdge("WP4", "WP6", 45);
        airspace.addEdge("WP7", "WP1", 30);
        airspace.addEdge("WP1", "WP7", 30);
        airspace.addEdge("WP7", "WP3", 35);
        airspace.addEdge("WP3", "WP7", 35);
        airspace.addEdge("WP8", "WP2", 35);
        airspace.addEdge("WP2", "WP8", 35);
        airspace.addEdge("WP8", "WP4", 40);
        airspace.addEdge("WP4", "WP8", 40);

        radar[2][2] = 'A';
        radar[17][17] = 'A';
        radar[10][10] = 'A';
        radar[2][17] = 'A';
        radar[17][2] = 'A';
    }

    void clearScreen() { system("cls"); }

    void displayRadar()
    {
        setColor(COLOR_HEADER);
        cout << "\n|||===========================================================|||\n";
        cout << "|||              SKYNET RADAR - AIRSPACE VIEW                 |||\n";
        cout << "|||===========================================================|||\n";
        setColor(COLOR_RESET);

        setColor(COLOR_SUBTITLE);
        cout << "     ";
        for (int i = 0; i < GRID_SIZE; i++) cout << i % 10 << " ";
        cout << "\n";
        setColor(COLOR_RESET);

        for (int i = 0; i < GRID_SIZE; i++) {
            setColor(COLOR_SUBTITLE);
            cout << "  " << i % 10 << "  ";
            setColor(COLOR_RESET);

            for (int j = 0; j < GRID_SIZE; j++) {
                if (radar[i][j] == 'A') setColor(COLOR_SUCCESS);
                else if (radar[i][j] == 'P') setColor(COLOR_CRITICAL);
                else setColor(COLOR_SUBTITLE);

                cout << radar[i][j];
                setColor(COLOR_RESET);
                cout << " ";
            }
            cout << "\n";
        }

        setColor(COLOR_INFO);
        cout << "\n  Legend: ";
        setColor(COLOR_SUCCESS);
        cout << "[A] Airport  ";
        setColor(COLOR_CRITICAL);
        cout << "[P] Plane  ";
        setColor(COLOR_SUBTITLE);
        cout << "[.] Empty Sky\n";
        setColor(COLOR_RESET);
    }

    void displayLandingQueue()
    {
        setColor(COLOR_HEADER);
        cout << "\n|||===========================================================|||\n";
        cout << "|||              LANDING QUEUE (Priority System)              |||\n";
        cout << "|||===========================================================|||\n";
        setColor(COLOR_RESET);

        try {
            Aircraft next = landingQueue.peek();
            setColor(COLOR_SUCCESS);
            cout << "  NEXT TO LAND: ";
            setColor(MENU);
            cout << next.flightID;
            setColor(COLOR_INFO);
            cout << " [Priority: ";

            if (next.priority == 1) setColor(COLOR_DANGER);
            else if (next.priority == 2) setColor(COLOR_WARNING);
            else setColor(COLOR_INFO);

            cout << next.priority << " - " << getPriorityString(next.priority);
            setColor(COLOR_INFO);
            cout << "]\n";

            setColor(COLOR_SUBTITLE);
            cout << "  Aircraft in queue: ";
            setColor(MENU);
            cout << landingQueue.getSize() << endl;
            setColor(COLOR_RESET);
        }
        catch (EmptyQueueException& e) {
            setColor(COLOR_WARNING);
            cout << "  No aircraft in landing queue.\n";
            setColor(COLOR_RESET);
        }
    }

    string getPriorityString(int priority)
    {
        switch (priority) {
        case 1: return "CRITICAL (Emergency)";
        case 2: return "HIGH (Low Fuel)";
        case 3: return "MEDIUM (Commercial)";
        case 4: return "LOW (Cargo/Private)";
        default: return "UNKNOWN";
        }
    }

    void displayStatistics()
    {
        setColor(COLOR_HEADER);
        cout << "\n|||===========================================================|||\n";
        cout << "|||                    SYSTEM STATISTICS                      |||\n";
        cout << "|||===========================================================|||\n";
        setColor(COLOR_RESET);

        setColor(COLOR_INFO);
        cout << "  Active Flights: ";
        setColor(MENU);
        cout << aircraftRegistry.getCount() << endl;

        setColor(COLOR_INFO);
        cout << "  Landing Queue: ";
        setColor(MENU);
        cout << landingQueue.getSize() << endl;

        setColor(COLOR_INFO);
        cout << "  Completed Flights: ";
        setColor(MENU);
        cout << flightLog.getCount() << endl;

        setColor(COLOR_RESET);
    }

public:
    SkyNetATC()
    {
        srand((unsigned)time(0));
        initializeRadar();
        initializeAirspace();
    }

    void addFlight()
    {
        try {
            setColor(COLOR_HEADER);
            cout << "\n|||===========================================================|||\n";
            cout << "|||                     ADD NEW FLIGHT                        |||\n";
            cout << "|||===========================================================|||\n";
            setColor(COLOR_RESET);

            string flightID = readString("  Flight ID (e.g., PK-786): ");
            string model = readString("  Aircraft Model (e.g., Boeing737): ");
            int fuelLevel = readInt("  Fuel Level (0-100%): ", 0, 100);
            string origin = readString("  Origin Airport: ");
            string destination = readString("  Destination Airport: ");
            int priority = readInt("  Priority (1=CRITICAL, 2=HIGH, 3=MEDIUM, 4=LOW): ", 1, 4);

            Aircraft newAircraft(flightID, model, fuelLevel, origin, destination, priority);
            newAircraft.validate();

            string airportList[10];
            int airportCount = 0;
            airspace.getAirports(airportList, airportCount);

            if (airportCount == 0)
                throw InvalidInputException("No airports available");

            int randomAirport = rand() % airportCount;
            string startNode = airportList[randomAirport];

            GraphNode* startGraphNode = airspace.getNodeByName(startNode);
            if (startGraphNode == NULL)
                throw InvalidInputException("Selected airport not found");

            if (airspace.isNodeOccupied(startNode))
                throw CollisionException("(" + toString(startGraphNode->x) + ", " + toString(startGraphNode->y) + ")");

            newAircraft.x = startGraphNode->x;
            newAircraft.y = startGraphNode->y;
            newAircraft.currentNode = startNode;

            aircraftRegistry.insert(flightID, newAircraft);
            landingQueue.insert(newAircraft);
            airspace.occupyNode(startNode, flightID);
            radar[newAircraft.x][newAircraft.y] = 'P';

            setColor(COLOR_SUCCESS);
            cout << "\n  Flight " << flightID << " added successfully!\n";
            cout << "  Starting at: " << startNode << " (" << newAircraft.x << ", " << newAircraft.y << ")\n";
            setColor(COLOR_RESET);
        }
        catch (AircraftException& e) {
            setColor(COLOR_DANGER);
            cout << "\n  Issue: " << e.what() << "\n";
            setColor(COLOR_RESET);
        }
    }

    void declareEmergency()
    {
        try {
            setColor(COLOR_HEADER);
            cout << "\n|||===========================================================|||\n";
            cout << "|||                   DECLARE EMERGENCY                       |||\n";
            cout << "|||===========================================================|||\n";
            setColor(COLOR_RESET);

            string flightID = readString("  Enter Flight ID: ");
            Aircraft* aircraft = aircraftRegistry.search(flightID);

            if (aircraft == NULL)
                throw FlightNotFoundException(flightID);

            setColor(COLOR_WARNING);
            cout << "\n  Emergency Types:\n";
            setColor(COLOR_RESET);
            cout << "    1. Medical Emergency\n";
            cout << "    2. Engine Failure\n";
            cout << "    3. Hijack\n";
            cout << "    4. Low Fuel\n";

            int choice = readInt("  Choice: ", 1, 4);
            int newPriority = 1;

            if (choice == 4) {
                int fuelLevel = readInt("  Enter current fuel level (0-100%): ", 0, 100);
                aircraft->fuelLevel = fuelLevel;
                newPriority = (fuelLevel < 10) ? 2 : 3;
            }

            aircraft->priority = newPriority;
            aircraft->status = "EMERGENCY";
            aircraftRegistry.update(flightID, *aircraft);
            landingQueue.decreaseKey(flightID, newPriority);

            setColor(COLOR_SUCCESS);
            cout << "\n  Emergency declared! " << flightID << " moved to priority " << newPriority << "\n";
            if (choice == 4 && aircraft->fuelLevel < 10)
                cout << "  Fuel level " << aircraft->fuelLevel << "% (LOW FUEL - HIGH PRIORITY)\n";
            setColor(COLOR_RESET);
        }
        catch (AircraftException& e) {
            setColor(COLOR_DANGER);
            cout << "\n  Issue: " << e.what() << "\n";
            setColor(COLOR_RESET);
        }
    }

    void landFlight()
    {
        try {
            Aircraft landed = landingQueue.extractMin();
            radar[landed.x][landed.y] = '.';
            airspace.freeNode(landed.currentNode);
            landed.status = "Landed";

            // Get real timestamp (seconds since epoch)
            auto now = chrono::system_clock::now();
            landed.timestamp = chrono::duration_cast<chrono::seconds>(
                now.time_since_epoch()).count();

            aircraftRegistry.remove(landed.flightID);
            flightLog.insert(landed);

            setColor(COLOR_SUCCESS);
            cout << "\n  Flight " << landed.flightID << " has landed safely!\n";
            setColor(COLOR_INFO);
            cout << "    Priority was: " << getPriorityString(landed.priority) << "\n";
            cout << "    Landed at: " << landed.currentNode << "\n";

            // Display timestamp
            time_t rawtime = (time_t)landed.timestamp;
            struct tm timeinfo;
            localtime_s(&timeinfo, &rawtime);
            char buffer[80];
            strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeinfo);
            cout << "    Time: " << buffer << "\n";

            setColor(COLOR_RESET);
        }
        catch (EmptyQueueException& e) {
            setColor(COLOR_WARNING);
            cout << "\n  " << e.what() << "\n";
            setColor(COLOR_RESET);
        }
    }

    void searchFlight()
    {
        try {
            setColor(COLOR_HEADER);
            cout << "\n|||===========================================================|||\n";
            cout << "|||                    SEARCH FLIGHT                          |||\n";
            cout << "|||===========================================================|||\n";
            setColor(COLOR_RESET);

            string flightID = readString("  Enter Flight ID: ");
            Aircraft* aircraft = aircraftRegistry.search(flightID);

            if (aircraft == NULL)
                throw FlightNotFoundException(flightID);

            setColor(COLOR_HEADER);
            cout << "\n|||===========================================================|||\n";
            cout << "|||                 FLIGHT INFORMATION                        |||\n";
            cout << "|||===========================================================|||\n";
            setColor(COLOR_RESET);

            setColor(COLOR_INFO);
            cout << "  Flight ID:    " << aircraft->flightID << endl;
            cout << "  Model:        " << aircraft->model << endl;
            cout << "  Fuel Level:   ";
            if (aircraft->fuelLevel < 10) setColor(COLOR_DANGER);
            else if (aircraft->fuelLevel < 30) setColor(COLOR_WARNING);
            else setColor(COLOR_SUCCESS);
            cout << aircraft->fuelLevel << "%\n";
            setColor(COLOR_INFO);
            cout << "  Origin:       " << aircraft->origin << endl;
            cout << "  Destination:  " << aircraft->destination << endl;
            cout << "  Priority:     ";
            if (aircraft->priority == 1) setColor(COLOR_DANGER);
            else if (aircraft->priority == 2) setColor(COLOR_WARNING);
            else setColor(COLOR_INFO);
            cout << getPriorityString(aircraft->priority) << endl;
            setColor(COLOR_INFO);
            cout << "  Status:       ";
            if (aircraft->status == "EMERGENCY") setColor(COLOR_DANGER);
            else if (aircraft->status == "Landed") setColor(COLOR_SUCCESS);
            else setColor(MENU);
            cout << aircraft->status << endl;
            setColor(COLOR_INFO);
            cout << "  Position:     (" << aircraft->x << ", " << aircraft->y << ")\n";
            cout << "  Current Node: " << aircraft->currentNode << endl;
            setColor(COLOR_RESET);
        }
        catch (FlightNotFoundException& e) {
            setColor(COLOR_DANGER);
            cout << "\n  " << e.what() << "\n";
            setColor(COLOR_RESET);
        }
    }

    void printFlightLog()
    {
        flightLog.printLog();
    }

    void findSafeRoute()
    {
        try {
            setColor(COLOR_HEADER);
            cout << "\n|||===========================================================|||\n";
            cout << "|||              FIND SAFE ROUTE (Dijkstra)                   |||\n";
            cout << "|||===========================================================|||\n";
            setColor(COLOR_RESET);

            setColor(COLOR_INFO);
            cout << "  Available Nodes:\n";
            setColor(COLOR_SUCCESS);
            cout << "    * PSW, LHR, ISB, FSD, KHI (Airports)\n";
            setColor(COLOR_SUBTITLE);
            cout << "    * WP1, WP2, WP3, WP4, WP5, WP6, WP7, WP8 (Waypoints)\n";
            setColor(COLOR_RESET);

            string currentPos = readString("\n  Current Position: ");
            string destination = readString("  Destination: ");

            airspace.dijkstra(currentPos, destination);
        }
        catch (AircraftException& e) {
            setColor(COLOR_DANGER);
            cout << "\n  Issue: " << e.what() << "\n";
            setColor(COLOR_RESET);
        }
    }

    /*
    DESIGN NOTE: Heap/Hash Synchronization (Issue 3)
    - Hash table stores master aircraft state
    - Heap stores priority queue for landing
    - We keep priority field synchronized manually
    - This design is acceptable for semester scope
    */
    void emergencyRoute()
    {
        try {
            setColor(COLOR_HEADER);
            cout << "\n|||===========================================================|||\n";
            cout << "|||            EMERGENCY ROUTE TO NEAREST AIRPORT             |||\n";
            cout << "|||===========================================================|||\n";
            setColor(COLOR_RESET);

            string flightID = readString("  Enter Flight ID: ");
            Aircraft* aircraft = aircraftRegistry.search(flightID);

            if (aircraft == NULL)
                throw FlightNotFoundException(flightID);

            string nearestAirport = airspace.findNearestAirport(aircraft->currentNode);

            if (nearestAirport.empty()) {
                setColor(COLOR_DANGER);
                cout << "\n  No reachable airport found!\n";
                setColor(COLOR_RESET);
                return;
            }

            setColor(COLOR_CRITICAL);
            cout << "\n  EMERGENCY ROUTE FOR: " << flightID << "\n";
            setColor(COLOR_INFO);
            cout << "  Current Position: " << aircraft->currentNode << "\n";
            cout << "  Nearest Airport: " << nearestAirport << "\n\n";
            setColor(COLOR_RESET);

            // Get Dijkstra path
            string pathArray[50];
            int pathCount = 0;
            airspace.getDijkstraPath(aircraft->currentNode, nearestAirport, pathArray, pathCount);

            // Display path
            setColor(COLOR_INFO);
            cout << "  Calculated Path: ";
            setColor(COLOR_RESET);
            for (int i = 0; i < pathCount; i++) {
                setColor(MENU);
                cout << pathArray[i];
                if (i < pathCount - 1) {
                    setColor(COLOR_SUBTITLE);
                    cout << " -> ";
                }
            }
            cout << endl;

            // Ask if pilot wants to follow emergency route
            setColor(COLOR_WARNING);
            cout << "\n  Execute emergency landing procedure? (1=Yes, 0=No): ";
            setColor(COLOR_RESET);
            int executeEmergency = readInt("", 0, 1);

            if (executeEmergency == 1) {
                // Move step-by-step to airport
                setColor(COLOR_CRITICAL);
                cout << "\n  Emergency Descent:\n";
                setColor(COLOR_RESET);

                for (int step = 0; step < pathCount - 1; step++) {
                    string currentNodeName = pathArray[step];
                    string nextNodeName = pathArray[step + 1];

                    GraphNode* nextNode = airspace.getNodeByName(nextNodeName);

                    // Check collision
                    if (airspace.isNodeOccupied(nextNodeName)) {
                        setColor(COLOR_DANGER);
                        cout << "    Collision! Node occupied.\n";
                        setColor(COLOR_RESET);
                        throw CollisionException("(" + toString(nextNode->x) + ", " + toString(nextNode->y) + ")");
                    }

                    // Move
                    radar[aircraft->x][aircraft->y] = '.';
                    airspace.freeNode(currentNodeName);

                    aircraft->x = nextNode->x;
                    aircraft->y = nextNode->y;
                    aircraft->currentNode = nextNodeName;

                    airspace.occupyNode(nextNodeName, flightID);
                    radar[aircraft->x][aircraft->y] = 'P';

                    setColor(COLOR_CRITICAL);
                    cout << "    Down: " << currentNodeName << " -> " << nextNodeName << "\n";
                    setColor(COLOR_RESET);
                }

                aircraftRegistry.update(flightID, *aircraft);

                setColor(COLOR_SUCCESS);
                cout << "\n  Emergency landing complete at " << nearestAirport << "!\n";
                setColor(COLOR_RESET);
            }
            else {
                setColor(COLOR_INFO);
                cout << "\n  Emergency route calculated but not executed.\n";
                cout << "  Pilot discretion: follow path manually or use Move Flight option.\n";
                setColor(COLOR_RESET);
            }
        }
        catch (AircraftException& e) {
            setColor(COLOR_DANGER);
            cout << "\n  Issue: " << e.what() << "\n";
            setColor(COLOR_RESET);
        }
    }

    void moveFlight()
    {
        try {
            setColor(COLOR_HEADER);
            cout << "\n|||===========================================================|||\n";
            cout << "|||                    MOVE FLIGHT (DIJKSTRA PATH)            |||\n";
            cout << "|||===========================================================|||\n";
            setColor(COLOR_RESET);

            string flightID = readString("  Enter Flight ID: ");
            Aircraft* aircraft = aircraftRegistry.search(flightID);

            if (aircraft == NULL)
                throw FlightNotFoundException(flightID);

            setColor(COLOR_INFO);
            cout << "  Current Node: " << aircraft->currentNode << "\n";

            string destination = readString("  Enter Destination Node: ");
            GraphNode* destNode = airspace.getNodeByName(destination);

            if (destNode == NULL)
                throw InvalidInputException("Destination node not found");

            // Get full Dijkstra path
            string pathArray[50];
            int pathCount = 0;
            airspace.getDijkstraPath(aircraft->currentNode, destination, pathArray, pathCount);

            // Display path
            setColor(COLOR_INFO);
            cout << "\n  Calculated Path: ";
            setColor(COLOR_RESET);
            for (int i = 0; i < pathCount; i++) {
                setColor(MENU);
                cout << pathArray[i];
                if (i < pathCount - 1) {
                    setColor(COLOR_SUBTITLE);
                    cout << " -> ";
                }
            }
            cout << endl;

            // Move step-by-step through path
            setColor(COLOR_INFO);
            cout << "\n  Moving step-by-step:\n";
            setColor(COLOR_RESET);

            for (int step = 0; step < pathCount - 1; step++) {
                string currentNodeName = pathArray[step];
                string nextNodeName = pathArray[step + 1];

                GraphNode* nextNode = airspace.getNodeByName(nextNodeName);

                // Check collision at next node
                if (airspace.isNodeOccupied(nextNodeName)) {
                    setColor(COLOR_DANGER);
                    cout << "    Node " << nextNodeName << " occupied! Path blocked.\n";
                    setColor(COLOR_RESET);
                    throw CollisionException("(" + toString(nextNode->x) + ", " + toString(nextNode->y) + ")");
                }

                // Move to next node
                radar[aircraft->x][aircraft->y] = '.';
                airspace.freeNode(currentNodeName);

                aircraft->x = nextNode->x;
                aircraft->y = nextNode->y;
                aircraft->currentNode = nextNodeName;

                airspace.occupyNode(nextNodeName, flightID);
                radar[aircraft->x][aircraft->y] = 'P';

                setColor(COLOR_SUCCESS);
                cout << "    Step " << (step + 1) << ": " << currentNodeName << " -> " << nextNodeName
                    << " (" << aircraft->x << ", " << aircraft->y << ")\n";
                setColor(COLOR_RESET);
            }

            aircraftRegistry.update(flightID, *aircraft);

            setColor(COLOR_SUCCESS);
            cout << "\n  Flight " << flightID << " reached " << destination << " successfully!\n";
            setColor(COLOR_RESET);
        }
        catch (AircraftException& e) {
            setColor(COLOR_DANGER);
            cout << "\n  Issue: " << e.what() << "\n";
            setColor(COLOR_RESET);
        }
    }

    void checkCollisions()
    {
        setColor(COLOR_HEADER);
        cout << "\n|||===========================================================|||\n";
        cout << "|||                COLLISION DETECTION                        |||\n";
        cout << "|||===========================================================|||\n";
        setColor(COLOR_RESET);

        airspace.checkAllCollisions();
    }

    void saveData()
    {
        try {
            aircraftRegistry.saveToFile("aircraft_registry.txt");
            flightLog.saveToFile("flight_log.txt");

            setColor(COLOR_SUCCESS);
            cout << "\n  Data saved successfully!\n";
            setColor(COLOR_INFO);
            cout << "    ---> aircraft_registry.txt\n";
            cout << "    ---> flight_log.txt\n";
            setColor(COLOR_RESET);
        }
        catch (FileOperationException& e) {
            setColor(COLOR_DANGER);
            cout << "\n  Issue: " << e.what() << "\n";
            setColor(COLOR_RESET);
        }
    }

    void loadData()
    {
        try {
            setColor(COLOR_INFO);
            cout << "\n  Loading saved data...\n";

            // IMPORTANT: Clear heap before loading to prevent duplicates
            MinHeap tempHeap;  // Create temporary heap
            landingQueue = tempHeap;  // Replace with empty heap

            aircraftRegistry.loadFromFile("aircraft_registry.txt");

            Aircraft allAircraft[100];
            int count = 0;
            aircraftRegistry.getAllAircraft(allAircraft, count);

            // Rebuild landing queue from loaded aircraft
            for (int i = 0; i < count; i++) {
                if (allAircraft[i].status != "Landed") {
                    landingQueue.insert(allAircraft[i]);
                }
            }

            flightLog.loadFromFile("flight_log.txt");

            // Rebuild radar
            initializeRadar();

            radar[2][2] = 'A';
            radar[17][17] = 'A';
            radar[10][10] = 'A';
            radar[2][17] = 'A';
            radar[17][2] = 'A';

            // Restore planes and re-occupy nodes in graph
            for (int i = 0; i < count; i++) {
                if (allAircraft[i].status != "Landed") {
                    radar[allAircraft[i].x][allAircraft[i].y] = 'P';
                    try {
                        airspace.occupyNode(allAircraft[i].currentNode, allAircraft[i].flightID);
                    }
                    catch (CollisionException& e) {
                        // Log collision but continue loading
                        setColor(COLOR_WARNING);
                        cout << "  Warning: " << e.what() << " (collision during load)\n";
                        setColor(COLOR_RESET);
                        airspace.freeNode(allAircraft[i].currentNode);
                    }
                }
            }

            setColor(COLOR_SUCCESS);
            cout << "  Data loaded successfully!\n";
            cout << "  Loaded " << count << " aircraft records\n";
            cout << "  Landing queue rebuilt\n";
            cout << "  Flight log restored\n";
            cout << "  Radar refreshed\n";
            setColor(COLOR_RESET);
        }
        catch (FileOperationException& e) {
            setColor(COLOR_WARNING);
            cout << "  No saved data found. Starting fresh.\n";
            setColor(COLOR_RESET);
        }
    }

    void displayMenu()
    {
        setColor(COLOR_HEADER);
        cout << "\n|||===========================================================|||\n";
        cout << "|||         SKYNET AIR TRAFFIC CONTROL                        |||\n";
        cout << "|||===========================================================|||\n";
        setColor(COLOR_RESET);

        setColor(MENU);
        cout << "  1. "; setColor(COLOR_RESET);
        cout << "Display Airspace Visualizer (Radar)\n";
        setColor(MENU);
        cout << "  2. "; setColor(COLOR_RESET);
        cout << "Add New Flight\n";
        setColor(MENU);
        cout << "  3. "; setColor(COLOR_RESET);
        cout << "Declare Emergency\n";
        setColor(MENU);
        cout << "  4. "; setColor(COLOR_RESET);
        cout << "Process Landing (Land Next Flight)\n";
        setColor(MENU);
        cout << "  5. "; setColor(COLOR_RESET);
        cout << "Search Flight by ID\n";
        setColor(MENU);
        cout << "  6. "; setColor(COLOR_RESET);
        cout << "Print Flight Log (AVL Tree)\n";
        setColor(MENU);
        cout << "  7. "; setColor(COLOR_RESET);
        cout << "Find Safe Route (Dijkstra)\n";
        setColor(MENU);
        cout << "  8. "; setColor(COLOR_RESET);
        cout << "Emergency Route to Nearest Airport\n";
        setColor(MENU);
        cout << "  9. "; setColor(COLOR_RESET);
        cout << "Move Flight to New Node\n";
        setColor(MENU);
        cout << "  10. "; setColor(COLOR_RESET);
        cout << "Check for Collisions\n";
        setColor(MENU);
        cout << "  11. "; setColor(COLOR_RESET);
        cout << "Save System Data\n";
        setColor(MENU);
        cout << "  12. "; setColor(COLOR_RESET);
        cout << "Load System Data\n";
        setColor(MENU);
        cout << "  0. "; setColor(COLOR_RESET);
        cout << "Exit System\n";

        setColor(COLOR_SUBTITLE);
        cout << "----------------------------------------------------------------\n";
        setColor(COLOR_RESET);
    }

    void run()
    {
        int choice;
        while (true) {
            displayMenu();
            displayLandingQueue();
            displayStatistics();

            choice = readInt("\n  Enter choice: ", 0, 12);

            switch (choice) {
            case 1:
                clearScreen();
                displayRadar();
                break;
            case 2:
                addFlight();
                break;
            case 3:
                declareEmergency();
                break;
            case 4:
                landFlight();
                break;
            case 5:
                searchFlight();
                break;
            case 6:
                printFlightLog();
                break;
            case 7:
                findSafeRoute();
                break;
            case 8:
                emergencyRoute();
                break;
            case 9:
                moveFlight();
                break;
            case 10:
                checkCollisions();
                break;
            case 11:
                saveData();
                break;
            case 12:
                loadData();
                break;
            case 0:
                setColor(COLOR_SUCCESS);
                cout << "\n  Shutting down SkyNet ATC. Goodbye!\n";
                setColor(COLOR_RESET);
                return;
            }

            setColor(COLOR_SUBTITLE);
            cout << "\n  Press Enter to continue...";
            setColor(COLOR_RESET);
            cin.ignore();
            cin.get();
            clearScreen();
        }
    }
};

int main()
{
    srand((unsigned)time(0));

    setColor(COLOR_HEADER);
    cout << "\n|||===========================================================|||\n";
    cout << "|||                                                           |||\n";
    cout << "|||          SKYNET AIR TRAFFIC CONTROL                       |||\n";
    cout << "|||              Simulation System                            |||\n";
    cout << "|||                                                           |||\n";
    cout << "|||          Data Structures End Sem Project                  |||\n";
    cout << "|||                                                           |||\n";
    cout << "|||===========================================================|||\n";
    setColor(COLOR_RESET);

    SkyNetATC system;

    setColor(COLOR_INFO);
    cout << "\n  Initializing system...\n";
    cout << "  Loading airspace graph...\n";
    cout << "  Calibrating radar...\n";
    setColor(COLOR_SUCCESS);
    cout << "\n  System Ready!\n";
    setColor(COLOR_RESET);

    setColor(COLOR_SUBTITLE);
    cout << "\n  Press Enter to start...";
    setColor(COLOR_RESET);
    cin.get();

    system.run();
    return 0;
}