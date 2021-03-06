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



// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_svx.hxx"

#include <svx/sdr/contact/objectcontacttools.hxx>
#include <vcl/outdev.hxx>
#include <basegfx/matrix/b2dhommatrix.hxx>
#include <basegfx/range/b2drange.hxx>
#include <vcl/gdimtf.hxx>
#include <basegfx/tools/canvastools.hxx>
#include <drawinglayer/processor2d/vclmetafileprocessor2d.hxx>
#include <drawinglayer/processor2d/vclpixelprocessor2d.hxx>
#include <drawinglayer/processor2d/canvasprocessor.hxx>
#include <vcl/window.hxx>

//////////////////////////////////////////////////////////////////////////////

using namespace com::sun::star;

//////////////////////////////////////////////////////////////////////////////

namespace sdr
{
	namespace contact
	{
		drawinglayer::processor2d::BaseProcessor2D* createBaseProcessor2DFromOutputDevice(
            OutputDevice& rTargetOutDev, 
            const drawinglayer::geometry::ViewInformation2D& rViewInformation2D)
		{
			const GDIMetaFile* pMetaFile = rTargetOutDev.GetConnectMetaFile();
			const bool bOutputToRecordingMetaFile(pMetaFile && pMetaFile->IsRecord() && !pMetaFile->IsPause());

			if(bOutputToRecordingMetaFile)
			{
				// create MetaFile Vcl-Processor and process
				return new drawinglayer::processor2d::VclMetafileProcessor2D(rViewInformation2D, rTargetOutDev);
			}
			else
			{
#ifdef WIN32
                // for a first AA incarnation VCL-PixelRenderer will be okay since
                // simple (and fast) GDIPlus support over VCL will be used.
                // Leaving the code below as a hint for what to do when we will
                // use canvas renderers in the future

                //static SvtOptionsDrawinglayer aSvtOptionsDrawinglayer;
                
                //if(false && aSvtOptionsDrawinglayer.IsAntiAliasing())
                //{
    			//	// for WIN32 AA, create cairo canvas processor
	    		//	return new drawinglayer::processor2d::canvasProcessor2D(rViewInformation2D, rTargetOutDev);
                //}
                //else
                //{
    				// create Pixel Vcl-Processor
	    			return new drawinglayer::processor2d::VclPixelProcessor2D(rViewInformation2D, rTargetOutDev);
                //}
#else
                static bool bTryTestCanvas(false);

                if(bTryTestCanvas)
                {
    				// create test-cancas-Processor
	    			return new drawinglayer::processor2d::canvasProcessor2D(rViewInformation2D, rTargetOutDev);
                }
                else
                {
    				// create Pixel Vcl-Processor
	    			return new drawinglayer::processor2d::VclPixelProcessor2D(rViewInformation2D, rTargetOutDev);
                }
#endif
			}
		}
	} // end of namespace contact
} // end of namespace sdr

//////////////////////////////////////////////////////////////////////////////
// eof
