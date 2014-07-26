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

#ifndef DOF_DISTRIBUTION_H_
#define DOF_DISTRIBUTION_H_

#include <igatools/base/config.h>
#include <igatools/basis_functions/spline_space.h>
#include <igatools/geometry/cartesian_grid_element_accessor.h>
#include <igatools/utils/concatenated_iterator.h>

IGA_NAMESPACE_OPEN

/**
 *
 * Class to handle the distribution of the basis function
 * indices storing what is known as the local to global map.
 *
 * It is a helper class for the BSplineSpace.
 *
 * @author pauletti, 2014
 *
 */
template<int dim, int range = 1, int rank = 1>
class DofDistribution
{
public:
    using Space = SplineSpace<dim, range, rank>;
    using MultiplicityTable = typename Space::MultiplicityTable;
    using SpaceDimensionTable = typename Space::SpaceDimensionTable;
    using IndexDistributionTable =
        StaticMultiArray<DynamicMultiArray<Index,dim>,range,rank>;

    enum class DistributionPolicy
    {
        standard, component, other
    };

    DofDistribution() = delete;

    DofDistribution(std::shared_ptr<CartesianGrid<dim> > grid,
                    const MultiplicityTable &accum_mult,
                    const SpaceDimensionTable &n_basis,
                    const SpaceDimensionTable &n_elem_basis,
                    DistributionPolicy pol = DistributionPolicy::standard);

    void reassign_dofs(const IndexDistributionTable &index_distribution, const DistributionPolicy pol);

    std::vector<Index> get_loc_to_global_indices(const TensorIndex<dim> &elem_tensor_id) const;

    std::vector<Index> get_loc_to_global_indices(const Index &elem_flat_id) const;


    TensorIndex<dim>
    basis_flat_to_tensor(const Index index, const Index comp) const
    {
        return index_distribution_(comp).flat_to_tensor(index);
    }


    Index
    basis_tensor_to_flat(const TensorIndex<dim> &tensor_index,
                         const Index comp) const
    {
        return index_distribution_(comp).tensor_to_flat(tensor_index);
    }
    /**
     * Print the class content
     */
    void print_info(LogStream &out) const;


    /** Add an @p offset to the dofs. */
    void add_dofs_offset(const Index offset);


private:

    IndexDistributionTable index_distribution_;

#if 0
    //TODO (martinelli, Jun 27, 2014): I think this should be removed and use instead some kind of iterator
    DynamicMultiArray<std::vector<Index>, dim> element_loc_to_global_;

    DynamicMultiArray<std::vector<Index>, dim> create_element_loc_to_global_from_index_distribution(
        std::shared_ptr<const CartesianGrid<dim> > grid,
        const MultiplicityTable &accum_mult,
        const SpaceDimensionTable &n_elem_basis,
        const IndexDistributionTable &index_distribution) const;
#endif


    void create_element_loc_to_global_view(
        std::shared_ptr<const CartesianGrid<dim> > grid,
        const MultiplicityTable &accum_mult,
        const SpaceDimensionTable &n_elem_basis);


    using DofsComponentContainer = std::vector<Index>;
    using DofsComponentView = ContainerView<DofsComponentContainer>;
    using DofsComponentConstView = ConstContainerView<DofsComponentContainer>;
    using DofsIterator = ConcatenatedIterator<DofsComponentView>;
    using DofsConstIterator = ConcatenatedConstIterator<DofsComponentConstView>;
    using DofsView = ConstView<DofsIterator,DofsConstIterator>;
    DynamicMultiArray<DofsView, dim> element_loc_to_global_view_;

    DistributionPolicy policy_;

public:

    const IndexDistributionTable &get_index_distribution() const
    {
        return index_distribution_;
    }

};

IGA_NAMESPACE_CLOSE

#endif
