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
 *  Test for Vector class
 *
 *  author: pauletti
 *  date: Apr 5, 2013
 *
 */

#include "../tests.h"

#include <igatools/linear_algebra/epetra_vector.h>
#include <igatools/functions/ig_grid_function.h>
#include <igatools/functions/function_element.h>
#include <igatools/basis_functions/bspline.h>
#include <igatools/basis_functions/bspline_element.h>

using namespace EpetraTools;

void non_contig_indices()
{
  OUTSTART



  std::set<Index> dofs = {1, 3, 5};
  const SafeSTLVector<Index> dofs_vec(dofs.begin(), dofs.end());
  Epetra_SerialComm comm;
  auto map = std::make_shared<Map>(-1, dofs_vec.size(), dofs_vec.data(), 0, comm);
  auto vec = create_vector(*map);
  out.begin_item("vec");
  vec->print_info(out);
  out.end_item();

  const int dim = 1;
  using Basis = BSpline<dim>;
  using Function = IgGridFunction<dim,1>;
  auto grid = Grid<dim>::create(5);
  const int deg = 1;
  auto space = SplineSpace<dim>::create(deg, grid);
  auto basis = Basis::create(space);
  auto dof_distribution = space->get_dof_distribution();
  dof_distribution->set_all_dofs_property_status(DofProperties::active,false);
  dof_distribution->set_dof_property_status(DofProperties::active,dofs,true);

  auto coeff = create_vector(*basis, DofProperties::active,comm);
  out.begin_item("coeff");
  coeff->print_info(out);
  out.end_item();

  auto F = Function::create(basis, *coeff);
  auto vec1 = F->get_coefficients();
  out.begin_item("vec1");
  vec1.print_info(out);
  out.end_item();

  auto vec2 = F->get_coefficients();
  out.begin_item("vec2");
  vec2.print_info(out);
  out.end_item();

  OUTEND
}



int main()
{
  non_contig_indices();
  return  0;
}
