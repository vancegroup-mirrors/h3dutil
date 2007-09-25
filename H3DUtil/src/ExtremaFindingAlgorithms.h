//////////////////////////////////////////////////////////////////////////////
//    Copyright 2004-2007, SenseGraphics AB
//
//    This file is part of H3D API.
//
//    H3D API is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    H3D API is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with H3D API; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//    A commercial license is also available. Please contact us at 
//    www.sensegraphics.com for more information.
//
//
/// \file ExtremumFindingAlgorithms.h
/// \brief Base class for all H3D ExtremumFindingAlgorithmss
///
//
//
//////////////////////////////////////////////////////////////////////////////
#ifndef __EXTREMUMFINDINGALGORITHMS_H__
#define __EXTREMUMFINDINGALGORITHMS_H__

#include <H3DUtil.h>
#include <LinAlgTypes.h>

namespace H3DUtil {

  /// This algorithm tries to minimize a function and is called
  /// <i>downhill simplex method<i/>. The method only require function
  /// evaluations, not derivatives. The method is due to J.A Nelder and 
  /// R. Mead. They published it in "Computer Journal" vol. 7 pages 
  /// 391-398. The method can also be found in section 10.4 in the book
  /// "Numerical recipes in C. The art of Scientific Computing. Second edition"
  /// by authors William H. Press, Saul A. Teukolsky, William T. Vetterling and
  /// Brian P. Flannery.
  ///
  /// The class only contains a couple of static methods. Call amoeba to
  /// minimize a function.
  class H3DUTIL_API DownhillSimplexMethod {
    public:
      /// Will minimize the function func.
      /// InputType is a type that describes a vector in NR_DIM nr of 
      /// dimensions. Should have array capabilities.
      /// OutputType is the output of the function to minimize. 
      /// \param p pointer to an array of InputType, the number of
      /// items in the array is NR_DIMS + 1.
      /// \param func is the function to minimize.
      /// \param min_point will contain the result of the minimization.
      /// \param user_data user defined data sent to func.
      /// \param ftol fractional convergence tolerance to be achieved in the
      /// function value. This is a limit on how accurate the minimization
      /// has to be.
      /// \param max_iterations Is the maximum number of iterations.
      template< class InputType,
                class OutputType,
                int NR_DIMS >
      static void amoeba( InputType *p,
                          OutputType(*func)( InputType, void * ),
                          InputType &min_point,
                          void *user_data,
                          OutputType ftol = 1e-8,
                          int max_iterations = -1 ) {
        InputType psum;
        int ihi, ilo, inhi, mpts = NR_DIMS + 1;
        OutputType sum, ytry; 

        OutputType y[3];

        unsigned int nr_iterations = 0;

        // initialize the y with the function values for the starting points
        for( int i = 0; i < mpts; i++ ) {
          y[i] = func( p[i], user_data );
        }

        get_psum< InputType, OutputType >( psum, p, sum, mpts );

        while( max_iterations < 0 || nr_iterations < max_iterations ) {
          // find highest, lowest and next-highest point
          ilo = 0;
          ihi = y[1] > y[2] ? (inhi=2,1) : (inhi = 1,2);
          for( int i = 0; i < mpts; i++ ) {
            if( y[i] <= y[ilo] ) ilo = i;
            if( y[i] > y[ihi] ) {
              inhi = ihi;
              ihi = i;
            } else 
              if( y[i] > y[inhi] && i != ihi ) inhi = i;
          }

          // check for convergence
          OutputType denom = fabs(y[ihi]) +fabs(y[ilo] ) + 1e-10;
          OutputType rtol = 2.0*fabs( y[ihi]-y[ilo])/denom;

          if( rtol < ftol ) {
            min_point = p[ilo];
            break;
          }

          // reflect
          ytry = amotry<InputType, OutputType, NR_DIMS>( p, y, psum, func, ihi, -1, user_data );
          if( ytry <= y[ilo] ) {
            // expand
            ytry = amotry<InputType, OutputType, NR_DIMS>( p, y, psum, func, ihi, 2, user_data );
          } else if( ytry >= y[inhi] ) {
            // contract
            OutputType ysave = y[ihi];
            ytry = amotry<InputType, OutputType, NR_DIMS>( p, y, psum, func, ihi, 0.5, user_data );
            if( ytry >= ysave ) {
              for( int i = 0; i < mpts; i++ ) {
                if( i != ilo ) {
                  for( int j = 0; j < NR_DIMS; j++ ) {
                    p[i][j] = psum[j]=0.5*(p[i][j]+p[ilo][j]);
                  }
                  y[i] = (*func)(psum, user_data);
                }
              }
              get_psum< InputType, OutputType >( psum, p, sum, mpts );
            }
          }
          nr_iterations++;
        }
      }

    protected:

      /// Help function for amoeba.
      template< class InputType,
                class OutputType >
      static inline void get_psum( InputType &psum, InputType *p, OutputType &sum, int mpts ) {
        for( int j = 0; j < 2; j++ ) {
          sum = 0.0;
          for( int i = 0; i < mpts; i++ ) sum += p[i][j];
          psum[j] = sum;
        }
      }

      /// Help function for amoeba.
      template< class InputType,
                class OutputType,
                int NR_DIMS >
      static inline OutputType amotry( InputType *p,
                                OutputType *y,
                                InputType &psum,
                                OutputType(*func)( InputType, void * ),
                                int ihi,
                                OutputType fac,
                                void *user_data ) {
        InputType ptry;
        OutputType fac1 = (1.0 - fac ) / NR_DIMS; //fads
        OutputType fac2 = fac1 - fac;

        for( int j = 0; j < NR_DIMS; j++ ) 
          ptry[j] = psum[j]*fac1 - p[ihi][j]*fac2;
        OutputType ytry = (*func)(ptry, user_data );
        if( ytry < y[ihi] ) {
          y[ihi] = ytry;
          for( int j = 0; j < NR_DIMS; j++ ) {
            psum[j] += ptry[j] - p[ihi][j];
            p[ihi][j] = ptry[j];
          }
        }
        return ytry;
      }
    };
}

#endif
