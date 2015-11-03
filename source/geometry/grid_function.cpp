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

#include <igatools/geometry/grid_function.h>
#include <igatools/geometry/grid_function_element.h>

using std::shared_ptr;

IGA_NAMESPACE_OPEN

template<int dim_, int space_dim_>
GridFunction<dim_, space_dim_>::
GridFunction(const SharedPtrConstnessHandler<GridType> &grid)
  :
  grid_(grid)
{
//  Assert(grid_ != nullptr, ExcNullPtr());
}






template<int dim_, int space_dim_>
auto
GridFunction<dim_, space_dim_>::
get_grid() const -> std::shared_ptr<const GridType>
{
  return grid_.get_ptr_const_data();
}


template<int dim_, int space_dim_>
auto
GridFunction<dim_, space_dim_>::
create_cache_handler() const
-> std::unique_ptr<ElementHandler>
{
  return std::make_unique<ElementHandler>(this->shared_from_this());
}



template<int dim_, int space_dim_>
auto
GridFunction<dim_, space_dim_>::
create_element(const ListIt &index, const PropId &prop) const
-> std::unique_ptr<ElementAccessor>
{
  using Elem = ElementAccessor;
  auto elem = std::make_unique<Elem>(this->shared_from_this(), index, prop);
  Assert(elem != nullptr, ExcNullPtr());

  return elem;
}




#if 0

template<int dim_, int space_dim_>
auto
GridFunction<dim_, space_dim_>::
create_element(const ListIt &index, const PropId &prop)
-> std::unique_ptr<ElementAccessor>
{
  using Elem = ElementAccessor;
  auto elem = std::make_unique<Elem>(this->shared_from_this(), index, prop);
  Assert(elem != nullptr, ExcNullPtr());

  return elem;
}


template<int dim_, int space_dim_>
auto
GridFunction<dim_, space_dim_>::
begin(const PropId &prop) -> ElementIterator
{
  return ElementIterator(this->shared_from_this(),
  grid_->get_elements_with_property(prop).begin(), prop);
}


template<int dim_, int space_dim_>
auto
GridFunction<dim_, space_dim_>::
end(const PropId &prop) -> ElementIterator
{
  return ElementIterator(this->shared_from_this(),
  grid_->get_elements_with_property(prop).end(),
  prop);
}
#endif



template<int dim_, int space_dim_>
auto
GridFunction<dim_, space_dim_>::
begin(const PropId &prop) const -> ElementIterator
{
  return this->cbegin(prop);
}



template<int dim_, int space_dim_>
auto
GridFunction<dim_, space_dim_>::
end(const PropId &prop) const -> ElementIterator
{
  return this->cend(prop);
}



template<int dim_, int space_dim_>
auto
GridFunction<dim_, space_dim_>::
cbegin(const PropId &prop) const -> ElementIterator
{
  return ElementIterator(this->shared_from_this(),
                         grid_->get_elements_with_property(prop).begin(),
                         prop);
}



template<int dim_, int space_dim_>
auto
GridFunction<dim_, space_dim_>::
cend(const PropId &prop) const -> ElementIterator
{
  return ElementIterator(this->shared_from_this(),
                         grid_->get_elements_with_property(prop).end(),
                         prop);
}


#ifdef MESH_REFINEMENT
template<int dim_, int space_dim_>
boost::signals2::connection
GridFunction<dim_, space_dim_>::
connect_insert_knots(const typename Grid<dim_>::SignalInsertKnotsSlot &subscriber)
{
  return grid_.get_ptr_data()->connect_insert_knots(subscriber);
}

template<int dim_, int space_dim_>
void
GridFunction<dim_,space_dim_>::
create_connection_for_insert_knots(const std::shared_ptr<self_t> &grid_function)
{
  Assert(grid_function != nullptr, ExcNullPtr());
  Assert(&(*grid_function) == &(*this), ExcMessage("Different objects."));

  auto func_to_connect =
    std::bind(&self_t::rebuild_after_insert_knots,
              grid_function.get(),
              std::placeholders::_1,
              std::placeholders::_2);

  using SlotType = typename Grid<dim_>::SignalInsertKnotsSlot;
  this->connect_insert_knots(SlotType(func_to_connect).track_foreign(grid_function));
}


#endif // MESH_REFINEMENT


IGA_NAMESPACE_CLOSE

#include <igatools/geometry/grid_function.inst>

