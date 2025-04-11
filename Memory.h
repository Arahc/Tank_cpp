/*
 * @brief memory manager
 * @file Memory.h
 * used for memory management
 * use instructive linked list to manage the memory
 * = Linux list_head
 // pintOS
 * mostly copy from `Registry.h`
 */

#pragma once
#include <stddef.h>
#include <utility>

// memory control

struct memNode {
    memNode *nxt;
    memNode *pre;
    memNode() : nxt(nullptr), pre(nullptr) {}
    ~memNode() {
        nxt = nullptr;
        pre = nullptr;
    }
};

template <typename T> class memList {
  private:
    memNode _begin;
    memNode _end;

    size_t _size;

  public:
    memList() {
        _begin.pre = _end.nxt = nullptr;
        _begin.nxt = &_end;
        _end.pre = &_begin;
        _size = 0;
    }
    void memAdd(T *ptr) {
        memNode *node = static_cast<memNode *>(ptr);
        node->pre = _end.pre;
        node->nxt = &_end;
        _end.pre->nxt = node;
        _end.pre = node;
        ++_size;
    }
    void memRemove(T *ptr) {
        memNode *node = static_cast<memNode *>(ptr);
        if (node->pre && node->nxt) {
            node->pre->nxt = node->nxt;
            node->nxt->pre = node->pre;
        }
        --_size;
    }
    template <typename... Args> T *emplace(Args &&...args) {
        T *obj = new T(std::forward<Args>(args)...);
        if (obj)
            memAdd(obj);
        return obj;
    }
    void memDelete(T *obj) {
        if (!obj)
            return;
        memRemove(obj);
        delete obj;
    }

    size_t size() {
        return _size;
    }
    bool empty() {
        return _size == 0;
    }
    void clear() {
        memNode *node = _begin.nxt;
        while (node != &_end) {
            memNode *tmp = node;
            node = node->nxt;
            memDelete(static_cast<T *>(tmp));
        }
        _begin.nxt = &_end;
        _end.pre = &_begin;
        _size = 0;
    }

    // Iterator
    class Iterator {
      private:
        memNode *it;

      public:
        Iterator(memNode *_it) : it(_it) {}

        T &operator*() {
            return *static_cast<T *>(it);
        }
        T *operator->() {
            return static_cast<T *>(it);
        }
        Iterator &operator++() {
            it = it->nxt;
            return *this;
        }
        Iterator &operator--() {
            it = it->pre;
            return *this;
        }
        bool operator==(const Iterator &jt) const {
            return it == jt.it;
        }
        bool operator!=(const Iterator &jt) const {
            return it != jt.it;
        }
    };
    Iterator insert(T *ptr) {
        // push back and return the iterator
        memNode *node = static_cast<memNode *>(ptr);
        node->pre = _end.pre;
        node->nxt = &_end;
        _end.pre->nxt = node;
        _end.pre = node;
        ++_size;
        return Iterator(node);
    }
    Iterator erase(Iterator it) {
        Iterator nx = it;
        ++nx;
        memDelete(&(*it));
        // DONE not memDelete(*it), it is &(*it)
        return nx;
    }
    Iterator begin() {
        return Iterator(_begin.nxt);
    }
    Iterator end() {
        return Iterator(&_end);
    }
};

#define memRegist                                                                                                      \
  public                                                                                                               \
    memNode
