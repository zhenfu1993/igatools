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

/**
 *  @file
 *  @brief  TensorSize serialization
 *  @author  martinelli
 *  @date 2015-09-08
 */

#include "../tests.h"
#include <igatools/utils/tensor_size.h>


template <int N>
void ts_serialization()
{
  OUTSTART

  using T = TensorSize<N>;

  T size(3);

  const std::string filename = "ts.xml";

  {
    ofstream os(filename);
    OArchive archive(os);
    archive << size;
  }
  out.begin_item("TensorSize<" + std::to_string(N) + "> before serialization");
  size.print_info(out);
  out.end_item();

  T size_tmp;
  {
    ifstream is(filename);
    IArchive archive(is);
    archive >> size_tmp;
  }
  out.begin_item("TensorSize<" + std::to_string(N) + "> after serialization");
  size_tmp.print_info(out);
  out.end_item();

  OUTEND
}



int main()
{
  ts_serialization<0>();
  ts_serialization<1>();
  ts_serialization<2>();
  ts_serialization<3>();
  return 0;
}
