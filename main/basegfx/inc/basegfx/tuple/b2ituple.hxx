/**************************************************************
 * 
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 *************************************************************/



#ifndef _BGFX_TUPLE_B2ITUPLE_HXX
#define _BGFX_TUPLE_B2ITUPLE_HXX

#include <sal/types.h>


namespace basegfx
{
	/** Base class for all Points/Vectors with two sal_Int32 values

		This class provides all methods common to Point
		avd Vector classes which are derived from here.

		@derive Use this class to implement Points or Vectors
		which are based on two sal_Int32 values
	*/
	class B2ITuple
	{
	protected:
		sal_Int32										mnX;
		sal_Int32										mnY;
							
	public:					
		/**	Create a 2D Tuple

        	The tuple is initialized to (0, 0)
		*/
		B2ITuple() 
		:	mnX(0), 
			mnY(0) 
		{}

		/**	Create a 2D Tuple

			@param fX
			This parameter is used to initialize the X-coordinate
			of the 2D Tuple.
			
			@param fY
			This parameter is used to initialize the Y-coordinate
			of the 2D Tuple.
		*/
		B2ITuple(sal_Int32 fX, sal_Int32 fY) 
		:	mnX( fX ), 
			mnY( fY ) 
		{}

		/**	Create a copy of a 2D Tuple

			@param rTup
			The 2D Tuple which will be copied.
		*/
		B2ITuple(const B2ITuple& rTup) 
		:	mnX( rTup.mnX ), 
			mnY( rTup.mnY ) 
		{}

		~B2ITuple() 
		{}

		/// Get X-Coordinate of 2D Tuple
		sal_Int32 getX() const
		{ 
			return mnX; 
		}

		/// Get Y-Coordinate of 2D Tuple
		sal_Int32 getY() const
		{ 
			return mnY; 
		}

		/// Set X-Coordinate of 2D Tuple
		void setX(sal_Int32 fX)
		{
			mnX = fX;
		}

		/// Set Y-Coordinate of 2D Tuple
		void setY(sal_Int32 fY)
		{
			mnY = fY;
		}

		/// Array-access to 2D Tuple
		const sal_Int32& operator[] (int nPos) const 
		{ 
			// Here, normally one if(...) should be used. In the assumption that
			// both sal_Int32 members can be accessed as an array a shortcut is used here.
			// if(0 == nPos) return mnX; return mnY;
			return *((&mnX) + nPos);
		}

		/// Array-access to 2D Tuple
		sal_Int32& operator[] (int nPos)
		{ 
			// Here, normally one if(...) should be used. In the assumption that
			// both sal_Int32 members can be accessed as an array a shortcut is used here.
			// if(0 == nPos) return mnX; return mnY;
			return *((&mnX) + nPos);
		}

		// operators
		//////////////////////////////////////////////////////////////////////

		B2ITuple& operator+=( const B2ITuple& rTup ) 
		{ 
			mnX += rTup.mnX;
			mnY += rTup.mnY; 
			return *this; 
		}

		B2ITuple& operator-=( const B2ITuple& rTup ) 
		{ 
			mnX -= rTup.mnX;
			mnY -= rTup.mnY; 
			return *this; 
		}

		B2ITuple& operator/=( const B2ITuple& rTup ) 
		{ 
			mnX /= rTup.mnX;
			mnY /= rTup.mnY; 
			return *this; 
		}

		B2ITuple& operator*=( const B2ITuple& rTup ) 
		{ 
			mnX *= rTup.mnX;
			mnY *= rTup.mnY; 
			return *this; 
		}

		B2ITuple& operator*=(sal_Int32 t) 
		{ 
			mnX *= t; 
			mnY *= t; 
			return *this; 
		}

		B2ITuple& operator/=(sal_Int32 t) 
		{ 
			mnX /= t; 
			mnY /= t; 
			return *this; 
		}

		B2ITuple operator-(void) const 
		{	
			return B2ITuple(-mnX, -mnY); 
		}
		
		bool equalZero() const { return mnX == 0 && mnY == 0; }

		bool operator==( const B2ITuple& rTup ) const 
		{ 
			return this == &rTup || (rTup.mnX == mnX && rTup.mnY == mnY);
		}

		bool operator!=( const B2ITuple& rTup ) const 
		{ 
			return !(*this == rTup);
		}
		
		B2ITuple& operator=( const B2ITuple& rTup ) 
		{ 
			mnX = rTup.mnX;
			mnY = rTup.mnY; 
			return *this; 
		}

		static const B2ITuple& getEmptyTuple();
	};

	// external operators
	//////////////////////////////////////////////////////////////////////////

    class B2DTuple;

	B2ITuple minimum(const B2ITuple& rTupA, const B2ITuple& rTupB); 

	B2ITuple maximum(const B2ITuple& rTupA, const B2ITuple& rTupB); 

	B2ITuple absolute(const B2ITuple& rTup); 

	B2DTuple interpolate(const B2ITuple& rOld1, const B2ITuple& rOld2, double t); 

	B2DTuple average(const B2ITuple& rOld1, const B2ITuple& rOld2); 
	
	B2DTuple average(const B2ITuple& rOld1, const B2ITuple& rOld2, const B2ITuple& rOld3);

	B2ITuple operator+(const B2ITuple& rTupA, const B2ITuple& rTupB);

	B2ITuple operator-(const B2ITuple& rTupA, const B2ITuple& rTupB);

	B2ITuple operator/(const B2ITuple& rTupA, const B2ITuple& rTupB);

	B2ITuple operator*(const B2ITuple& rTupA, const B2ITuple& rTupB);

	B2ITuple operator*(const B2ITuple& rTup, sal_Int32 t);

	B2ITuple operator*(sal_Int32 t, const B2ITuple& rTup);

	B2ITuple operator/(const B2ITuple& rTup, sal_Int32 t);

	B2ITuple operator/(sal_Int32 t, const B2ITuple& rTup);

} // end of namespace basegfx

#endif /* _BGFX_TUPLE_B2ITUPLE_HXX */
