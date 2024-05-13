#pragma once
#include <tools/EventHandler.h>

namespace Twin2Engine::Core {
	template<class T, class U, class C>
	static std::map<size_t, size_t> LoadResources(
		const Tools::Func<std::string, const T&>& pathGetter,
		const Tools::Func<size_t, const T&>& idGetter,
		const Tools::Func<C, const T&>& dataGetter,
		const U& resources, const std::map<size_t, size_t>& loadedIds,
		const Tools::Func<bool, size_t>& unloader,
		const Tools::Func<bool, const C&, size_t&>& loader)
	{
		using namespace std;

		hash<string> hasher = hash<string>();

		for (auto& id : loadedIds) {
			if (unloader(id.second)) {
				
			}
		}

		map<size_t, size_t> ret;
		for (auto& data : resources) {
			size_t id = 0;
			if (loader(dataGetter(data), id)) {
				ret[idGetter(data)] = id;
			}
		}

		return ret;

		/*map<size_t, size_t> finalIds;


		map<size_t, size_t> currentIds = loadedIds;
		map<size_t, C> data;
		map<size_t, size_t> pathHashes;
		for (const auto& path : resources) {
			string extractedPath = pathGetter(path);
			size_t extractedId = idGetter(path);
			data[extractedId] = dataGetter(path);
			pathHashes[extractedId] = hasher(extractedPath);
		}

		vector<size_t> toUnload, toLoad, toRemove;
		for (auto& path : pathHashes) {
			toLoad.push_back(path.first);
		}

		for (auto& h : currentIds) {
			bool hasHash = false;
			for (auto& h2 : pathHashes) {
				if (h2.second == h.second) {
					hasHash = true;
					break;
				}
			}

			if (!hasHash) {
				toUnload.push_back(h.first);
			}
			else {
				for (size_t i = 0; i < toLoad.size(); ++i) {
					if (pathHashes[toLoad[i]] == h.second) {
						finalIds[toLoad[i]] = h.second;
						toLoad.erase(toLoad.begin() + i);
						toRemove.push_back(h.first);
					}
				}
			}
		}

		// Removing from Current
		for (auto& id : toRemove) {
			currentIds.erase(id);
		}

		// Unloading
		for (size_t id : toUnload) {
			if (unloader(currentIds[id])) {
				currentIds.erase(id);
			}
		}

		// Loading
		map<size_t, size_t> newIds;
		for (size_t id : toLoad) {
			size_t loadedId = 0;
			if (loader(data[id], loadedId)) {
				finalIds[id] = loadedId;
			}
		}

		// Add Rest Ids
		if (currentIds.size() > 0) {
			vector<size_t> freeIds;
			size_t maxId = 0;
			for (size_t i = 0; i < finalIds.size(); ++i) {
				if (!finalIds.contains(i)) {
					freeIds.push_back(i);
				}
				else {
					if (i > maxId) {
						maxId = i;
					}
				}
			}
			for (auto& currId : currentIds) {
				if (finalIds.contains(currId.first)) {
					if (freeIds.size() > 0) {
						finalIds[freeIds[0]] = currId.second;
						finalIds.erase(finalIds.begin());
					}
					else {
						finalIds[++maxId] = currId.second;
					}
				}
			}
		}
	
		return finalIds;*/
	}
}