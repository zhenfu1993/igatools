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

#include <igatools/base/quadrature_lib.h>
#include <igatools/base/exceptions.h>


#include <limits>



IGA_NAMESPACE_OPEN

namespace
{
template <typename number>
number abs(const number a)
{
  return ((a>0) ? a : -a);
}

void eval_legendre_polynomial(const int n,
                              const long double x,
                              long double &P,
                              long double &d1P,
                              long double &d2P,
                              long double &d3P
                             )
{
  // compute L_n (x)
  P = 1.;
  long double P_prev = 0.;
  long double P_tmp = 0.;
  for (int j=0; j<n; ++j)
  {
    P_tmp = P_prev;
    P_prev = P;
    P = ((2.*j+1.) * x * P_prev - j * P_tmp) / (j+1) ;
  }

  const long double one_over_den = 1.0 / (1.0 - x*x) ;

  d1P = (P_prev - x*P) * n * one_over_den ;
  d2P = (2.0 *x*d1P - n*(n+1)*P) * one_over_den ;
  d3P = (2.0 *x*d2P - (n*(n+1)-2)*d1P) * one_over_den ;
}


long double eval_newton_update(const long double &f, const long double &df)
{
  return - f / df ;
}

long double eval_halley_update(const long double &f, const long double &d1f, const long double &d2f)
{
  return -f / (d1f - 0.5 * d2f * f / d1f)  ;
}


// Find the 1D Gauss-Legendre quadrature rule with n points
void gauss_legendre_quadrature(const int n,
                               SafeSTLVector<Real> &points,
                               SafeSTLVector<Real> &weights)
{
  Assert(n>0, ExcMessage("n points should be bigger than 0"))

  const int m = (n+1)/2;

  // tolerance for the Newton iteration below. we need to make it
  // adaptive since on some machines (for example PowerPC) long
  // double is the same as double -- in that case we can only get to
  // a certain multiple of the accuracy of double there, while on
  // other machines we'd like to go further down.  The situation is
  // complicated by the fact that even if long double exists and is
  // described by std::numeric_limits, we may not actually get the
  // additional precision. One case where this happens is on x86,
  // where one can set hardware flags that disable long double
  // precision even for long double variables. these flags are not
  // usually set, but for example matlab sets them and this then
  // breaks deal.II code that is run as a subroutine to matlab...  a
  // similar situation exists, btw, when running programs under
  // valgrind up to and including at least version 3.3: valgrind's
  // emulator only supports 64 bit arithmetic, even for 80 bit long
  // double.
  const long double
  long_double_eps = static_cast<long double>(std::numeric_limits<long double>::epsilon()),
  double_eps      = static_cast<long double>(std::numeric_limits<double>::epsilon());

  // now check whether long Real is more accurate than Real, and
  // set tolerances accordingly. generate a one that Really is
  // generated at run-time and is not optimized away by the
  // compiler. that makes sure that the tolerance is set at run-time
  // with the current behavior, not at compile-time (not doing so
  // leads to trouble with valgrind for example).
  volatile long double runtime_one = 1.0;
  const long double tolerance
    = (runtime_one + long_double_eps != runtime_one
       ?
       std::max(double_eps / 100, long_double_eps * 5)
       :
       double_eps * 5
      );


  for (int i=1; i<=m; ++i)
  {
    long double z = std::cos(numbers::PI * (i-.25)/(n+.5));

    long double P;
    long double d1P ;
    long double d2P ;
    long double d3P ;

    long double dz = 0.0 ;

    // root-finding iteration
    do
    {
      eval_legendre_polynomial(n, z, P, d1P, d2P, d3P) ;
      dz = eval_newton_update(P, d1P) ;
      z = z + dz ;
    }
    while (abs(dz) > tolerance);

    const Real x = .5*z;
    points[i-1] = .5-x ;
    points[n-i] = .5+x ;

    const Real w = 1./((1.-z*z)*d1P*d1P);
    weights[i-1] = w ;
    weights[n-i] = w ;

  }
}



// Find the 1D Gauss-Lobatto quadrature rule with n points
void gauss_lobatto_quadrature(const int n,
                              SafeSTLVector<Real> &points,
                              SafeSTLVector<Real> &weights)
{

  if (n == 0)
    return;

  const int m = (n+1)/2;

  // tolerance for the Newton iteration below. we need to make it
  // adaptive since on some machines (for example PowerPC) long
  // double is the same as double -- in that case we can only get to
  // a certain multiple of the accuracy of double there, while on
  // other machines we'd like to go further down.  The situation is
  // complicated by the fact that even if long double exists and is
  // described by std::numeric_limits, we may not actually get the
  // additional precision. One case where this happens is on x86,
  // where one can set hardware flags that disable long double
  // precision even for long double variables. these flags are not
  // usually set, but for example matlab sets them and this then
  // breaks deal.II code that is run as a subroutine to matlab...  a
  // similar situation exists, btw, when running programs under
  // valgrind up to and including at least version 3.3: valgrind's
  // emulator only supports 64 bit arithmetic, even for 80 bit long
  // doubles.
  const long double
  long_double_eps = static_cast<long double>(std::numeric_limits<long double>::epsilon()),
  double_eps      = static_cast<long double>(std::numeric_limits<double>::epsilon());

  // now check whether long double is more accurate than double, and
  // set tolerances accordingly. generate a one that Really is
  // generated at run-time and is not optimized away by the
  // compiler. that makes sure that the tolerance is set at run-time
  // with the current behavior, not at compile-time (not doing so
  // leads to trouble with valgrind for example).
  volatile long double runtime_one = 1.0;
  const long double tolerance
    = (runtime_one + long_double_eps != runtime_one
       ?
       std::max(double_eps / 100, long_double_eps * 5)
       :
       double_eps * 5
      );


  points[0] = 0 ;
  points[n-1] = 1.0 ;

  const double w = 1./(n*(n-1));
  weights[0] = w ;
  weights[n-1] = w ;
  for (int i=2; i<=m; ++i)
  {
    long double z = (1- (3.0/8.0) * (n-2.0) / pow(n-1,3)) * std::cos(numbers::PI*(i-0.75)/(n-0.75));

    long double P;
    long double d1P ;
    long double d2P ;
    long double d3P ;
    long double dz = 0.0 ;

    // root-finding iteration
    do
    {
      eval_legendre_polynomial(n-1, z, P, d1P, d2P, d3P) ;
      dz = eval_halley_update(d1P, d2P, d3P) ;
      z = z + dz ;
    }
    while (abs(dz) > tolerance);

    const Real x = .5*z;
    points[i-1] = .5-x ;
    points[n-i] = .5+x ;

    const Real w = 1./(n*(n-1)*P*P);
    weights[i-1] = w ;
    weights[n-i] = w ;
  }
}


// Uniform quadrature in [0,1] with n points.
// The points coordinate are the uniform subdivision of the [0,1] interval in n points
// The weights are all equal to one divided by the number of points.
void uniform_quadrature(const int n,
                        SafeSTLVector<Real> &points,
                        SafeSTLVector<Real> &weights)
{
  Assert(n >= 2, ExcLowerRange(n, 2)) ;

  const Real h = 1.0 / (n - 1) ;

  const Real w = 1.0 / n ;

  for (int iPt = 0 ; iPt < n ; iPt++)
  {
    points [ iPt ] = iPt * h ;
    weights[ iPt ] = w ;
  }
}

} // end anonymous namespace


//--------------------------------------------------------------------------------------------------
template< int dim >
QGauss<dim>::
QGauss(const Size num_points) :
  QGauss<dim>(TensorSize<dim>(num_points))
{}



template< int dim >
QGauss< dim >::
QGauss(const TensorSize<dim> num_points) :
  Quadrature<dim>(num_points, &gauss_legendre_quadrature)
{}


template< int dim >
std::shared_ptr< QGauss< dim > >
QGauss< dim >::create(const Size num_points)
{
  return (std::make_shared< QGauss<dim> >(num_points)) ;
}


template< int dim >
std::shared_ptr< QGauss< dim > >
QGauss< dim >::create(const TensorSize<dim> num_points)
{
  return (std::make_shared< QGauss<dim> >(num_points)) ;
}


template< int dim >
std::shared_ptr< const QGauss< dim > >
QGauss< dim >::const_create(const Size num_points)
{
  return (std::make_shared< QGauss<dim> >(num_points)) ;
}


template< int dim >
std::shared_ptr< const QGauss< dim > >
QGauss< dim >::const_create(const TensorSize<dim> num_points)
{
  return (std::make_shared< QGauss<dim> >(num_points)) ;
}


template< int dim >
QGaussLobatto< dim >::
QGaussLobatto(const Size num_points)
  :
  QGaussLobatto(TensorSize<dim>(num_points))
{}



template< int dim >
QGaussLobatto< dim >::
QGaussLobatto(const TensorSize<dim> num_points) :
  Quadrature< dim >(num_points,&gauss_lobatto_quadrature)
{
#ifndef NDEBUG
  // Gauss-Lobatto schemes needs at least 2 points in each direction
  for (int i = 0; i < dim; ++i)
    Assert(num_points[i] >= 2, ExcLowerRange(num_points[i], 2)) ;
#endif
}



template< int dim >
std::shared_ptr< QGaussLobatto< dim > >
QGaussLobatto< dim >::create(const Size num_points)
{
  return (std::make_shared< QGaussLobatto< dim > >(num_points)) ;
}



template< int dim >
std::shared_ptr< QGaussLobatto< dim > >
QGaussLobatto< dim >::create(const TensorSize<dim> num_points)
{
  return (std::make_shared< QGaussLobatto< dim > >(num_points)) ;
}



template< int dim >
QUniform<dim>::
QUniform(const Size num_points)
  :
  QUniform(TensorSize<dim>(num_points))
{}



template< int dim >
QUniform<dim>::QUniform(const TensorSize<dim> num_points)
  :
  Quadrature< dim >(num_points,&uniform_quadrature)
{
#ifndef NDEBUG
  // Uniform schemes needs at least 2 points in each direction
  for (int i = 0; i < dim; ++i)
    Assert(num_points[i] >= 2, ExcLowerRange(num_points[i], 2)) ;
#endif
}



template< int dim >
std::shared_ptr< QUniform< dim > >
QUniform< dim >::create(const Size num_points)
{
  return (std::make_shared< QUniform<dim> >(num_points)) ;
}


template< int dim >
std::shared_ptr< QUniform< dim > >
QUniform< dim >::create(const TensorSize<dim> num_points)
{
  return (std::make_shared< QUniform<dim> >(num_points)) ;
}



template< int dim >
QTrapez<dim>::QTrapez()
  :
  QUniform<dim>(2)
{}


template< int dim >
std::shared_ptr< QTrapez< dim > >
QTrapez< dim >::create()
{
  return (std::make_shared< QTrapez<dim> >()) ;
}

IGA_NAMESPACE_CLOSE

#include <igatools/base/quadrature_lib.inst>
