#ifndef COLLIDER
#define COLLIDER

#include <iostream>
#include "vector2.hpp"
#include <vector>

struct Collider
{
    std::vector<vector2> top;
    std::vector<vector2> right;
    std::vector<vector2> bottom;
    std::vector<vector2> left;

    void Build(float w, float h)
    {
        top.clear();
        top.push_back({-w / 2, -h / 2});
        top.push_back({w / 2, -h / 2});

        bottom.clear();
        bottom.push_back({-w / 2, h / 2});
        bottom.push_back({w / 2, h / 2});

        right.clear();
        right.push_back({w / 2, -h / 2});
        right.push_back({w / 2, h / 2});
        right.push_back({w / 2, 0});
        
        left.clear();
        left.push_back({-w / 2, -h / 2});
        left.push_back({-w / 2, h / 2});
        left.push_back({-w / 2, 0});

    }

};

#endif