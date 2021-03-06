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

#define _SFX_CANCEL_CXX
#include <bf_svtools/cancel.hxx>

#include <vos/mutex.hxx>
#include <tools/debug.hxx>

#include <bf_svtools/smplhint.hxx>
#include <bf_svtools/cnclhint.hxx>

#ifndef INCLUDED_RTL_INSTANCE_HXX
#include <rtl/instance.hxx>
#endif

namespace { struct lclMutex : public rtl::Static< ::vos::OMutex, lclMutex >{}; }

namespace binfilter
{

//=========================================================================

SfxCancelManager::SfxCancelManager( SfxCancelManager *pParent )
:	_pParent( pParent )
{
}

//-------------------------------------------------------------------------

SfxCancelManager::~SfxCancelManager()
{
	DBG_ASSERT( _pParent || !_aJobs.Count(), "deleting SfxCancelManager in use" );
	for ( USHORT n = _aJobs.Count(); n--; )
		_aJobs.GetObject(n)->SetManager( _pParent );
}

//-------------------------------------------------------------------------

BOOL SfxCancelManager::CanCancel() const

/*	[Beschreibung]

	Liefert TRUE wenn an diesem CancelManager oder an einem Parent
	ein Job l"auft.
*/

{
    ::vos::OGuard aGuard( lclMutex::get() );
	return _aJobs.Count() > 0 || ( _pParent && _pParent->CanCancel() );
}

//-------------------------------------------------------------------------

void SfxCancelManager::Cancel( BOOL bDeep )

/*	[Beschreibung]

	Diese Methode markiert alle angemeldeten <SfxCancellable>-Instanzen
	als suspendiert.
*/

{
    ::vos::OGuard aGuard( lclMutex::get() );
	SfxCancelManagerWeak xWeak( this );
	for ( USHORT n = _aJobs.Count(); n-- && xWeak.Is(); )
		if ( n < _aJobs.Count() )
			_aJobs.GetObject(n)->Cancel();
	if ( xWeak.Is() && _pParent )
		_pParent->Cancel( bDeep );
}

//-------------------------------------------------------------------------

void SfxCancelManager::InsertCancellable( SfxCancellable *pJob )

/*	[Beschreibung]

	Diese interne Methode tr"agt 'pJob' in die Liste der unterbrechbaren
	Jobs ein und Broadcastet dies. Jeder <SfxCancellable> darf nur
	maximal einmal angemeldet sein, dies geschiet in seinem Ctor.
*/

{
#ifdef GPF_ON_EMPTY_TITLE
	if ( !pJob->GetTitle() )
	{
		DBG_ERROR( "SfxCancellable: empty titles not allowed (Vermummungsverbot)" )
		*(int*)0 = 0;
	}
#endif

    ::vos::OClearableGuard aGuard( lclMutex::get() );
	_aJobs.C40_INSERT( SfxCancellable, pJob, _aJobs.Count() );

    aGuard.clear();
	Broadcast( SfxSimpleHint( SFX_HINT_CANCELLABLE ) );
}

//-------------------------------------------------------------------------


void SfxCancelManager::RemoveCancellable( SfxCancellable *pJob )

/*	[Beschreibung]

	Diese interne Methode tr"agt 'pJob' aus die Liste der unterbrechbaren
	Jobs aus und Broadcastet dies. Dieser Aufruf mu\s paarig nach einem
	<InsertCancellable> erfolgen und wird im Dtor des <SfxCancellable>
	ausgel"ost.
*/

{
    ::vos::OClearableGuard aGuard( lclMutex::get() );
	const SfxCancellable *pTmp = pJob;
    USHORT nPos = _aJobs.GetPos( pTmp );
    if ( nPos != 0xFFFF )
    {
        _aJobs.Remove( nPos , 1 );
        aGuard.clear();
        Broadcast( SfxSimpleHint( SFX_HINT_CANCELLABLE ) );
        Broadcast( SfxCancelHint( pJob, SFXCANCELHINT_REMOVED ) );
    }
}

//-------------------------------------------------------------------------

SfxCancellable::~SfxCancellable()
{
    SfxCancelManager* pMgr = _pMgr;
    if ( pMgr )
        pMgr->RemoveCancellable( this );
}

//-------------------------------------------------------------------------

void SfxCancellable::Cancel()

/*	[Description]

	This virtual function is called when the user hits the cancel-button.
	If you overload it, you can stop your activities. Please always call
	'SfxCancellable::Cancel()'.
*/

{
#ifdef GFP_ON_NO_CANCEL
	if ( _bCancelled < 5 )
		++_bCancelled;
	else
	{
		delete this;
	}
#else
	_bCancelled = TRUE;
#endif
}

//-------------------------------------------------------------------------

void SfxCancellable::SetManager( SfxCancelManager *pMgr )
{
    SfxCancelManager* pTmp = _pMgr;
    if ( pTmp )
        pTmp->RemoveCancellable( this );
	_pMgr = pMgr;
    if ( pMgr )
        pMgr->InsertCancellable( this );
}

//-------------------------------------------------------------------------

TYPEINIT1(SfxCancelHint, SfxHint);

SfxCancelHint::SfxCancelHint( SfxCancellable* pJob, USHORT _nAction )
{
	pCancellable = pJob;
	nAction = _nAction;
}


}
