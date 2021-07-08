#include "Luddite/Core/pch.hpp"

namespace Luddite
{
template <typename T>
class LUDDITE_API Asset
{
        private:
        unsigned int m_ReferenceCount = 0;
        void IncrementReferences() {m_ReferenceCount++;}
        void DecrementReferences() {m_ReferenceCount--;}
        bool stay_loaded = false;
        // friend class Asset<T>::Handle;

        protected:
        Asset(const Asset& other) {}
        Asset operator=(const Asset& other) {return *this;}

        Asset() {}

        public:
        class Handle
        {
                private:
                T* m_pAsset = nullptr;
                public:
                Handle() : m_pAsset(nullptr) {};
                Handle(const Handle& other)
                {
                        if (m_pAsset)
                                m_pAsset->DecrementReferences();
                        m_pAsset = other.operator->();
                        m_pAsset->IncrementReferences();
                }
                Handle(T* p)
                        : m_pAsset(p)
                {
                        m_pAsset->IncrementReferences();
                }
                ~Handle()
                {
                        m_pAsset->DecrementReferences();
                }

                void CreateFakeUser() {m_pAsset.stay_loaded = true;}
                void RemoveFakeUser() {m_pAsset.stay_loaded = false;}

                T* operator->() const
                {
                        return m_pAsset;
                }
        };

        void ReplaceData(const T& other)
        {
                int refcount = m_ReferenceCount;
                static_cast<T*>(this)->operator=(other);
                m_ReferenceCount = refcount;
        }

        const bool StayLoaded() const {return stay_loaded;}
        const int GetReferenceCount() const {return m_ReferenceCount;}
        virtual ~Asset() {}
};
}