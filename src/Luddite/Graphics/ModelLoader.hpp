#pragma once
#include "Luddite/pch.hpp"
#include "Luddite/Core.hpp"
#include "Luddite/BasicAllocator.hpp"

#include "Luddite/Graphics/Model.hpp"

namespace Luddite
{
class LUDDITE_API ModelLoader
{
public:
        std::shared_ptr<BasicModel> GetBasicModel(const std::string& filepath);
private:
        class BasicModelAllocator : public BasicAllocator<BasicModel>
        {
public:
                std::shared_ptr<BasicModel> Allocate(const std::string& filepath) override;
private:
                void LoadOBJ(std::shared_ptr<BasicModel> model, std::string filepath);
        };
        BasicModelAllocator m_ModelAllocator;
};
}