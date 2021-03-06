//-+--------------------------------------------------------------------
// Igatools a general purpose Isogeometric analysis library.
// Copyright (C) 2012-2016  by the igatools authors (see authors.txt).
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

/*
 *  Test for the BSpline element iterator derivatives values
 *  in a non homogenous range
 *  author: pauletti
 *  date: Aug 21, 2014
 *
 */

#include "../tests.h"

#include <igatools/base/quadrature_lib.h>
#include <igatools/basis_functions/bspline_element.h>
#include <igatools/basis_functions/bspline_handler.h>

const SafeSTLArray<basis_element::Flags, 3> der_flag =
{
  basis_element::Flags::value,
  basis_element::Flags::gradient,
  basis_element::Flags::hessian
};

template <int der, int dim, int range=1, int rank=1>
void elem_derivatives(const int n_knots,
                      typename SplineSpace<dim,range,rank>::DegreeTable &deg)
{
  OUTSTART

  using Basis = BSpline<dim, range, rank>;
  using EndBehaviour = typename Basis::EndBehaviour;
  using EndBehaviourTable = typename Basis::EndBehaviourTable;
  auto grid  = Grid<dim>::create(n_knots);

//  typename Basis::PeriodicityTable periodic((typename Basis::Periodicity(SafeSTLArray<bool, dim>(false))));
  EndBehaviour eb(BasisEndBehaviour::interpolatory);
  EndBehaviourTable ebt(eb);
  auto int_mult = SplineSpace<dim,range,rank>::get_multiplicity_from_regularity(InteriorReg::maximum,
                  deg, grid->get_num_intervals());
  auto space = SplineSpace<dim,range,rank>::create(deg,grid,int_mult);
  auto basis = Basis::create(space, ebt);

  auto flag = der_flag[der];
  auto quad = QGauss<dim>::create(2);


  auto elem = basis->begin();
  auto end  = basis->end();

  auto elem_handler = basis->create_cache_handler();
  elem_handler->template set_flags<dim>(flag);
  elem_handler->init_element_cache(elem,quad);

  using Elem = typename Basis::ElementAccessor;
  using _Value = typename Elem::_Value;
  using _Gradient = typename Elem::_Gradient;
  using _Hessian = typename Elem::_Hessian;

  out.begin_item("Derivative order : " +std::to_string(der));
  for (; elem != end; ++elem)
  {
    out << "Element index : " << elem->get_index() << std::endl;
    elem_handler->fill_element_cache(elem);
    if (der == 0)
    {
      elem->template get_basis_data<_Value,dim>(0,DofProperties::active).print_info(out);
    }
    else if (der == 1)
    {
      elem->template get_basis_data<_Gradient,dim>(0,DofProperties::active).print_info(out);
    }
    else if (der == 2)
    {
      elem->template get_basis_data<_Hessian,dim>(0,DofProperties::active).print_info(out);
    }
    else
      AssertThrow(false,ExcMessage("Invalid derivative order."));
    //*/
  }
  out.end_item();

  OUTEND
}


int main()
{
  out.depth_console(10);

  const int values = 0;
  const int grad   = 1;
  const int hess   = 2;

  const int n_knots = 3;
  typename SplineSpace<2,2,1>::DegreeTable deg1 = { {{3,2}}, {{2,3}} };
  elem_derivatives<values, 2, 2>(n_knots, deg1);
  elem_derivatives<grad,   2, 2>(n_knots, deg1);
  elem_derivatives<hess,   2, 2>(n_knots, deg1);

  typename SplineSpace<3,3,1>::DegreeTable
  deg2 = { {{3,2,2}}, {{2,3,2}}, {{2,2,3}} };
  elem_derivatives<values, 3, 3>(n_knots, deg2);
  elem_derivatives<grad,   3, 3>(n_knots, deg2);
  elem_derivatives<hess,   3, 3>(n_knots, deg2);
//*/
}

