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
#include "precompiled_sw.hxx"


#define _SVSTDARR_STRINGSDTOR
#define _SVSTDARR_USHORTS
#define _SVSTDARR_LONGS
#define _SVSTDARR_ULONGS
#include <hintids.hxx>

#include <svl/svstdarr.hxx>
#include <editeng/unolingu.hxx>
#include <editeng/langitem.hxx>
#include <swtypes.hxx>
#include <tools/resid.hxx>
#include <comcore.hrc>
#include <authfld.hxx>
#include <expfld.hxx>
#include <pam.hxx>
#include <cntfrm.hxx>
#include <tox.hxx>
#include <txmsrt.hxx>
#include <doctxm.hxx>
#include <fmtfld.hxx>
#include <txtfld.hxx>
#include <ndtxt.hxx>
#include <doc.hxx>
#include <unofldmid.h>
#include <unoprnms.hxx>
#include <switerator.hxx>
#include <unomid.h>

using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::beans;
using namespace ::com::sun::star::lang;
using rtl::OUString;


typedef SwAuthEntry* SwAuthEntryPtr;
SV_DECL_PTRARR_DEL( SwAuthDataArr, SwAuthEntryPtr, 5, 5 )
SV_IMPL_PTRARR( SwAuthDataArr, SwAuthEntryPtr )


typedef SwTOXSortKey* TOXSortKeyPtr;
SV_DECL_PTRARR_DEL( SortKeyArr, TOXSortKeyPtr, 5, 5 )
SV_IMPL_PTRARR( SortKeyArr, TOXSortKeyPtr )


SwAuthEntry::SwAuthEntry(const SwAuthEntry& rCopy)
	: nRefCount(0)
{
	for(sal_uInt16 i = 0; i < AUTH_FIELD_END; i++)
		aAuthFields[i] = rCopy.aAuthFields[i];
}
// --------------------------------------------------------
sal_Bool 	SwAuthEntry::operator==(const SwAuthEntry& rComp)
{
	for(sal_uInt16 i = 0; i < AUTH_FIELD_END; i++)
		if(aAuthFields[i] != rComp.aAuthFields[i])
			return sal_False;
	return sal_True;
}
// --------------------------------------------------------
SwAuthorityFieldType::SwAuthorityFieldType(SwDoc* pDoc)
	: SwFieldType( RES_AUTHORITY ),
	m_pDoc(pDoc),
	m_pDataArr(new SwAuthDataArr ),
	m_pSequArr(new SvLongs(5, 5)),
	m_pSortKeyArr(new SortKeyArr(3, 3)),
	m_cPrefix('['),
    m_cSuffix(']'),
	m_bIsSequence(sal_False),
	m_bSortByDocument(sal_True),
    m_eLanguage((LanguageType)::GetAppLanguage())
{
}

SwAuthorityFieldType::SwAuthorityFieldType( const SwAuthorityFieldType& rFType)
	: SwFieldType( RES_AUTHORITY ),
	m_pDataArr(new SwAuthDataArr ),
	m_pSequArr(new SvLongs(5, 5)),
	m_pSortKeyArr(new SortKeyArr(3, 3)),
	m_cPrefix(rFType.m_cPrefix),
    m_cSuffix(rFType.m_cSuffix),
	m_bIsSequence(rFType.m_bIsSequence),
	m_bSortByDocument(rFType.m_bSortByDocument),
    m_eLanguage(rFType.m_eLanguage),
    m_sSortAlgorithm(rFType.m_sSortAlgorithm)
{
	for(sal_uInt16 i = 0; i < rFType.m_pSortKeyArr->Count(); i++)
		m_pSortKeyArr->Insert((*rFType.m_pSortKeyArr)[i], i);
}

SwAuthorityFieldType::~SwAuthorityFieldType()
{
//	DBG_ASSERT(!m_pDataArr->Count(), "Array is not empty");
	m_pSortKeyArr->DeleteAndDestroy(0, m_pSortKeyArr->Count());
	delete m_pSortKeyArr;
	delete m_pSequArr;
	delete m_pDataArr;
}

SwFieldType* 	SwAuthorityFieldType::Copy()  const
{
	return new SwAuthorityFieldType(m_pDoc);
}

void	SwAuthorityFieldType::RemoveField(long nHandle)
{
#ifdef DBG_UTIL
	sal_Bool bRemoved = sal_False;
#endif
	for(sal_uInt16 j = 0; j < m_pDataArr->Count(); j++)
	{
		SwAuthEntry* pTemp = m_pDataArr->GetObject(j);
		long nRet = (long)(void*)pTemp;
		if(nRet == nHandle)
		{
#ifdef DBG_UTIL
			bRemoved = sal_True;
#endif
			pTemp->RemoveRef();
			if(!pTemp->GetRefCount())
			{
				m_pDataArr->DeleteAndDestroy(j, 1);
				//re-generate positions of the fields
				DelSequenceArray();
			}
			break;
		}
	}
#ifdef DBG_UTIL
	DBG_ASSERT(bRemoved, "Field unknown" );
#endif
}

long	SwAuthorityFieldType::AddField(const String& rFieldContents)
{
	long nRet = 0;
	SwAuthEntry* pEntry = new SwAuthEntry;
	for( sal_uInt16 i = 0; i < AUTH_FIELD_END; ++i )
		pEntry->SetAuthorField( (ToxAuthorityField)i,
						rFieldContents.GetToken( i, TOX_STYLE_DELIMITER ));

	for(sal_uInt16 j = 0; j < m_pDataArr->Count() && pEntry; j++)
	{
		SwAuthEntry* pTemp = m_pDataArr->GetObject(j);
		if(*pTemp == *pEntry)
		{
			DELETEZ(pEntry);
			nRet = (long)(void*)pTemp;
			pTemp->AddRef();
		}
	}
	//if it is a new Entry - insert
	if(pEntry)
	{
		nRet = (long)(void*)pEntry;
		pEntry->AddRef();
		m_pDataArr->Insert(pEntry, m_pDataArr->Count());
		//re-generate positions of the fields
		DelSequenceArray();
	}
	return nRet;
}

sal_Bool SwAuthorityFieldType::AddField(long nHandle)
{
	sal_Bool bRet = sal_False;
	for( sal_uInt16 j = 0; j < m_pDataArr->Count(); j++ )
	{
		SwAuthEntry* pTemp = m_pDataArr->GetObject(j);
		long nTmp = (long)(void*)pTemp;
		if( nTmp == nHandle )
		{
			bRet = sal_True;
			pTemp->AddRef();
			//re-generate positions of the fields
			DelSequenceArray();
			break;
		}
	}
	DBG_ASSERT(bRet, "::AddField(long) failed");
	return bRet;
}

const SwAuthEntry*	SwAuthorityFieldType::GetEntryByHandle(long nHandle) const
{
	const SwAuthEntry* pRet = 0;
	for(sal_uInt16 j = 0; j < m_pDataArr->Count(); j++)
	{
		const SwAuthEntry* pTemp = m_pDataArr->GetObject(j);
		long nTmp = (long)(void*)pTemp;
		if( nTmp == nHandle )
		{
			pRet = pTemp;
			break;
		}
	}
	ASSERT( pRet, "invalid Handle" );
	return pRet;
}

void SwAuthorityFieldType::GetAllEntryIdentifiers(
				SvStringsDtor& rToFill )const
{
	for(sal_uInt16 j = 0; j < m_pDataArr->Count(); j++)
	{
		SwAuthEntry* pTemp = m_pDataArr->GetObject(j);
		rToFill.Insert( new String( pTemp->GetAuthorField(
					AUTH_FIELD_IDENTIFIER )), rToFill.Count() );
	}
}

const SwAuthEntry* 	SwAuthorityFieldType::GetEntryByIdentifier(
								const String& rIdentifier)const
{
	const SwAuthEntry* pRet = 0;
	for( sal_uInt16 j = 0; j < m_pDataArr->Count(); ++j )
	{
		const SwAuthEntry* pTemp = m_pDataArr->GetObject(j);
		if( rIdentifier == pTemp->GetAuthorField( AUTH_FIELD_IDENTIFIER ))
		{
			pRet = pTemp;
			break;
		}
	}
	return pRet;
}

bool SwAuthorityFieldType::ChangeEntryContent(const SwAuthEntry* pNewEntry)
{
    bool bChanged = false;
	for( sal_uInt16 j = 0; j < m_pDataArr->Count(); ++j )
	{
		SwAuthEntry* pTemp = m_pDataArr->GetObject(j);
		if(pTemp->GetAuthorField(AUTH_FIELD_IDENTIFIER) ==
					pNewEntry->GetAuthorField(AUTH_FIELD_IDENTIFIER))
		{
			for(sal_uInt16 i = 0; i < AUTH_FIELD_END; i++)
				pTemp->SetAuthorField((ToxAuthorityField) i,
					pNewEntry->GetAuthorField((ToxAuthorityField)i));
            bChanged = true;
			break;
		}
	}
    return bChanged;
}

/*-------------------------------------------------------------------------
  appends a new entry (if new) and returns the array position
  -----------------------------------------------------------------------*/
sal_uInt16	SwAuthorityFieldType::AppendField( const SwAuthEntry& rInsert )
{
	sal_uInt16 nRet = 0;
	for( nRet = 0; nRet < m_pDataArr->Count(); ++nRet )
	{
		SwAuthEntry* pTemp = m_pDataArr->GetObject( nRet );
		if( *pTemp == rInsert )
		{
			break;
			//ref count unchanged
		}
	}

	//if it is a new Entry - insert
	if( nRet == m_pDataArr->Count() )
		m_pDataArr->Insert( new SwAuthEntry( rInsert ), nRet );

	return nRet;
}

long	SwAuthorityFieldType::GetHandle(sal_uInt16 nPos)
{
	long nRet = 0;
	if( nPos < m_pDataArr->Count() )
	{
		SwAuthEntry* pTemp = m_pDataArr->GetObject(nPos);
		nRet = (long)(void*)pTemp;
	}
	return nRet;
}

sal_uInt16	SwAuthorityFieldType::GetSequencePos(long nHandle)
{
	//find the field in a sorted array of handles,
#ifdef DBG_UTIL
	sal_Bool bCurrentFieldWithoutTextNode = sal_False;
#endif
	if(m_pSequArr->Count() && m_pSequArr->Count() != m_pDataArr->Count())
		DelSequenceArray();
	if(!m_pSequArr->Count())
	{
		SwTOXSortTabBases aSortArr;
		SwIterator<SwFmtFld,SwFieldType> aIter( *this );

        SwTOXInternational aIntl(m_eLanguage, 0, m_sSortAlgorithm);

		for( SwFmtFld* pFmtFld = aIter.First(); pFmtFld; pFmtFld = aIter.Next() )
		{
            const SwTxtFld* pTxtFld = pFmtFld->GetTxtFld();
            if(!pTxtFld || !pTxtFld->GetpTxtNode())
			{
#ifdef DBG_UTIL
                if(nHandle == ((SwAuthorityField*)pFmtFld->GetFld())->GetHandle())
					bCurrentFieldWithoutTextNode = sal_True;
#endif
				continue;
			}
            const SwTxtNode& rFldTxtNode = pTxtFld->GetTxtNode();
            SwPosition aFldPos(rFldTxtNode);
            SwDoc& rDoc = *(SwDoc*)rFldTxtNode.GetDoc();
            SwCntntFrm *pFrm = rFldTxtNode.getLayoutFrm( rDoc.GetCurrentLayout() );
            const SwTxtNode* pTxtNode = 0;
            if(pFrm && !pFrm->IsInDocBody())
                pTxtNode = GetBodyTxtNode( rDoc, aFldPos, *pFrm );
            //if no text node could be found or the field is in the document
            //body the directly available text node will be used
            if(!pTxtNode)
                pTxtNode = &rFldTxtNode;
            if( pTxtNode->GetTxt().Len() && pTxtNode->getLayoutFrm( rDoc.GetCurrentLayout() ) &&
                pTxtNode->GetNodes().IsDocNodes() )
			{
                SwTOXAuthority* pNew = new SwTOXAuthority( *pTxtNode,
															*pFmtFld, aIntl );

				for(short i = 0; i < aSortArr.Count(); ++i)
				{
					SwTOXSortTabBase* pOld = aSortArr[i];
					if(*pOld == *pNew)
					{
						//only the first occurence in the document
						//has to be in the array
						if(*pOld < *pNew)
							DELETEZ(pNew);
						else // remove the old content
							aSortArr.DeleteAndDestroy( i, 1 );
						break;
					}
				}
				//if it still exists - insert at the correct position
				if(pNew)
				{
					short j;

					for( j = 0; j < aSortArr.Count(); ++j)
					{
						SwTOXSortTabBase* pOld = aSortArr[j];
						if(*pNew < *pOld)
							break;
					}
					aSortArr.Insert(pNew, j );
				}
			}
		}

		for(sal_uInt16 i = 0; i < aSortArr.Count(); i++)
		{
			const SwTOXSortTabBase& rBase = *aSortArr[i];
			SwFmtFld& rFmtFld = ((SwTOXAuthority&)rBase).GetFldFmt();
			SwAuthorityField* pAFld = (SwAuthorityField*)rFmtFld.GetFld();
			m_pSequArr->Insert(pAFld->GetHandle(), i);
		}
		aSortArr.DeleteAndDestroy(0, aSortArr.Count());
	}
	//find nHandle
	sal_uInt16 nRet = 0;
	for(sal_uInt16 i = 0; i < m_pSequArr->Count(); i++)
	{
		if((*m_pSequArr)[i] == nHandle)
		{
			nRet = i + 1;
			break;
		}
	}
	ASSERT(bCurrentFieldWithoutTextNode || nRet, "Handle not found")
	return nRet;
}

sal_Bool    SwAuthorityFieldType::QueryValue( Any& rVal, sal_uInt16 nWhichId ) const
{
    switch( nWhichId )
	{
	case FIELD_PROP_PAR1:
	case FIELD_PROP_PAR2:
		{
			OUString sVal;
            sal_Unicode uRet = FIELD_PROP_PAR1 == nWhichId ? m_cPrefix : m_cSuffix;
			if(uRet)
				sVal = OUString(uRet);
			rVal <<= sVal;
		}
		break;
	case FIELD_PROP_PAR3:
        rVal <<= OUString(GetSortAlgorithm());
		break;

	case FIELD_PROP_BOOL1:
	case FIELD_PROP_BOOL2:
		{
            sal_Bool bVal = FIELD_PROP_BOOL1 == nWhichId ? m_bIsSequence: m_bSortByDocument;
			rVal.setValue(&bVal, ::getBooleanCppuType());
		}
		break;

	case FIELD_PROP_LOCALE:
        rVal <<= SvxCreateLocale(GetLanguage());
		break;

	case FIELD_PROP_PROP_SEQ:
		{
			Sequence<PropertyValues> aRet(m_pSortKeyArr->Count());
			PropertyValues* pValues = aRet.getArray();
			OUString sProp1( C2U(SW_PROP_NAME_STR(UNO_NAME_SORT_KEY)) ),
					 sProp2( C2U(SW_PROP_NAME_STR(UNO_NAME_IS_SORT_ASCENDING)));
			for(sal_uInt16 i = 0; i < m_pSortKeyArr->Count(); i++)
			{
				const SwTOXSortKey*	pKey = (*m_pSortKeyArr)[i];
				pValues[i].realloc(2);
				PropertyValue* pValue = pValues[i].getArray();
				pValue[0].Name = sProp1;
				pValue[0].Value <<= sal_Int16(pKey->eField);
				pValue[1].Name = sProp2;
				pValue[1].Value.setValue(&pKey->bSortAscending, ::getBooleanCppuType());
			}
			rVal <<= aRet;
		}
		break;
	default:
		DBG_ERROR("illegal property");
	}
	return sal_True;
}

sal_Bool    SwAuthorityFieldType::PutValue( const Any& rAny, sal_uInt16 nWhichId )
{
	sal_Bool bRet = sal_True;
	String sTmp;
    switch( nWhichId )
	{
	case FIELD_PROP_PAR1:
	case FIELD_PROP_PAR2:
    {
        ::GetString( rAny, sTmp );
        sal_Unicode uSet = sTmp.GetChar(0);
        if( FIELD_PROP_PAR1 == nWhichId )
            m_cPrefix = uSet;
        else
            m_cSuffix = uSet;
    }
    break;
	case FIELD_PROP_PAR3:
		SetSortAlgorithm( ::GetString( rAny, sTmp ));
		break;

	case FIELD_PROP_BOOL1:
		m_bIsSequence = *(sal_Bool*)rAny.getValue();
		break;
	case FIELD_PROP_BOOL2:
		m_bSortByDocument = *(sal_Bool*)rAny.getValue();
		break;

	case FIELD_PROP_LOCALE:
		{
	        Locale aLocale;
    	    if( 0 != (bRet = rAny >>= aLocale ))
	            SetLanguage( SvxLocaleToLanguage( aLocale ));
		}
		break;

	case FIELD_PROP_PROP_SEQ:
		{
			Sequence<PropertyValues> aSeq;
			if( 0 != (bRet = rAny >>= aSeq) )
			{
				m_pSortKeyArr->DeleteAndDestroy(0, m_pSortKeyArr->Count());
				const PropertyValues* pValues = aSeq.getConstArray();
				for(sal_Int32 i = 0; i < aSeq.getLength() && i < USHRT_MAX / 4; i++)
				{
					const PropertyValue* pValue = pValues[i].getConstArray();
					SwTOXSortKey* pSortKey = new SwTOXSortKey;
					for(sal_Int32 j = 0; j < pValues[i].getLength(); j++)
					{
						if(pValue[j].Name.equalsAsciiL(SW_PROP_NAME(UNO_NAME_SORT_KEY)))
						{
							sal_Int16 nVal = -1; pValue[j].Value >>= nVal;
							if(nVal >= 0 && nVal < AUTH_FIELD_END)
								pSortKey->eField = (ToxAuthorityField) nVal;
							else
								bRet = sal_False;
						}
						else if(pValue[j].Name.equalsAsciiL(SW_PROP_NAME(UNO_NAME_IS_SORT_ASCENDING)))
						{
							pSortKey->bSortAscending = *(sal_Bool*)pValue[j].Value.getValue();
						}
					}
					m_pSortKeyArr->Insert(pSortKey, m_pSortKeyArr->Count());
				}
			}
		}
		break;
	default:
		DBG_ERROR("illegal property");
	}
	return bRet;
}

void SwAuthorityFieldType::Modify( const SfxPoolItem* pOld, const SfxPoolItem *pNew )
{
	//re-generate positions of the fields
	DelSequenceArray();
	NotifyClients( pOld, pNew );
}

sal_uInt16 SwAuthorityFieldType::GetSortKeyCount() const
{
	return m_pSortKeyArr->Count();
}

const SwTOXSortKey*  SwAuthorityFieldType::GetSortKey(sal_uInt16 nIdx) const
{
	SwTOXSortKey* pRet = 0;
	if(m_pSortKeyArr->Count() > nIdx)
		pRet = (*m_pSortKeyArr)[nIdx];
	DBG_ASSERT(pRet, "Sort key not found");
	return pRet;
}

void SwAuthorityFieldType::SetSortKeys(sal_uInt16 nKeyCount, SwTOXSortKey aKeys[])
{
	m_pSortKeyArr->DeleteAndDestroy(0, m_pSortKeyArr->Count());
	sal_uInt16 nArrIdx = 0;
	for(sal_uInt16 i = 0; i < nKeyCount; i++)
		if(aKeys[i].eField < AUTH_FIELD_END)
			m_pSortKeyArr->Insert(new SwTOXSortKey(aKeys[i]), nArrIdx++);
}

SwAuthorityField::SwAuthorityField( SwAuthorityFieldType* pInitType,
									const String& rFieldContents )
    : SwField(pInitType),
    m_nTempSequencePos( -1 )
{
    m_nHandle = pInitType->AddField( rFieldContents );
}

SwAuthorityField::SwAuthorityField( SwAuthorityFieldType* pInitType,
												long nSetHandle )
	: SwField( pInitType ),
    m_nHandle( nSetHandle ),
    m_nTempSequencePos( -1 )
{
    pInitType->AddField( m_nHandle );
}

SwAuthorityField::~SwAuthorityField()
{
    ((SwAuthorityFieldType* )GetTyp())->RemoveField(m_nHandle);
}

String	SwAuthorityField::Expand() const
{
	SwAuthorityFieldType* pAuthType = (SwAuthorityFieldType*)GetTyp();
	String sRet;
	if(pAuthType->GetPrefix())
		sRet.Assign(pAuthType->GetPrefix());

	if( pAuthType->IsSequence() )
	{
       if(!pAuthType->GetDoc()->IsExpFldsLocked())
           m_nTempSequencePos = pAuthType->GetSequencePos( m_nHandle );
       if( m_nTempSequencePos >= 0 ) 
           sRet += String::CreateFromInt32( m_nTempSequencePos );
	}
	else
	{
        const SwAuthEntry* pEntry = pAuthType->GetEntryByHandle(m_nHandle);
		//TODO: Expand to: identifier, number sequence, ...
		if(pEntry)
			sRet += pEntry->GetAuthorField(AUTH_FIELD_IDENTIFIER);
	}
	if(pAuthType->GetSuffix())
		sRet += pAuthType->GetSuffix();
	return sRet;
}

SwField* SwAuthorityField::Copy() const
{
	SwAuthorityFieldType* pAuthType = (SwAuthorityFieldType*)GetTyp();
    return new SwAuthorityField(pAuthType, m_nHandle);
}

const String&	SwAuthorityField::GetFieldText(ToxAuthorityField eField) const
{
	SwAuthorityFieldType* pAuthType = (SwAuthorityFieldType*)GetTyp();
    const SwAuthEntry* pEntry = pAuthType->GetEntryByHandle( m_nHandle );
	return pEntry->GetAuthorField( eField );
}

void	SwAuthorityField::SetPar1(const String& rStr)
{
	SwAuthorityFieldType* pInitType = (SwAuthorityFieldType* )GetTyp();
    pInitType->RemoveField(m_nHandle);
    m_nHandle = pInitType->AddField(rStr);
}

String SwAuthorityField::GetDescription() const
{
    return SW_RES(STR_AUTHORITY_ENTRY);
}


const char* aFieldNames[] =
{
	"Identifier",
	"BibiliographicType",
	"Address",
	"Annote",
	"Author",
	"Booktitle",
	"Chapter",
	"Edition",
	"Editor",
	"Howpublished",
	"Institution",
	"Journal",
	"Month",
	"Note",
	"Number",
	"Organizations",
	"Pages",
	"Publisher",
	"School",
	"Series",
	"Title",
	"Report_Type",
	"Volume",
	"Year",
	"URL",
	"Custom1",
	"Custom2",
	"Custom3",
	"Custom4",
	"Custom5",
	"ISBN"
};

sal_Bool    SwAuthorityField::QueryValue( Any& rAny, sal_uInt16 /*nWhichId*/ ) const
{
	if(!GetTyp())
		return sal_False;
    const SwAuthEntry* pAuthEntry = ((SwAuthorityFieldType*)GetTyp())->GetEntryByHandle(m_nHandle);
	if(!pAuthEntry)
		return sal_False;
	Sequence <PropertyValue> aRet(AUTH_FIELD_END);
	PropertyValue* pValues = aRet.getArray();
	for(sal_Int16 i = 0; i < AUTH_FIELD_END; i++)
	{
		pValues[i].Name = C2U(aFieldNames[i]);
		const String& rField = pAuthEntry->GetAuthorField((ToxAuthorityField) i);
		if(i == AUTH_FIELD_AUTHORITY_TYPE)
			pValues[i].Value <<= sal_Int16(rField.ToInt32());
		else
			pValues[i].Value <<= OUString(rField);
	}
	rAny <<= aRet;
	return sal_False;
}

sal_Int16 lcl_Find(const OUString& rFieldName)
{
	for(sal_Int16 i = 0; i < AUTH_FIELD_END; i++)
		if(!rFieldName.compareToAscii(aFieldNames[i]))
			return i;
	return -1;
}
//----------------------------------------------------------------------------
sal_Bool    SwAuthorityField::PutValue( const Any& rAny, sal_uInt16 /*nWhichId*/ )
{
    if(!GetTyp() || !((SwAuthorityFieldType*)GetTyp())->GetEntryByHandle(m_nHandle))
		return sal_False;

	Sequence <PropertyValue> aParam;
	if(!(rAny >>= aParam))
		return sal_False;

	String sToSet;
	sToSet.Fill(AUTH_FIELD_ISBN, TOX_STYLE_DELIMITER);
	const PropertyValue* pParam = aParam.getConstArray();
	for(sal_Int32 i = 0; i < aParam.getLength(); i++)
	{
		sal_Int16 nFound = lcl_Find(pParam[i].Name);
		if(nFound >= 0)
		{
			OUString sContent;
			if(AUTH_FIELD_AUTHORITY_TYPE == nFound)
			{
				sal_Int16 nVal = 0;
				pParam[i].Value >>= nVal;
				sContent = OUString::valueOf((sal_Int32)nVal);
			}
			else
				pParam[i].Value >>= sContent;
			sToSet.SetToken(nFound, TOX_STYLE_DELIMITER, sContent);
		}
	}

    ((SwAuthorityFieldType*)GetTyp())->RemoveField(m_nHandle);
    m_nHandle = ((SwAuthorityFieldType*)GetTyp())->AddField(sToSet);

	return sal_False;
}

SwFieldType* SwAuthorityField::ChgTyp( SwFieldType* pFldTyp )
{
	SwAuthorityFieldType* pSrcTyp = (SwAuthorityFieldType*)GetTyp(),
						* pDstTyp = (SwAuthorityFieldType*)pFldTyp;
	if( pSrcTyp != pDstTyp )
	{

        const SwAuthEntry* pEntry = pSrcTyp->GetEntryByHandle( m_nHandle );
		sal_uInt16 nHdlPos = pDstTyp->AppendField( *pEntry );
        pSrcTyp->RemoveField( m_nHandle );
        m_nHandle = pDstTyp->GetHandle( nHdlPos );
        pDstTyp->AddField( m_nHandle );
		SwField::ChgTyp( pFldTyp );
	}
	return pSrcTyp;
}

