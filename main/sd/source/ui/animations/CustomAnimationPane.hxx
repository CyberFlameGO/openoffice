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



#ifndef _SD_CUSTOMANIMATIONPANE_HXX
#define _SD_CUSTOMANIMATIONPANE_HXX

#include <com/sun/star/drawing/XDrawView.hpp>
#include <com/sun/star/frame/XModel.hpp>
#include <vcl/dialog.hxx>
#include "CustomAnimationPreset.hxx"
#include "CustomAnimationList.hxx"
#include "CustomAnimationCreateDialog.hxx"

#include "motionpathtag.hxx"
#include "misc/scopelock.hxx"

#include <vector>

class PushButton;
class FixedLine;
class FixedText;
class ListBox;
class ComboBox;
class CheckBox;
class SdDrawDocument;

namespace com { namespace sun { namespace star { namespace animations {
	class XAnimationNode;
} } } }

namespace sd { namespace tools {
class EventMultiplexerEvent;
} }

namespace sd {

class PropertyControl;
class STLPropertySet;
class ViewShellBase;

typedef std::vector< rtl::Reference< MotionPathTag > > MotionPathTagVector;

// --------------------------------------------------------------------

class CustomAnimationPane : public Control, public ICustomAnimationListController
{
	friend class MotionPathTag;
public:
	CustomAnimationPane( ::Window* pParent, ViewShellBase& rBase, const Size& rMinSize );
	virtual ~CustomAnimationPane();

	// callbacks
	void onSelectionChanged();
	void onChangeCurrentPage();
	void onChange( bool bCreate );
	void onRemove();
	void onChangeStart();
	void onChangeStart( sal_Int16 nNodeType );
	void onChangeProperty();
	void onChangeSpeed();

	// methods
	void preview( const ::com::sun::star::uno::Reference< ::com::sun::star::animations::XAnimationNode >& xAnimationNode );
	void remove( CustomAnimationEffectPtr& pEffect );

	// Control
	virtual void Resize();
	virtual void StateChanged( StateChangedType nStateChange );
	virtual void KeyInput( const KeyEvent& rKEvt );

	// ICustomAnimationListController
	virtual void onSelect();
	virtual void onDoubleClick();
	virtual void onContextMenu( sal_uInt16 nSelectedPopupEntry );

	void addUndo();

	void updatePathFromMotionPathTag( const rtl::Reference< MotionPathTag >& xTag );

private:
	void addListener();
	void removeListener();
	void updateLayout();
	void updateControls();
	void updateMotionPathTags();
	void markShapesFromSelectedEffects();

	void showOptions( sal_uInt16 nPage = 0 );
	void moveSelection( bool bUp );
	void onPreview( bool bForcePreview );

	void createPath( PathKind eKind, std::vector< ::com::sun::star::uno::Any >& rTargets, double fDuration );

	STLPropertySet* createSelectionSet();
	void changeSelection( STLPropertySet* pResultSet, STLPropertySet* pOldSet );

	::com::sun::star::uno::Any getProperty1Value( sal_Int32 nType, CustomAnimationEffectPtr pEffect );
	bool setProperty1Value( sal_Int32 nType, CustomAnimationEffectPtr pEffect, const ::com::sun::star::uno::Any& rValue );

	DECL_LINK( implControlHdl, Control* );
	DECL_LINK( implPropertyHdl, Control* );
    DECL_LINK(EventMultiplexerListener, tools::EventMultiplexerEvent*);
    DECL_LINK(lateInitCallback, Timer*);

private:
	ViewShellBase& mrBase;

	const CustomAnimationPresets* mpCustomAnimationPresets;

	FixedLine*	mpFLModify;
	PushButton*	mpPBAddEffect;
	PushButton* mpPBChangeEffect;
	PushButton*	mpPBRemoveEffect;
	FixedLine*	mpFLEffect;
	FixedText*	mpFTStart;
	ListBox*	mpLBStart;
	FixedText*	mpFTProperty;
	PropertyControl*	mpLBProperty;
	PushButton*	mpPBPropertyMore;
	FixedText*	mpFTSpeed;
	ComboBox*	mpCBSpeed;
	CustomAnimationList*	mpCustomAnimationList;
	FixedText*	mpFTChangeOrder;
	PushButton*	mpPBMoveUp;
	PushButton* mpPBMoveDown;
	FixedLine*	mpFLSeperator1;
	PushButton*	mpPBPlay;
	PushButton* mpPBSlideShow;
	FixedLine*	mpFLSeperator2;
	CheckBox*	mpCBAutoPreview;

	String		maStrModify;
	String		maStrProperty;

	sal_Int32	mnPropertyType;

	Size		maMinSize;

	EffectSequence maListSelection;
	::com::sun::star::uno::Any maViewSelection;

	MainSequencePtr	mpMainSequence;

	::com::sun::star::uno::Reference< ::com::sun::star::drawing::XDrawPage > mxCurrentPage;
	::com::sun::star::uno::Reference< ::com::sun::star::drawing::XDrawView > mxView;
	::com::sun::star::uno::Reference< ::com::sun::star::frame::XModel > mxModel;

    /** The mpCustomAnimationPresets is initialized either on demand or
        after a short time after the construction of a new object of this
        class.  This timer is responsible for the later.
    */
    Timer maLateInitTimer;

    /** This method initializes the mpCustomAnimationPresets on demand and
        returns a reference to the list.
    */
    const CustomAnimationPresets& getPresets (void);

	MotionPathTagVector maMotionPathTags;

	ScopeLock maSelectionLock;
};

}

#endif // _SD_CUSTOMANIMATIONPANE_HXX
