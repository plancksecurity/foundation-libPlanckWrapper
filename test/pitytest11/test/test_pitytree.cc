#include "../src/PityTree.hh"
#include "../src/PityTest.hh"
#include "../../../src/std_utils.hh"
#include "../../../src/utils.hh"
#include <iostream>
#include <exception>
using namespace pEp;
using namespace pEp::PityTest11;

// -----------------------------------------------------------------------------------------------

class AbstractNode : public PityTree<AbstractNode> {
public:
    // Constructors
    AbstractNode() = delete;
    explicit AbstractNode(const std::string &name);
    explicit AbstractNode(const std::string &name, AbstractNode &parent);
    AbstractNode(const AbstractNode &rhs, AbstractNode &self);

    // methods
    virtual int implMe(int magic_nr) = 0;
    AbstractNode *clone() override = 0;

    // fields
    std::string color;
    int age{};
};

AbstractNode::AbstractNode(const std::string &name) : PityTree<AbstractNode>(*this, name) {}

AbstractNode::AbstractNode(const std::string &name, AbstractNode &parent) :
    PityTree<AbstractNode>(*this, name, parent)
{
}

AbstractNode::AbstractNode(const AbstractNode &rhs, AbstractNode &self) :
    PityTree<AbstractNode>(rhs, self)
{
    color = rhs.color;
    age = rhs.age;
}


// -----------------------------------------------------------------------------------------------

class ANode : public AbstractNode {
public:
    explicit ANode(const std::string &name);
    explicit ANode(const std::string &name, AbstractNode &parent);
    ANode(const ANode &rhs);
    ANode *clone() override;
    int implMe(int magic_nr) override;
};

ANode::ANode(const std::string &name) : AbstractNode(name) {}
ANode::ANode(const std::string &name, AbstractNode &parent) : AbstractNode(name, parent) {}
ANode::ANode(const ANode &rhs) : AbstractNode(rhs, *this) {}

int ANode::implMe(int magic_nr)
{
    return 23;
}

ANode *ANode::clone()
{
    return new ANode(*this);
}

// -----------------------------------------------------------------------------------------------

class BNode : public AbstractNode {
public:
    explicit BNode(const std::string &name);
    explicit BNode(const std::string &name, AbstractNode &parent);
    BNode(const BNode &rhs);
    BNode *clone() override;
    int implMe(int magic_nr) override;
};

BNode::BNode(const std::string &name) : AbstractNode(name) {}
BNode::BNode(const std::string &name, AbstractNode &parent) : AbstractNode(name, parent) {}
BNode::BNode(const BNode &rhs) : AbstractNode(rhs, *this) {}

int BNode::implMe(int magic_nr)
{
    return 42;
}

BNode *BNode::clone()
{
    return new BNode(*this);
}

// -----------------------------------------------------------------------------------------------

void not_throwing()
{
    throw std::runtime_error{ "Fsd" };
}

void throwing()
{
    throw std::runtime_error{ "Fsd" };
}

int main(int argc, char *argv[])
{
    // Create lone node
    ANode a{ "a" };
    std::cout << a.getPath() << std::endl;
    PITYASSERT(a.isRoot() == true, "a");
    PITYASSERT(a.getName() == "a", "b");
    PITYASSERT(&(a.getRoot()) == &a, "c");
    PITYASSERT(a.getParent() == nullptr, "d");
    PITYASSERT(a.getChildRefs().size() == 0, "e");

    // Create node here, and make it a child of another node
    BNode b{ "b", a };
    std::cout << b.getPath() << std::endl;
    PITYASSERT(a.isRoot() == true, "f");
    PITYASSERT(&(a.getRoot()) == &a, "g");
    PITYASSERT(a.getParent() == nullptr, "1");
    PITYASSERT(a.getChildRefs().size() == 1, "2");

    PITYASSERT(&b == &(a.getChild("b")), "3");
    PITYASSERT_THROWS(a.getChild("invalid"), "4");
    PITYASSERT(b.isRoot() == false, "5");
    PITYASSERT(&(b.getRoot()) == &a, "6");
    PITYASSERT(b.getParent() == &a, "7");
    PITYASSERT(b.getChildRefs().size() == 0, "8");

    // Create a copy of the node in the parent node
    b.addNew<ANode>("c").addNew<ANode>("d");
    std::cout << a.to_string() << std::endl;

    b.addNew<BNode>("c1").addNew<BNode>("e");
    std::cout << a.to_string() << std::endl;

    b.getChild("c1").getChild("e").addCopy(ANode(a), "a1");
    std::cout << a.to_string() << std::endl;

    a.getChild("b").addCopy(ANode(a), a.getName() + "1");
    std::cout << a.to_string() << std::endl;
}
