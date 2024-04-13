#ifndef _SECTORS_GENERATOR_H_
#define _SECTORS_GENERATOR_H_

#include <Generation/AMapElementGenerator.h>

namespace Generation
{
	class SectorsGenerator : public AMapElementGenerator
	{

	public:

		virtual void Generate(Tilemap::HexagonalTilemap* tilemap) override;


	};
}

#endif // !_SECTORS_GENERATOR_H_