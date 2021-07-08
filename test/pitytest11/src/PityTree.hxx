// This file is under GNU General Public License 3.0
// see LICENSE.txt

#ifndef PITYTEST_PITYTREE_HXX
#define PITYTEST_PITYTREE_HXX

#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <sys/stat.h>
#include <functional>
#include <algorithm>
#include <sstream>
#include <exception>
#include <memory>
#include <unordered_map>
#include <sys/wait.h>
#include "PityTree.hh"


namespace pEp {
    namespace PityTest11 {

        // RootNode
        template<class T>
        PityTree<T>::PityTree(T& self, const std::string& name) :
            _self{ self }, _parent{ nullptr }, _nodename{ _normalizeName(name) }, _childrefs{},
            _childobjs{}
        {
        }

        // LeafNode
        template<class T>
        PityTree<T>::PityTree(T& self, const std::string& name, T& parent) :
            _self(self), _parent{ nullptr }, _nodename{ _normalizeName(name) }, _childrefs{},
            _childobjs{}
        {
            parent.addRef(_self);
        }

        // Copy
        template<class T>
        PityTree<T>::PityTree(const PityTree<T>& rhs, T& owner) : _self{ owner }
        {
            _nodename = rhs._nodename;
            _parent = nullptr;
            _copyChildRefs(rhs);
        }

        template<class T>
        PityTree<T>& PityTree<T>::operator=(const PityTree<T>& rhs)
        {
            _nodename = rhs._nodename;
            _parent = nullptr;
            _copyChildRefs(rhs);
            return *this;
        }

        template<typename T>
        template<typename CT, typename... Args>
        CT& PityTree<T>::addNew(Args&&... args)
        {
            static_assert(std::is_base_of<T, CT>::value, "T must be base of CT");
            std::shared_ptr<CT> tmp = std::make_shared<CT>(std::forward<Args>(args)...);
            _childobjs.push_back(tmp);
            addRef(*tmp.get());
            return *tmp.get();
        }

        template<typename T>
        template<typename CT>
        CT& PityTree<T>::addCopy(const CT&& child, const std::string& new_name)
        {
            static_assert(std::is_base_of<T, CT>::value, "PityTree<T> must be a base of T");
            CT* tmpraw = new CT(child);
            _childobjs.push_back(ChildObj(tmpraw));
            if (new_name != "") {
                tmpraw->setName(new_name);
            }
            addRef(*tmpraw);
            return *tmpraw;
        }

        template<class T>
        T& PityTree<T>::addRef(T& child)
        {
            child.setParent(&_self);
            _childrefs.insert(ChildRef(child.getName(), child));
            return child;
        }

        template<class T>
        void PityTree<T>::setParent(T* parent)
        {
            _parent = parent;
        }

        template<class T>
        T* PityTree<T>::getParent() const
        {
            return _parent;
        }

        template<class T>
        bool PityTree<T>::isRoot() const
        {
            if (_parent == nullptr) {
                return true;
            } else {
                return false;
            }
        }

        template<class T>
        void PityTree<T>::setName(const std::string& name)
        {
            _nodename = name;
        }

        template<class T>
        std::string PityTree<T>::getName() const
        {
            return _nodename;
        }

        template<class T>
        std::string PityTree<T>::getPath() const
        {
            std::string ret;

            if (!isRoot()) {
                ret = _parent->getPath() + "/" + getName();
            } else {
                ret = getName();
            }
            return ret;
        }

        template<class T>
        std::string PityTree<T>::to_string(bool recursive, int indent)
        {
            std::string ret;
            std::stringstream builder;
            builder << std::string(indent * 4, ' ');
            builder << getName();
            builder << std::endl;
            ret = builder.str();

            if (recursive) {
                if (!getChildRefs().empty()) {
                    indent++;
                    for (ChildRef child : getChildRefs()) {
                        ret += child.second.to_string(true, indent);
                    }
                    indent--;
                }
            }
            return ret;
        }

        template<class T>
        T& PityTree<T>::getRoot()
        {
            if (!isRoot()) {
                return _parent->getRoot();
            } else {
                return _self;
            }
        }

        template<class T>
        typename PityTree<T>::ChildRefs PityTree<T>::getChildRefs() const
        {
            return _childrefs;
        }

        template<class T>
        T& PityTree<T>::getChild(const std::string& name)
        {
            T* ret = nullptr;
            try {
                ret = &getChildRefs().at(name);
            } catch (const std::exception& e) {
                throw std::invalid_argument("PityNode not found: '" + name + "'");
            }
            return *ret;
        }

        // name is alphanumeric only (everything else will be replaced by an underscore)
        // static
        template<class T>
        std::string PityTree<T>::_normalizeName(std::string name)
        {
            replace_if(
                name.begin(),
                name.end(),
                [](char c) -> bool { return !isalnum(c); },
                '_');

            return name;
        }

        // When you copy a treenode, you need to create a copy of all children
        // and take ownership
        template<class T>
        void PityTree<T>::_copyChildRefs(const PityTree<T>& rhs)
        {
            for (const ChildRef& cr : rhs.getChildRefs()) {
                _childobjs.push_back(ChildObj(cr.second.clone()));
                T& ret = *_childobjs.back().get();
                addRef(ret);
            }
        }
    } // namespace PityTest11
} // namespace pEp

#endif