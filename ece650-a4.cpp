#include <iostream>
#include <sstream>
#include <vector>
#include <set>
#include <string.h>
#include <memory>
#include "minisat/core/Solver.h"
#include "minisat/core/SolverTypes.h"
#include <queue>
#include <list>
#include <algorithm>
#include <map>

using namespace std;

char remove_spaces(string ip)
{
    char a;
    string ip1;
    int first_char = 0;

    for (int i = 0; i < ip.length(); i++)
    {
        // cout << ip.length();
        // cout << i;
        if (ip[i] != ' ')
        {
            // cout<< ip[i] << endl;
            ip1[first_char] = ip[i];
            // cout<< ip1[first_char] << endl;
            break;
        }
    }
    a = ip1[first_char];

    return a;
}

char input_parser(string input)
{
    char inp;
    inp = remove_spaces(input);
    return inp;
}

void find_vertex_cover(char parsed_input, string input)
{
    static int vertex_count;
    string vertices;
    char ver;
    istringstream isstr(input);
    if (parsed_input == 'V')
    {
        isstr >> ver >> vertex_count;
    }

    else if (parsed_input == 'E')
    {
        isstr >> ver >> vertices;
        if ((vertices[0] == '{') && (vertices[vertices.length() - 1]) == '}')
        {
            vector<int> V, V1, V2, W;

            for (int i = 0; i < vertices.length(); i++)
            {
                if (vertices[i] == '<')
                {
                    string a = "";
                    int a1 = 0;
                    while (vertices[++i] != ',')
                    {
                        a += vertices[i];
                    }
                    istringstream ias(a);
                    ias >> a1;
                    if (a1 < 1)
                    {
                        cout << "Error: Vertex value less than 1 detected" << endl;
                        return;
                    }

                    V.push_back(a1);

                    string b = "";
                    int b1 = 0;
                    while (vertices[++i] != '>')
                    {
                        b += vertices[i];
                    }
                    istringstream ibs(b);
                    ibs >> b1;
                    if (b1 < 1)
                    {
                        cout << "Error: Vertex value less than 1 detected" << endl;
                        return;
                    }
                    if (a1 == b1)
                    {
                        cout << "Error: Self-loop detected" << endl;
                        return;
                    }

                    V.push_back(b1);
                }
            }
            if (V.size() == 0)
            {
                cout << "Error: Invalid Graph" << endl;
            }
            else
            {

                copy(V.begin(), V.end(), back_inserter(V1));

                int comp = 0;
                for (int i = 0; i < V.size(); i++)
                {
                    if (comp < V[i])
                    {
                        comp = V[i];
                    }
                }

                if (comp > vertex_count)
                {
                    cout << "Error: Invalid vertices." << endl;
                }
                else
                {
                    bool z = true;
                    while (z)
                    {
                        map<int, int> occurrence;

                        for (int i = 0; i < V.size(); i++)
                        {
                            if (V[i] != -1)
                            {
                                occurrence[V[i]]++;
                            }
                        }

                        int max_count = 0;
                        int max_num = 0;
                        for (auto i : occurrence)
                        {
                            if (i.second > max_count)
                            {
                                max_count = i.second;
                                max_num = i.first;
                                // cout << "Max_Num: " << max_num << endl;
                            }
                        }

                        // cout << "Max_Num: " << max_num << " Max_Count: " << max_count << endl;
                        W.push_back(max_num);
                        // V.erase(remove(V.begin(), V.end(), max_num), V.end());

                        for (int i = 0; i < V.size(); i += 2)
                        {
                            if (V[i] == max_num)
                            {
                                // cout << "i" << endl;
                                V[i] = -1;
                                V[i + 1] = -1;
                            }

                            else if (V[i + 1] == max_num)
                            {
                                // cout << "i+1" << endl;
                                V[i] = -1;
                                V[i + 1] = -1;
                            }
                        }

                        V.erase(remove(V.begin(), V.end(), -1), V.end());

                        if (V.empty())
                        {
                            z = false;
                        }
                    }
                }
            }
            sort(W.begin(), W.end());

            for (int i = 0; i < W.size(); i++)
            {
                if (i == W.size() - 1)
                {
                    cout << W[i];
                }
                else
                {
                    cout << W[i] << " ";
                }
            }
            cout << "" << endl;
        }
        else
        {
            cout << "Error: Invalid vertices format";
            return;
        }
    }
}

int main()
{
    string input;
    while (getline(cin, input))
    {
        char parsed_input = input_parser(input);
        find_vertex_cover(parsed_input, input);
    }
    if (cin.eof())
    {
        exit(0);
    }
    return 0;
}
