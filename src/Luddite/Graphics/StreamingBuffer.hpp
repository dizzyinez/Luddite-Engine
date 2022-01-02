#pragma once
#include "Luddite/Core/pch.hpp"
#include "Luddite/Graphics/DiligentInclude.hpp"
#include "Graphics/GraphicsTools/interface/MapHelper.hpp"

namespace Luddite
{
template <typename T>
class StreamingBuffer
{
        public:
        StreamingBuffer(Diligent::IRenderDevice* pDevice, Diligent::BIND_FLAGS bind_flags, Diligent::BUFFER_MODE buffer_mode, uint32_t max_quantity, const char* name = "", bool always_overwrite = true)
                : m_BufferSize{max_quantity * static_cast<uint32_t>(sizeof(T))},
                m_AlwaysOverwrite{always_overwrite}
        {
                Diligent::BufferDesc BuffDesc;
                BuffDesc.Name = name;
                BuffDesc.Mode = buffer_mode;
                BuffDesc.Usage = Diligent::USAGE_DYNAMIC;
                BuffDesc.BindFlags = bind_flags;
                BuffDesc.CPUAccessFlags = Diligent::CPU_ACCESS_WRITE;
                BuffDesc.ElementByteStride = sizeof(T);
                BuffDesc.Size = max_quantity * sizeof(T);
                pDevice->CreateBuffer(BuffDesc, nullptr, &m_pBuffer);
        }
        T* Allocate(Diligent::IDeviceContext* pCtx, uint32_t quantity)
        {
                auto size = quantity * sizeof(T);
                if (m_AlwaysOverwrite)
                {
                        m_MappedData.Unmap();
                        m_MappedData.Map(pCtx, m_pBuffer, Diligent::MAP_WRITE, Diligent::MAP_FLAG_DISCARD);
                        m_CurrOffset = 0;
                }
                else
                {
                        if (m_CurrOffset + size > m_BufferSize)
                        {
                                m_MappedData.Unmap();
                                m_CurrOffset = 0;
                        }
                        if (m_MappedData == nullptr)
                                m_MappedData.Map(pCtx, m_pBuffer, Diligent::MAP_WRITE, m_CurrOffset == 0 ? Diligent::MAP_FLAG_DISCARD : Diligent::MAP_FLAG_NO_OVERWRITE);
                }
                auto offset = m_CurrOffset;
                m_CurrOffset += size;
                // return reinterpret_cast<T*>(reinterpret_cast<uint8_t*>((void*)m_MappedData) + offset);
                return reinterpret_cast<T*>((uint8_t*)m_MappedData + offset);
        }
        void Upload()
        {
                m_MappedData.Unmap();
        }
        inline Diligent::RefCntAutoPtr<Diligent::IBuffer> GetBuffer() {return m_pBuffer;}

        private:
        uint32_t m_BufferSize;
        Diligent::RefCntAutoPtr<Diligent::IBuffer> m_pBuffer;
        Diligent::MapHelper<uint8_t> m_MappedData;
        uint32_t m_CurrOffset;
        bool m_AlwaysOverwrite; //required on vulkan backend
};
}
