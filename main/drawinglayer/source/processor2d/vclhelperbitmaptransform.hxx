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



#ifndef INCLUDED_DRAWINGLAYER_PROCESSOR2D_VCLHELPERBITMAPTRANSFORM_HXX
#define INCLUDED_DRAWINGLAYER_PROCESSOR2D_VCLHELPERBITMAPTRANSFORM_HXX

#include <vcl/bitmapex.hxx>

//////////////////////////////////////////////////////////////////////////////
// predefines

namespace basegfx { 
	class B2DHomMatrix; 
	class BColorModifierStack;
}

//////////////////////////////////////////////////////////////////////////////
// support methods for vcl direct gradient renderering

namespace drawinglayer
{
	BitmapEx impTransformBitmapEx(
		const BitmapEx& rSource, 
		const Rectangle& rCroppedRectPixel, 
		const basegfx::B2DHomMatrix& rTransform);

	BitmapEx impModifyBitmapEx(
		const basegfx::BColorModifierStack& rBColorModifierStack, 
		const BitmapEx& rSource);
} // end of namespace drawinglayer

//////////////////////////////////////////////////////////////////////////////

#endif // INCLUDED_DRAWINGLAYER_PROCESSOR2D_VCLHELPERBITMAPTRANSFORM_HXX

// eof
