#ifndef LEARN_OPENGL_MAP_HPP
#define LEARN_OPENGL_MAP_HPP

#include <iostream>
#include <memory>
#include <vector>
#include <queue>
#include <random>
#include <thread>

const size_t WIDTH = 16;
const size_t LENGTH = 16;

/*
 * 地图类 单例模式实现
 */
class Map {
public:
    enum type_t {
        ROAD_UNVISITED = 1,
        ROAD_VISITED,
        ACTUAL_ROAD,
        WALL,
        START,
        END
    };
private:
    struct node {
        int x = 0;							// 节点坐标
        int y = 0;
        bool bObstacle = false;
        bool bVisited = false;			// Have we searched this node before?
        type_t type = ROAD_UNVISITED;
        float fGlobalGoal = INFINITY;				// Distance to goal so far
        float fLocalGoal = INFINITY;				// Distance to goal if we took the alternative route
        std::shared_ptr<node> parent = nullptr;
        std::vector<std::shared_ptr<node>> vecNeighbours;
        node(int x, int y, bool is_obstacle) : x(x), y(y), bObstacle(is_obstacle) {};
    };
    typedef std::shared_ptr<node> Node;

    std::vector<std::vector<Node>> data_{};

    Node nodeStart = nullptr;
    Node nodeEnd = nullptr;
private:
    Map() {
        data_ = std::vector<std::vector<Node>>(LENGTH, std::vector<Node>());
        for (int x = 0; x < WIDTH; ++x) {
            for (int y = 0; y < LENGTH; ++y) {
                data_[x].emplace_back(std::make_shared<node>(x, y, false));
            }
        }
        // 计算每个节点的neighbor
        for (int x = 0; x < WIDTH; x++) {
            for (int y = 0; y < LENGTH; y++) {
                if (y > 0)
                    data_[x][y]->vecNeighbours.emplace_back(data_[x + 0][y - 1]);
                if (y < LENGTH - 1)
                    data_[x][y]->vecNeighbours.emplace_back(data_[x + 0][y + 1]);
                if (x > 0)
                    data_[x][y]->vecNeighbours.emplace_back(data_[x - 1][y + 0]);
                if (x < WIDTH - 1)
                    data_[x][y]->vecNeighbours.emplace_back(data_[x + 1][y + 0]);
            }
        }
        // 添加墙体
        for (int i = 4; i < 12; ++i) {
            data_[i][11]->bObstacle = true;
            data_[i][11]->type = WALL;
        }
        for (int i = 6; i < 12; ++i) {
            data_[11][i]->bObstacle = true;
            data_[11][i]->type = WALL;
        }
        for (int i = 3; i < 12; ++i) {
            data_[i][5]->bObstacle = true;
            data_[i][5]->type = WALL;
        }
        nodeStart = data_[2][LENGTH / 2];
        nodeStart->type = START;
        nodeEnd = data_[WIDTH - 3][LENGTH / 2];
        nodeEnd->type = END;
    }
public:
    Map(const Map &other) = delete;
    void operator=(const Map&) = delete;
    static Map& getInstance() {
        static Map map_;
        return map_;
    }

    std::vector<std::vector<Node>> &data() {
        return data_;
    }

    std::pair<int, int> getStart() {
        return std::pair<int, int>{nodeStart->x, nodeStart->y};
    }

    // 算法来源：https://www.youtube.com/watch?v=icZj67PTFhc
    void solve_AStar() {
        // 欧几里得距离
        auto distance = [](const Node& a, const Node& b) {
            return sqrtf(pow((a->x - b->x), 2) + pow((a->y - b->y), 2));
        };

        // 计算启发值
        auto heuristic = [distance](const Node& a, const Node& b) {
            return distance(a, b);
        };

        Node nodeCurrent = nodeStart;
        nodeStart->fLocalGoal = 0.0f;
        nodeStart->fGlobalGoal = heuristic(nodeStart, nodeEnd);


        auto cmp = [](const Node& left, const Node& right) { return left->fGlobalGoal > right->fGlobalGoal; };
        // openSet 未遍历的节点
        std::priority_queue<Node, std::vector<Node>, decltype(cmp)> openSet(cmp);
        openSet.emplace(nodeStart);

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        while (!openSet.empty() && nodeCurrent != nodeEnd) {
            while (!openSet.empty() && openSet.top()->bVisited) {
                openSet.pop();
            }
            if (openSet.empty()) break;

            nodeCurrent = openSet.top();
            nodeCurrent->bVisited = true;
            if (!(nodeCurrent->type == END || nodeCurrent->type == START)) {
                nodeCurrent->type = ROAD_VISITED;
            }

            for (auto &nodeNeighbor : nodeCurrent->vecNeighbours) {
                // 没有被visit过且不是墙体才能进入openSet
                if (!nodeNeighbor->bVisited && !nodeNeighbor->bObstacle) {
                    openSet.emplace(nodeNeighbor);
                }
                // 计算当前neighbor节点(是currentNode的邻居的条件下)的g(n)值 [f(n) = g(n) + h(n)]
                auto fPossiblyLowerGoal = nodeCurrent->fLocalGoal + distance(nodeCurrent, nodeNeighbor);
                // 如果当前新值比nodeNeighbor原来的local值小，更新nodeNeighbor的local值和他的parent
                if (fPossiblyLowerGoal < nodeNeighbor->fLocalGoal) {
                    nodeNeighbor->parent = nodeCurrent;
                    nodeNeighbor->fLocalGoal = fPossiblyLowerGoal;

                    nodeNeighbor->fGlobalGoal = nodeNeighbor->fLocalGoal + heuristic(nodeNeighbor, nodeEnd);
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }

        // 画线
        auto curr = nodeEnd->parent;
        // 没有结果
        if (curr == nullptr) {
            return;
        }
        while (curr != nodeStart) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            curr->type = ACTUAL_ROAD;
            curr = curr->parent;
        }
    }
};
#endif //LEARN_OPENGL_MAP_HPP
