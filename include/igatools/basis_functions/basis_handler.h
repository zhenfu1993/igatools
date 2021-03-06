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


#ifndef __BASIS_ELEMENT_HANDLER_H_
#define __BASIS_ELEMENT_HANDLER_H_

#include <igatools/base/config.h>
#include <igatools/basis_functions/basis.h>
#include <igatools/base/tuple_utils.h>
#include <igatools/basis_functions/basis_element.h>

IGA_NAMESPACE_OPEN



/**
 *
 * @ingroup handlers
 */
template <int dim,int codim,int range,int rank>
class BasisHandler
{
private:
  using self_t = BasisHandler<dim,codim,range,rank>;

public:
  using Bs = Basis<dim,codim,range,rank>;
  using ElementAccessor = typename Bs::ElementAccessor;
  using ElementIterator = typename Bs::ElementIterator;


  using IndexType = typename Bs::IndexType;


private:
  using eval_pts_variant = QuadVariants<dim>;
  using topology_variant = TopologyVariants<dim>;


protected:
  /** @name Constructors */
  ///@{
  /**
   * Default constructor. Not allowed to be used
   */
  BasisHandler() = delete;


  BasisHandler(const std::shared_ptr<const Bs> &basis);

  /**
   * Copy constructor. Not allowed to be used.
   */
  BasisHandler(const self_t &elem_handler) = delete;

  /**
   * Move constructor. Not allowed to be used.
   */
  BasisHandler(self_t &&elem_handler) = delete;

public:

  /**
   * Destructor.
   */
  virtual ~BasisHandler() = default;

  ///@}


  virtual void set_flags_impl(const topology_variant &topology,
                              const typename basis_element::Flags &flag) = 0;

  template<int sdim>
  void set_flags(const typename basis_element::Flags &flag);

  void set_element_flags(const typename basis_element::Flags &flag);

  virtual void init_cache_impl(ElementAccessor &elem,
                               const eval_pts_variant &quad) const = 0;


  template <int sdim>
  void init_cache(ElementAccessor &elem,
                  const std::shared_ptr<const Quadrature<sdim>> &quad) const;

  template <int sdim>
  void init_cache(ElementIterator &elem,
                  const std::shared_ptr<const Quadrature<sdim>> &quad) const;

  void init_element_cache(ElementAccessor &elem,
                          const std::shared_ptr<const Quadrature<dim>> &quad) const;

  void init_element_cache(ElementIterator &elem,
                          const std::shared_ptr<const Quadrature<dim>> &quad) const;

  void init_face_cache(ElementAccessor &elem,
                       const std::shared_ptr<const Quadrature<(dim > 0) ? dim-1 : 0>> &quad) const;

  void init_face_cache(ElementIterator &elem,
                       const std::shared_ptr<const Quadrature<(dim > 0) ? dim-1 : 0>> &quad) const;

  virtual void fill_cache_impl(const topology_variant &topology,
                               ElementAccessor &elem,
                               const int s_id) const = 0;

  template <int sdim>
  void fill_cache(ElementAccessor &elem, const int s_id) const;

  template <int sdim>
  void fill_cache(ElementIterator &elem, const int s_id) const;

  void fill_element_cache(ElementAccessor &elem) const;

  void fill_element_cache(ElementIterator &elem) const;

  void fill_face_cache(ElementAccessor &elem, const int s_id) const;

  void fill_face_cache(ElementIterator &elem, const int s_id) const;

  typename ElementAccessor::CacheType &
  get_element_cache(ElementAccessor &elem) const;

public:


  virtual void print_info(LogStream &out) const = 0;


  std::shared_ptr<const Bs> get_basis() const;

private:
  std::shared_ptr<const Bs> basis_;





protected:
  SafeSTLArray<typename basis_element::Flags, dim + 1> flags_;



};

IGA_NAMESPACE_CLOSE

#endif // __BASIS_ELEMENT_HANDLER_H_
