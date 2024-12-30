#include <iostream>
#include <ctime>
#include <cstdlib>
#include <unistd.h>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <signal.h>
#include <stdlib.h>
#include <time.h>
#include <set>
#include <sstream>
#include <tuple>
using namespace std;

class Point
{
private:
    double xcoor;
    double ycoor;

public:
    Point(double xcoor, double ycoor)
    {
        this->xcoor = xcoor;
        this->ycoor = ycoor;
    }

    double getXcoor() { return this->xcoor; }
    void setXcoor(double xcoor) { this->xcoor = xcoor; }
    double getYcoor() { return this->ycoor; }
    void setYcoor(double ycoor) { this->ycoor = ycoor; }

    int min()
    {
        if (this->getXcoor() < this->getYcoor())
            return this->getXcoor();
        else
            return this->getYcoor();
    }
    int max()
    {
        if (this->getXcoor() > this->getYcoor())
            return this->getXcoor();
        else
            return this->getYcoor();
    }
    bool operator<(const Point &p) const
    {
        if (this->xcoor < p.xcoor)
            return true;
        if (this->xcoor == p.xcoor && this->ycoor < p.ycoor)
            return true;
        return false;
    }
};

class Line
{
private:
    Point *sourcePoint;
    Point *destinPoint;

public:
    Line(Point *sourcePoint, Point *destinPoint)
    {
        this->sourcePoint = sourcePoint;
        this->destinPoint = destinPoint;
    }

    Point *getSourcePoint() { return this->sourcePoint; }
    Point *getDestinPoint() { return this->destinPoint; }
};

class Street
{
private:
    string streetName;
    vector<Line *> streetLines;

public:
    Street(string streetName)
    {
        this->streetName = streetName;
    }
    void addPath(Line *newLine) { this->streetLines.push_back(newLine); }
    string getStreetName() { return this->streetName; }
    vector<Line *> getStreetLines() { return this->streetLines; }
};

class StreetDatabase
{
private:
    vector<Street *> streets;

public:
    vector<Street *> getStreets()
    {
        return this->streets;
    }
    void addStreet(Street *newStreet)
    {
        this->streets.push_back(newStreet);
    }
};

string converttostring(char *a)
{
    string s = "";
    for (int i = 0; i < 10; i++)
    {
        s = s + a[i];
    }
    return s;
}
bool check = false;

int get_gradient(Line *l)
{
    double x1 = l->getSourcePoint()->getXcoor();
    double y1 = l->getSourcePoint()->getYcoor();
    double x2 = l->getDestinPoint()->getXcoor();
    double y2 = l->getDestinPoint()->getYcoor();

    int m = -5;
    if (x1 != x2)
    {
        m = (1. / (x1 - x2) * (y1 - y2));
        return m;
    }
    return m;
}

bool find_parallel(Line *l1, Line *l2)
{
    if (get_gradient(l1) != get_gradient(l2))
    {
        return false;
    }
    return true;
}

double get_intersect(Line *l)
{
    return l->getSourcePoint()->getYcoor() - (get_gradient(l) * l->getSourcePoint()->getXcoor());
}

Point *find_intersection(Line *l1, Line *l2)
{

    if (!find_parallel(l1, l2))
    {
        Point *p;
        double x, y;
        if ((get_gradient(l1) != -5) && (get_gradient(l2) != -5))
        {
            x = (1 / (get_gradient(l1) - get_gradient(l2))) * (get_intersect(l2) - get_intersect(l1));
            y = (get_gradient(l1) * x) + get_intersect(l1);
        }
        else
        {
            if (get_gradient(l1) == -5)
            {
                x = l1->getSourcePoint()->getXcoor();
                y = (get_gradient(l2) * x) + get_intersect(l2);
            }
            if (get_gradient(l2) == -5)
            {
                x = l2->getSourcePoint()->getXcoor();
                y = (get_gradient(l1) * x) + get_intersect(l1);
            }
        }
        p = new Point(x, y);
        return p;
    }
}

double find_distance(double x1, double x2, double y1, double y2)
{
    double dist = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
    return dist;
}

bool check_intersect(Line *l1, Line *l2)
{
    Point *A = l1->getSourcePoint();
    Point *B = l1->getDestinPoint();
    Point *C = l2->getSourcePoint();
    Point *D = l2->getDestinPoint();

    double x1 = A->getXcoor(), y1 = A->getYcoor();
    double x2 = B->getXcoor(), y2 = B->getYcoor();
    double x3 = C->getXcoor(), y3 = C->getYcoor();
    double x4 = D->getXcoor(), y4 = D->getYcoor();

    // Calculate the denominator
    double denominator = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);

    // Lines are parallel if the denominator is zero
    if (denominator == 0)
        return false;

    // Calculate the intersection point's x and y using determinants
    double intersect_x = ((x1 * y2 - y1 * x2) * (x3 - x4) - (x1 - x2) * (x3 * y4 - y3 * x4)) / denominator;
    double intersect_y = ((x1 * y2 - y1 * x2) * (y3 - y4) - (y1 - y2) * (x3 * y4 - y3 * x4)) / denominator;

    // Check if the intersection point is within both line segments' bounding boxes
    if (intersect_x >= std::min(x1, x2) && intersect_x <= std::max(x1, x2) &&
        intersect_y >= std::min(y1, y2) && intersect_y <= std::max(y1, y2) &&
        intersect_x >= std::min(x3, x4) && intersect_x <= std::max(x3, x4) &&
        intersect_y >= std::min(y3, y4) && intersect_y <= std::max(y3, y4))
    {
        return true;
    }

    return false;
}

bool check_two_street(Street *st, StreetDatabase *st_database)
{
    for (Street *s : st_database->getStreets())
    {
        vector<Line *> s_lines = s->getStreetLines();
        vector<Line *> st_lines = st->getStreetLines();
        for (Line *l : st_lines)
        {
            for (Line *inner_line : s_lines)
            {
                if (check_intersect(l, inner_line))
                    return true;
            }
        }
    }
    return false;
}
void create_streets(int no_of_streets, int num_points, int number_of_seconds, int coordinate_range)
{
    srand(time(NULL));
    int random_seconds = rand() % (number_of_seconds - 5 + 1) + 5;
    StreetDatabase *st_database = new StreetDatabase();
    int st_no = rand() % (no_of_streets - 2 + 1) + 2;
    for (int k = 0; k < st_no; k++)
    {

        char alphabets[52] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};
        char sname[10];

        int i = 0;
        while (i < 10)
        {
            int temp = rand() % 52;
            sname[i] = alphabets[temp];
            i++;
        }
        int segments;
        do
        {
            segments = rand() % num_points + 1;
        } while (segments < 1);
        int points = segments;
        Street *st;
        do
        {
            st = new Street(converttostring(sname));
            Point *p1;
            Point *p2;
            int limit = 0;
            set<string> uniquePoints;
            for (int j = 0; j < points; j++)
            {
                limit++;
                int a, b;
                if (j == 0)
                {
                    a = (rand() % (2 * coordinate_range + 1)) - coordinate_range;
                    b = (rand() % (2 * coordinate_range + 1)) - coordinate_range;
                    p1 = new Point(a, b);
                }
                Line *l;
                bool check_intersection;
                do
                {
                    check_intersection = false;
                    a = (rand() % (2 * coordinate_range + 1)) - coordinate_range;
                    b = (rand() % (2 * coordinate_range + 1)) - coordinate_range;
                    p2 = new Point(a, b);
                    l = new Line(p1, p2);
                    vector<Line *> v_lines = st->getStreetLines();
                    for (Line *f_l : v_lines)
                    {
                        // bool result = check_intersect(l, f_l);
                        // cout<<result<<"res"<<endl;
                        if (check_intersect(l, f_l))
                        {
                            check_intersection = true;
                            break;
                        }
                    }
                } while (check_intersection == true);
                p1 = p2;
                st->addPath(l);
                // if (limit > 25)
                // {
                //     cout << "Error: Maximum limit 25 reached" << endl;
                //     exit(0);
                //     break;
                // }
            }

        } while (k == st_no - 1 && (!check_two_street(st, st_database)));

        // to print
        set<Point> uniquePoints;
        cout << "add" << " " << "\"" << st->getStreetName() << "\"" << " ";
        int t = 0;
        for (Line *lin : st->getStreetLines())
        {
            if (t == 0)
            {

                Point *pp1 = lin->getSourcePoint();
                Point *pp2 = lin->getDestinPoint();
                if (uniquePoints.insert(*pp1).second)
                {
                    cout << "(" << pp1->getXcoor() << "," << pp1->getYcoor() << ") ";
                }
                if (uniquePoints.insert(*pp2).second)
                {
                    cout << "(" << pp2->getXcoor() << "," << pp2->getYcoor() << ") ";
                }
                // cout << "(" << pp1->getXcoor() << "," << pp1->getYcoor() << ") ";
                // cout << "(" << pp2->getXcoor() << "," << pp2->getYcoor() << ") ";
            }
            else
            {
                Point *pp1 = lin->getDestinPoint();
                if (uniquePoints.insert(*pp1).second)
                {
                    cout << "(" << pp1->getXcoor() << "," << pp1->getYcoor() << ") ";
                }

                // cout << "(" << pp1->getXcoor() << "," << pp1->getYcoor() << ") ";
            }
            t++;
        }

        st_database->addStreet(st);
        cout << endl;
    }

    cout << "gg" << endl;

    sleep(random_seconds);

    vector<Street *> temp_st = st_database->getStreets();
    for (Street *st : temp_st)
    {
        vector<Line *> lines = st->getStreetLines();
        cout << "rm" << " " << "\"" << st->getStreetName() << "\"";
        cout << endl;
    }
}

int main(int argc, char **argv)
{
    srand(time(NULL));
    opterr = 0;
    string temp;
    int snum = 10, nnum = 5, lnum = 5, cnum = 20, c;
    while ((c = getopt(argc, argv, "s:n:l:c:")) != -1)
    {

        if (c == 's')
        {
            temp = optarg;
            snum = atoi(temp.c_str());
            if (snum < 2)
            {
                cerr << "Error: Number of streets should be in range of 2 and 10." << endl;
                pid_t parent_pid = getppid();
                kill(parent_pid, SIGTERM);
                exit(1);
            }
        }
        else if (c == 'n')
        {
            temp = optarg;
            nnum = atoi(temp.c_str());

            if (nnum < 1)
            {
                cerr << "Error: Number of line segments in a street cant be less than 1." << endl;
                pid_t parent_pid = getppid();
                kill(parent_pid, SIGTERM);
                exit(1);
            }
        }
        else if (c == 'l')
        {
            temp = optarg;
            lnum = atoi(temp.c_str());

            if (lnum < 5)
            {
                cerr << "Error: Minimum number of seconds is 5." << endl;
                pid_t parent_pid = getppid();
                kill(parent_pid, SIGTERM);
                exit(1);
            }
        }
        else if (c == 'c')
        {
            temp = optarg;
            cnum = atoi(temp.c_str());

            if (cnum < 1)
            {
                cerr << "Error: Invalid c value." << endl;
                pid_t parent_pid = getppid();
                kill(parent_pid, SIGTERM);
                exit(1);
            }
        }
        else if (c == '?')
        {
            if (optopt == 's' || optopt == 'n' || optopt == 'l' || optopt == 'c')
            {
                cerr << "Error: Argument requried." << endl;
                pid_t parent_pid = getppid();
                kill(parent_pid, SIGTERM);
                exit(1);
            }

            else
            {
                cerr << "Error: Invalid option." << endl;
                pid_t parent_pid = getppid();
                kill(parent_pid, SIGTERM);
                exit(1);
            }
        }
        else
        {
            return 0;
        }
    }
    while (true)
    {
        create_streets(snum, nnum, lnum, cnum);
    }

    return 0;
}
