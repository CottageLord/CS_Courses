#pragma once
/*
* code to read the file and store it in Path Map
*/
#include <iostream>
#include <string>
#include <vector>
#include <fstream>

using namespace std;

void static readMap(const std::string& filename, std::vector<std::vector<char>>& PathMap)
{

    std::ifstream file(filename);
    std::vector<char> row;
    std::string input;
    while (file >> input) {
        row.clear();
        for (char i : input) {
            row.push_back(i);
        }
        PathMap.insert(PathMap.begin(), row);
    }

    //To check if there are equal number of values in each row.
    size_t length = PathMap[0].size();
    for (size_t i = 1; i < PathMap.size(); i++) {
        if (length != PathMap[i].size()) {
            printf("Error in input file.");
            PathMap.clear();
            return;
        }
    }
}

void static PrintMap(std::vector<std::vector<char>> PathMap) {
    for (int i = PathMap.size() - 1; i >= 0; i--)
    {
        for (int j = 0; j < PathMap[0].size(); j++)
        {
            std::cout << PathMap[i][j];
        }
        std::cout << endl;

    }
}



enum class mode { Standard, Expanded, All };
enum class heuristic { StraightLine, Manhattan };

mode convertMode(const std::string str)
{
    if (str == "Standard")
    {
        return mode::Standard;
    }
    else if (str == "Expanded") {
        return mode::Expanded;
    }

    else if (str == "All") {
        return mode::All;
    }
    else throw("illegal value of mode");
}
heuristic convertHeuristic(const std::string str)
{
    if (str == "StraightLine")
    {
        return heuristic::StraightLine;
    }
    else if (str == "Manhattan")
    {
        return heuristic::Manhattan;
    }

    else throw("Illegal value of heuristic");
}