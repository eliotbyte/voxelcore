#pragma once

#include <memory>
#include <mutex>
#include <vector>
#include <queue>

namespace util {
    struct AlignedDeleter {
        void operator()(void* p) const {
            std::free(p);
        }
    };

    template <class T>
    class ObjectsPool {
    public:
        ObjectsPool(size_t preallocated = 0) {
            for (size_t i = 0; i < preallocated; i++) {
                allocateNew();
            }
        }

        template<typename... Args>
        std::shared_ptr<T> create(Args&&... args) {
            std::lock_guard lock(mutex);
            if (freeObjects.empty()) {
                allocateNew();
            }
            auto ptr = freeObjects.front();
            freeObjects.pop();
            new (ptr)T(std::forward<Args>(args)...);
            return std::shared_ptr<T>(reinterpret_cast<T*>(ptr), [this](T* ptr) {
                std::lock_guard lock(mutex);
                freeObjects.push(ptr);
            });
        }
    private:
        std::vector<std::unique_ptr<void, AlignedDeleter>> objects;
        std::queue<void*> freeObjects;
        std::mutex mutex;

        void allocateNew() {
            std::unique_ptr<void, AlignedDeleter> ptr(
                std::aligned_alloc(alignof(T), sizeof(T))
            );
            freeObjects.push(ptr.get());
            objects.push_back(std::move(ptr));
        }
    };
}
