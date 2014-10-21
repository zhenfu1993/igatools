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

/*
 *  Test for Function class, as a prototype for an spline function
 *  author: pauletti
 *  date: Oct 11, 2014
 */

#include "../tests.h"

#include <igatools/base/ig_function.h>
#include <igatools/base/quadrature_lib.h>
#include <igatools/base/function_element.h>
#include <igatools/basis_functions/bspline_space.h>
#include <igatools/basis_functions/bspline_element_accessor.h>


template<int dim, int range>
void test()
{
    using Space = BSplineSpace<dim>;
    using Function = IgFunction<Space>;

    auto flag = ValueFlags::value | ValueFlags::gradient |
                ValueFlags::hessian;
    auto quad = QGauss<dim>(2);
    auto grid = CartesianGrid<dim>::create(3);
    const int deg = 1;
    auto space = Space::create(deg, grid);
    typename Function::CoeffType coeff(space->get_num_basis());
    coeff(0) = 1.;
    shared_ptr<NewFunction<dim,0,range,1>> F
                                        = std::make_shared<Function>(flag, quad, space, coeff);


    GridForwardIterator<FunctionElement<dim,0,range,1>> elem(grid, 0);
    GridForwardIterator<FunctionElement<dim,0,range,1>> end(grid, IteratorState::pass_the_end);

    F->init_elem(elem);
    for (; elem != end; ++elem)
    {
        F->fill_elem(elem);
//        elem->get_points().print_info(out);
//        out << endl;
        elem->get_values().print_info(out);
        out << endl;
        elem->get_gradients().print_info(out);
        out << endl;
        elem->get_hessians().print_info(out);
        out << endl;
    }
}


int main()
{
    test<2,1>();
//    test<3,3>();

    return 0;
}
