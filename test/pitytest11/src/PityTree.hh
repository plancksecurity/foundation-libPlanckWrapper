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
//            static_assert(std::is_base_of<PityTree<T>, T>::value, "PityTree<T> must be a base of T");
        public:
            using Children = std::map<const std::string, T&>;

            explicit PityTree(T& self, const std::string& name);
            explicit PityTree(T& self, const std::string& name, T& parent);

            T& add(T& node);

            T* getParent() const;
            Children getChildren() const;

            T& getRoot();

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
            const std::string _nodename;
            T& _self;
            T* _parent = nullptr; //nullptr if RootUnit
            Children _children;   // map to guarantee uniqueness of sibling-names
        };
    }; // namespace PityTest11
};     // namespace pEp

#include "PityTree.hxx"
#endif
