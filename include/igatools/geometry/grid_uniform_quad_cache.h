//-+--------------------------------------------------------------------
// Igatools a general purpose Isogeometric analysis library.
// Copyright (C) 2012-2014  by the igatools authors (see authors.txt).
//
// This file is part of the igatools library.
//
// The igatools library is free software: you can use it, redistribute
// it and/or modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation, either
// version 3 of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//-+--------------------------------------------------------------------

#ifndef GRID_UNIFORM_QUAD_CACHE_H_
#define GRID_UNIFORM_QUAD_CACHE_H_

#include <igatools/base/config.h>
#include <igatools/base/cache_status.h>
#include <igatools/base/value_flags_handler.h>
#include <igatools/base/quadrature.h>
#include <igatools/geometry/cartesian_grid.h>

IGA_NAMESPACE_OPEN

/**
 * @brief Global CartesianGrid uniform quadrature
 * computational optimization cache, storing the interval length
 * in each direction.
 *
 */
template <int dim_>
class GridUniformQuadCache : public CacheStatus
{
    using GridType = CartesianGrid<dim_>;
    using ElementIterator = typename GridType::ElementIterator;


public:
    static const int dim = dim_;

    //Allocates and fill the (global) cache
    GridUniformQuadCache(std::shared_ptr<const GridType> grid,
                         const ValueFlags flag,
                         const Quadrature<dim> &quad);

    //Allocates the ElementIterator element_cache
    void init_element_cache(ElementIterator &elem);

    //Fill the ElementIterator element_cache
    void fill_element_cache(ElementIterator &elem);

    void print_info(LogStream &out) const;

private:
    std::shared_ptr<const GridType> grid_;

    GridElemValueFlagsHandler flags_;

    CartesianProductArray<Real, dim> lengths_;

    Quadrature<dim> quad_;
};

IGA_NAMESPACE_CLOSE

#endif /* GRID_UNIFORM_QUAD_CACHE_H_ */
