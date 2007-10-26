#ifndef MANTID_QUAT_H_
#define MANTID_QUAT_H_

#include <iostream> 

namespace Mantid
{
namespace Geometry
{
//Forward declarations
class V3D; 
class M33;

class Quat
{
	/** @class Quat Quat.h Geometry/Quat.h
			@brief Class for quaternions 
		    @version 1.0
		    @author Laurent C Chapon, ISIS RAL
		    @date 10/10/2007
		    
		    Templated class for quaternions. 
		    Quaternions are the 3D generalization of complex numbers
		    Quaternions are used for roations in 3D spaces and  
		    often implemented for computer graphics applications.
		    Quaternion can be written q=W+ai+bj+ck where 
		    w is the scalar part, and a, b, c the 3 imaginary parts.
		    Quaternion multiplication is non-commutative.
		    i*j=-j*i=k
		    j*k=-k*j=i
		    k*i=-i*k=j
		    Rotation of an angle theta around a normalized axis (u,v,w) can be simply
		    written W=cos(theta/2), a=u*sin(theta/2), b=v*sin(theta/2), c=w*sin(theta/2)
		    This class support all arithmetic operations for quaternions
		    @ignore 
		    Copyright � 2007 ???RAL???

		    This file is part of Mantid.

		    Mantid is free software; you can redistribute it and/or modify
		    it under the terms of the GNU General Public License as published by
		    the Free Software Foundation; either version 3 of the License, or
		    (at your ption) any later version.

		    Mantid is distributed in the hope that it will be useful,
		    but WITHOUT ANY WARRANTY; without even the implied warranty of
		    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
		    GNU General Public License for more details.

		    You should have received a copy of the GNU General Public License
		    along with this program.  If not, see <http://www.gnu.org/licenses/>.
		    
		    File change history is stored at: <https://svn.mantidproject.org/mantid/trunk/Code/Mantid>
		*/
	public:
		Quat();       
		Quat(const double, const double, const double, const double); 
		Quat(const Quat&);
		//! Set quaternion form an angle in degrees and an axis
		Quat(const double&, const V3D&);
		Quat(const M33&);
		~Quat();
		void operator()(const double, const double, const double, const double);
		void operator()(const double, const double);
		//! Set quaternion from a 3x3 matrix
		void operator()(const M33&);
		void set(const double, const double, const double, const double);
		void setAngleAxis(const double, const V3D&);
		void setRotMatrix(const M33&);
		//! Norm of a quaternion
		double norm() const;
		//! Norm squared
		double norm2() const;
		//! Initialized to identity
		void init();
		//! Nornmalize
		void normalize();
		//! Take the complex conjugate
		void conjugate();  
		//! Inverse a quaternion (in the sense of rotaion inversion)
		void inverse();
		//! Convert quaternion rotation to an OpenGL matrix [4x4] matrix 
		//! stored as an linear array of 16 double
		//! The function glRotated must be called
		void GLMatrix(double[16]);
		//! Overload operators
		Quat  operator+(const Quat&) const;
		Quat& operator+=(const Quat&);
		Quat  operator-(const Quat&) const;
		Quat& operator-=(const Quat&);
		Quat  operator*(const Quat&) const;
		Quat& operator*=(const Quat&);
		bool   operator==(const Quat&) const;
		bool   operator!=(const Quat&) const;
		const double& operator[](int) const;
		double& operator[](int);
		void printSelf(std::ostream&) const;
	private:
		double w, a, b, c;
};
	
		//! Overload operator <<. This calls Quat::printself()
 std::ostream& operator<<(std::ostream&, const Quat&);

} // Namespace Mantid

} // Namespace Geometry

#endif /*MANTID_QUAT_H_*/
