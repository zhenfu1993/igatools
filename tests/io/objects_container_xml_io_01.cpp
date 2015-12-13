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
 * Testing the objects container writer and parser
 * author: P. Antolin
 * date: Nov 27, 2015
 *
 */

#include "../tests.h"
#include <igatools/base/objects_container.h>
#include <igatools/geometry/grid_function_lib.h>
#include <igatools/functions/ig_grid_function.h>
#include <igatools/functions/ig_function.h>
#include <igatools/functions/function_lib.h>
#include <igatools/basis_functions/bspline.h>
#include <igatools/basis_functions/nurbs.h>
#include <igatools/io/objects_container_xml_parser.h>
#include <igatools/io/objects_container_xml_writer.h>

using namespace iga;
using namespace std;

template <int dim, int codim, int range, int rank>
void write_container (const string &file_path)
{
  Epetra_SerialComm comm;

  // Creating grid
  auto grid = Grid<dim>::create(4);
  const int deg = 2;

  // Creating spline spaces
  auto space_1 = SplineSpace<dim,range>::create(deg, grid);
  auto space_2 = SplineSpace<dim,1>::create(deg, grid);

  // Creating bspline spaces
  auto ref_basis_1 = BSpline<dim,range>::create(space_1);
  auto ref_basis_2 = BSpline<dim,1>::create(space_2);

  // Creating nurbs space
  auto map_1 = EpetraTools::create_map(*ref_basis_2, "active", comm);
  const auto w = EpetraTools::create_vector(*map_1);
  (*w)[0] = 1.;
  auto wf = IgGridFunction<dim, 1>::create(ref_basis_2, *w, "active");
  auto ref_basis_3 = NURBS<dim,range,rank>::create(ref_basis_1, wf);

  // Creating domain
  auto map_2 = EpetraTools::create_map(*ref_basis_3, "active", comm);
  const auto pts = EpetraTools::create_vector(*map_2);
  (*pts)[0] = 2.;
  auto grid_func = IgGridFunction<dim, dim+codim>::create(ref_basis_3, *pts, "active");
  auto domain = Domain<dim>::create(grid_func, "my_domain");

  // Creating phys space
  auto phys_basis = PhysicalSpaceBasis<dim,range>::create(ref_basis_3, domain);

  auto map_3 = EpetraTools::create_map(*phys_basis, "active", comm);
  auto coeff = EpetraTools::create_vector(*map_3);
  (*coeff)[0] = 3.;
  auto func = IgFunction<dim,codim,range,rank>::create(phys_basis, *coeff);


  // Adding grid functions
  const auto id_func = grid_functions::IdentityGridFunction<dim>::create(grid);

  typename grid_functions::ConstantGridFunction<dim, range>::Value b;
  const auto ct_func = grid_functions::ConstantGridFunction<dim, range>::create(grid, b);

   typename grid_functions::LinearGridFunction<dim, range>::template Derivative<1> A;
   const auto li_func = grid_functions::LinearGridFunction<dim, range>::create(grid, A, b);

  // Adding functions
  typename functions::ConstantFunction<dim, codim, range, rank>::Value b2;
  const auto ct_func_2 = functions::ConstantFunction<dim, codim, range, rank>::create(domain, b2);

   typename functions::LinearFunction<dim, codim, range>::template Derivative<1> A2;
   const auto li_func_2 = functions::LinearFunction<dim, codim, range>::create(domain, A2, b2);

  const auto container = ObjectsContainer::create();
  container->insert_object<Function<dim,0,range,1>>(func);
  container->insert_object<GridFunction<dim,range>>(id_func);
  container->insert_object<GridFunction<dim,range>>(ct_func);
  container->insert_object<GridFunction<dim,range>>(li_func);
  container->insert_object<Function<dim,codim,range,rank>>(ct_func_2);
  container->insert_object<Function<dim,codim,range,rank>>(li_func_2);

  ObjectsContainerXMLWriter::write(file_path, container);

};


void read_container (const string &file_path)
{
  const auto container = ObjectsContainerXMLParser::parse(file_path);
  container->print_info(out);
}


int main()
{
  string file_path;
  OUTSTART
  file_path = "test_2_0_2_1.xml";
  out.begin_item (string("Parsing writing/reading file") + file_path);
  write_container<2, 0, 2, 1>(file_path);
  read_container (file_path);
  out.end_item ();
  OUTEND

  OUTSTART
  file_path = "test_3_0_3_1.xml";
  out.begin_item (string("Parsing writing/reading file") + file_path);
  write_container<3, 0, 3, 1>(file_path);
  read_container (file_path);
  out.end_item ();
  OUTEND

  return 0;
}
