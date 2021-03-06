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


#ifndef _SFX_FRMDESCRHXX
#define _SFX_FRMDESCRHXX

#include <bf_sfx2/sfxsids.hrc>

#ifndef _SFXPOOLITEM_HXX //autogen
#include <bf_svtools/poolitem.hxx>
#endif
#ifndef _URLOBJ_HXX //autogen
#include <tools/urlobj.hxx>
#endif
#ifndef _STRING_HXX //autogen
#include <tools/string.hxx>
#endif
#ifndef _SVARRAY_HXX //autogen
#include <bf_svtools/svarray.hxx>
#endif
#ifndef _SV_GEN_HXX
#include <tools/gen.hxx>
#endif
class SvStream;
class SvStrings;
class Wallpaper;
namespace binfilter {
class SfxItemSet;

struct SfxFrameDescriptor_Impl;
struct SfxFrameSetDescriptor_Impl;
class SfxFrameDescriptor;
typedef SfxFrameDescriptor* SfxFrameDescriptorPtr;
SV_DECL_PTRARR(SfxFramesArr, SfxFrameDescriptorPtr, 4, 2)//STRIP008 ;

//===========================================================================
// Die SfxFrame...descriptoren bauen eine rekursive Struktur auf, die
// s"amtliche zur Anzeige des Frame-Dokuments erforderlichen Daten umfa\st.
// "Uber einen SfxFrameSetDescriptor hat man Zugriff auf die gesamte darunter
// liegende Struktur.
// Aufgrund der besonderen Eigenschaften des SfxFrames-Dokuments ist ein
// SfxFramesSetDescriptor nicht nur der Inhalt dieses Dokuments, sondern
// beschreibt auch die View darauf.
// Das FrameSet wird aus Zeilen aufgebaut, die wiederum die eigentlichen
// Fenster enthalten. Eine Zeile kann horizontal oder vertikal ausgerichtet
// sein, woraus sich auch das Alignment des FrameSets ergibt.
//===========================================================================

enum ScrollingMode
{
	ScrollingYes,
	ScrollingNo,
	ScrollingAuto
};

enum SizeSelector
{
	SIZE_ABS,
	SIZE_PERCENT,
	SIZE_REL
};

#define BORDER_SET			2
#define BORDER_YES			1
#define BORDER_NO			0
#define SPACING_NOT_SET		-1L
#define SIZE_NOT_SET		-1L

class SfxFrameSetDescriptor
{
friend class SfxFrameDescriptor;

	SfxFramesArr 			aFrames;
	String					aBitmapName;
	SfxFrameSetDescriptor_Impl* pImp;
	String					aDocumentTitle;
	SfxFrameDescriptor*		pParentFrame;
	long					nFrameSpacing;
	USHORT					nHasBorder;
	USHORT					nMaxId;
	BOOL					bIsRoot;
	BOOL					bRowSet;
	SvStrings*				pScripts;
	SvStrings*				pComments;
};

struct SfxFrameProperties;

class SfxFrameDescriptor
{
friend class SfxFrameSetDescriptor;

	SfxFrameSetDescriptor* 	pParentFrameSet;    // Der Vater
	SfxFrameSetDescriptor*	pFrameSet;			// Wenn Kinder da sind, die
												// Set-Daten dazu
	INetURLObject			aURL;
	INetURLObject			aActualURL;
	String					aName;
	Size					aMargin;
	long					nWidth;
	ScrollingMode			eScroll;
	SizeSelector			eSizeSelector;
	USHORT					nHasBorder;
	USHORT					nItemId;
	BOOL					bResizeHorizontal;
	BOOL					bResizeVertical;
	BOOL					bHasUI;
	BOOL                    bReadOnly;
	SfxFrameDescriptor_Impl* pImp;
	SvStrings*				pScripts;
	SvStrings*				pComments;

public:
							SfxFrameDescriptor(SfxFrameSetDescriptor* pSet);
							~SfxFrameDescriptor();

							// Eigenschaften

							// FileName/URL
	const INetURLObject&	GetURL() const
							{ return aURL; }
	void					SetURL( const String& rURL );
	const INetURLObject&	GetActualURL() const
							{ return aActualURL; }
	void                    SetReadOnly( BOOL bSet ) { bReadOnly = bSet;}
	BOOL                    IsReadOnly(  ) const { return bReadOnly;}

							// Size
	void					SetWidth( long n )
							{ nWidth = n; }
	void					SetWidthPercent( long n )
							{ nWidth = n; eSizeSelector = SIZE_PERCENT; }
	void					SetWidthRel( long n )
							{ nWidth = n; eSizeSelector = SIZE_REL; }
	void					SetWidthAbs( long n )
							{ nWidth = n; eSizeSelector = SIZE_ABS; }
	long					GetWidth() const
							{ return nWidth; }
	SizeSelector			GetSizeSelector() const
							{ return eSizeSelector; }
	BOOL					IsResizable() const
							{ return bResizeHorizontal && bResizeVertical; }
	void					SetResizable( BOOL bRes )
							{ bResizeHorizontal = bResizeVertical = bRes; }

							// FrameName
	const String&			GetName() const
							{ return aName; }
	void					SetName( const String& rName )
							{ aName = rName; }

							// Margin, Scrolling
	const Size&				GetMargin() const
							{ return aMargin; }
	void					SetMargin( const Size& rMargin )
							{ aMargin = rMargin; }
	ScrollingMode			GetScrollingMode() const
							{ return eScroll; }
	void					SetScrollingMode( ScrollingMode eMode )
							{ eScroll = eMode; }

	BOOL					IsFrameBorderOn() const
							{ return ( nHasBorder & BORDER_YES ) != 0; }

	void					SetFrameBorder( BOOL bBorder )
							{
								nHasBorder = bBorder ?
											BORDER_YES | BORDER_SET :
											BORDER_NO | BORDER_SET;
							}
	BOOL					IsFrameBorderSet() const
							{ return (nHasBorder & BORDER_SET) != 0; }
	void					ResetBorder()
							{ nHasBorder = 0; }

	BOOL					HasUI() const
							{ return bHasUI; }
	void					SetHasUI( BOOL bOn )
							{ bHasUI = bOn; }

							// Attribute f"ur das Splitwindow
	USHORT					GetItemId() const
							{ return nItemId; }
	void					SetItemId( USHORT nId )
							{ nItemId = nId; }

							// Rekursion, Iteration
	SfxFrameSetDescriptor*	GetParent() const
							{ return pParentFrameSet; }
	SfxFrameSetDescriptor*	GetFrameSet() const
							{ return pFrameSet; }
	void					SetFrameSet( SfxFrameSetDescriptor* pSet)
							{
								if ( pFrameSet )
									pFrameSet->pParentFrame = NULL;
								pFrameSet = pSet;
								if ( pSet )
									pSet->pParentFrame = this;
							}

							// Kopie z.B. f"ur die Views
	SfxFrameDescriptor*		Clone(SfxFrameSetDescriptor *pFrame=NULL, BOOL bWithIds = TRUE ) const;

};

// Kein Bock, einen operator= zu implementieren...
struct SfxFrameProperties
{
	String								aURL;
	String								aName;
	long								lMarginWidth;
	long								lMarginHeight;
	long								lSize;
	long								lSetSize;
	long								lFrameSpacing;
	long								lInheritedFrameSpacing;
	ScrollingMode						eScroll;
	SizeSelector						eSizeSelector;
	SizeSelector						eSetSizeSelector;
	BOOL								bHasBorder;
	BOOL								bBorderSet;
	BOOL								bResizable;
	BOOL								bSetResizable;
	BOOL								bIsRootSet;
	BOOL								bIsInColSet;
	BOOL								bHasBorderInherited;
	SfxFrameDescriptor*              	pFrame;

private:
	SfxFrameProperties( SfxFrameProperties& ) {}
public:
										SfxFrameProperties()
											: pFrame( 0 ),
											  lMarginWidth( SIZE_NOT_SET ),
											  lMarginHeight( SIZE_NOT_SET ),
											  lSize( 1L ),
											  lSetSize( 1L ),
											  lFrameSpacing( SPACING_NOT_SET ),
											  lInheritedFrameSpacing( SPACING_NOT_SET ),
											  eScroll( ScrollingAuto ),
											  eSizeSelector( SIZE_REL ),
											  eSetSizeSelector( SIZE_REL ),
											  bHasBorder( TRUE ),
											  bHasBorderInherited( TRUE ),
											  bBorderSet( TRUE ),
											  bResizable( TRUE ),
											  bSetResizable( TRUE ),
											  bIsRootSet( FALSE ),
											  bIsInColSet( FALSE ) {}

										~SfxFrameProperties() { delete pFrame; }

	int             					operator ==( const SfxFrameProperties& ) const;
	SfxFrameProperties&				    operator =( const SfxFrameProperties &rProp );
};

class SfxFrameDescriptorItem : public SfxPoolItem
{
	SfxFrameProperties					aProperties;
public:
										TYPEINFO();

										SfxFrameDescriptorItem ( const USHORT nId = SID_FRAMEDESCRIPTOR )
											: SfxPoolItem( nId )
										{}

										SfxFrameDescriptorItem( const SfxFrameDescriptorItem& rCpy )
											: SfxPoolItem( rCpy )
										{
											aProperties = rCpy.aProperties;
										}

	virtual								~SfxFrameDescriptorItem();

	virtual int             			operator ==( const SfxPoolItem& ) const;
	SfxFrameDescriptorItem&				operator =( const SfxFrameDescriptorItem & );

	virtual SfxItemPresentation 		GetPresentation( SfxItemPresentation ePres,
											SfxMapUnit eCoreMetric,
											SfxMapUnit ePresMetric,
                                            UniString &rText, const ::IntlWrapper * = 0 ) const;

	virtual SfxPoolItem*    			Clone( SfxItemPool *pPool = 0 ) const;
	//virtual SfxPoolItem*    			Create(SvStream &, USHORT) const;
	//virtual SvStream&					Store(SvStream &, USHORT nItemVersion ) const;
	//virtual USHORT						GetVersion( USHORT nFileFormatVersion ) const;

	const SfxFrameProperties&			GetProperties() const
										{ return aProperties; }
	void 								SetProperties( const SfxFrameProperties& rProp )
										{ aProperties = rProp; }
};

}//end of namespace binfilter
#endif // #ifndef _SFX_FRMDESCRHXX

