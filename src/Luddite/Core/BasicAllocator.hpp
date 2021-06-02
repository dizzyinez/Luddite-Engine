#pragma once
#include "Luddite/Core/pch.hpp"
#include "Luddite/Core/Core.hpp"
#include "Luddite/Core/Logging.hpp"

namespace Luddite
{
template <typename T, typename ID = std::string>
class BasicAllocator
{
public:
        std::shared_ptr<T> Get(ID id)
        {
                auto it = resources.find(id);
                if (it != resources.end())
                {
                        if (!it->second.expired())
                        {
                                // std::cout << "resource already loaded at path: " << id << std::endl;
                                return std::shared_ptr<T>(it->second);
                        }
                        else
                        {
                                LD_LOG_INFO("Resouce {} expired, reloading", id);
                                std::cout << "resource expired: " << id << std::endl;
                                std::shared_ptr<T> resource = Allocate(id);
                                it->second = resource;
                                return resource;
                        }
                }
                else
                {
                        std::shared_ptr<T> resource = Allocate(id);
                        resources.insert(std::make_pair(id, resource));
                        return resource;
                }
        }
        bool Has(ID id)
        {
                auto it = resources.find(id);
                if (it != resources.end())
                {
                        if (!it->second.expired())
                        {
                                return true;
                        }
                }
                return false;
        }

        std::shared_ptr<T> Add(ID id, std::shared_ptr<T> resource)
        {
                auto it = resources.find(id);
                if (it != resources.end())
                {
                        if (it->second.expired())
                        {
                                it->second = resource;
                                return resource;
                        }
                        LD_LOG_WARN("Attempted to add {} to allocator, but it already exists", id);
                        return std::shared_ptr<T>(it->second);
                }
                else
                {
                        resources.insert(std::make_pair(id, resource));
                        return resource;
                }
        }

        bool AddFakeUser(ID id)
        {
                auto it = fake_users.find(id);
                if (it != fake_users.end())
                {
                        return false;
                }
                else
                {
                        fake_users.insert(std::make_pair(id, Get(id)));
                        return true;
                }
        }

        bool AddFakeUser(ID id, std::shared_ptr<T> resource)
        {
                auto it = fake_users.find(id);
                if (it != fake_users.end())
                {
                        return false;
                }
                else
                {
                        fake_users.insert(std::make_pair(id, resource));
                        return true;
                }
        }

        bool RemoveFakeUser(ID id)
        {
                auto it = fake_users.find(id);
                if (it != fake_users.end())
                {
                        fake_users.erase(it);
                        return true;
                }
                else
                {
                        return false;
                }
        }

protected:
        virtual std::shared_ptr<T> Allocate(const ID& id) = 0;
// {
//         return std::make_shared<T>();
// }

        std::unordered_map<ID, std::shared_ptr<T> > fake_users;
        std::unordered_map<ID, std::weak_ptr<T> > resources;
};
}