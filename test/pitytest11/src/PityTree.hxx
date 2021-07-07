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


namespace pEp {
    namespace PityTest11 {

        template<class T>
        PityTree<T>::PityTree(T& self, const std::string& name) :
            _self{ self }, _nodename{ _normalizeName(name) }
        {
        }

        template<class T>
        PityTree<T>::PityTree(T& self, const std::string& name, T& parent) :
            _self(self), _nodename{ _normalizeName(name) }
        {
            parent.add(_self);
        }

        template<class T>
        T& PityTree<T>::add(T& node)
        {
            node.setParent(&_self);
            _children.insert(std::pair<const std::string, T&>(node.getName(), node));
            return node;
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
                if (!getChildren().empty()) {
                    indent++;
                    for (auto child : getChildren()) {
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
        typename PityTree<T>::Children PityTree<T>::getChildren() const
        {
            return _children;
        }

        template<class T>
        T& PityTree<T>::getChild(const std::string& name)
        {
            return _children.at(name);
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
    } // namespace PityTest11
} // namespace pEp

#endif