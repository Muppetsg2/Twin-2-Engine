#pragma once

#include <Generation/MapHexTile.h>
#include <Generation/MapSector.h>
#include <Generation/MapRegion.h>


//namespace Generation
//{
//	class MapRegion;
//	class MapHexTile;
//	class MapSector;
//
//}

namespace YAML
{
	template<> struct convert<Generation::MapHexTile::HexTileType> {
		static Node encode(const Generation::MapHexTile::HexTileType& rhs) {
			Node node;
			node = (int)rhs;
			return node;
		}

		static bool decode(const Node& node, Generation::MapHexTile::HexTileType& rhs) {
			if (!node.IsScalar()) return false;
			rhs = static_cast<Generation::MapHexTile::HexTileType>(node.as<int>());
			return true;
		}
	};


	template<> struct convert<Generation::MapSector::SectorType> {
		static Node encode(const Generation::MapSector::SectorType& rhs) {
			Node node;
			node = (int)rhs;
			return node;
		}

		static bool decode(const Node& node, Generation::MapSector::SectorType& rhs) {
			if (!node.IsScalar()) return false;
			rhs = static_cast<Generation::MapSector::SectorType>(node.as<int>());
			return true;
		}
	};


	template<> struct convert<Generation::MapRegion::RegionType> {
		static Node encode(const Generation::MapRegion::RegionType& rhs) {
			Node node;
			node = (int)rhs;
			return node;
		}

		static bool decode(const Node& node, Generation::MapRegion::RegionType& rhs) {
			if (!node.IsScalar()) return false;
			rhs = static_cast<Generation::MapRegion::RegionType>(node.as<int>());
			return true;
		}
	};
}


//#include <Generation/MapHexTile.h>
//#include <Generation/MapSector.h>
//#include <Generation/MapRegion.h>