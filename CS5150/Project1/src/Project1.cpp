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
#include <cmath>
#include "Project1.h"
#include "dataStructures.h"

using namespace std;

inline double manhattan_heuristic(Position a, Position b) {
    return ((double)std::abs(a.first - b.first)) + ((double)std::abs(a.second - b.second));
}

inline double straight_line_heuristic(Position a, Position b) {
    return std::sqrt((a.first - b.first) * (a.first - b.first) +
        (a.second - b.second) * (a.second - b.second));
}
/*
* Code to plan the path between the start and end destination.
*/
void find_path(vector<vector<char>>& outputMap,
    std::unordered_map<Position, Position>& came_from,
    Position start, Position end,
    vector<Position>& expanded, vector<Position>& touched,
    heuristic heuristic) {

    // unordered_map - record the costs for each visited grid
    std::unordered_map<Position, double> cost_so_far;
    // frontier - temporarily store all will-go-to neighbors
    PriorityQueue<double, Position> frontier;

    // initialize the frontier queue. The priority doesn't matter
    // as this is just the start point and will be discarded at the beginning
    frontier.put(0, start); // heuristic(start, goal)
    came_from[start] = start;
    cost_so_far[start] = 0;
    Position last_visited;

    // record the sizes for bound checking.
    // we assume that data is trimmed (same length each line)
    int data_height = outputMap.size();
    int data_width = outputMap[0].size();

    //std::cout << "h: " << data_height << " w: " << data_width << std::endl;

    while (!frontier.empty()) {
        last_visited = frontier.get();
        /*
        std::cout << "Checking: [" << last_visited.first << " : "
            << last_visited.second << "]" << std::endl;*/
            // early quit
        if (last_visited == end) break;
        // visit neighbors
        // get the neighbor pos and cost
        for (const Move_n_cost& current_info :
            get_neighbors(outputMap, last_visited, data_height, data_width)) {
            Position current = current_info.first;
            double curr_cost = current_info.second;
            // calculate the current cost for this position
            double new_cost = cost_so_far[last_visited] + curr_cost;

            // if the current pos have been updated with cost or is a lower cost record
            if (cost_so_far.find(current) == cost_so_far.end()
                || new_cost < cost_so_far[current]) {
                // update cost
                cost_so_far[current] = new_cost;
                // update priority (cheapest to front)
                // apply heuristics differently
                double heuristic_dist = 0.0;
                if (curr_cost > 0) {
                    if (heuristic == heuristic::Manhattan)
                        heuristic_dist = manhattan_heuristic(current, end);
                    else heuristic_dist = straight_line_heuristic(current, end);
                }
                double priority = new_cost + heuristic_dist;
                frontier.put(priority, current);
                came_from[current] = last_visited;
            }
        }
        // after got all neighbors, mark the current tile as expanded
        expanded.push_back(last_visited);
    }
    // if there are still nodes in the frontier, mark them as touched
    while (!frontier.empty()) touched.push_back(frontier.get());
}
/*
* Code to mark the special nodes (expanded, touched, path, etc.)
*/
void reconstruct_path(vector<vector<char>>& outputMap,
    std::unordered_map<Position, Position>& came_from,
    Position start, Position end,
    vector<Position>& expanded, vector<Position>& touched,
    mode mode) {

    // mark all expanded
    if (mode != mode::Standard) {
        for (Position pos : expanded) {
            outputMap[pos.first][pos.second] = 'o';
        }
    }
    // mark all touched but unexpanded
    if (mode == mode::All) {
        for (Position pos : touched) {
            outputMap[pos.first][pos.second] = 'c';
        }
    }
    // trace back from the end
    Position current = end;
    // mark dest
    outputMap[current.first][current.second] = 'd';
    while (current != start && !came_from.empty()) {
        current = came_from[current];
        outputMap[current.first][current.second] = '+';
        //if (came_from.size() <= 1) break;
    }
    // mark start
    outputMap[current.first][current.second] = 's';
}

void PlanPath(vector<vector<char>>& outputMap,
    Position start, Position end, mode mode, heuristic heuristic)
{
    using namespace std::chrono;

    steady_clock::time_point clock_begin = steady_clock::now();

    // =========================== implement the code ============================

    // came from - record the optimized single node-to-node path
    std::unordered_map<Position, Position> came_from;
    std::vector<Position> expanded;
    std::vector<Position> touched;
    find_path(outputMap, came_from, start, end, expanded, touched, heuristic);
    reconstruct_path(outputMap, came_from, start, end, expanded, touched, mode);

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
    try
    {
        // read input
        vector<vector<char>> PathMap;
        string loc;
        string modeType, heuristicType;
        std::pair<int, int> start, end;

        cout << "Enter the location of the file:";
        std::cin >> loc;
        readMap(loc, PathMap);
        cout << "the input map is:\n";
        PrintMap(PathMap);
        vector<vector<char>> outputMap;
        outputMap = PathMap;
        cout << "Enter the mode:";
        cin >> modeType;
        mode mode1 = convertMode(modeType);
        cout << "Enter the heuristic:";
        cin >> heuristicType;
        heuristic heuristic1 = convertHeuristic(heuristicType);
        /* "flip" the x/y as the input file is read from top to bottom
            1 2
            3 4 ----------where (0, 1) is 1
            is read as
          {{1, 2}
           {3, 4}} -------where [0][1] is 2
        */
        cout << "Start x co-ordinate:";
        cin >> start.second;
        cout << "Start y co-ordinate:";
        cin >> start.first;
        cout << "End x-cordinate:";
        cin >> end.second;
        cout << "End y-cordinate:";
        cin >> end.first;

        int data_height = outputMap.size();
        int data_width = outputMap[0].size();
        // a simple input check
        if (is_valid_pos(outputMap, data_height, data_width, start.first, start.second) &&
            is_valid_pos(outputMap, data_height, data_width, end.first, end.second)) {
            PlanPath(outputMap, start, end, mode1, heuristic1);
        }
        else {
            std::cout << "Invalid Input: out of bound or blocked start/end point" << std::endl;
        }
    }
    catch (...)
    {
        std::cout << "Invalid input!" << std::endl;
        return 1;
    }
    return 0;
}
