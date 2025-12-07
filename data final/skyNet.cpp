#include "SkyNet.h"
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
        {
            for (int j = 0; j < GRID_SIZE; j++)
            {
                radar[i][j] = '.';
            }
        }
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

        // Connect airports to waypoints (DIRECTED both directions)
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

        // Connect waypoints (DIRECTED - both directions)
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

        // Update radar with airports
        radar[2][2] = 'A';
        radar[17][17] = 'A';
        radar[10][10] = 'A';
        radar[2][17] = 'A';
        radar[17][2] = 'A';

        // Update radar with waypoints
        radar[5][5] = 'W';
        radar[15][5] = 'W';
        radar[5][15] = 'W';
        radar[15][15] = 'W';
        radar[10][2] = 'W';
        radar[10][18] = 'W';
        radar[2][10] = 'W';
        radar[18][10] = 'W';
    }

    void clearScreen()
    {
        system("cls");
    }

    void displayRadar()
    {
        setColor(COLOR_HEADER);
        cout << "\n|||===========================================================|||\n";
        cout << "|||              SKYNET RADAR - AIRSPACE VIEW                 |||\n";
        cout << "|||===========================================================|||\n";
        setColor(COLOR_RESET);
        setColor(COLOR_SUBTITLE);
        cout << "     ";
        for (int i = 0; i < GRID_SIZE; i++)
        {
            cout << i % 10 << " ";
        }
        cout << "\n";
        setColor(COLOR_RESET);

        for (int i = 0; i < GRID_SIZE; i++) 
        {
            setColor(COLOR_SUBTITLE);
            cout << "  " << i % 10 << "  ";
            setColor(COLOR_RESET);
            for (int j = 0; j < GRID_SIZE; j++)
            {
                if (radar[i][j] == 'A') 
                {
                    setColor(COLOR_SUCCESS);
                    cout << radar[i][j];
                }
                else if (radar[i][j] == 'P') 
                {
                    setColor(COLOR_CRITICAL);
                    cout << radar[i][j];
                }
                else if (radar[i][j] == 'W')
                {
                    setColor(COLOR_INFO);
                    cout << radar[i][j];
                }
                else
                {
                    setColor(COLOR_SUBTITLE);
                    cout << radar[i][j];
                }
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
        setColor(COLOR_INFO);
        cout << "[W] Waypoint  ";
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

        try
        {
            Aircraft next = landingQueue.peek();
            setColor(COLOR_SUCCESS);
            cout << "  NEXT TO LAND: ";
            setColor(MENU);
            cout << next.flightID;
            setColor(COLOR_INFO);
            cout << " [Priority: ";
            if (next.priority == 1)
            {
                setColor(COLOR_DANGER);
            }
            else if (next.priority == 2) 
            {
                setColor(COLOR_WARNING);
            }
            else 
            {
                setColor(COLOR_INFO);
            }
            cout << next.priority << " - " << getPriorityString(next.priority);
            setColor(COLOR_INFO);
            cout << "]\n";
            setColor(COLOR_SUBTITLE);
            cout << "  Aircraft in queue: ";
            setColor(MENU);
            cout << landingQueue.getSize() << endl;
            setColor(COLOR_RESET);
        }
        catch (EmptyQueueException& e)
        {
            setColor(COLOR_WARNING);
            cout << "  No aircraft in landing queue.\n";
            setColor(COLOR_RESET);
        }
    }

    string getPriorityString(int priority) 
    {
        switch (priority)
        {
        case 1: 
            return "CRITICAL (Emergency)";
        case 2: 
            return "HIGH (Low Fuel)";
        case 3:
            return "MEDIUM (Commercial)";
        case 4: 
            return "LOW (Cargo/Private)";
        default:
            return "UNKNOWN";
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

    int stringToInt(string str)
    {
        int result = 0;
        bool negative = false;
        int start = 0;
        if (str[0] == '-')
        {
            negative = true;
            start = 1;
        }
        for (int i = start; i < str.length(); i++)
        {
            if (str[i] >= '0' && str[i] <= '9')
            {
                result = result * 10 + (str[i] - '0');
            }
        }
        return negative ? -result : result;
    }

public:
    SkyNetATC()
    {
        initializeRadar();
        initializeAirspace();
    }
    void addFlight() 
    {
        try 
        {
            string flightID, model, origin, destination;
            int fuelLevel, priority;
            setColor(COLOR_HEADER);
            cout << "\n|||===========================================================|||\n";
            cout << "|||                     ADD NEW FLIGHT                        |||\n";
            cout << "|||===========================================================|||\n";
            setColor(COLOR_RESET);
            setColor(COLOR_INFO);
            cout << "  Flight ID (e.g., PK-786): ";
            setColor(COLOR_RESET);
            cin >> flightID;
            setColor(COLOR_INFO);
            cout << "  Aircraft Model (e.g., Boeing737): ";
            setColor(COLOR_RESET);
            cin >> model;
            setColor(COLOR_INFO);
            cout << "  Fuel Level (0-100%): ";
            setColor(COLOR_RESET);
            string fuelStr;
            cin >> fuelStr;
            fuelLevel = stringToInt(fuelStr);
            if (fuelLevel < 0 || fuelLevel > 100)
            {
                throw InvalidInputException("Fuel Level (must be 0-100)");
            }
            setColor(COLOR_INFO);
            cout << "  Origin Airport: ";
            setColor(COLOR_RESET);
            cin >> origin;
            setColor(COLOR_INFO);
            cout << "  Destination Airport: ";
            setColor(COLOR_RESET);
            cin >> destination;
            setColor(COLOR_INFO);
            cout << "  Priority (1=CRITICAL, 2=HIGH, 3=MEDIUM, 4=LOW): ";
            setColor(COLOR_RESET);
            string prioStr;
            cin >> prioStr;
            priority = stringToInt(prioStr);
            if (priority < 1 || priority > 4)
            {
                throw InvalidInputException("Priority (must be 1-4)");
            }
            Aircraft newAircraft(flightID, model, fuelLevel, origin, destination, priority);
            newAircraft.validate();
            string airportList[10];
            int airportCount = 0;
            airspace.getAirports(airportList, airportCount);
            if (airportCount == 0) 
            {
                throw InvalidInputException("No airports available");
            }
            int randomAirport = (rand() % airportCount);
            if (randomAirport < 0)
                randomAirport = -randomAirport;
            string startNode = airportList[randomAirport];
            GraphNode* startGraphNode = airspace.getNodeByName(startNode);
            if (startGraphNode == NULL)
            {
                throw InvalidInputException("Selected airport not found");
            }
            // Check for collision at start node
            if (airspace.isNodeOccupied(startNode)) 
            {
                string pos = "(";
                char xStr[10], yStr[10];
                int xi = 0, yi = 0;
                int xVal = startGraphNode->x;
                int yVal = startGraphNode->y;
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

                for (int i = xi - 1; i >= 0; i--)
                    pos += xStr[i];
                pos += ", ";
                for (int i = yi - 1; i >= 0; i--) 
                    pos += yStr[i];
                pos += ")";
                throw CollisionException(pos);
            }

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
        catch (InvalidInputException& e) 
        {
            setColor(COLOR_DANGER);
            cout << "\n  Issue: " << e.what() << "\n";
            setColor(COLOR_RESET);
        }
        catch (HeapOverflowException& e) 
        {
            setColor(COLOR_DANGER);
            cout << "\n  Issue: " << e.what() << "\n";
            setColor(COLOR_RESET);
        }
        catch (CollisionException& e) 
        {
            setColor(COLOR_DANGER);
            cout << "\n  Issue: " << e.what() << "\n";
            setColor(COLOR_RESET);
        }
    }

    void declareEmergency()
    {
        try {
            string flightID;
            setColor(COLOR_HEADER);
            cout << "\n|||===========================================================|||\n";
            cout << "|||                   DECLARE EMERGENCY                       |||\n";
            cout << "|||===========================================================|||\n";
            setColor(COLOR_RESET);
            setColor(COLOR_INFO);
            cout << "  Enter Flight ID: ";
            setColor(COLOR_RESET);
            cin >> flightID;
            Aircraft* aircraft = aircraftRegistry.search(flightID);
            if (aircraft == NULL) 
            {
                throw FlightNotFoundException(flightID);
            }
            setColor(COLOR_WARNING);
            cout << "\n  Emergency Types:\n";
            setColor(COLOR_RESET);
            cout << "    1. Medical Emergency\n";
            cout << "    2. Engine Failure\n";
            cout << "    3. Hijack\n";
            cout << "    4. Low Fuel\n";
            setColor(COLOR_INFO);
            cout << "  Choice: ";
            setColor(COLOR_RESET);
            string choiceStr;
            cin >> choiceStr;
            int choice = stringToInt(choiceStr);
            if (choice < 1 || choice > 4)
            {
                throw InvalidInputException("Emergency Type (must be 1-4)");
            }
            int newPriority = (choice >= 1 && choice <= 3) ? 1 : 2;
            if (choice == 4)
            {
                aircraft->fuelLevel = 5;
            }
            aircraft->priority = newPriority;
            aircraft->status = "EMERGENCY";
            aircraftRegistry.update(flightID, *aircraft);
            landingQueue.decreaseKey(flightID, newPriority);
            setColor(COLOR_SUCCESS);
            cout << "\n  Emergency declared! " << flightID << " moved to priority " << newPriority << "\n";
            if (choice == 4)
            {
                cout << "  Fuel level set to 5% (CRITICAL)\n";
            }
            setColor(COLOR_RESET);
        }
        catch (FlightNotFoundException& e)
        {
            setColor(COLOR_DANGER);
            cout << "\n  Issue: " << e.what() << "\n";
            setColor(COLOR_RESET);
        }
        catch (InvalidInputException& e) 
        {
            setColor(COLOR_DANGER);
            cout << "\n  Issue: " << e.what() << "\n";
            setColor(COLOR_RESET);
        }
    }

    void landFlight()
    {
        try
        {
            Aircraft landed = landingQueue.extractMin();
            radar[landed.x][landed.y] = '.';
            airspace.freeNode(landed.currentNode);
            landed.status = "Landed";
            landed.timestamp = rand();
            aircraftRegistry.remove(landed.flightID);
            flightLog.insert(landed);
            setColor(COLOR_SUCCESS);
            cout << "\n  Flight " << landed.flightID << " has landed safely!\n";
            setColor(COLOR_INFO);
            cout << "    Priority was: " << getPriorityString(landed.priority) << "\n";
            cout << "    Landed at: " << landed.currentNode << "\n";
            setColor(COLOR_RESET);
        }
        catch (EmptyQueueException& e)
        {
            setColor(COLOR_WARNING);
            cout << "\n  " << e.what() << "\n";
            setColor(COLOR_RESET);
        }
    }
    void searchFlight()
    {
        try
        {
            string flightID;
            setColor(COLOR_HEADER);
            cout << "\n|||===========================================================|||\n";
            cout << "|||                    SEARCH FLIGHT                          |||\n";
            cout << "|||===========================================================|||\n";
            setColor(COLOR_RESET);
            setColor(COLOR_INFO);
            cout << "  Enter Flight ID: ";
            setColor(COLOR_RESET);
            cin >> flightID;
            Aircraft* aircraft = aircraftRegistry.search(flightID);
            if (aircraft == NULL) 
            {
                throw FlightNotFoundException(flightID);
            }
            setColor(COLOR_HEADER);
            cout << "\n|||===========================================================|||\n";
            cout << "|||                 FLIGHT INFORMATION                        |||\n";
            cout << "|||===========================================================|||\n";
            setColor(COLOR_RESET);
            setColor(COLOR_INFO);
            cout << "  Flight ID:    ";
            setColor(MENU);
            cout << aircraft->flightID << endl;
            setColor(COLOR_INFO);
            cout << "  Model:        ";
            setColor(MENU);
            cout << aircraft->model << endl;
            setColor(COLOR_INFO);
            cout << "  Fuel Level:   ";
            if (aircraft->fuelLevel < 10) 
            {
                setColor(COLOR_DANGER);
            }
            else if (aircraft->fuelLevel < 30)
            {
                setColor(COLOR_WARNING);
            }
            else
            {
                setColor(COLOR_SUCCESS);
            }
            cout << aircraft->fuelLevel << "%\n";
            setColor(COLOR_INFO);
            cout << "  Origin:       ";
            setColor(MENU);
            cout << aircraft->origin << endl;
            setColor(COLOR_INFO);
            cout << "  Destination:  ";
            setColor(MENU);
            cout << aircraft->destination << endl;
            setColor(COLOR_INFO);
            cout << "  Priority:     ";
            if (aircraft->priority == 1) 
            {
                setColor(COLOR_DANGER);
            }
            else if (aircraft->priority == 2)
            {
                setColor(COLOR_WARNING);
            }
            else 
            {
                setColor(COLOR_INFO);
            }
            cout << getPriorityString(aircraft->priority) << endl;
            setColor(COLOR_INFO);
            cout << "  Status:       ";
            if (aircraft->status == "EMERGENCY")
            {
                setColor(COLOR_DANGER);
            }
            else if (aircraft->status == "Landed")
            {
                setColor(COLOR_SUCCESS);
            }
            else
            {
                setColor(MENU);
            }
            cout << aircraft->status << endl;
            setColor(COLOR_INFO);
            cout << "  Position:     ";
            setColor(MENU);
            cout << "(" << aircraft->x << ", " << aircraft->y << ")\n";
            setColor(COLOR_INFO);
            cout << "  Current Node: ";
            setColor(MENU);
            cout << aircraft->currentNode << endl;
            setColor(COLOR_RESET);
        }
        catch (FlightNotFoundException& e) 
        {
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
        try 
        {
            string currentPos, destination;
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
            setColor(COLOR_INFO);
            cout << "\n  Current Position: ";
            setColor(COLOR_RESET);
            cin >> currentPos;
            setColor(COLOR_INFO);
            cout << "  Destination: ";
            setColor(COLOR_RESET);
            cin >> destination;
            airspace.dijkstra(currentPos, destination);
        }
        catch (InvalidInputException& e) 
        {
            setColor(COLOR_DANGER);
            cout << "\n  Issue: " << e.what() << "\n";
            setColor(COLOR_RESET);
        }
    }
    void emergencyRoute() 
    {
        try
        {
            string flightID;
            setColor(COLOR_HEADER);
            cout << "\n|||===========================================================|||\n";
            cout << "|||            EMERGENCY ROUTE TO NEAREST AIRPORT             |||\n";
            cout << "|||===========================================================|||\n";
            setColor(COLOR_RESET);
            setColor(COLOR_INFO);
            cout << "  Enter Flight ID: ";
            setColor(COLOR_RESET);
            cin >> flightID;
            Aircraft* aircraft = aircraftRegistry.search(flightID);
            if (aircraft == NULL) 
            {
                throw FlightNotFoundException(flightID);
            }
            string nearestAirport = airspace.findNearestAirport(aircraft->currentNode);
            if (nearestAirport.empty())
            {
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
            airspace.dijkstra(aircraft->currentNode, nearestAirport);
        }
        catch (FlightNotFoundException& e) 
        {
            setColor(COLOR_DANGER);
            cout << "\n  Issue: " << e.what() << "\n";
            setColor(COLOR_RESET);
        }
    }
    void moveFlight()
    {
        try
        {
            string flightID, destination;
            setColor(COLOR_HEADER);
            cout << "\n|||===========================================================|||\n";
            cout << "|||                    MOVE FLIGHT                            |||\n";
            cout << "|||===========================================================|||\n";
            setColor(COLOR_RESET);
            setColor(COLOR_INFO);
            cout << "  Enter Flight ID: ";
            setColor(COLOR_RESET);
            cin >> flightID;
            Aircraft* aircraft = aircraftRegistry.search(flightID);
            if (aircraft == NULL) 
            {
                throw FlightNotFoundException(flightID);
            }
            setColor(COLOR_INFO);
            cout << "  Current Node: " << aircraft->currentNode << "\n";
            cout << "  Enter Destination Node: ";
            setColor(COLOR_RESET);
            cin >> destination;
            GraphNode* destNode = airspace.getNodeByName(destination);
            if (destNode == NULL)
            {
                throw InvalidInputException("Destination node");
            }
            if (airspace.isNodeOccupied(destination)) 
            {
                string pos = "(";
                char xStr[10], yStr[10];
                int xi = 0, yi = 0;
                int xVal = destNode->x;
                int yVal = destNode->y;

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

                for (int i = xi - 1; i >= 0; i--)
                    pos += xStr[i];
                pos += ", ";
                for (int i = yi - 1; i >= 0; i--) 
                    pos += yStr[i];
                pos += ")";
                throw CollisionException(pos);
            }
            radar[aircraft->x][aircraft->y] = '.';
            airspace.freeNode(aircraft->currentNode);
            aircraft->x = destNode->x;
            aircraft->y = destNode->y;
            aircraft->currentNode = destination;
            airspace.occupyNode(destination, flightID);
            radar[aircraft->x][aircraft->y] = 'P';
            aircraftRegistry.update(flightID, *aircraft);
            setColor(COLOR_SUCCESS);
            cout << "\n  Flight " << flightID << " moved to " << destination << " (" << aircraft->x << ", " << aircraft->y << ")\n";
            setColor(COLOR_RESET);
        }
        catch (FlightNotFoundException& e)
        {
            setColor(COLOR_DANGER);
            cout << "\n  Issue: " << e.what() << "\n";
            setColor(COLOR_RESET);
        }
        catch (InvalidInputException& e) 
        {
            setColor(COLOR_DANGER);
            cout << "\n  Issue: " << e.what() << "\n";
            setColor(COLOR_RESET);
        }
        catch (CollisionException& e) 
        {
            setColor(COLOR_DANGER);
            cout << "\n  Issue: " << e.what() << "\n";
            setColor(COLOR_RESET);
        }
    }

    void saveData() 
    {
        try
        {
            aircraftRegistry.saveToFile("aircraft_registry.txt");
            flightLog.saveToFile("flight_log.txt");
            setColor(COLOR_SUCCESS);
            cout << "\n  Data saved successfully!\n";
            setColor(COLOR_INFO);
            cout << "    ---> aircraft_registry.txt\n";
            cout << "    ---> flight_log.txt\n";
            setColor(COLOR_RESET);
        }
        catch (FileOperationException& e)
        {
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
            aircraftRegistry.loadFromFile("aircraft_registry.txt");
            setColor(COLOR_SUCCESS);
            cout << "  Data loaded successfully!\n";
            setColor(COLOR_RESET);
        }
        catch (FileOperationException& e) 
        {
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
        cout << "  1. ";
        setColor(COLOR_RESET);
        cout << "Display Airspace Visualizer (Radar)\n";
        setColor(MENU);
        cout << "  2. ";
        setColor(COLOR_RESET);
        cout << "Add New Flight\n";
        setColor(MENU);
        cout << "  3. ";
        setColor(COLOR_RESET);
        cout << "Declare Emergency\n";
        setColor(MENU);
        cout << "  4. ";
        setColor(COLOR_RESET);
        cout << "Process Landing (Land Next Flight)\n";
        setColor(MENU);
        cout << "  5. ";
        setColor(COLOR_RESET);
        cout << "Search Flight by ID\n";
        setColor(MENU);
        cout << "  6. ";
        setColor(COLOR_RESET);
        cout << "Print Flight Log (AVL Tree)\n";
        setColor(MENU);
        cout << "  7. ";
        setColor(COLOR_RESET);
        cout << "Find Safe Route (Dijkstra)\n";
        setColor(MENU);
        cout << "  8. ";
        setColor(COLOR_RESET);
        cout << "Emergency Route to Nearest Airport\n";
        setColor(MENU);
        cout << "  9. ";
        setColor(COLOR_RESET);
        cout << "Move Flight to New Node\n";
        setColor(MENU);
        cout << "  10. ";
        setColor(COLOR_RESET);
        cout << "Save System Data\n";
        setColor(MENU);
        cout << "  11. ";
        setColor(COLOR_RESET);
        cout << "Load System Data\n";
        setColor(MENU);
        cout << "  0. ";
        setColor(COLOR_RESET);
        cout << "Exit System\n";
        setColor(COLOR_SUBTITLE);
        cout << "----------------------------------------------------------------\n";
        setColor(COLOR_RESET);
    }
    void run() 
    {
        int choice;
        while (true)
        {
            displayMenu();
            displayLandingQueue();
            displayStatistics();
            setColor(COLOR_INFO);
            cout << "\n  Enter choice: ";
            setColor(COLOR_RESET);
            string choiceStr;
            cin >> choiceStr;
            choice = stringToInt(choiceStr);
            switch (choice)
            {
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
                saveData();
                break;
            case 11:
                loadData();
                break;
            case 0:
                setColor(COLOR_SUCCESS);
                cout << "\n  Shutting down SkyNet ATC. Goodbye!\n";
                setColor(COLOR_RESET);
                return;
            default:
                setColor(COLOR_DANGER);
                cout << "\n  Invalid choice! Please try again.\n";
                setColor(COLOR_RESET);
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