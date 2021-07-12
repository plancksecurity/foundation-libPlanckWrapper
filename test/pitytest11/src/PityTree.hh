// This file is under GNU General Public License 3.0
// see LICENSE.txt

#ifndef PITYTEST_PITYTREE_HH
#define PITYTEST_PITYTREE_HH

//#include "../../../src/pEpLog.hh"
#include <string>
#include <map>
#include <memory>
#include <functional>
#include <type_traits>
#include <vector>

namespace pEp {
    namespace PityTest11 {
        template<class T>
        class PityTree {
            // TODO: NEEEEED THIS
            // static_assert(std::is_base_of<PityTree<T>, T>::value, "PityTree<T> must be a base of T");
        public:
            using ChildObj = std::shared_ptr<T>;
            using ChildObjs = std::vector<ChildObj>;
            using ChildRef = std::pair<const std::string, T&>;
            using ChildRefs = std::map<const std::string, T&>;

            // Constructors
            explicit PityTree(T& self, const std::string& name);
            explicit PityTree(T& self, const std::string& name, T& parent);
            explicit PityTree(const PityTree& rhs, T& owner);

            // copy-assign
//            PityTree& operator=(const PityTree<T>& rhs);

            // clone
            virtual PityTree* clone() = 0;

            // Append
            // creates a new instance of CT, add the new instance as child and returns a ref to it
            template<typename CT, typename... Args>
            CT& addNew(Args&&... args);

            // Creates a copy, adds the copy as child and returns a ref to it
            template<typename CT>
            CT& addCopy(const CT&& child, const std::string& new_name = "");

            template<typename CT>
            CT& addCopy(const CT& child, const std::string& new_name = "");

            // Just adds child as a non-owned reference.
            T& addRef(T& child);

            // Query
            virtual T& getSelf() = 0;
            T* getParent() const;
            ChildRefs getChildRefs() const;
            int getChildCount() const;
            T& getChild(const std::string& name);
            T& getRoot();

            void setName(const std::string& name);
            std::string getName() const;
            std::string getPath() const;
            bool isRoot() const; // true if has no parent

            std::string to_string(bool recursive = true, int indent = 0);

            //TODO HACK in PityTransport, this should be private
            static std::string _normalizeName(std::string name);

            virtual ~PityTree() = default;

        protected:
            void setParent(T* const parent);

        private:
            void _copyChildRefs(const PityTree<T>& rhs);
            // Fields
            std::string _nodename;
            T& _self;
            T* _parent;           //nullptr if RootUnit
            ChildRefs _childrefs; // map to guarantee uniqueness of sibling-names
            ChildObjs _childobjs;
        };
    }; // namespace PityTest11
};     // namespace pEp

#include "PityTree.hxx"
#endif
