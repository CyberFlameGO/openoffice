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



#ifndef _SDR_OVERLAY_OVERLAYPOLYPOLYGON_HXX
#define _SDR_OVERLAY_OVERLAYPOLYPOLYGON_HXX

#include <svx/sdr/overlay/overlayobject.hxx>
#include <basegfx/polygon/b2dpolypolygon.hxx>

//////////////////////////////////////////////////////////////////////////////

namespace sdr
{
	namespace overlay
	{
		class SVX_DLLPUBLIC OverlayPolyPolygonStriped : public OverlayObject
		{
		protected:
			// geometry
			basegfx::B2DPolyPolygon				maPolyPolygon;

			// geometry creation for OverlayObject
			virtual drawinglayer::primitive2d::Primitive2DSequence createOverlayObjectPrimitive2DSequence();

		public:
			OverlayPolyPolygonStriped(const basegfx::B2DPolyPolygon& rPolyPolygon);
			virtual ~OverlayPolyPolygonStriped();

			// change geometry
			basegfx::B2DPolyPolygon getPolyPolygon() const { return maPolyPolygon; }
			void setPolyPolygon(const basegfx::B2DPolyPolygon& rNew);

			// react on stripe definition change
			virtual void stripeDefinitionHasChanged();
		};
	} // end of namespace overlay
} // end of namespace sdr

//////////////////////////////////////////////////////////////////////////////

#endif //_SDR_OVERLAY_OVERLAYPOLYPOLYGON_HXX

// eof
