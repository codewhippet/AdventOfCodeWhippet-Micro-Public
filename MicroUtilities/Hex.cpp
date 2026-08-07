#include "stdafx.h"
#include "Hex.h"

template <typename COORDINATE_TYPE>
const std::array<HexFlat::Hex<COORDINATE_TYPE>, 6>& HexFlat::Hex<COORDINATE_TYPE>::NeighbourDirections()
{
    static const std::array<HexFlat::Hex<COORDINATE_TYPE>, 6> directions =
    {
        HexFlat::Hex<COORDINATE_TYPE>::North(),
        HexFlat::Hex<COORDINATE_TYPE>::NorthEast(),
        HexFlat::Hex<COORDINATE_TYPE>::SouthEast(),
        HexFlat::Hex<COORDINATE_TYPE>::South(),
        HexFlat::Hex<COORDINATE_TYPE>::SouthWest(),
        HexFlat::Hex<COORDINATE_TYPE>::NorthWest()
    };
    return directions;
}

template <typename COORDINATE_TYPE>
const std::array<HexPointy::Hex<COORDINATE_TYPE>, 6>& HexPointy::Hex<COORDINATE_TYPE>::NeighbourDirections()
{
    static const std::array<HexPointy::Hex<COORDINATE_TYPE>, 6> directions =
    {
        HexPointy::Hex<COORDINATE_TYPE>::NorthEast(),
        HexPointy::Hex<COORDINATE_TYPE>::East(),
        HexPointy::Hex<COORDINATE_TYPE>::SouthEast(),
        HexPointy::Hex<COORDINATE_TYPE>::SouthWest(),
        HexPointy::Hex<COORDINATE_TYPE>::West(),
        HexPointy::Hex<COORDINATE_TYPE>::NorthWest()
    };
    return directions;
}

template struct HexFlat::Hex<int32_t>;

template struct HexPointy::Hex<int32_t>;