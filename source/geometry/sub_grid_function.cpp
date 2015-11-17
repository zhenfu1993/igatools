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

#include <igatools/geometry/sub_grid_function.h>
#include <igatools/geometry/sub_grid_function_element.h>
#include <igatools/geometry/sub_grid_function_handler.h>

using std::shared_ptr;

IGA_NAMESPACE_OPEN

template<int sdim,int dim,int space_dim>
SubGridFunction<sdim,dim,space_dim>::
SubGridFunction(const SharedPtrConstnessHandler<SupFunc> &sup_func,
                const int s_id,
                const SubGridMap &sub_grid_elem_map,
                const SharedPtrConstnessHandler<GridType> &grid)
  :
  base_t(grid),
  sup_func_(sup_func),
  s_id_(s_id),
  elems_property_("boundary"),
  sub_grid_elem_map_(sub_grid_elem_map)
{
//    LogStream out;
  for (const auto &elems_id : sub_grid_elem_map_)
  {
    id_elems_sub_grid_.insert(elems_id.first);
    id_elems_sup_grid_.insert(elems_id.second);
//      out << "Sub elem ID: " << elems_id.first << "    Sup elem ID: " << elems_id.second << std::endl;
  }
}


template<int sdim,int dim,int space_dim>
GridIterator<GridFunctionElement<sdim,space_dim> >
SubGridFunction<sdim,dim,space_dim>::
cbegin(const PropId &prop) const
{
//    LogStream out;
//    out << "cbegin" <<std::endl;

  auto elem = std::make_unique<SubGridFunctionElement<sdim,dim,space_dim>>
              (
                std::dynamic_pointer_cast<const self_t>(this->shared_from_this()),
                id_elems_sub_grid_.begin(),
                prop);

//    elem->print_info(out);


  //TODO: (martinelli, Nov 16,2015): the iterator is not using the property!
  return GridIterator<GridFunctionElement<sdim,space_dim>>(
           std::move(elem)
         );
}

template<int sdim,int dim,int space_dim>
GridIterator<GridFunctionElement<sdim,space_dim> >
SubGridFunction<sdim,dim,space_dim>::
cend(const PropId &prop) const
{
  //TODO: (martinelli, Nov 16,2015): the iterator is not using the property!
  return GridIterator<GridFunctionElement<sdim,space_dim>>(
           std::move(std::make_unique<SubGridFunctionElement<sdim,dim,space_dim>>
                     (
                       std::dynamic_pointer_cast<const self_t>(this->shared_from_this()),
                       id_elems_sub_grid_.end(),
                       prop))
         );
}


template<int sdim,int dim,int space_dim>
auto
SubGridFunction<sdim,dim,space_dim>::
const_create(const std::shared_ptr<const SupFunc> &func,
             const int s_id,
             const SubGridMap &sub_grid_elem_map,
             const std::shared_ptr<const GridType> &grid)
-> std::shared_ptr<const self_t>
{
  return std::make_shared<self_t>(SharedPtrConstnessHandler<SupFunc>(func),
  s_id,
  sub_grid_elem_map,
  SharedPtrConstnessHandler<GridType>(grid));
}

template<int sdim,int dim,int space_dim>
auto
SubGridFunction<sdim,dim,space_dim>::
create(const std::shared_ptr<const SupFunc> &func,
       const int s_id,
       const SubGridMap &sub_grid_elem_map,
       const std::shared_ptr<GridType> &grid)
-> std::shared_ptr<self_t>
{
  return std::make_shared<self_t>(SharedPtrConstnessHandler<SupFunc>(func),
  s_id,
  sub_grid_elem_map,
  SharedPtrConstnessHandler<GridType>(grid));
}

template<int sdim,int dim,int space_dim>
std::unique_ptr<GridFunctionHandler<sdim,space_dim> >
SubGridFunction<sdim,dim,space_dim>::
create_cache_handler() const
{
  return std::make_unique<SubGridFunctionHandler<sdim,dim,space_dim>>(
           std::dynamic_pointer_cast<const self_t>(this->shared_from_this())
         );
}

template<int sdim,int dim,int space_dim>
std::unique_ptr<GridFunctionElement<sdim,space_dim> >
SubGridFunction<sdim,dim,space_dim>::
create_element(const ListIt &index, const PropId &prop) const
{
  using Elem = SubGridFunctionElement<sdim,dim,space_dim>;
  auto elem = std::make_unique<Elem>(
                std::dynamic_pointer_cast<const self_t>(this->shared_from_this()),
                index, prop);
  Assert(elem != nullptr, ExcNullPtr());

  return elem;
}


template<int sdim,int dim,int space_dim>
void
SubGridFunction<sdim,dim,space_dim>::
rebuild_after_insert_knots(
  const SafeSTLArray<SafeSTLVector<double>, sdim> &new_knots,
  const GridType &old_grid)
{
  AssertThrow(false,ExcNotImplemented());
}



template<int sdim,int dim,int space_dim>
void
SubGridFunction<sdim,dim,space_dim>::
print_info(LogStream &out) const
{
  out.begin_item("SubGridFunction<" + std::to_string(sdim) + ">");

  out.begin_item("Sup. function:");
  sup_func_->print_info(out);
  out.end_item();


  out << "Sub-element topology ID: " << s_id_ << std::endl;

  out.begin_item("Sub elements ID:");
  id_elems_sub_grid_.print_info(out);
  out.end_item();

  out.end_item();
}


template<int sdim,int dim,int space_dim>
auto
SubGridFunction<sdim,dim,space_dim>::
get_sup_grid_function() const -> std::shared_ptr<const SupFunc>
{
  return sup_func_.get_ptr_const_data();
}


template<int sdim,int dim,int space_dim>
const SafeSTLSet<typename Grid<sdim>::IndexType> &
SubGridFunction<sdim,dim,space_dim>::
get_id_elems_sub_grid() const
{
  return id_elems_sub_grid_;
}

template<int sdim,int dim,int space_dim>
const SafeSTLSet<typename Grid<dim>::IndexType> &
SubGridFunction<sdim,dim,space_dim>::
get_id_elems_sup_grid() const
{
  return id_elems_sup_grid_;
}

template<int sdim,int dim,int space_dim>
const typename Grid<dim>::IndexType &
SubGridFunction<sdim,dim,space_dim>::
get_sup_element_id(const typename Grid<sdim>::IndexType &sub_elem_id) const
{
  return sub_grid_elem_map_.at(sub_elem_id);
}

template<int sdim,int dim,int space_dim>
auto
SubGridFunction<sdim,dim,space_dim>::
get_sub_grid_elem_map() const -> const SubGridMap &
{
  return sub_grid_elem_map_;
}

template<int sdim,int dim,int space_dim>
const SafeSTLSet<typename Grid<sdim>::IndexType> &
SubGridFunction<sdim,dim,space_dim>::
get_elements_with_property(const PropId &elems_property) const
{
  //TODO: (martinelli, Nov 16,2015): the property is not used!
  return id_elems_sub_grid_;
}

IGA_NAMESPACE_CLOSE

#include <igatools/geometry/sub_grid_function.inst>
