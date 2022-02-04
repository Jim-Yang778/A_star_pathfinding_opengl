#ifndef LEARN_OPENGL_MAP_HPP
#define LEARN_OPENGL_MAP_HPP

#include <iostream>
#include <memory>
#include <vector>
#include <random>

const size_t WIDTH = 15;
const size_t LENGTH = 15;

struct node{
    bool reachable = true;
    std::shared_ptr<node> parent = nullptr;
    node() = default;
    explicit node(bool reach) : reachable(reach) {};
};
typedef std::shared_ptr<node> Node;

class Map {
public:
    Map(Map &other) = delete;
    void operator=(const Map&) = delete;
    static Map& getInstance() {
        static Map map_;
        return map_;
    }
    std::vector<std::vector<Node>> &data() {
        return data_;
    }
private:
    Map() {
        data_ = std::vector<std::vector<Node>>(LENGTH, std::vector<Node>());
        std::random_device dev;
        std::default_random_engine engine(dev());
        std::uniform_int_distribution<int> random_unreachable(0, 4);
        for (auto &line : data_) {
            for (int i = 0; i < WIDTH; ++i) {
                int tmp = random_unreachable(engine);
                if (!tmp) {
                    line.emplace_back(std::make_shared<node>(false));
                } else {
                    line.emplace_back(std::make_shared<node>(true));
                }
            }
        }
        for (auto &line : data_) {
            for (auto &each : line) {
                if (each->reachable) {
                    std::cout << "1 ";
                } else {
                    std::cout << "0 ";
                }
            }
            std::cout << "\n";
        }
    }
    std::vector<std::vector<Node>> data_{};
};
#endif //LEARN_OPENGL_MAP_HPP
