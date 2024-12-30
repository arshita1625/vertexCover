#include <iostream>
#include <string>
#include <sstream>
// #include <bits/stdc++.h>
#include <list>
#include <vector>
#include <climits>
#include <cstring>
using namespace std;
class Shortest_path
{
public:
    int numVertices;
    vector<int> *adjacencyList;
    Shortest_path() : numVertices(0), adjacencyList(nullptr) {}
    ~Shortest_path()
    { // Destructor to deallocate memory
        if (adjacencyList != nullptr)
        {
            delete[] adjacencyList; // Properly delete adjacency list
            adjacencyList = nullptr;
        }
    }
    // string path;

    bool performBFS(int start, int end, int totalVertices, std::vector<int> &predecessor, std::vector<int> &distance)
    {
        list<int> queue;
        // bool visited[totalVertices + 1];
        vector<bool> visited(totalVertices + 1, false);
        for (int i = 1; i <= totalVertices; i++)
        {

            // visited[i] = false;
            distance[i] = INT_MAX;
            predecessor[i] = -1;
            // queue.push_back(i);
        }
        visited[start] = true;
        distance[start] = 0;
        queue.push_back(start);
        while (!queue.empty())
        {
            int ele = queue.front();
            // cout << adjacencyList[ele].size() << endl;
            // cout << "ele" << ele << endl;
            queue.pop_front();
            for (int i = 0; i < adjacencyList[ele].size(); i++)
            {
                if (visited[adjacencyList[ele][i]] == false)
                {
                    visited[adjacencyList[ele][i]] = true;
                    distance[adjacencyList[ele][i]] = distance[ele] + 1;
                    predecessor[adjacencyList[ele][i]] = ele;
                    queue.push_back(adjacencyList[ele][i]);
                    if (adjacencyList[ele][i] == end)
                    {
                        return true;
                    }
                }
            }
        }

        return false;
    }
    void findShortestDistance(int start, int destination)
    {
        // int pred[numVertices + 1], dist[numVertices + 1];
        std::vector<int>
            pred(numVertices + 1);
        std::vector<int> dist(numVertices + 1);

        if (performBFS(start, destination, numVertices, pred, dist) == false)
        {
            cout << "Error: Entered source vertex and destination vertex are not connected\n";
            exit(0);
            // return;
        }
        vector<int> route;
        int d = destination;
        route.push_back(d);
        while (pred[d] != -1)
        {
            route.push_back(pred[d]);
            d = pred[d];
        }
        for (int i = route.size() - 1; i >= 0; i--)
            if (i == 0)
                cout << route[i] << endl;
            else
                cout << route[i] << "-";
    }

    void initializeGraph(int vertices)
    {
        if (adjacencyList != nullptr)
        {
            delete[] adjacencyList; // Free previously allocated memory
        }
        numVertices = vertices;
        adjacencyList = new vector<int>[numVertices + 1];
    }

    void connectEdge(int x1, int x2)
    {
        try

        {
            if (x1 > numVertices || x2 > numVertices || x1 < 1 || x2 < 1)
            {
                cout << "Error: Vertex index out of bounds" << endl;
                exit(0);
                // return;
            }
            for (int j = 0; j < adjacencyList[x1].size(); j++)
            {
                bool edge_exists = false;
                // int neighbor = adjacencyList[x1][j];
                for (int neighbor : adjacencyList[x1])
                {
                    if (neighbor == x2)
                    {
                        edge_exists = true;
                        break;
                    }
                }

                if (edge_exists)
                {
                    cout << "Error: Edge already exists" << endl;
                    exit(0);
                    // return;
                }
            }
            adjacencyList[x1].push_back(x2);
            adjacencyList[x2].push_back(x1);
        }
        catch (const char *msg)
        {
            // cout<<msg<<endl;
            // throw msg;
        }
    }
};
Shortest_path sh;
string command;
string final_data;
int len = 0;
string processInput(string in)
{
    string word;
    string out;
    istringstream splits(in, istringstream::in);

    splits >> command;

    if (command.compare("V") == 0)
    {

        splits >> word;
        int number = std::stoi(word);
        bool isNumeric = true;
        for (size_t i = 0; i < word.length(); i++)
        {
            if (!isdigit(word[i]))
            {
                isNumeric = false;
                break;
            }
        }
        if (!isNumeric or number <= 1)
        {
            if (number != 0)
            {
                cout << "Error: Invalid command" << endl;
                // return out;
                exit(0);
            }
        }
        stringstream value(word);
        value >> len;
        sh.initializeGraph(len);
    }

    else if (command.compare("E") == 0)
    {
        try
        {
            // sh.initializeGraph(len);
            splits >> word;
            // int n = word.length();
            // std::vector<char> arr(word.length() + 1);
            std::string arr = word;
            // char arr[130];
            // strncpy(arr, word.c_str(), sizeof(arr));
            // strcpy(arr, word.c_str());
            string x = "",
                   y = "";
            // bool error = false;

            for (int i = 0; i < word.length(); i++)
            {
                if (arr[i] == '<')
                {

                    while (arr[++i] != ',')
                    {

                        x.push_back(arr[i]);
                    }

                    while (arr[++i] != '>')
                    {

                        y.push_back(arr[i]);
                    }
                    stringstream x_val(x);
                    stringstream y_val(y);
                    int x_value = 0, y_value = 0;
                    x_val >> x_value;
                    y_val >> y_value;
                    if (x_value > sh.numVertices || x_value < 1 || y_value > sh.numVertices || y_value < 1 || x_value == y_value)
                    {
                        cout << "Error: Invalid input" << endl;
                        exit(0);
                    }
                    sh.connectEdge(x_value, y_value);

                    x = "";
                    y = "";
                }
            }
        }
        catch (const char *msg)
        {
            sh.initializeGraph(0);
            cout << "Error: Invalid Input" << endl;
            exit(0);
        }
    }
    else if (command.compare("s") == 0)
    {

        string start;
        string end;
        int x = 0, y = 0;
        splits >> start;

        stringstream value(start);
        value >> x;

        splits >> end;
        stringstream value2(end);
        value2 >> y;
        try
        {
            sh.findShortestDistance(x, y);
            exit(0);
        }
        catch (...)
        {
            cout << "Error: Invalid Command" << endl;
            exit(0);
        }
    }
    else
    {
        cout << "Error: Invalid command" << endl;
        exit(0);
    }
    return out;
}
int main(int argc, char **argv)
{
    string input;
    string data;

    while (true)
    {

        try
        {
            getline(cin, input);
            if (input.empty())
            {
                continue;
            }

            if (cin.eof())
            {
                break;
            }
            data = processInput(input);
        }
        catch (...)
        {
            cout << "Error: Invalid Command." << endl;
            exit(0);
        }
    }

    return 0;
}