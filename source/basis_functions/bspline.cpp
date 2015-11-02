//-+--------------------------------------------------------------------
// Igatools a general purpose Isogeometric analysis library.
// Copyright (C) 2012-2015  by the igatools authors (see authors.txt).
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

#include <igatools/basis_functions/bspline.h>
#include <igatools/basis_functions/bspline_element_handler.h>
#include <igatools/functions/sub_function.h>
//#include <igatools/functions/identity_function.h>
//#include <igatools/geometry/grid_function_lib.h>


using std::endl;

using std::shared_ptr;
using std::make_shared;
using std::const_pointer_cast;

IGA_NAMESPACE_OPEN


template<int dim_, int range_, int rank_>
BSpline<dim_, range_, rank_>::
BSpline(const SharedPtrConstnessHandler<SpaceData> &space_data,
             const EndBehaviourTable &end_b)
  :
  space_data_(space_data),
  end_b_(end_b),
  operators_(*space_data_,end_b),
  end_interval_(end_b.get_comp_map()),
  dof_distribution_(std::make_shared<DofDistribution<dim_,range_,rank_>>(
                     space_data->get_num_basis_table(),
                     space_data->get_degree_table(),
                     space_data->get_periodic_table()))
{
//    Assert(space_data_ != nullptr,ExcNullPtr());
  Assert(dof_distribution_ != nullptr,ExcNullPtr());

  //------------------------------------------------------------------------------
// TODO (pauletti, Dec 24, 2014): after it work it should be recoded properly

  const auto &sp_data = *this->space_data_;
  const auto &grid = *sp_data.get_grid();
  const auto &degree_table = sp_data.get_degree_table();
  const auto rep_knots = sp_data.compute_knots_with_repetition(end_b_);

  for (auto i : end_interval_.get_active_components_id())
  {
    for (int dir=0; dir<dim; ++dir)
    {
      const auto p = degree_table[i][dir];

      const auto &knots_coord_dir = grid.get_knot_coordinates().get_data_direction(dir);

      const auto x1 = knots_coord_dir[1];
      const auto a = knots_coord_dir[0];
      const auto x0 = rep_knots[i].get_data_direction(dir)[p];
      end_interval_[i][dir].first = (x1-a) / (x1-x0);

      const auto xk= *(knots_coord_dir.end()-2);
      const auto b = *(knots_coord_dir.end()-1);
      const auto xk1 = *(rep_knots[i].get_data_direction(dir).end() - (p+1));
      end_interval_[i][dir].second = (b-xk) / (xk1-xk);
    } // end loop dir
  } // end loop i
  //------------------------------------------------------------------------------



#if 0
  //------------------------------------------------------------------------------
  this->dof_distribution_->add_dofs_property(this->dofs_property_active_);
  this->dof_distribution_->set_all_dofs_property_status(this->dofs_property_active_,true);
  //------------------------------------------------------------------------------
#endif
}

#if 0
template<int dim_, int range_, int rank_>
BSpline<dim_, range_, rank_>::
BSpline(const DegreeTable &deg,
             const SharedPtrConstnessHandler<GridType> &grid,
             const MultiplicityTable &interior_mult,
             const PeriodicityTable &periodic,
             const EndBehaviourTable &end_b)
  :
  BSpline(
   grid.data_is_const() ?
   SharedPtrConstnessHandler<SpaceData>(SpaceData::const_create(deg, grid.get_ptr_const_data(), interior_mult, periodic)) :
   SharedPtrConstnessHandler<SpaceData>(SpaceData::create(deg, grid.get_ptr_data(), interior_mult, periodic)),
   end_b)
{}
#endif



template<int dim_, int range_, int rank_>
auto
BSpline<dim_, range_, rank_>::
create(const std::shared_ptr<SpaceData> &space_data,
       const EndBehaviourTable &end_b)
-> shared_ptr<self_t>
{
  auto sp = shared_ptr<self_t>(
    new self_t(SharedPtrConstnessHandler<SpaceData>(space_data), end_b));
  Assert(sp != nullptr, ExcNullPtr());

#ifdef MESH_REFINEMENT
  sp->create_connection_for_insert_knots(sp);
#endif

  return sp;
}

template<int dim_, int range_, int rank_>
auto
BSpline<dim_, range_, rank_>::
const_create(const std::shared_ptr<const SpaceData> &space_data,
             const EndBehaviourTable &end_b)
-> shared_ptr<const self_t>
{
  auto sp = shared_ptr<const self_t>(
    new self_t(SharedPtrConstnessHandler<SpaceData>(space_data), end_b));
  Assert(sp != nullptr, ExcNullPtr());

  return sp;
}


template<int dim_, int range_, int rank_>
auto
BSpline<dim_, range_, rank_>::
get_this_space() const -> shared_ptr<const self_t>
{
  auto ref_sp = const_cast<self_t *>(this)->shared_from_this();
  auto bsp_space = std::dynamic_pointer_cast<self_t>(ref_sp);
  Assert(bsp_space != nullptr,ExcNullPtr());

  return bsp_space;
}

template<int dim_, int range_, int rank_>
auto
BSpline<dim_, range_, rank_>::
create_element(const ListIt &index, const PropId &property) const
-> std::unique_ptr<SpaceElement<dim_,0,range_,rank_> >
{
  using Elem = BSplineElement<dim_,range_,rank_>;

  std::unique_ptr<SpaceElement<dim_,0,range_,rank_>>
  elem = std::make_unique<Elem>(this->get_this_space(),index,property);
  Assert(elem != nullptr, ExcNullPtr());

  return elem;
}

template<int dim_, int range_, int rank_>
auto
BSpline<dim_, range_, rank_>::
create_ref_element(const ListIt &index, const PropId &property) const
-> std::unique_ptr<ReferenceElement<dim_,range_,rank_> >
{
  using Elem = BSplineElement<dim_,range_,rank_>;

  std::unique_ptr<ReferenceElement<dim_,range_,rank_>>
  elem = std::make_unique<Elem>(this->get_this_space(),index,property);
  Assert(elem != nullptr, ExcNullPtr());

  return elem;
}


template<int dim_, int range_, int rank_>
template<int sdim>
auto
BSpline<dim_, range_, rank_>::
get_sub_bspline_space(const int s_id,
                      InterSpaceMap<sdim> &dof_map,
                      const std::shared_ptr<const Grid<sdim>> &sub_grid_in) const
-> std::shared_ptr<const BSpline<sdim, range_, rank_> >
{
  static_assert(sdim == 0 || (sdim > 0 && sdim < dim_),
  "The dimensionality of the sub_grid is not valid.");

  std::shared_ptr<const Grid<sdim>> sub_grid;
  if (sub_grid_in != nullptr)
  {
#ifndef NDEBUG
    typename Grid<dim_>::template SubGridMap<sdim> elem_map;
    sub_grid = this->get_grid()->template get_sub_grid<sdim>(s_id, elem_map);
    Assert(*sub_grid_in == *sub_grid,ExcMessage("Invalid input grid."));
#endif
    sub_grid = sub_grid_in;
  }
  else
  {
    typename Grid<dim_>::template SubGridMap<sdim> elem_map;
    sub_grid = this->get_grid()->template get_sub_grid<sdim>(s_id, elem_map);
  }

  auto sub_mult   = this->space_data_->template get_sub_space_mult<sdim>(s_id);
  auto sub_degree = this->space_data_->template get_sub_space_degree<sdim>(s_id);
  auto sub_periodic = this->space_data_->template get_sub_space_periodicity<sdim>(s_id);

  using SubBasis = BSpline<sdim,range,rank>;

  using SubEndBT = typename SubBasis::EndBehaviourTable;
  auto &k_elem = UnitElement<dim>::template get_elem<sdim>(s_id);
  const auto &active_dirs = k_elem.active_directions;

  SubEndBT sub_end_b(end_b_.get_comp_map());
  for (int comp : end_b_.get_active_components_id())
    for (int j=0; j<sdim; ++j)
      sub_end_b[comp][j] = end_b_[comp][active_dirs[j]];

  auto sub_spline_space =
  SplineSpace<sdim,range_,rank_>::const_create(sub_degree,sub_grid,sub_mult,sub_periodic);
  auto sub_basis = SubBasis::const_create(sub_spline_space, sub_end_b);

  // Creating the mapping between the space degrees of freedom
  const int n_dir = k_elem.constant_directions.size();
#ifndef NDEBUG
  for (int comp : end_b_.get_active_components_id())
    for (int j=0; j<n_dir; ++j)
      Assert(end_b_[comp][k_elem.constant_directions[j]] ==
      BasisEndBehaviour::interpolatory,
      ExcNotImplemented());
#endif


  TensorIndex<dim> tensor_index;
  int comp_i = 0;
  dof_map.resize(sub_basis->get_num_basis());
  const auto &sub_space_index_table = sub_basis->get_ptr_const_dof_distribution()->get_index_table();
  const auto     &space_index_table = this->get_ptr_const_dof_distribution()->get_index_table();
  for (auto comp : SpaceData::components)
  {
    const auto n_basis = sub_basis->get_num_basis(comp);
    const auto &sub_local_indices = sub_space_index_table[comp];
    const auto &elem_global_indices = space_index_table[comp];

    for (Index sub_i = 0; sub_i < n_basis; ++sub_i, ++comp_i)
    {
      const auto sub_base_id = sub_local_indices.flat_to_tensor(sub_i);

      for (int j=0; j<sdim; ++j)
        tensor_index[active_dirs[j]] = sub_base_id[j];
      for (int j=0; j<n_dir; ++j)
      {
        auto dir = k_elem.constant_directions[j];
        auto val = k_elem.constant_values[j];
        const int fixed_id = val * (this->get_num_basis(comp, dir) - 1);
        tensor_index[dir] = fixed_id;

      }
      dof_map[comp_i] = elem_global_indices(tensor_index);
    }
  }

  return sub_basis;
}






template<int dim_, int range_, int rank_>
auto
BSpline<dim_, range_, rank_>::
get_grid() const -> std::shared_ptr<const Grid<dim_>>
{
  return space_data_->get_grid();
}





template<int dim_, int range_, int rank_>
void
BSpline<dim_, range_, rank_>::
get_element_dofs(
  const IndexType elem_tensor_id,
  SafeSTLVector<Index> &dofs_global,
  SafeSTLVector<Index> &dofs_local_to_patch,
  SafeSTLVector<Index> &dofs_local_to_elem,
  const std::string &dofs_property) const
{
  const auto &sp_data = *space_data_;
  const auto &accum_mult = sp_data.accumulated_interior_multiplicities();

  const auto &dof_distr = *this->dof_distribution_;
  const auto &index_table = dof_distr.get_index_table();

  dofs_global.clear();
  dofs_local_to_patch.clear();
  dofs_local_to_elem.clear();

//    const auto &elem_tensor_id = this->get_grid()->flat_to_tensor(element_id);

  Index dof_loc_to_elem = 0;
  for (const auto comp : SpaceData::components)
  {
    const auto &index_table_comp = index_table[comp];

    const auto dof_t_origin = accum_mult[comp].cartesian_product(elem_tensor_id);

    const auto &elem_comp_dof_t_id = sp_data.get_dofs_tensor_id_elem_table()[comp];

//        if (dofs_property == DofProperties::active)
//        {
//            for (const auto loc_dof_t_id : elem_comp_dof_t_id)
//            {
//                const auto dof_global = index_table_comp(dof_t_origin + loc_dof_t_id);
//                dofs_global.emplace_back(dof_global);
//
//                const auto dof_loc_to_patch = this->dof_distribution_->global_to_patch_local(dof_global);
//                dofs_local_to_patch.emplace_back(dof_loc_to_patch);
//
//                dofs_local_to_elem.emplace_back(dof_loc_to_elem);
//
//                ++dof_loc_to_elem;
//            } // end loop loc_dof_t_id
//        }
//        else
    {
      for (const auto loc_dof_t_id : elem_comp_dof_t_id)
      {
        const auto dof_global = index_table_comp(dof_t_origin + loc_dof_t_id);
        if (dof_distr.test_if_dof_has_property(dof_global, dofs_property))
        {
          dofs_global.emplace_back(dof_global);

          const auto dof_loc_to_patch = dof_distr.global_to_patch_local(dof_global);
          dofs_local_to_patch.emplace_back(dof_loc_to_patch);

          dofs_local_to_elem.emplace_back(dof_loc_to_elem);

        }
        ++dof_loc_to_elem;
      } // end loop loc_dof_t_id
    }

  } // end comp loop
}



#ifdef MESH_REFINEMENT



template <int dim_,int range_,int rank_>
void
BSpline<dim_,range_,rank_>::
refine_h(const Size n_subdivisions)
{
  space_data_.get_ptr_data()->refine_h(n_subdivisions);
}


template<int dim_, int range_, int rank_>
void
BSpline<dim_, range_, rank_>::
rebuild_after_insert_knots(
  const SafeSTLArray<SafeSTLVector<Real>,dim> &knots_to_insert,
  const Grid<dim> &old_grid)
{
  this->ref_space_previous_refinement_ =
    BSpline<dim_,range_,rank_>::const_create(
      this->space_data_->get_spline_space_previous_refinement(),
      this->end_b_);


  this->dof_distribution_ = shared_ptr<DofDistribution<dim_,range_,rank_>>(
                              new DofDistribution<dim_,range_,rank_>(
                                this->space_data_->get_num_basis_table(),
                                this->space_data_->get_degree_table(),
                                this->space_data_->get_periodic_table()));

  operators_ = BernsteinExtraction<dim,range,rank>(*this->space_data_,end_b_);
}



#endif //MESH_REFINEMENT

template<int dim_, int range_, int rank_>
void
BSpline<dim_, range_, rank_>::
print_info(LogStream &out) const
{
  out.begin_item("Spline Space:");
  this->space_data_->print_info(out);
  out.end_item();


  out.begin_item("DoFs Distribution:");
  this->dof_distribution_->print_info(out);
  out.end_item();


  out.begin_item("Bernstein Extraction:");
  operators_.print_info(out);
  out.end_item();
}


template<int dim_, int range_, int rank_>
auto
BSpline<dim_, range_, rank_>::
get_degree_table() const -> const DegreeTable &
{
  return this->space_data_->get_degree_table();
}

template<int dim_, int range_, int rank_>
auto
BSpline<dim_, range_, rank_>::
get_periodicity() const -> const PeriodicityTable &
{
  return space_data_->get_periodicity();
}

template<int dim_, int range_, int rank_>
auto
BSpline<dim_, range_, rank_>::
get_end_behaviour_table() const -> const EndBehaviourTable &
{
  return end_b_;
};


template<int dim_, int range_, int rank_>
bool
BSpline<dim_, range_, rank_>::
is_bspline() const
{
  return true;
}

template<int dim_, int range_, int rank_>
auto
BSpline<dim_, range_, rank_>::
create_cache_handler() const
-> std::unique_ptr<SpaceElementHandler<dim_,0,range_,rank_>>
{
  return std::make_unique<BSplineElementHandler<dim_,range_,rank_>>(
    this->get_this_space());
}


template<int dim_, int range_, int rank_>
auto
BSpline<dim_, range_, rank_>::
get_ptr_const_dof_distribution() const -> shared_ptr<const DofDistribution<dim,range,rank> >
{
  return dof_distribution_;
}

template<int dim_, int range_, int rank_>
auto
BSpline<dim_, range_, rank_>::
get_ptr_dof_distribution() -> shared_ptr<DofDistribution<dim,range,rank> >
{
  return dof_distribution_;
}




#ifdef SERIALIZATION

template<int dim_, int range_, int rank_>
template<class Archive>
void
BSpline<dim_, range_, rank_>::
serialize(Archive &ar)
{
  using std::to_string;
  const std::string base_name = "ReferenceSpace_" +
                                to_string(dim_) + "_" +
                                to_string(0) + "_" +
                                to_string(range_) + "_" +
                                to_string(rank_);

  ar &make_nvp(base_name,base_class<BaseSpace>(this));

  ar &make_nvp("space_data_",space_data_);

  ar &make_nvp("end_b_",end_b_);

  ar &make_nvp("operators_",operators_);

  ar &make_nvp("end_interval_",end_interval_);

  ar &make_nvp("dof_distribution_",dof_distribution_);

//    ar &make_nvp("dofs_tensor_id_elem_table_",dofs_tensor_id_elem_table_);
}
#endif // SERIALIZATION

IGA_NAMESPACE_CLOSE


#include <igatools/basis_functions/bspline.inst>


#ifdef SERIALIZATION

//using BSpSpaceAlias0_1_1 = iga::BSpline<0,1,1>;
//CEREAL_REGISTER_DYNAMIC_INIT(BSpSpaceAlias0_1_1);

#endif // SERIALIZATION