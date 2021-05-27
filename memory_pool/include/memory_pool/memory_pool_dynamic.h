#pragma once

#include <vector>
#include <memory>
#include <utility>
#include <list>
#include <array>
#include <stdint.h>
#include <typeindex>
#include <unordered_map>
#include <functional>

#include "memory_pool/version.h"
#include "memory_pool/memory_pool.h"

namespace memory_pool {
    //*/
    template<
        typename idx_t = int,
        typename size_t = size_t
    >
    class MemoryPoolDynamic //: public MemoryPool<sizeof(element_t), chunck_size_t, max_chunks_t, alignment_t, idx_t, size_t>
    {
    public:
        using idx_type = idx_t;
        using size_type = size_t;
        using memory_pool_type = memory_pool::MemoryPool<idx_type, size_type>;
        using chunk_type = typename memory_pool_type::chunk_type;
        /*MemoryPoolDynamic()
            : MemoryPoolDynamic(1024 * 1024 * 5, 16)
        {}*/
        MemoryPoolDynamic(size_type chunkSizeBytes = 1024 * 1024 * 5, size_type alignment=16)
            : m_chunkSizeBytes(chunkSizeBytes)
            , m_alignment(alignment)
        {}

        template<typename element_type, typename... Args>
        element_type* create(Args&&... args)
        {
            auto& pool = getOrCreatePool<element_type>();
            return pool.pool.create<element_type>(std::forward< Args >(args)...);
        }

        template<typename element_type>
        bool destroy(element_type* ptr)
        {
            return getOrCreatePool<element_type>().pool.destroy<element_type>(ptr) || destroy(static_cast<void*>(ptr));
        }

        bool destroy(void* ptr)
        {
            std::type_index poolIdx = typeid(int);
            idx_type chunkIdx;
            idx_type idx;
            if (find(ptr, poolIdx, chunkIdx, idx))
            {
                m_pools.at(poolIdx).destructor(ptr);
                return m_pools.at(poolIdx).pool.deallocate(ptr);
            }
            return false;
        }

        template<typename element_type>
        void destruct(element_type* ptr)
        {
            if (!getOrCreatePool<element_type>().pool.contains(ptr))
                return destruct(static_cast<void*>(ptr));

            ptr->~element_type();
            return true;
        }

        bool destruct(void* ptr)
        {
            std::type_index poolIdx = typeid(int); // no standard constructor =(
            idx_type chunkIdx;
            idx_type idx;
            if (find(ptr, poolIdx, chunkIdx, idx))
            {
                m_pools.at(poolIdx).destructor(ptr);
                return true;
            }
            return false;
        }

        template<typename element_type>
        bool allocate(element_type*& ptr)
        {
            auto& pool = getOrCreatePool<element_type>();
            return pool.pool.allocate<element_type>(ptr);
        }
        
        template<typename element_type>
        bool deallocate(element_type* ptr)
        {
            auto& pool = getOrCreatePool<element_type>();
            return pool.pool.deallocate(ptr);
        }

        bool deallocate(void* ptr)
        {
            std::type_index poolIdx;
            idx_type chunkIdx;
            idx_type idx;
            return (
                find(ptr, poolIdx, chunkIdx, idx)
                && m_pools.at(poolIdx).pool.deallocate(ptr);
            );
        }

        bool find(const void* ptr, std::type_index& poolIdx, idx_type& chunkIdx, idx_type& idx) const
        {
            for(auto it = m_pools.begin(); it != m_pools.end(); ++it)
            {
                auto& pool = it->second;
                if (pool.pool.find(ptr, chunkIdx, idx))
                {
                    poolIdx = it->first;
                    return true;
                }
            }
            return false;
        }

        template<typename element_type = void>
        element_type* getItem(const std::type_index& poolIdx, idx_type chunkIdx, idx_type idx) const
        {
            return m_pools.at(poolIdx).getItem<element_type>(chunkIdx, idx);
        }

        template<typename element_type>
        inline bool contains(const void* ptr) const
        {
            return getOrCreatePool<element_type>().pool.contains(ptr);
        }

        inline bool contains(const void* ptr) const
        {
            for(auto it = m_pools.begin(); it != m_pools.end(); ++it)
                if (it->second.pool.contains(ptr))
                    return true;
            return false;
        }

        ~MemoryPoolDynamic()
        {
            // call type destructors for all allocated items
            for(auto it = m_pools.begin(); it != m_pools.end(); ++it)
            {
                auto& type_index = it->first;
                auto& pool = it->second;
                for (size_type k=0; k < pool.pool.countChunks(); ++k)
                {
                    auto& chunk = pool.pool.getChunk(k);
                    for (size_type i=0; i < chunk.chunkSize(); ++i)
                    {
                        if (chunk.isAllocated(i))
                        {
                            pool.destructor(chunk.at(i));
                        }
                    }
                }
            }
        }

    protected:
        using DestructorCallable = std::function<void(void*)>;

        template<typename element_type>
        static DestructorCallable destructType()
        {
            return [] (void* ptr) {
                static_cast<element_type*>(ptr)->~element_type();
            };
        }

        struct MemoryPoolWithDestructor
        {
            template<typename... Args>
            MemoryPoolWithDestructor(DestructorCallable destructor, Args&&... args)
                : pool(std::forward< Args >(args)...)
                , destructor(destructor)
            {}
            memory_pool_type pool;
            DestructorCallable destructor;
        };


        template<typename element_type>
        MemoryPoolWithDestructor& getOrCreatePool()
        {
            std::type_index type_index(typeid(element_type));
            if (!m_pools.count(type_index))
            {
                size_type elementSize = sizeof(element_type);
                size_type chunkSize = m_chunkSizeBytes / chunk_type::SlotSize(elementSize, m_alignment);
                m_pools.emplace(
                    std::piecewise_construct,
                    std::forward_as_tuple(type_index),
                    std::forward_as_tuple(destructType<element_type>(), chunkSize, elementSize, m_alignment)
                );

            }
            return m_pools.at(type_index);
        }

        std::unordered_map<std::type_index, MemoryPoolWithDestructor> m_pools;
        const size_type m_chunkSizeBytes;
        const size_type m_alignment;
    };


} // namespace memory_pool
