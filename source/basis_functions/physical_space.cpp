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

#include <igatools/basis_functions/physical_space.h>
#include <igatools/functions/function.h>
#include <igatools/functions/sub_function.h>
#include <igatools/basis_functions/phys_space_element_handler.h>


using std::shared_ptr;
using std::make_shared;
using std::unique_ptr;

using std::const_pointer_cast;
using std::endl;

IGA_NAMESPACE_OPEN


template <int dim_, int range_, int rank_, int codim_, Transformation type_>
const SafeSTLArray<int, PhysicalSpace<dim_, range_, rank_, codim_, type_>::n_components>
PhysicalSpace<dim_, range_, rank_, codim_, type_>::components =
  sequence<PhysicalSpace<dim_, range_, rank_, codim_, type_>::n_components>();


template <int dim_, int range_, int rank_, int codim_, Transformation type_>
PhysicalSpace<dim_, range_, rank_, codim_, type_>::
PhysicalSpace(const shared_ptr<RefSpace> &ref_space,
              const shared_ptr<PhysDomain> &phys_domain)
  :
  base_t(ref_space->get_ptr_grid()),
  ref_space_(ref_space),
  phys_domain_(phys_domain)
{
  Assert(this->get_ptr_grid() == phys_domain_->get_grid_function()->get_grid(),
         ExcMessage("The space and the physical domain must have the same grid!"));

//TODO(pauletti, Jan 18, 2014): put static assert on h_div, h_curl range and rank
}

template <int dim_, int range_, int rank_, int codim_, Transformation type_>
PhysicalSpace<dim_, range_, rank_, codim_, type_>::
PhysicalSpace(const shared_ptr<const RefSpace> &ref_space,
              const shared_ptr<const PhysDomain> &phys_domain)
  :
  base_t(ref_space->get_ptr_const_grid()),
  ref_space_(ref_space),
  phys_domain_(phys_domain)
{
  Assert(this->get_ptr_const_grid() == phys_domain_->get_grid_function()->get_grid(),
         ExcMessage("The space and the physical domain must have the same grid!"));
//TODO(pauletti, Jan 18, 2014): put static assert on h_div, h_curl range and rank
}


template <int dim_, int range_, int rank_, int codim_, Transformation type_>
auto
PhysicalSpace<dim_, range_, rank_, codim_, type_>::
create(const shared_ptr<RefSpace> &ref_space,
       const shared_ptr<PhysDomain> &phys_domain) -> shared_ptr<self_t>
{
//  Assert(map_func != nullptr, ExcNullPtr());
//  Assert(map_func.unique(), ExcNotUnique());
  auto sp = shared_ptr<self_t>(new self_t(ref_space, phys_domain));

#ifdef MESH_REFINEMENT
  sp->create_connection_for_insert_knots(sp);
#endif // MESH_REFINEMENT

  return sp;
}

template <int dim_, int range_, int rank_, int codim_, Transformation type_>
auto
PhysicalSpace<dim_, range_, rank_, codim_, type_>::
const_create(const shared_ptr<const RefSpace> &ref_space,
             const shared_ptr<const PhysDomain> &phys_domain) -> shared_ptr<const self_t>
{
//  Assert(ref_space != nullptr, ExcNullPtr());
//  Assert(map_func != nullptr, ExcNullPtr());
//  Assert(map_func.unique(), ExcNotUnique());
  auto sp = shared_ptr<self_t>(new self_t(ref_space, phys_domain));
  Assert(sp != nullptr,ExcNullPtr());

  return sp;
}

template <int dim_, int range_, int rank_, int codim_, Transformation type_>
auto
PhysicalSpace<dim_, range_, rank_, codim_, type_>::
get_this_space() const -> std::shared_ptr<const self_t >
{
  auto sp = const_cast<self_t *>(this)->shared_from_this();
  auto this_space = std::dynamic_pointer_cast<self_t>(sp);
  Assert(this_space != nullptr,ExcNullPtr());

  return this_space;
}


template <int dim_, int range_, int rank_, int codim_, Transformation type_>
auto
PhysicalSpace<dim_, range_, rank_, codim_, type_>::
create_element(const ListIt &index, const PropId &property) const
-> std::unique_ptr<SpaceElement<dim_,codim_,range_,rank_,type_>>
{
  std::unique_ptr<SpaceElement<dim_,codim_,range_,rank_,type_>>
  elem = std::make_unique<ElementAccessor>(this->get_this_space(),index,property);
  Assert(elem != nullptr, ExcNullPtr());

  return elem;
}



template <int dim_, int range_, int rank_, int codim_, Transformation type_>
auto
PhysicalSpace<dim_, range_, rank_, codim_, type_>::
get_reference_space() const -> shared_ptr<const RefSpace>
{
  return ref_space_.get_ptr_const_data();
}



template <int dim_, int range_, int rank_, int codim_, Transformation type_>
auto
PhysicalSpace<dim_, range_, rank_, codim_, type_>::
get_reference_space() -> shared_ptr<RefSpace>
{
  return ref_space_.get_ptr_data();
}



template <int dim_, int range_, int rank_, int codim_, Transformation type_>
template<int k>
auto
PhysicalSpace<dim_, range_, rank_, codim_, type_>::
get_sub_space(const int s_id, InterSpaceMap<k> &dof_map,
              std::shared_ptr<Grid<k>> sub_grid,
              SubGridMap<k> &elem_map) const
-> std::shared_ptr<SubSpace<k> >
{
  AssertThrow(false,ExcNotImplemented());
  return nullptr;
  /*
  using SubMap = SubMapFunction<k, dim, space_dim>;
  auto grid =  this->get_grid();

  auto sub_ref_space = ref_space_->get_ref_sub_space(s_id, dof_map, sub_grid);
  shared_ptr<const typename SubMap::SupFunc> F;
  //  auto F = this->phys_domain_->get_function();
  AssertThrow(false,ExcNotImplemented());
  auto sub_map_func = SubMap::create(sub_grid, F, s_id, elem_map);
  auto sub_space = SubSpace<k>::create_nonconst(sub_ref_space, sub_map_func);
  return sub_space;
  //*/
}



#if 0
template <int dim_, int range_, int rank_, int codim_, Transformation type_>
auto
PhysicalSpace<dim_, range_, rank_, codim_, type_>::
get_face_space(const Index face_id,
               SafeSTLVector<Index> &face_to_element_dofs) const -> shared_ptr<FaceSpace>
{
  auto elem_map = std::make_shared<typename GridType::FaceGridMap >();
  auto face_ref_sp = ref_space_->get_ref_face_space(face_id, face_to_element_dofs, *elem_map);
  auto map  = push_forward_->get_mapping();

  auto fmap = MappingSlice<FaceSpace::PushForwardType::dim, FaceSpace::PushForwardType::codim>::
  create(map, face_id, face_ref_sp->get_grid(), elem_map);
  auto fpf = FaceSpace::PushForwardType::create(fmap);
  auto face_space = FaceSpace::create(face_ref_sp,fpf);

  return face_space;
}


template <int dim_, int range_, int rank_, int codim_, Transformation type_>
Index
PhysicalSpace<dim_, range_, rank_, codim_, type_>::
get_id() const
{
  return ref_space_->get_id();
}
#endif


template <int dim_, int range_, int rank_, int codim_, Transformation type_>
void
PhysicalSpace<dim_, range_, rank_, codim_, type_>::
get_element_dofs(
  const IndexType element_id,
  SafeSTLVector<Index> &dofs_global,
  SafeSTLVector<Index> &dofs_local_to_patch,
  SafeSTLVector<Index> &dofs_local_to_elem,
  const std::string &dofs_property) const
{
  return ref_space_->get_element_dofs(
           element_id,
           dofs_global,
           dofs_local_to_patch,
           dofs_local_to_elem,dofs_property);
}




template <int dim_, int range_, int rank_, int codim_, Transformation type_>
auto
PhysicalSpace<dim_, range_, rank_, codim_, type_>::
get_ptr_const_dof_distribution() const -> std::shared_ptr<const DofDistribution<dim, range, rank> >
{
  return ref_space_->get_ptr_const_dof_distribution();
}



template <int dim_, int range_, int rank_, int codim_, Transformation type_>
auto
PhysicalSpace<dim_, range_, rank_, codim_, type_>::
get_ptr_dof_distribution() -> std::shared_ptr<DofDistribution<dim, range, rank> >
{
  return ref_space_.get_ptr_data()->get_ptr_dof_distribution();
}




template <int dim_, int range_, int rank_, int codim_, Transformation type_>
void
PhysicalSpace<dim_, range_, rank_, codim_, type_>::
print_info(LogStream &out) const
{
  out.begin_item("Reference space:");
  ref_space_->print_info(out);
  out.end_item();

  out.begin_item("Physical domain:");
  phys_domain_->print_info(out);
  out.end_item();
}



template <int dim_, int range_, int rank_, int codim_, Transformation type_>
auto
PhysicalSpace<dim_, range_, rank_, codim_, type_>::
create_cache_handler() const -> std::unique_ptr<SpaceElementHandler<dim_,codim_,range_,rank_,type_>>
{
  return ElementHandler::create(this->get_this_space());
}





template <int dim_, int range_, int rank_, int codim_, Transformation type_>
int
PhysicalSpace<dim_, range_, rank_, codim_, type_>::
get_max_degree() const
{
  return ref_space_->get_max_degree();
}


#ifdef MESH_REFINEMENT

template <int dim_, int range_, int rank_, int codim_, Transformation type_>
void
PhysicalSpace<dim_, range_, rank_, codim_, type_>::
create_connection_for_insert_knots(std::shared_ptr<self_t> space)
{
  Assert(space != nullptr, ExcNullPtr());
  Assert(&(*space) == &(*this), ExcMessage("Different objects."));

  auto func_to_connect =
    std::bind(&self_t::rebuild_after_insert_knots,
              space.get(),
              std::placeholders::_1,
              std::placeholders::_2);

  using SlotType = typename Grid<dim>::SignalInsertKnotsSlot;
  this->get_ptr_grid()->connect_insert_knots(
    SlotType(func_to_connect).track_foreign(space));
}



template <int dim_, int range_, int rank_, int codim_, Transformation type_>
void
PhysicalSpace<dim_, range_, rank_, codim_, type_>::
rebuild_after_insert_knots(
  const SafeSTLArray<SafeSTLVector<Real>,dim> &knots_to_insert,
  const Grid<dim> &old_grid)
{
  auto prev_ref_space =
    std::const_pointer_cast<RefSpace>(
      std::dynamic_pointer_cast<const RefSpace>(ref_space_->get_space_previous_refinement()));
  Assert(prev_ref_space != nullptr, ExcNullPtr());

  AssertThrow(false,ExcNotImplemented());
  //TODO (martinelli, 25 Sep 2015): fix the refinement for the domain
#if 0
  Assert(this->get_ptr_map_func()->get_function_previous_refinement() != nullptr, ExcNullPtr());
  Assert(this->get_ptr_map_func()->get_function_previous_refinement().unique(), ExcNotUnique());

  this->phys_space_previous_refinement_ =
    PhysicalSpace<dim_,range_,rank_,codim_,type_>::create(
      prev_ref_space,this->get_ptr_map_func()->get_function_previous_refinement());
#endif
}

#endif

#if 0
#ifdef SERIALIZATION
template <int dim_, int range_, int rank_, int codim_, Transformation type_>
template<class Archive>
void
PhysicalSpace<dim_, range_, rank_, codim_, type_>::
serialize(Archive &ar, const unsigned int version)
{
  ar &boost::serialization::make_nvp("PhysicalSpace_base_t",
                                     boost::serialization::base_object<base_t>(*this));

  ar.template register_type<BSplineSpace<dim_,range_,rank_> >();
#ifdef NURBS
  ar.template register_type<NURBSSpace<dim_,range_,rank_> >();
#endif // NURBS

  ar &boost::serialization::make_nvp("ref_space_",ref_space_);

  auto tmp = const_pointer_cast<self_t>(phys_space_previous_refinement_);
  ar &boost::serialization::make_nvp("phys_space_previous_refinement_",tmp);
  phys_space_previous_refinement_ = tmp;
}

///@}
#endif // SERIALIZATION
#endif

IGA_NAMESPACE_CLOSE

#include <igatools/basis_functions/physical_space.inst>


