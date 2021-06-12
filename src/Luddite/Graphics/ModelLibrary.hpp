#pragma once
#include "Luddite/Core/pch.hpp"
#include "Luddite/Core/Core.hpp"
#include "Luddite/Core/BasicAllocator.hpp"

#include "Luddite/Graphics/Model.hpp"

namespace Luddite
{
class LUDDITE_API ModelLoader
{
        public:
        static std::shared_ptr<BasicModel> GetBasicModel(const std::string& filepath);
        private:
        class BasicModelAllocator : public BasicAllocator<BasicModel>
        {
                public:
                std::shared_ptr<BasicModel> Allocate(const std::string& filepath) override;
                private:
                void CreateBuffers(std::shared_ptr<BasicModel> model);
                void LoadOBJ(std::shared_ptr<BasicModel> model, std::string filepath);
        };
        static inline BasicModelAllocator m_ModelAllocator;
};
}