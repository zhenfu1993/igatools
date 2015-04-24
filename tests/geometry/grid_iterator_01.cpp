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
 *  Test for the CartesianGrid Iterator init_all_caches(...)
 *
 *  author: pauletti
 *  date: 2014-10-31
 *
 */

#include "../tests.h"

#include <igatools/base/quadrature_lib.h>
#include <igatools/geometry/cartesian_grid.h>
#include <igatools/geometry/grid_element_handler.h>
#include <igatools/geometry/cartesian_grid_element.h>


template <int dim>
void elem_measure(const int n_knots = 5)
{
    OUTSTART

    auto grid = CartesianGrid<dim>::create(n_knots);
    auto flag = ValueFlags::measure|ValueFlags::w_measure;

    QGauss<dim> quad(2);
    QGauss<dim-1> quad_face(2);
    const int face_id = 0;

    GridElementHandler<dim> cache(grid);
    cache.template reset<dim>(flag, quad);
    auto elem = grid->begin();
    cache.init_element_cache(elem);

    cache.template reset<dim-1>(flag, quad_face);
    cache.init_face_cache(elem);
//    cache.template init_cache<dim-1>(elem);

//    cache.init_all_caches(elem);

    for (; elem != grid->end(); ++elem)
    {
        elem->print_info(out);

        cache.fill_element_cache(elem);
        out << "Measure: " << elem->template get_measure<dim>(0) << endl;
        out.begin_item("Weighted Measure:");
        elem->template get_w_measures<dim>(0).print_info(out);
        out.end_item();

        cache.fill_face_cache(elem, face_id);
        out << "Sub elem Measure: " << elem->template get_measure<dim-1>(face_id) << endl;
        out.begin_item("Weighted Measure:");
        elem->template get_w_measures<dim-1>(face_id).print_info(out);
        out.end_item();
    }

    OUTEND
}



int main()
{
    elem_measure<1>();
    elem_measure<2>();
    elem_measure<3>();

    return  0;
}
