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


#ifndef ELLIPTIC_OPERATORS_H_
#define ELLIPTIC_OPERATORS_H_

#include <igatools/base/config.h>
#include <igatools/linear_algebra/dense_matrix.h>


#include <chrono>

IGA_NAMESPACE_OPEN

//TODO: move the definitions of the class member in the .cpp and provide the instantiations


//#define TIME_PROFILING


/**
 * @brief Base class containing the interfaces for the evaluation of some elliptic operators
 * (mass matrix, stiffness matrix, etc.) on a Bezier element.
 *
 * All public methods take as input two BasisElement object
 * (one for the test space and the other for the trial space) and has an output argument that is
 * the local matrix relative to the elliptic operator that is evaluated.
 *
 * @note All public methods are pure virtual: the definitions must be implemented by derived classes.
 *
 * @author M. Martinelli
 * @date 16 Apr 2014
 */
template <int dim_,int codim_,int range_,int rank_>
class EllipticOperators
{
  using self_t = EllipticOperators<dim_,codim_,range_,rank_>;

public:
  /** Type for the element accessor of the <em>test</em> physical space. */
  using ElemTest = BasisElement<dim_,codim_,range_,rank_>;

  /** Type for the element accessor of the <em>trial</em> physical space. */
  using ElemTrial = ElemTest;

  static const int       dim = dim_;
  static const int space_dim = dim_ + codim_;


  using Clock = std::chrono::high_resolution_clock;
  using TimePoint = std::chrono::time_point<Clock>;
  using Duration = std::chrono::duration<Real>;

  /** @name Constructors */
  ///@{
  /**
   * Default constructor.
   * In Debug mode, it checks if the template arguments are consistent.
   */
  EllipticOperators();


  /** Copy constructor. */
  EllipticOperators(const self_t &in) = delete;


  /** Move constructor. */
  EllipticOperators(self_t &&in) = delete;

  /** Destructor. */
  ~EllipticOperators() = default;
  ///@}


  /** @name Assignment operators */
  ///@{
  /** Copy assignment operator. */
  self_t &
  operator=(const self_t &in) = delete;


  /** Move assignment operator. */
  self_t &
  operator=(self_t &&in) = delete;
  ///@}
#if 0
  /**
   * This function evaluates the local (i.e. element-based) matrix \f$ A_e \f$
   * for witch its entries are
   * \f[
        (A_e)_{ij} = \int_{\Omega_e} \phi^{e,\text{test}}_i(x) C(x) \phi^{e,\text{trial}}_j(x) \; d \Omega.
     \f]
   * The matrix \f$ A_e \f$ is commonly referred as <em>local mass-matrix</em>.
   */
  virtual void eval_operator_u_v(
    const ElemTest &elem_test,
    const ElemTrial &elem_trial,
    const ValueVector<Real> &c,
    DenseMatrix &operator_u_v) const
  {
    AssertThrow(false,ExcDeprecated());
  }

  /**
   * This function evaluates the local (i.e. element-based) matrix \f$ A_e \f$
   * for witch its entries are
   * \f[
        (A_e)_{ij} = \int_{\Omega_e} \sum_{r=1}^{sp\_dim} \sum_{s=1}^{sp\_dim}
        \bigl( \nabla \phi^{e,\text{test}}_i \bigr)_r
        \, C_{sr}(x) \,
        \bigl( \nabla \phi^{e,\text{trial}}_j \bigr)_s \; d \Omega.
     \f]
   * The matrix \f$ A_e \f$ is commonly referred as <em>local stiffness-matrix</em>.
   */
  virtual void eval_operator_gradu_gradv(
    const ElemTest &elem_test,
    const ElemTrial &elem_trial,
    const std::vector<TMatrix<space_dim,space_dim>> &coeffs,
    DenseMatrix &operator_gradu_gradv) const
  {
    AssertThrow(false,ExcDeprecated());
  }

  /**
   * This function evaluates the local (i.e. element-based) vector \f$ f_e \f$
   * for witch its entries are
   * \f[
        (f_e)_{i} = \int_{\Omega_e} \phi^{e,\text{test}}_i
        f(x)  \; d \Omega.
     \f]
   */
  virtual void eval_operator_rhs_v(
    const ElemTest &elem_test,
    const ValueVector<typename Basis<dim_,codim_,range_,rank_>::Value> &f,
    DenseVector &operator_rhs_v) const
  {
    AssertThrow(false,ExcDeprecated());
  }


  /**
   * This function evaluates the local (i.e. element-based) matrix \f$ A_e \f$
   * for witch its entries are
   * \f[
        (A_e)_{ij} = \int_{\Omega_e} \sum_{s=1}^{sp\_dim}
        \phi^{e,\text{test}}_i
        \, \beta_{s}(x) \,
        \bigl( \nabla \phi^{e,\text{trial}}_j \bigr)_s \; d \Omega
        = \int_{\Omega_e}
        \phi^{e,\text{test}}_i
        \, \vec{\beta}(x) \, \cdot \,
        \nabla \phi^{e,\text{trial}}_j \; d \Omega .
     \f]
   */
  virtual void eval_operator_gradu_v(
    const ElemTest &elem_test,
    const ElemTrial &elem_trial,
    const ValueVector<typename Basis<dim_,codim_,range_,rank_>::Gradient> &beta,
    DenseMatrix &operator_gradu_v) const
  {
    AssertThrow(false,ExcDeprecated());
  }
#endif


protected:

  /** Returns true if the space for the test functions and the trial functions is the same. */
  bool test_if_same_space(const ElemTest &elem_test,const ElemTrial &elem_trial) const ;

  mutable Duration elapsed_time_operator_u_v_;

  mutable Duration elapsed_time_operator_gradu_gradv_;

  mutable Duration elapsed_time_operator_gradu_v_;


  mutable Duration elapsed_time_initialization_;
  mutable Duration elapsed_time_compute_phi1Dtest_phi1Dtrial_;
  mutable Duration elapsed_time_sum_factorization_;


};

template <int dim_,int codim_,int range_,int rank_>
inline
EllipticOperators<dim_,codim_,range_,rank_>::
EllipticOperators()
  :
  elapsed_time_operator_u_v_(0.0),
  elapsed_time_operator_gradu_gradv_(0.0),
  elapsed_time_operator_gradu_v_(0.0),
  elapsed_time_initialization_(0.0),
  elapsed_time_compute_phi1Dtest_phi1Dtrial_(0.0),
  elapsed_time_sum_factorization_(0.0)
{
//    AssertThrow(false,ExcNotImplemented());
#if 0

  //-----------------------------------------------------------------
  Assert(PhysSpaceTest::dim == PhysSpaceTrial::dim,
         ExcDimensionMismatch(PhysSpaceTest::dim,PhysSpaceTrial::dim));
  Assert(PhysSpaceTest::space_dim == PhysSpaceTrial::space_dim,
         ExcDimensionMismatch(PhysSpaceTest::space_dim,PhysSpaceTrial::space_dim));
  Assert(PhysSpaceTest::range == PhysSpaceTrial::range,
         ExcDimensionMismatch(PhysSpaceTest::range,PhysSpaceTrial::range));
  Assert(PhysSpaceTest::rank == PhysSpaceTrial::rank,
         ExcDimensionMismatch(PhysSpaceTest::rank,PhysSpaceTrial::rank));

//    Assert(PhysSpaceTest::range == 1,ExcDimensionMismatch(PhysSpaceTest::range,1));
  //-----------------------------------------------------------------
#endif
  Assert(rank_ == 1,ExcDimensionMismatch(rank_,1));
}


template <int dim_,int codim_,int range_,int rank_>
bool
EllipticOperators<dim_,codim_,range_,rank_>::
test_if_same_space(const ElemTest &elem_test,const ElemTrial &elem_trial) const
{
  AssertThrow(false,ExcNotImplemented());
#if 0
  //--------------------------------------------------------------------------
  // checks that the mapping used in the test space and in the trial space is the same
  Assert(elem_test.get_physical_space()->get_push_forward()->get_mapping() ==
         elem_trial.get_physical_space()->get_push_forward()->get_mapping(),
         ExcMessage("Test and trial spaces must have the same mapping (and the same grid)!"));
  //--------------------------------------------------------------------------

  //--------------------------------------------------------------------------
  // checks that the grid used in the test space and in the trial space is the same
  Assert(elem_test.get_physical_space()->get_reference_space()->get_grid() ==
         elem_trial.get_physical_space()->get_reference_space()->get_grid(),
         ExcMessage("Test and trial spaces must have the same grid!"));
  //--------------------------------------------------------------------------


  //--------------------------------------------------------------------------
  // checks that the elements on the grid are the same
  Assert(static_cast<const CartesianGridElementAccessor<dim> &>(elem_test.get_ref_space_accessor()) ==
         static_cast<const CartesianGridElementAccessor<dim> &>(elem_trial.get_ref_space_accessor()),
         ExcMessage("Different elements for test space and trial space."));
  //--------------------------------------------------------------------------

  // the test is true only if the element accessors reference the same memory location
  //TODO(MM 08 apr 2014): this is a really crude/raw test. Maybe a better test would be
  // a comparison of iterator index, grid, knots values and multiplicities of the underlying
  // reference space and also push-forward and mapping.
  return (&elem_test==&elem_trial)?true:false;
#endif

  return false;
}





IGA_NAMESPACE_CLOSE


#endif // #ifndef ELLIPTIC_OPERATORS_H_
