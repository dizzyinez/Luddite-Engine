#pragma once
#include "Luddite/Core/pch.hpp"
#include "Luddite/Core/Asset.hpp"
#include "yaml-cpp/yaml.h"

namespace Luddite
{
typedef uint64_t AssetID;
template <typename T, typename Subclass>
struct LUDDITE_API AssetLibrary
{
        public:
        typename T::Handle GetAsset(const AssetID& id)
        {
                std::lock_guard<std::mutex> lock(m_Mutex);
                auto it = m_AssetMap.find(id);
                LD_VERIFY(it != m_AssetMap.end(), "Asset of ID: {} doesn't exist!", id);

                if (!it->second.m_pAsset)
                {
                        it->second.m_pAsset = new T(m_PlaceholderAsset);

                        auto it_future = m_LoadFutures.find(id);
                        if (it_future == m_LoadFutures.end())
                        {
                                m_LoadFutures.insert(std::make_pair(id, std::async(std::launch::async, &Subclass::LoadFromFile, static_cast<Subclass*>(this), it->second.user_format_path)));
                        }
                }
                return typename T::Handle(it->second.m_pAsset);
        }

        void MergeLoadedAssets()
        {
                std::lock_guard<std::mutex> lock(m_Mutex);
                //load ready futures
                std::vector<AssetID> loaded;
                for (auto& pair : m_LoadFutures)
                {
                        std::future<T*>& future = pair.second;
                        if (future.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                        {
                                auto& asset = m_AssetMap[pair.first];
                                if (asset.m_pAsset != &m_PlaceholderAsset)
                                        UnloadAsset(asset.m_pAsset);
                                T* p = future.get();
                                asset.m_pAsset->ReplaceData(*p);
                                delete p;
                                loaded.push_back(pair.first);
                        }
                }
                for (auto& id : loaded)
                {
                        m_LoadFutures.erase(id);
                }

                //free obsolete futures
                for (auto rit = m_ObsoleteFutures.rbegin(); rit != m_ObsoleteFutures.rend(); rit++)
                {
                        std::future<T*>& future = (*rit);
                        if (future.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
                        {
                                T* p = future.get();
                                UnloadAsset(p);
                                delete p;
                                m_ObsoleteFutures.erase(--rit.base());
                        }
                }
        }

        void ReloadAsset(const AssetID& id)
        {
                auto it = m_AssetMap.find(id);
                if (it != m_AssetMap.end())
                {
                        auto it_future = m_LoadFutures.find(id);
                        if (it_future != m_LoadFutures.end())
                        {
                                m_ObsoleteFutures.push_back(std::move(it_future->second));
                        }
                        m_LoadFutures[id] = std::async(std::launch::async, &Subclass::LoadFromFile, static_cast<Subclass*>(this), it->second.user_format_path);
                }
        }

        void ReleaseUnusedAssets()
        {
                for (auto& pair : m_AssetMap)
                {
                        AssetDesc& asset = pair.second;
                        if (asset.m_pAsset->GetReferenceCount() == 0 && !asset.m_pAsset->StayLoaded())
                        {
                                UnloadAsset(asset.m_pAsset);
                                delete asset.m_pAsset;
                                asset.m_pAsset = nullptr;
                        }
                }
        }

        void ReleaseUnusedAssetsForced()
        {
                for (auto& pair : m_AssetMap)
                {
                        AssetDesc& asset = pair.second;
                        if (asset.m_pAsset->GetReferenceCount() == 0)
                        {
                                UnloadAsset(asset.m_pAsset);
                                delete asset.m_pAsset;
                                asset.m_pAsset = nullptr;
                        }
                }
        }

        void ReleaseAllAssets()
        {
                for (auto& pair : m_AssetMap)
                {
                        AssetDesc& asset = pair.second;
                        UnloadAsset(asset.m_pAsset);
                        delete asset.m_pAsset;
                        asset.m_pAsset = nullptr;
                }
        }

        void RefreshAssetsFromFilesystem()
        {
                bool assets_changed = false;
                std::lock_guard<std::mutex> lock(m_Mutex);
                for (auto& path : std::filesystem::recursive_directory_iterator(m_AssetBaseDir))
                {
                        if (path.is_regular_file())
                        {
                                std::wstring extension = path.path().extension().wstring();
                                if (std::find(m_Extensions.begin(), m_Extensions.end(), extension) != m_Extensions.end())
                                {
                                        std::filesystem::path rel_path = std::filesystem::relative(path, m_AssetBaseDir);
                                        static std::hash<std::string> hasher;
                                        AssetID id = hasher(path.path().string());
                                        auto last_write = std::filesystem::last_write_time(path);
                                        if (!m_AssetListYaml[id])
                                        {
                                                assets_changed = true;
                                                m_AssetListYaml[id]["Name"] = rel_path.stem().string();
                                                m_AssetListYaml[id]["Dir"] = path.path().string();
                                                m_AssetMap.insert(std::make_pair(id, AssetDesc{
                                                                path,
                                                                path,
                                                                last_write,
                                                                nullptr
                                                        }));
                                        }
                                        else
                                        {
                                                auto& asset = m_AssetMap.find(id)->second;
                                                if (asset.last_write != last_write)
                                                {
                                                        asset.last_write = last_write;
                                                        ReloadAsset(id);
                                                }
                                        }
                                }
                        }
                }

                std::vector<AssetID> deleted;
                for (auto& pair : m_AssetMap)
                {
                        auto& id = pair.first;
                        auto& asset = pair.second;
                        if (!std::filesystem::exists(asset.user_format_path))
                        {
                                assets_changed = true;
                                deleted.push_back(id);
                        }
                }
                for (auto& id : deleted)
                {
                        // m_AssetMap.erase(id);
                        m_AssetListYaml.remove(id);
                }

                //Save assets yaml if assets have been created or deleted
                if (assets_changed)
                {
                        YAML::Emitter out;
                        out << m_AssetListYaml;
                        std::ofstream out_stream(m_AssetBaseDir / "AssetList.yaml");
                        out_stream << out.c_str();
                        out_stream.close();
                }
        }

        virtual void Initialize() {}

        void InitializeFiles()
        {
                std::filesystem::file_status status{};
                const std::filesystem::path yaml_path = m_AssetBaseDir / "AssetList.yaml";
                if (std::filesystem::status_known(status) ? std::filesystem::exists(status) : std::filesystem::exists(yaml_path))
                        m_AssetListYaml = YAML::LoadFile(yaml_path.string());
                for (YAML::const_iterator it = m_AssetListYaml.begin(); it != m_AssetListYaml.end(); ++it)
                {
                        std::filesystem::path dir = it->second["Dir"].as<std::string>();
                        m_AssetMap.insert(std::make_pair(it->first.as<uint64_t>(), AssetDesc{
                                        dir,
                                        dir,
                                        std::filesystem::last_write_time(dir),
                                        nullptr
                                }));
                }
                RefreshAssetsFromFilesystem();
        }

        ~AssetLibrary()
        {
                ReleaseAllAssets();
        }
        protected:
        struct AssetDesc
        {
                std::filesystem::path user_format_path;
                std::filesystem::path compressed_format_path;
                std::filesystem::file_time_type last_write;
                T* m_pAsset = nullptr;
        };
        virtual T* LoadFromFile(const std::filesystem::path& path) = 0;
        virtual void AfterLoadProcessing(T* pAsset) {}
        virtual void UnloadAsset(T* pAsset) {}
        virtual void SaveUserFormat(const std::filesystem::path& path) {}
        virtual void SaveCompressedFormat(const std::filesystem::path& path) {}
        T m_PlaceholderAsset;

        std::filesystem::path m_AssetBaseDir;
        std::vector<std::wstring> m_Extensions;

        private:
        std::unordered_map<AssetID, AssetDesc> m_AssetMap;
        std::unordered_map<AssetID, std::future<T*> > m_LoadFutures;
        std::vector<std::future<T*> > m_ObsoleteFutures{};
        YAML::Node m_AssetListYaml;
        std::mutex m_Mutex;
};
}