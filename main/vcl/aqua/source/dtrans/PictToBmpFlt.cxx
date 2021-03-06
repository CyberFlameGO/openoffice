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



/* This is a work-around to prevent 'deprecated' warning for 'KillPicture' API 
   Hopefully we can get rid of this whole code again when the OOo PICT filter
   are good enough to be used see #i78953 thus this hack would vanish to again.
 */
#include <premac.h>
#include <AvailabilityMacros.h>
#undef DEPRECATED_ATTRIBUTE
#define DEPRECATED_ATTRIBUTE 

#include <Carbon/Carbon.h>
#include <QuickTime/QuickTime.h>
#include <postmac.h>

#include "PictToBmpFlt.hxx"

bool PICTtoBMP(com::sun::star::uno::Sequence<sal_Int8>& aPict, 
			   com::sun::star::uno::Sequence<sal_Int8>& aBmp)
{
    
  bool result = false;

  ComponentInstance bmpExporter;
  if (OpenADefaultComponent(GraphicsExporterComponentType,
							kQTFileTypeBMP,
							&bmpExporter) != noErr)
	{
	  return result;
	}

  Handle hPict;
  if (PtrToHand(aPict.getArray(), &hPict, aPict.getLength()) != noErr)
	{
	  return result;
	}

  Handle hBmp;
  if ((GraphicsExportSetInputPicture(bmpExporter, (PicHandle)hPict) != noErr) || 
	  ((hBmp = NewHandleClear(0)) == NULL))
	{
	  CloseComponent(bmpExporter);
	  DisposeHandle(hPict);
	  return result;
	}

  if ((GraphicsExportSetOutputHandle(bmpExporter, hBmp) == noErr) &&
	  (GraphicsExportDoExport(bmpExporter, NULL) == noErr))
	{
	  size_t sz = GetHandleSize(hBmp);
	  aBmp.realloc(sz);

	  HLock(hBmp);
	  rtl_copyMemory(aBmp.getArray(), ((sal_Int8*)*hBmp), sz);
	  HUnlock(hBmp);

	  result = true;
	} 

  DisposeHandle(hPict);
  DisposeHandle(hBmp);
  CloseComponent(bmpExporter);

  return result;
}

bool BMPtoPICT(com::sun::star::uno::Sequence<sal_Int8>& aBmp, 
			   com::sun::star::uno::Sequence<sal_Int8>& aPict)
{
  bool result = false;

  Handle hBmp;
  ComponentInstance pictExporter;
  if ((PtrToHand(aBmp.getArray(), &hBmp, aBmp.getLength()) != noErr)) 
	{
	  return result;
	}

  if (OpenADefaultComponent(GraphicsImporterComponentType,
							kQTFileTypeBMP,
							&pictExporter) != noErr)
	{
	  DisposeHandle(hBmp);
	  return result;
	}
  
  if (GraphicsImportSetDataHandle(pictExporter, hBmp) != noErr)
	{
	  DisposeHandle(hBmp);
	  CloseComponent(pictExporter);
	  return result;
	}

  PicHandle hPict;
  if (GraphicsImportGetAsPicture(pictExporter, &hPict) == noErr)
	{
	  size_t sz = GetHandleSize((Handle)hPict);
	  aPict.realloc(sz);

	  HLock((Handle)hPict);
	  rtl_copyMemory(aPict.getArray(), ((sal_Int8*)*hPict), sz);
	  HUnlock((Handle)hPict);

	  // Release the data associated with the picture
	  // Note: This function is deprecated in Mac OS X 
	  // 10.4.
	  KillPicture(hPict); 

	  result = true;
	}
  
  DisposeHandle(hBmp);
  CloseComponent(pictExporter);
  
  return result;
}

bool ImageToBMP( com::sun::star::uno::Sequence<sal_Int8>& aPict, 
			     com::sun::star::uno::Sequence<sal_Int8>& aBmp,
			     NSBitmapImageFileType eInFormat)
{
    if( eInFormat == PICTImageFileType )
        return PICTtoBMP( aPict, aBmp );
    
    bool bResult = false;
    
    NSData* pData = [NSData dataWithBytesNoCopy: (void*)aPict.getConstArray() length: aPict.getLength() freeWhenDone: 0];
    if( pData )
    {
        NSBitmapImageRep* pRep = [NSBitmapImageRep imageRepWithData: pData];
        if( pRep )
        {
            NSData* pOut = [pRep representationUsingType: NSBMPFileType properties: nil];
            if( pOut )
            {
                aBmp.realloc( [pOut length] );
                [pOut getBytes: aBmp.getArray() length: aBmp.getLength()];
                bResult = (aBmp.getLength() != 0);
            }
        }
    }
    
    return bResult;
}

bool BMPToImage( com::sun::star::uno::Sequence<sal_Int8>& aBmp, 
			     com::sun::star::uno::Sequence<sal_Int8>& aPict,
			     NSBitmapImageFileType eOutFormat
			    )
{
    if( eOutFormat == PICTImageFileType )
        return BMPtoPICT( aBmp, aPict );
    
    bool bResult = false;
    
    NSData* pData = [NSData dataWithBytesNoCopy: const_cast<sal_Int8*>(aBmp.getConstArray()) length: aBmp.getLength() freeWhenDone: 0];
    if( pData )
    {
        NSBitmapImageRep* pRep = [NSBitmapImageRep imageRepWithData: pData];
        if( pRep )
        {
            NSData* pOut = [pRep representationUsingType: eOutFormat properties: nil];
            if( pOut )
            {
                aPict.realloc( [pOut length] );
                [pOut getBytes: aPict.getArray() length: aPict.getLength()];
                bResult = (aPict.getLength() != 0);
            }
        }
    }
    
    return bResult;
}
