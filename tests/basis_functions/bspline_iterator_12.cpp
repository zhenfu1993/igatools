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

/*
 *  Test for the BSplineSpace ElementHandler functions
 *  init_element_cache() and fill_element_cache()
 *
 *  author: pauletti
 *  date: Aug 21, 2014
 *
 */

#include "../tests.h"
#include <igatools/base/quadrature_lib.h>
#include <igatools/basis_functions/bspline_element.h>
#include <igatools/basis_functions/bspline_element_handler.h>

template <int dim, int range=1, int rank=1>
void space_cache_init_fill_elem(
  const ValueFlags flag,
  const int n_knots = 5, const int deg=1)
{
  OUTSTART

  using Space = BSplineSpace<dim, range, rank>;
  auto grid  = CartesianGrid<dim>::create(n_knots);
  auto space = Space::create(deg, grid);

  auto quad = QGauss<dim>(2);

  using ElemHandler = typename Space::ElementHandler;
  auto value_handler = ElemHandler::create(space);
  value_handler->reset(flag, quad);
  auto elem = space->begin();

  value_handler->init_element_cache(elem);
  value_handler->fill_element_cache(elem);
  elem->print_cache_info(out);

  OUTEND
}



int main()
{
  out.depth_console(10);
  space_cache_init_fill_elem<1>(ValueFlags::value, 2);
  space_cache_init_fill_elem<1>(ValueFlags::gradient, 3);
  space_cache_init_fill_elem<1>(ValueFlags::hessian, 3);

  space_cache_init_fill_elem<2>(ValueFlags::value);
  space_cache_init_fill_elem<2>(ValueFlags::gradient);
  space_cache_init_fill_elem<2>(ValueFlags::hessian);

  return  0;
}
