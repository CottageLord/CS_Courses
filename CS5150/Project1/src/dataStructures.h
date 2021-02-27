#pragma once

#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <array>
#include <vector>
#include <iostream>

typedef std::pair<int, int> Position;
typedef std::pair<Position, double> Move_n_cost;
const double DIAG_COEFFICIENT = 1.41; // sqrt 2

template<typename priority_v, typename Position>
class PriorityQueue {
public:
    // as four neighbors
    typedef std::pair<priority_v, Position> neighbor;
    // queue of neighbor, stored in vector, sorted smallest first
    std::priority_queue<neighbor, std::vector<neighbor>,
        std::greater<neighbor>> elements;

    inline bool empty() const {
        return elements.empty();
    }

    inline void put(priority_v priority, Position item) {
        elements.emplace(priority, item);
    }
    // return the position (x,y) of the nearest neighbor
    Position get() {
        Position shortest_neighbor = elements.top().second;
        elements.pop();
        return shortest_neighbor;
    }
};

// ===================== typedef pair<int, int> Position ====================//

/* implement hash function so we can use unordered_set/unordered_map on Position */
// make sure the hash overide is in std namespace for correct rederence
namespace std {
    template <> struct hash<Position> {
        std::size_t operator()(const Position& id) const noexcept {
            return std::hash<int>()(id.first ^ (id.second << 4));
        }
    };
}


bool operator == (Position a, Position b) {
    return a.first == b.first && a.second == b.second;
}

bool operator != (Position a, Position b) {
    return !(a == b);
}

bool operator < (Position a, Position b) {
    return std::tie(a.first, a.second) < std::tie(b.first, b.second);
}

// check if the given point is valid - within bounds, walkable (not "x")
inline bool is_valid_pos(vector<vector<char>>& outputMap,
    int data_height, int data_width, int i, int j) {
    return (j >= 0 && j < data_width&& i >= 0 && i < data_height&&
        std::isdigit(outputMap[i][j]));
}

void check_neighbor_bound(vector<vector<char>>& outputMap, vector<Move_n_cost>& neighbors,
    int data_height, int data_width, Position start, Position end, bool diagonal) {
    // if within bound and is valid path (digit)
    if (is_valid_pos(outputMap, data_height, data_width, end.first, end.second)) {
        // compute cost by taking average between start and end
        double cost = ((outputMap[start.first][start.second] - (char)'0') + 
            (outputMap[end.first][end.second] - (char)'0')) / 2;
        if (diagonal) cost *= DIAG_COEFFICIENT;
        //std::cout << "    Adding Neighbor " << i << " " << j << std::endl;
        neighbors.push_back(std::make_pair(end, cost));
    }
}
vector<Move_n_cost> get_neighbors(vector<vector<char>>& outputMap, Position local,
    int data_height, int data_width) {

    vector<Move_n_cost> neighbors;

    //  _o|__|__
    //  __|lc|__  Check diagonal neighbors, firstly check if passable
    //    |  | 
    Position neighbor;
    // top left corner
    // check if left or top is passable
    if (is_valid_pos(outputMap, data_height, data_width, local.first - 1, local.second) ||
        is_valid_pos(outputMap, data_height, data_width, local.first, local.second + 1)) {
        // if passable, check if diagonal neighbor is valid
        neighbor.first = local.first - 1;
        neighbor.second = local.second + 1;
        check_neighbor_bound(outputMap, neighbors, data_height, data_width,
            local, neighbor, true);
    }

    //  __|__|o_
    //  __|lc|__  
    //    |  | 

    // top right corner
    // check if left or top is passable
    if (is_valid_pos(outputMap, data_height, data_width, local.first + 1, local.second) ||
        is_valid_pos(outputMap, data_height, data_width, local.first, local.second + 1)) {
        // if passable, check if diagonal neighbor is valid
        neighbor.first = local.first + 1;
        neighbor.second = local.second + 1;
        check_neighbor_bound(outputMap, neighbors, data_height, data_width,
            local, neighbor, true);
    }

    //  __|__|__
    //  __|lc|__  
    //   o|  | 

    // bottom left corner
    // check if left or top is passable
    if (is_valid_pos(outputMap, data_height, data_width, local.first - 1, local.second) ||
        is_valid_pos(outputMap, data_height, data_width, local.first, local.second - 1)) {
        // if passable, check if diagonal neighbor is valid
        neighbor.first = local.first - 1;
        neighbor.second = local.second - 1;
        check_neighbor_bound(outputMap, neighbors, data_height, data_width,
            local, neighbor, true);
    }

    //  __|__|__
    //  __|lc|__  
    //    |  |o

    // bottom right corner
    // check if left or top is passable
    if (is_valid_pos(outputMap, data_height, data_width, local.first + 1, local.second) ||
        is_valid_pos(outputMap, data_height, data_width, local.first, local.second - 1)) {
        // if passable, check if diagonal neighbor is valid
        neighbor.first = local.first + 1;
        neighbor.second = local.second - 1;
        check_neighbor_bound(outputMap, neighbors, data_height, data_width,
            local, neighbor, true);
    }

    //  __|__|__
    //  __|lc|__  Check all neighbors to see if walkable
    //    |  | 
    // Down
    neighbor.first = local.first;
    neighbor.second = local.second - 1;
    check_neighbor_bound(outputMap, neighbors, data_height, data_width,
        local, neighbor, false);
    // Up
    neighbor.first = local.first;
    neighbor.second = local.second + 1;
    check_neighbor_bound(outputMap, neighbors, data_height, data_width,
        local, neighbor, false);
    // Left
    neighbor.first = local.first - 1;
    neighbor.second = local.second;
    check_neighbor_bound(outputMap, neighbors, data_height, data_width,
        local, neighbor, false);
    // Right
    neighbor.first = local.first + 1;
    neighbor.second = local.second;
    check_neighbor_bound(outputMap, neighbors, data_height, data_width,
        local, neighbor, false);

    return neighbors;
}
