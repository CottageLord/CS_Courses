// lab01.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
//Enum in header
//master class 
//make main file thin option menu
//make cpp for function
//test suites

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <chrono>
#include "Project1.h"

using namespace std;

/*
* Code to plan the path between the start and end destination.
*/

void PlanPath(vector<vector<char>> outputMap, int start[2], int destination[2],int mode,int heuristic)
{
    
    using namespace std::chrono;
    
    steady_clock::time_point clock_begin = steady_clock::now();

    //implement the code
    
    steady_clock::time_point clock_end = steady_clock::now();

    steady_clock::duration time_span = clock_end - clock_begin;

    double nseconds = double(time_span.count()) * steady_clock::period::num / steady_clock::period::den;

    std::cout << "It took me " << nseconds << " seconds.";
    std::cout << std::endl;
    //Printing Output
    PrintMap(outputMap);
}

int main()
{
    vector<vector<char>> PathMap;
    string loc;
    string modeType,heuristicType;
    int start[2], end[2];
    
    cout << "Enter the location of the file:";
    std::cin >> loc;
    readMap(loc,PathMap);
    cout << "the input map is:\n";
    PrintMap(PathMap);
    vector<vector<char>> outputMap;
    outputMap = PathMap;
    cout << "Enter the mode:";
    cin >> modeType;
    mode mode1=convertMode(modeType);
    cout << "Enter the heuristic:";
    cin >> heuristicType;
    heuristic heuristic1=convertHeuristic(heuristicType);
    
    cout << "Start x co-ordinate:";
    cin >> start[0];
    cout << "End x co-ordinate:";
    cin >> start[1];
    cout << "Start y-cordinate:";
    cin >> end[0];
    cout << "End y-cordinate:";
    cin >> end[1];
    PlanPath(outputMap, start, end,mode1,heuristic1); 
}
