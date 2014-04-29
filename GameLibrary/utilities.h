#ifndef _GFW_UTILITIES_H
#define _GFW_UTILITIES_H

namespace gfw {

template <class T>
void safeDelete(T*& ptr) {
    if (ptr != nullptr) {
        delete ptr;
        ptr = nullptr;
    }
}

template <class T>
void safeDeleteArray(T*& ptr) {
    if (ptr != nullptr) {
        delete[] ptr;
        ptr = nullptr;
    }
}

template <class T>
void safeRelease(T*& ptr) {
    if (ptr != nullptr) {
        ptr->Release();
        ptr = nullptr;
    }
}

} // namespace gfw

#endif