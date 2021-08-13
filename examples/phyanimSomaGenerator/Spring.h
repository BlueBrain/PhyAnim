#ifndef __EXAMPLES_NODE__
#define __EXAMPLES_NODE__

#include <unordered_set>

namespace examples
{
class Node
{
public:
    Node(phyanim::Vec3 pos, uint64_t id, bool fixed = false)
        : position(pos)
        , velocity(phyanim::Vec3::Zero())
        , force(phyanim::Vec3::Zero())
        , id(id)
        , fixed(fixed)
        , mass(1.0){};

    ~Node(){};

    phyanim::Vec3 position;

    phyanim::Vec3 velocity;

    phyanim::Vec3 force;

    double mass;

    uint64_t id;

    bool fixed;
};

class Spring;

typedef Spring* SpringPtr;

typedef std::vector<SpringPtr> Springs;

class Spring
{
public:
    Spring(Node* node0,
           Node* node1,
           double stiffness = 100.0,
           double res = -1.0)
        : node0(node0)
        , node1(node1)
        , stiffness(stiffness)
        , resLength(res)
    {
        if (resLength < 0.0) resLength = length();
    };

    virtual ~Spring(){};

    double length() { return (node0->position - node1->position).norm(); };

    Node* node0;

    Node* node1;

    double stiffness;

    double resLength;

    phyanim::Vec3 force;
};

struct SpringHash
{
public:
    size_t operator()(const Spring* spring) const
    {
        uint64_t id0 = spring->node0->id;
        uint64_t id1 = spring->node1->id;
        if (id1 > id0) std::swap(id1, id0);
        return std::hash<unsigned int>{}(id0) ^ std::hash<unsigned int>{}(id1);
    };
};

struct SpringEqual
{
public:
    bool operator()(const Spring* spring0, const Spring* spring1) const
    {
        uint64_t id00 = spring0->node0->id;
        uint64_t id01 = spring0->node1->id;
        uint64_t id10 = spring1->node0->id;
        uint64_t id11 = spring1->node1->id;
        if (id01 > id00) std::swap(id00, id01);
        if (id11 > id10) std::swap(id10, id11);
        return (id00 == id10) && (id01 == id11);
    };
};

typedef std::unordered_set<Spring*, SpringHash, SpringEqual> UniqueSprings;

class Triangle
{
public:
    Triangle(Node* node0, Node* node1, Node* node2)
        : node0(node0)
        , node1(node1)
        , node2(node2){};

    ~Triangle();

    Node* node0;

    Node* node1;

    Node* node2;
};

}  // namespace examples

#endif
