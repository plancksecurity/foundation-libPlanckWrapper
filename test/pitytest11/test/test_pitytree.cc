#include "../src/PityTree.hh"
#include "../src/PityTest.hh"
//#include "../../../src/utils.hh"
#include <iostream>
#include <exception>
using namespace pEp::PityTest11;

class Node : public PityTree<Node> {
public:
    Node() = delete;
    Node(const std::string &name);
    Node(const std::string &name, Node &parent);

    std::string color;
    int age;
};

Node::Node(const std::string &name) : PityTree<Node>(*this, name) {}

Node::Node(const std::string &name, Node &parent) : PityTree<Node>(*this, name, parent) {}

std::string readKey()
{
    std::string ret;
    std::cin >> ret;
    return ret;
}

void not_throwing(){
    throw std::runtime_error{"Fsd"};
}

void throwing(){
    throw std::runtime_error{"Fsd"};
}

int main(int argc, char *argv[])
{
    Node a{ "a" };
    std::cout << a.getPath() << std::endl;
    PITYASSERT(a.isRoot() == true, "");
    PITYASSERT(a.getName() == "a","");
    PITYASSERT(&(a.getRoot()) == &a,"");
    PITYASSERT(a.getParent() == nullptr,"");
    PITYASSERT(a.getChildren().size() == 0,"");

    Node b{ "b", a };
    std::cout << b.getPath() << std::endl;
    PITYASSERT(a.isRoot() == true,"");
    PITYASSERT(&(a.getRoot()) == &a,"");
    PITYASSERT(a.getParent() == nullptr,"");
    PITYASSERT(a.getChildren().size() == 1,"");
    PITYASSERT(&b == &(a.getChild("b")),"");
    PITYASSERT_THROWS(a.getChild("invalid"),"");

    PITYASSERT(b.isRoot() == false,"");
    PITYASSERT(&(b.getRoot()) == &a,"");
    PITYASSERT(b.getParent() == &a,"");
    PITYASSERT(b.getChildren().size() == 0,"");

    std::cout << a.to_string() << std::endl;
}
