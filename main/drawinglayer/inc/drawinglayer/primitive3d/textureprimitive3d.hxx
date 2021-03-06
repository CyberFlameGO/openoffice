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



#ifndef INCLUDED_DRAWINGLAYER_PRIMITIVE3D_TEXTUREPRIMITIVE3D_HXX
#define INCLUDED_DRAWINGLAYER_PRIMITIVE3D_TEXTUREPRIMITIVE3D_HXX

#include <drawinglayer/drawinglayerdllapi.h>
#include <drawinglayer/primitive3d/groupprimitive3d.hxx>
#include <drawinglayer/attribute/fillbitmapattribute.hxx>
#include <basegfx/matrix/b2dhommatrix.hxx>
#include <basegfx/vector/b2dvector.hxx>
#include <drawinglayer/attribute/fillgradientattribute.hxx>

//////////////////////////////////////////////////////////////////////////////

namespace drawinglayer
{
	namespace primitive3d
	{
        /** TexturePrimitive3D class

            This 3D grouping primitive is used to define a texture for
            3d geometry by embedding it. It is used as bae class for
            extended texture definitions
         */
		class DRAWINGLAYER_DLLPUBLIC TexturePrimitive3D : public GroupPrimitive3D
		{
		private:
            /// texture geometry definition
			basegfx::B2DVector							maTextureSize;

			/// bitfield
			/// flag if texture shall be modulated with white interpolated color
			unsigned									mbModulate : 1;

			/// flag if texture shall be filtered
			unsigned									mbFilter : 1;

		public:
            /// constructor
			TexturePrimitive3D(
				const Primitive3DSequence& rChildren, 
				const basegfx::B2DVector& rTextureSize, 
				bool bModulate, 
				bool bFilter);

			/// data read access
			const basegfx::B2DVector& getTextureSize() const { return maTextureSize; }
			bool getModulate() const { return mbModulate; }
			bool getFilter() const { return mbFilter; }

			/// compare operator
			virtual bool operator==(const BasePrimitive3D& rPrimitive) const;
		};
	} // end of namespace primitive3d
} // end of namespace drawinglayer

//////////////////////////////////////////////////////////////////////////////

namespace drawinglayer
{
	namespace primitive3d
	{
        /** UnifiedTransparenceTexturePrimitive3D class

            This 3D primitive expands TexturePrimitive3D to a unified
            transparence texture definition. All 3D primitives
            embedded here will be shown with the given transparency.
         */
		class DRAWINGLAYER_DLLPUBLIC UnifiedTransparenceTexturePrimitive3D : public TexturePrimitive3D
		{
		private:
            /// transparency definition
			double										mfTransparence;

		public:
            /// constructor
			UnifiedTransparenceTexturePrimitive3D(
				double fTransparence, 
				const Primitive3DSequence& rChildren);

			/// data read access
			double getTransparence() const { return mfTransparence; }

			/// compare operator
			virtual bool operator==(const BasePrimitive3D& rPrimitive) const;

			/// own getB3DRange implementation to include transparent geometries to BoundRect calculations
			virtual basegfx::B3DRange getB3DRange(const geometry::ViewInformation3D& rViewInformation) const;

            /// local decomposition.
			virtual Primitive3DSequence get3DDecomposition(const geometry::ViewInformation3D& rViewInformation) const;

			/// provide unique ID
			DeclPrimitrive3DIDBlock()
		};
	} // end of namespace primitive3d
} // end of namespace drawinglayer

//////////////////////////////////////////////////////////////////////////////

namespace drawinglayer
{
	namespace primitive3d
	{
        /** GradientTexturePrimitive3D class

            This 3D primitive expands TexturePrimitive3D to a gradient texture 
            definition. All 3D primitives embedded here will be shown with the 
            defined gradient.
         */
		class DRAWINGLAYER_DLLPUBLIC GradientTexturePrimitive3D : public TexturePrimitive3D
		{
		private:
            /// the gradient definition
			attribute::FillGradientAttribute		maGradient;

		public:
            /// constructor
			GradientTexturePrimitive3D(
				const attribute::FillGradientAttribute& rGradient, 
				const Primitive3DSequence& rChildren, 
				const basegfx::B2DVector& rTextureSize, 
				bool bModulate, 
				bool bFilter);

			/// data read access
			const attribute::FillGradientAttribute& getGradient() const { return maGradient; }

			/// compare operator
			virtual bool operator==(const BasePrimitive3D& rPrimitive) const;

			/// provide unique ID
			DeclPrimitrive3DIDBlock()
		};
	} // end of namespace primitive3d
} // end of namespace drawinglayer

//////////////////////////////////////////////////////////////////////////////

namespace drawinglayer
{
	namespace primitive3d
	{
        /** BitmapTexturePrimitive3D class

            This 3D primitive expands TexturePrimitive3D to a bitmap texture 
            definition. All 3D primitives embedded here will be shown with the 
            defined bitmap (maybe tiled if defined).
         */
		class DRAWINGLAYER_DLLPUBLIC BitmapTexturePrimitive3D : public TexturePrimitive3D
		{
		private:
            /// bitmap fill attribute
			attribute::FillBitmapAttribute		maFillBitmapAttribute;

		public:
            /// constructor
			BitmapTexturePrimitive3D(
				const attribute::FillBitmapAttribute& rFillBitmapAttribute, 
				const Primitive3DSequence& rChildren, 
				const basegfx::B2DVector& rTextureSize, 
				bool bModulate, 
				bool bFilter);

			/// data read access
			const attribute::FillBitmapAttribute& getFillBitmapAttribute() const { return maFillBitmapAttribute; }

			/// compare operator
			virtual bool operator==(const BasePrimitive3D& rPrimitive) const;

			/// provide unique ID
			DeclPrimitrive3DIDBlock()
		};
	} // end of namespace primitive3d
} // end of namespace drawinglayer

//////////////////////////////////////////////////////////////////////////////

namespace drawinglayer
{
	namespace primitive3d
	{
        /** TransparenceTexturePrimitive3D class

            This 3D primitive expands TexturePrimitive3D to a transparence texture 
            definition. For transparence definition, a gradient is used. The values in
            that gradient will be interpreted as luminance Transparence-Values. All 3D 
            primitives embedded here will be shown with the defined transparence.
         */
		class DRAWINGLAYER_DLLPUBLIC TransparenceTexturePrimitive3D : public GradientTexturePrimitive3D
		{
		public:
            /// constructor
			TransparenceTexturePrimitive3D(
				const attribute::FillGradientAttribute& rGradient, 
				const Primitive3DSequence& rChildren, 
				const basegfx::B2DVector& rTextureSize);

			/// compare operator
			virtual bool operator==(const BasePrimitive3D& rPrimitive) const;

			/// provide unique ID
			DeclPrimitrive3DIDBlock()
		};
	} // end of namespace primitive3d
} // end of namespace drawinglayer

//////////////////////////////////////////////////////////////////////////////

#endif //INCLUDED_DRAWINGLAYER_PRIMITIVE3D_TEXTUREPRIMITIVE3D_HXX

//////////////////////////////////////////////////////////////////////////////
// eof
