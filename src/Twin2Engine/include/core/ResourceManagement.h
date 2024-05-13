#pragma once
#include <tools/EventHandler.h>

namespace Twin2Engine::Core {
	template<class T, class U>
	static std::vector<size_t> LoadResources(const Tools::Func<std::string, const T&>& pathGetter,
		const U& resources, const std::vector<size_t>& loadedIds,
		const Tools::Func<bool, size_t>& unloader,
		const Tools::Func<bool, const std::string&, size_t&>& loader,
		const Tools::Func<std::vector<size_t>, const std::vector<size_t>&, const std::vector<size_t>&>& sorter)
	{
		using namespace std;

		hash<string> hasher = hash<string>();

		vector<size_t> currentIds = loadedIds;
		vector<string> paths;
		vector<size_t> pathHashes;
		for (const auto& path : resources) {
			string extractedPath = pathGetter(path);
			paths.push_back(extractedPath);
			pathHashes.push_back(hasher(extractedPath));
		}

		vector<size_t> toUnload, toLoad;
		for (size_t i = 0; i < paths.size(); ++i) {
			toLoad.push_back(i);
		}

		for (auto& h : currentIds) {
			bool hasHash = false;
			for (auto& h2 : pathHashes) {
				if (h2 == h) {
					hasHash = true;
					break;
				}
			}

			if (!hasHash) {
				toUnload.push_back(h);
			}
			else {
				for (size_t i = 0; i < toLoad.size(); ++i) {
					if (pathHashes[toLoad[i]] == h) {
						toLoad.erase(toLoad.begin() + i);
					}
				}
			}
		}

		// Unloading
		for (size_t id : toUnload) {
			if (unloader(id)) {
				for (size_t i = 0; i < currentIds.size(); ++i) {
					if (currentIds[i] == id) {
						currentIds.erase(currentIds.begin() + i);
						break;
					}
				}
			}
		}

		// Loading
		for (size_t pathIdx : toLoad) {
			size_t id = 0;
			if (loader(paths[pathIdx], id)) {
				currentIds.push_back(id);
			}
		}

		return sorter(pathHashes, currentIds);
	}
}