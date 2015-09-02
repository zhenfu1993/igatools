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


#ifndef SPACE_ELEMENT_BASE_H_
#define SPACE_ELEMENT_BASE_H_

#include <igatools/base/config.h>
#include <igatools/geometry/grid_element.h>
//#include <igatools/basis_functions/space.h>


IGA_NAMESPACE_OPEN




template <int> class SpaceBase;

//template <class Accessor> class CartesianGridIterator;

/**
 *
 * @ingroup serializable
 */
template <int dim>
class SpaceElementBase
{
private:
  using self_t = SpaceElementBase<dim>;

public:
  using Grid = CartesianGrid<dim>;
  using IndexType = typename Grid::IndexType;
  using List = typename Grid::List;
  using ListIt = typename Grid::ListIt;

  /** @name Constructors */
  ///@{
protected:
  /**
   * Default constructor. It does nothing but it is needed for the
   * <a href="http://www.boost.org/doc/libs/release/libs/serialization/">boost::serialization</a>
   * mechanism.
   */
  SpaceElementBase() = default;

public:
  /**
   * Constructs an accessor to element with index pointed by the iterator <tt>index</tt> of a
   * function space.
   */
  SpaceElementBase(const std::shared_ptr<const SpaceBase<dim>> &space,
                   const ListIt &index,
                   const PropId &prop = ElementProperties::active);



  /**
   * Copy constructor.
   * It can be used with different copy policies (i.e. deep copy or shallow copy).
   * The default behaviour (i.e. using the proper interface of a classic copy constructor)
   * uses the deep copy.
   */
  SpaceElementBase(const self_t &elem,
                   const CopyPolicy &copy_policy = CopyPolicy::deep);

  /**
   * Move constructor.
   */
  SpaceElementBase(self_t &&elem) = default;

  /**
   * Destructor.
   */
  ~SpaceElementBase() = default;
  ///@}


  /**
   * Return a reference to the GridElement.
   */
  ConstGridElement<dim> &get_grid_element();

  /**
   * Return a const-reference to the GridElement.
   */
  const ConstGridElement<dim> &get_grid_element() const;

  void print_info(LogStream &out) const;

  void print_cache_info(LogStream &out) const;

  void
  copy_from(const SpaceElementBase<dim> &elem,const CopyPolicy &copy_policy);


  /**
   * @name Functions for getting information about the element connectivity.
   */
  ///@{
  /**
   * Returns the global dofs of the local (non zero) basis functions
   * on the element.
   *
   * @note The dofs can be filtered invoking the function with the argument @p dof_property.
   * If @p dof_property is equal to DofProperties::active, then no filter is applied.
   *
   * For example:
   * \code
     auto loc_to_glob_all = elem->get_local_to_global(DofProperties::active);
     // loc_to_glob_all[0] is the global id of the first basis function on the element
     // loc_to_glob_all[1] is the global id of the second basis function on the element
     // ...
     auto loc_to_glob_active = elem->get_local_to_global(DofProperties::active);
     // loc_to_glob_active[0] is the global id of the first active basis function on the element
     // loc_to_glob_active[1] is the global id of the second active basis function on the element
     // ...
    \endcode
   *
   */
  SafeSTLVector<Index>
  get_local_to_global(const std::string &dofs_property) const;

  /**
   * Returns the patch dofs of the local (non zero) basis functions
   * on the element.
   *
   * @note The dofs can be filtered invoking the function with the argument @p dof_property.
   * If @p dof_property is equal to DofProperties::active, then no filter is applied.
   *
   */
  SafeSTLVector<Index>
  get_local_to_patch(const std::string &dofs_property) const;


  SafeSTLVector<Index>
  get_local_dofs(const std::string &dofs_property) const;


  /**
   *  Number of non zero basis functions with the given @p dofs_property,
   *  over the current element.
   */
  Size get_num_basis(const std::string &dofs_property) const;
  ///@}

#if 0
  /** @name Functions/operators for moving the element in the CartesianGrid used to build the Space.*/
  ///@{
  /**
   * Sets the index of the element using the flatten representation.
   * @note This function also updates the index for the tensor representation.
   * @warning This may be a dangerous function, be careful when using it
   * as it is easy to use incorrectly. Only use it if you know what you
   * are doing.
   */
  virtual void move_to(const Index flat_index) ;
  ///@}
#endif


public:

  /**
   * @name Comparison operators.
   *
   * @brief The comparison operators compares the <em>position</em> of the element in the grid.
   *
   * @warning To be comparable, two SpaceElementBase objects must be defined on the same space
   * (and therefore on the same grid),
   * otherwise an assertion will be raised (in Debug mode).
   */
  ///@{
  /** Returns TRUE if the two elements have the same index on the grid. */
  bool operator==(const self_t &a) const;


  /** Returns TRUE if the two elements have different indices on the grid. */
  bool operator!=(const self_t &a) const;

  /**
   * Returns TRUE if the the index of the element on the left of the operator <tt> < </tt>
   * is smaller than the the index of the element on the right.
   * */
  bool operator<(const self_t &a) const;

  /**
   * Returns TRUE if the the index of the element on the left of the operator <tt> < </tt>
   * is bigger than the the index of the element on the right.
   * */
  bool operator>(const self_t &a) const;
  ///@}

  virtual typename List::iterator &operator++()
  {
    return ++(*grid_elem_);
  }



  /** Returns the index of the element. */
  IndexType get_index() const;

  /** Return the cartesian grid from which the element belongs.*/
  std::shared_ptr<const CartesianGrid<dim> > get_grid() const;

  /**
   * Test if the element has a boundary face.
    */
  template<int k = (dim > 0) ? (dim-1) : 0 >
  bool is_boundary() const
  {
    return grid_elem_->is_boundary();
  }

  /**
   * Test if the face identified by @p face_id on the current element is on the
   * boundary of the cartesian grid.
   */
  template<int k = (dim > 0) ? (dim-1) : 0>
  bool is_boundary(const Index sub_elem_id) const
  {
    return grid_elem_->is_boundary(sub_elem_id);
  }


private:
  std::shared_ptr<ConstGridElement<dim>> grid_elem_;

  std::shared_ptr<const SpaceBase<dim>> space_;


#ifdef SERIALIZATION
  /**
   * @name Functions needed for boost::serialization
   * @see <a href="http://www.boost.org/doc/libs/release/libs/serialization/">boost::serialization</a>
   */
  ///@{
  friend class boost::serialization::access;

  template<class Archive>
  void
  serialize(Archive &ar, const unsigned int version);
  ///@}
#endif // SERIALIZATION




public:

};


IGA_NAMESPACE_CLOSE



#endif // #ifndef SPACE_ELEMENT_BASE_H_

