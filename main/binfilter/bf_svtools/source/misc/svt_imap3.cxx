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



#include <bf_svtools/imap.hxx>

#include <tools/debug.hxx>

namespace binfilter
{


/******************************************************************************
|*
|* Ctor
|*
\******************************************************************************/

IMapCompat::IMapCompat( SvStream& rStm, const USHORT nStreamMode ) :
			pRWStm		( &rStm ),
			nStmMode	( nStreamMode )
{
	DBG_ASSERT( nStreamMode == STREAM_READ || nStreamMode == STREAM_WRITE, "Wrong Mode!" );

	if ( !pRWStm->GetError() )
	{
		if ( nStmMode == STREAM_WRITE )
		{
			nCompatPos = pRWStm->Tell();
			pRWStm->SeekRel( 4 );
			nTotalSize = nCompatPos + 4;
		}
		else
		{
			UINT32 nTotalSizeTmp;
			*pRWStm >> nTotalSizeTmp;
			nTotalSize = nTotalSizeTmp;
			nCompatPos = pRWStm->Tell();
		}
	}
}


/******************************************************************************
|*
|* Dtor
|*
\******************************************************************************/

IMapCompat::~IMapCompat()
{
	if ( !pRWStm->GetError() )
	{
		if ( nStmMode == STREAM_WRITE )
		{
			const ULONG	nEndPos = pRWStm->Tell();

			pRWStm->Seek( nCompatPos );
			*pRWStm << (UINT32) ( nEndPos - nTotalSize );
			pRWStm->Seek( nEndPos );
		}
		else
		{
			const ULONG nReadSize = pRWStm->Tell() - nCompatPos;

			if ( nTotalSize > nReadSize )
				pRWStm->SeekRel( nTotalSize - nReadSize );
		}
	}
}



}
