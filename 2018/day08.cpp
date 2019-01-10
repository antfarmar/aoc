// Advent of Code 2018
// Day 8: Memory Maneuver
// https://adventofcode.com/2018/day/8

#include <algorithm>
#include <iostream>
#include <numeric>
#include <vector>

// node in a tree
struct Node {
    std::vector<Node> children;
    std::vector<int> metadata;
};

// Recursively parse the input tree nodes starting at the root
std::istream& operator>>(std::istream& is, Node& node) {
    int childCnt, metadataCnt;
    is >> childCnt >> metadataCnt;
    node.children.resize(childCnt);
    node.metadata.resize(metadataCnt);
    for (Node& child : node.children)
        is >> child;
    for (int& data : node.metadata)
        is >> data;
    return is;
}

// template <typename T>
// int sum(const std::vector<T>& v, int init) {
//     return std::accumulate(v.cbegin(), v.cend(), init);
// }

// Part 1
// What is the sum of all metadata entries?
// Your puzzle answer was 43825
//
// Recursively traverse the tree and sum each node's metadata
int sumMetadata(const Node& node) {
    return std::accumulate(
        node.children.cbegin(), node.children.cend(),
        std::accumulate(node.metadata.cbegin(), node.metadata.cend(), 0),
        [](const int& sum, const Node& c) { return sum + sumMetadata(c); });
}

// Part 2
// What is the value of the root node?
// Your puzzle answer was 19276
//
// Recursively traverse the tree, sum each node's metadata + children
int rootValue(const Node& node) {
    int rootSum = 0;
    if (node.children.empty())
        return sumMetadata(node);
    for (const int& idx : node.metadata)  // metadata = index to child
        if (size_t(idx - 1) < node.children.size())
            rootSum += rootValue(node.children[idx - 1]);
    return rootSum;
}

int main() {
    Node treeRootNode;
    std::cin >> treeRootNode;

    // Part 1: What is the sum of all metadata entries?  // 138 // 43825
    std::cout << "Part 1: Metadata Sum = " << sumMetadata(treeRootNode) << "\n";

    // Part 2: What is the value of the root node? // 66 // 19276
    std::cout << "Part 2: Root Value   = " << rootValue(treeRootNode) << "\n";
}