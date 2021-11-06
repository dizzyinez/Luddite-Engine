#pragma once
#include "Luddite/Core/pch.hpp"

namespace Luddite
{
template <typename T>
class Handle;
template <typename T>
class LUDDITE_API AssetRefCounter
{
        private:
        std::atomic<unsigned int> m_ReferenceCount{0};
        std::atomic<unsigned int> m_WeakReferenceCount{0};
        std::atomic<unsigned int> m_Version{0};
        std::unordered_map<void*, std::function<void(void*, T*, T*)> > m_ReloadFunctions{};
        bool stay_loaded = false;
        void IncrementReferences() {m_ReferenceCount++;}
        void DecrementReferences() {m_ReferenceCount--;}
        T* m_pAsset = nullptr;
        friend class Handle<T>;

        protected:
        //AssetRefCounter(const AssetRefCounter& other) = delete;

        public:
        AssetRefCounter() {};
        AssetRefCounter(T* asset) : m_pAsset(asset) {}
        AssetRefCounter& operator=(const AssetRefCounter& other) = default;
        std::mutex m_Mutex{};

        void SetReloadFunction(void* key, std::function<void(void*, T*, T*)> function)
        {
                std::lock_guard lock{m_Mutex};
                m_ReloadFunctions[key] = function;
        }

        void RemoveReloadFunction(void* key)
        {
                std::lock_guard lock{m_Mutex};
                m_ReloadFunctions.erase(key);
        }

        void ReplaceData(T* other)
        {
                std::lock_guard lock{m_Mutex};
                for (auto& pair : m_ReloadFunctions)
                        pair.second(pair.first, m_pAsset, other);
                if (m_pAsset)
                        delete m_pAsset;
                m_pAsset = other;
                m_Version++;
        }

        T* get() {return m_pAsset;}
        const T* get() const {return m_pAsset;}

        void set(T* p)
        {
                if (m_pAsset)
                        delete m_pAsset;
                m_pAsset = p;
        }

        void release()
        {
                if (m_pAsset)
                        delete m_pAsset;
                m_pAsset = nullptr;
        }

        bool valid() const {return m_pAsset != nullptr;}

        unsigned int GetVersion() const {return m_Version;}

        const bool StayLoaded() const {return stay_loaded;}
        const int GetReferenceCount() const {return m_ReferenceCount;}
        ~AssetRefCounter()
        {
                if (m_pAsset)
                        delete m_pAsset;
        }
};

template <class T>
// typename std::enable_if<std::is_base_of<AssetRefCounter<T>, T>::value>::type
class Handle
{
        private:
        AssetRefCounter<T>* m_pAssetRefCounter = nullptr;
        unsigned int m_Version = 0;
        public:
        Handle() : m_pAssetRefCounter() {};
        Handle(const Handle& other)
        {
                if (m_pAssetRefCounter)
                        m_pAssetRefCounter->DecrementReferences();
                m_pAssetRefCounter = other.getCounter();
                if (m_pAssetRefCounter)
                        m_pAssetRefCounter->IncrementReferences();
                m_Version = other.m_Version;
        }
        Handle(AssetRefCounter<T>* p)
                : m_pAssetRefCounter(p)
        {
                if (m_pAssetRefCounter)
                {
                        m_Version = p->m_Version;
                        m_pAssetRefCounter->IncrementReferences();
                }
        }
        ~Handle()
        {
                if (m_pAssetRefCounter)
                        m_pAssetRefCounter->DecrementReferences();
        }

        void CreateFakeUser() {m_pAssetRefCounter->stay_loaded = true;}
        void RemoveFakeUser() {m_pAssetRefCounter->stay_loaded = false;}

        T* operator->() const
        {
                return m_pAssetRefCounter->get();
        }

        T* get() const {return m_pAssetRefCounter->get();}
        AssetRefCounter<T>* getCounter() const {return m_pAssetRefCounter;}

        Handle& operator=(const Handle& other)
        {
                if (m_pAssetRefCounter)
                {
                        if (m_pAssetRefCounter->get() != other.m_pAssetRefCounter->get())
                        {
                                if (m_pAssetRefCounter)
                                        m_pAssetRefCounter->DecrementReferences();
                                m_pAssetRefCounter = other.getCounter();
                                if (m_pAssetRefCounter)
                                        m_pAssetRefCounter->IncrementReferences();
                        }
                        else
                                m_pAssetRefCounter = other.getCounter();
                }
                else
                        m_pAssetRefCounter = other.getCounter();
                m_Version = other.m_Version;
                return *this;
        }
        Handle& operator=(T* asset)
        {
                if (m_pAssetRefCounter != asset)
                {
                        if (m_pAssetRefCounter)
                                m_pAssetRefCounter->DecrementReferences();
                        m_pAssetRefCounter = asset;
                        if (m_pAssetRefCounter)
                                m_pAssetRefCounter->IncrementReferences();
                }
                m_Version = asset->m_Version;
                return *this;
        }
        inline bool valid() const {return m_pAssetRefCounter && m_pAssetRefCounter->valid();}
        inline bool operator==(const Handle& other) const {return m_pAssetRefCounter == other.m_pAssetRefCounter && m_Version == other.m_Version;}
        inline bool operator!=(const Handle& other) const {return m_pAssetRefCounter != other.m_pAssetRefCounter && m_Version != other.m_Version;}
};
}
namespace std
{
template<typename T>
struct hash<Luddite::Handle<T> >
{
        std::size_t operator()(const Luddite::Handle<T>& handle) const
        {
                return hash<T*>()(handle.get());
        }
};
}
