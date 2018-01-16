#include "BlobList.hh"

namespace pEp {
    namespace Common {
        BlobList::Iterator& BlobList::Iterator::operator= (const BlobList::Iterator& second)
        {
            p = second.p;
            return *this;
        }

        const BlobList::Blob& BlobList::Iterator::operator* ()
        {
            if (!p)
                throw out_of_range("BlobList::Iterator points to nowhere");

            if (_p != p) {
                _b = { p->filename, p->mime_type, p->value, p->size, p->disposition };
                _p = p;
            }
            return _b;
        }

        BlobList::Iterator& BlobList::Iterator::operator++ ()
        {
            if (!p)
                throw out_of_range("BlobList::Iterator points to nowhere");

            p = p->next;
            return *this;
        }

        BlobList::Iterator BlobList::Iterator::operator++ (int)
        {
            Iterator second = *this;
            ++(*this);
            return second;
        }

        BlobList::Iterator BlobList::end()
        {
            return BlobList::Iterator();
        }

        BlobList::Iterator BlobList::begin()
        {
            BlobList::Iterator _begin;
            _begin.iterating = this;
            _begin.p = this->bl.get();
            return _begin;
        }

        BlobList::BlobList()
            : bl(::new_bloblist(nullptr, 0, nullptr, nullptr), &::free_bloblist),
                _size(0)
        {
            if (!bl.get())
                throw bad_alloc();
            bl->release_value = release_value;
        }

        BlobList::BlobList(const BlobList& second)
            : bl(::bloblist_dup(second.bl.get()), &::free_bloblist),
                _size(second._size)
        {
            if (!bl.get())
                throw bad_alloc();
        }

        BlobList& BlobList::operator= (const BlobList& second)
        {
            bl = blobdata(::bloblist_dup(second.bl.get()), &::free_bloblist);
            return *this;
        }

        void BlobList::release_value(char *v)
        {
            delete[] v;
        }

        void BlobList::push_back(const Blob& val)
        {
            ::bloblist_add(bl.get(), val.blob, val.size, val.mime_type.c_str(), val.filename.c_str());
            ++_size;
        }
    };
};

