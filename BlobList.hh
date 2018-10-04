// C++ encapsulation of bloblist_t in pEp engine
// this file is under GNU GPL 3.0, see LICENSE.txt


#pragma once

#include <string>
#include <iterator>

#include <pEp/bloblist.h>

namespace pEp {
    namespace Common {
        using namespace std;

        class BlobList {
            public:
                struct Blob {
                    string filename;
                    string mime_type;
                    char *blob = nullptr;
                    size_t size = 0;
                    content_disposition_type cd = PEP_CONTENT_DISP_ATTACHMENT;
                };

                class Iterator : public iterator< forward_iterator_tag, Blob > {
                    friend class BlobList;

                    private:
                        ::bloblist_t *p = nullptr;
                        ::bloblist_t *_p = nullptr;
                        Blob _b;
                        BlobList *iterating = nullptr;

                    public:
                        Iterator() { }
                        Iterator(const Iterator& second) : p(second.p), iterating(second.iterating) { }
                        Iterator& operator= (const Iterator& second);
                        ~Iterator() { }
                        bool operator== (const Iterator& second) const { return p == second.p; }
                        bool operator!= (const Iterator& second) const { return p != second.p; }
                        const Blob& operator* ();
                        Iterator& operator++ ();
                        Iterator operator++ (int);
                };

                Iterator end();
                Iterator begin();

                BlobList();
                BlobList(const BlobList& second);
                BlobList& operator= (const BlobList& second);
                ~BlobList();

                void push_back(const Blob& val);
                size_t size() const { return _size; }
                bool empty() const { return _size == 0; }

                operator ::bloblist_t *() { return bl.get(); }
                operator const ::bloblist_t *() const { return bl.get(); }

            private:
                static void release_value(char *v);
                typedef unique_ptr< ::bloblist_t, decltype(&::free_bloblist) > blobdata;
                blobdata bl;
                size_t _size;
        };
    };
};

