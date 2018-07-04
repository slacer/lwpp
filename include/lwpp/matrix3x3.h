#ifndef MATRIX4X4_H
#define MATRIX4X4_H

#include <lwpp/Point3d.h>
#include <limits>

namespace lwpp
{
	//! @ingroup Helper
	template <typename T>
	class Matrix3x3
	{
	public:
		T m[3][3];
		//! Set identity matrix
		void SetIdentity() {
			for (int i = 0; i < 3; i++)
				for (int j = 0; j < 3; j++)
						m[i][j] = (i == j) ? 1.0 : 0.0;
		}
		//! Blank Constructor, sets the identity matrix
		Matrix3x3 () {
			SetIdentity();
		}
		Matrix4x4 (const Matrix4x4 &v)
		{
			for (int i = 0; i < 3; ++i)
			{
				m[i][0] = v.m[i][0];
				m[i][1] = v.m[i][1];
				m[i][2] = v.m[i][2];
			}
		}

		Matrix4x4 &operator=(const Matrix4x4 &v)
		{
			if (this != &v)
			{
				for (int i = 0; i < 3; ++i)
				{
					m[i][0] = v.m[i][0];
					m[i][1] = v.m[i][1];
					m[i][2] = v.m[i][2];
				}
			}
			return *this;
		}

		Matrix4x4 (T m00, T m01, T m02,
							 T m10, T m11, T m12,
							 T m20, T m21, T m22)
		{
			m[0][0] = m00;	m[0][1] = m01;	m[0][2] = m02;
			m[1][0] = m10;	m[1][1] = m11;	m[1][2] = m12;
			m[2][0] = m20;	m[2][1] = m21;	m[2][2] = m22;
		}
		/*
		For a homogeneous geometrical transformation matrix, you can get the roll, pitch and yaw angles, following the TRPY convention, using the following formulas:

		roll (rotation around z) : atan2(xy, xx)
		pitch (rotation around y) : -arcsin(xz)
		yaw (rotation around x) : atan2(yz,zz)

		where the matrix is defined in the form:

		[
		xx, yx, zx, px;
		xy, yy, zy, py;
		xz, yz, zz, pz;
		0, 0, 0, 1
		]

		Do use the full atan2 function so that you can get values from full trigonometric circle (ie. don't just use atan).
		*/

		Matrix4x4 (T v[3][3])
		{
			for (int i = 0; i < 3; ++i)
			{
				for (int j = 0; j < 3; ++j)
				{
					m[i][j] = v[i][j];
				}
			}
		}
		
		//! Compute the product of two matrices
		inline Matrix3x3 Mul(const Matrix3x3 &m1)
		{
			T r[3][3];
			for (int i = 0; i < 3; ++i)
			{
				for (int j = 0; j < 3; ++j)
				{
					r[i][j] = m[i][0] * m1.m[0][j] +
										m[i][1] * m1.m[1][j] +
										m[i][2] * m1.m[2][j];
				}
			}
			return Matrix3x3(r);
		}

		//! Gauss-Jordan elimination with partial pivoting
		Matrix4x4 &Inverse() 
		{
			int indxc[4], indxr[4];
			int ipiv[4] = { 0, 0, 0, 0 };
			Matrix4x4 minv(*this);
			//memcpy(minv, m, 4*4*sizeof(double));
			int j;
			for (int i = 0; i < 4; i++) 
			{
				int irow = -1, icol = -1;
				T big = 0.;
				// Choose pivot
				for (j = 0; j < 4; j++) 
				{
					if (ipiv[j] != 1) 
					{
						for (int k = 0; k < 4; k++) 
						{
							if (ipiv[k] == 0) 
							{
								if (abs(minv.m[j][k]) >= big) 
								{
									big = abs(minv.m[j][k]);
									irow = j;
									icol = k;
								}
							}
							else if (ipiv[k] > 1) return *this; // Error("Singular matrix in MatrixInvert");
						}
					}	
				}
				++ipiv[icol];
				// Swap rows _irow_ and _icol_ for pivot
				if (irow != icol) 
				{
					for (int k = 0; k < 4; ++k) Swap(minv.m[irow][k], minv.m[icol][k]);
				}
				indxr[i] = irow;
				indxc[i] = icol;
				if (minv.m[icol][icol] == 0.)	return *this; // Error("Singular matrix in MatrixInvert");
				// Set $m[icol][icol]$ to one by scaling row _icol_ appropriately
				T pivinv = 1.f / minv.m[icol][icol];
				minv.m[icol][icol] = 1.f;
				for (j = 0; j < 4; j++) minv.m[icol][j] *= pivinv;
				// Subtract this row from others to zero out their columns
				for (j = 0; j < 4; j++) 
				{
					if (j != icol) 
					{
						T save = minv.m[j][icol];
						minv.m[j][icol] = 0;
						for (int k = 0; k < 4; k++)
							minv.m[j][k] -= minv.m[icol][k]*save;
					}
				}
			}
			// Swap columns to reflect permutation
			for (j = 3; j >= 0; j--) 
			{
				if (indxr[j] != indxc[j]) 
				{
					for (int k = 0; k < 4; k++)
						Swap(minv.m[k][indxr[j]], minv.m[k][indxc[j]]);
				}
			}
			*this = minv;
			return *this;
		}
		/*!
		* Transform a Point3d with a Matrix4x4
		* @relates Point3d
		*/
		inline Point3<T> operator()(const Point3<T> &v) const
		{
			T xp = v.x*m[0][0] + v.y*m[1][0] + v.z*m[2][0] + m[3][0];
			T yp = v.x*m[0][1] + v.y*m[1][1] + v.z*m[2][1] + m[3][1];
			T zp = v.x*m[0][2] + v.y*m[1][2] + v.z*m[2][2] + m[3][2];
/*
			T wp = v.x*m[0][3] + v.y*m[1][3] + v.z*m[2][3] + m[3][3];

			if (wp > std::numeric_limits<T>::denorm_min())
			{
				return Point3<T>(xp, yp, zp) / wp;
			}
			*/
			return Point3<T>(xp, yp, zp);
		}

		/*!
		* Quickly Transform a Point3d with a Matrix4x4 (only using 3x3
		* @relates Point3d
		*/
		inline Point3<T> &Mul3x3 (Point3<T> &v) const
		{
			T xp = v.x*m[0][0] + v.y*m[1][0] + v.z*m[2][0];
			T yp = v.x*m[0][1] + v.y*m[1][1] + v.z*m[2][1];
			T zp = v.x*m[0][2] + v.y*m[1][2] + v.z*m[2][2];
			v.x = xp;
			v.y = yp;
			v.z = zp;
			return v;
		}
		/*!
		* Transform a Point3d with a Matrix4x4
		* @relates Point3d
		*/
		inline T operator()(Point3<T> *v) const
		{
			T x = v->x;
			T y = v->y;
			T z = v->z;
			v->x		 = x*m[0][0] + y*m[1][0] + z*m[2][0] + m[3][0];
			v->y		 = x*m[0][1] + y*m[1][1] + z*m[2][1] + m[3][1];
			v->z		 = x*m[0][2] + y*m[1][2] + z*m[2][2] + m[3][2];
			T w			 = x*m[0][3] + y*m[1][3] + z*m[2][3] + m[3][3];
			//if (w > std::numeric_limits<T>::denorm_min())
			if (w != 0.0)
			{
				*v /= w;
			}
			return w;
		}
		/*!
		* Transform a Vector3d with a Matrix4x4
		* @relates Vector3d
		*/
		inline Vector3<T> transform(const Vector3<T> &v) const
		{
			return Vector3<T>( v.x*m[0][0] + v.y*m[1][0] + v.z*m[2][0],
												 v.x*m[0][1] + v.y*m[1][1] + v.z*m[2][1],
												 v.x*m[0][2] + v.y*m[1][2] + v.z*m[2][2]);
		}
		inline Vector3<T> operator()(const Vector3<T> &v) const
		{
			return transform(v);
		}

	};                 

	typedef Vector3<double> Vector3d;
	typedef Vector3<float> Vector3f;

	typedef Matrix4x4<double> Matrix4x4d;
	typedef Matrix4x4<float> Matrix4x4f;
}                                 
#endif // MATRIX4X4_H