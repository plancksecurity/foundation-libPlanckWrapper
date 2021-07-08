// This file is under GNU General Public License 3.0
// see LICENSE.txt

#ifndef PITYTEST_PITYTREE_HH
#define PITYTEST_PITYTREE_HH

//#include "../../../src/pEpLog.hh"
#include <string>
#include <map>
#include <memory>
#include <unordered_map>
#include <functional>
#include <type_traits>

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

            virtual PityTree* clone() = 0;

            // Append
            T& addRef(T& node);

            template<typename... Args>
            T& addNew(Args&&... args);

            template<typename CT>
            T& addCopy(const CT&& t, const std::string& new_name = "");

            // Query
            T* getParent() const;
            ChildRefs getChildRefs() const;
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
