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


#ifndef _AUTHFLD_HXX
#define _AUTHFLD_HXX

#include <bf_svtools/bf_solar.h>

#ifndef _FLDBAS_HXX
#include <fldbas.hxx>
#endif
#ifndef _TOXE_HXX
#include <toxe.hxx>
#endif

#define _SVSTDARR_LONGS
#include <bf_svtools/svstdarr.hxx>
namespace binfilter {
class SvUShorts; 
extern String& GetString( const ::com::sun::star::uno::Any& rAny, String& rStr ); //STRIP008

class SwAuthDataArr;
/* -----------------21.09.99 13:32-------------------

 --------------------------------------------------*/
class SwAuthEntry
{
	String 		aAuthFields[AUTH_FIELD_END];
	USHORT 		nRefCount;
public:
    SwAuthEntry() : nRefCount(0){}
    SwAuthEntry( const SwAuthEntry& rCopy );
    BOOL            operator==(const SwAuthEntry& rComp);

    BOOL                    GetFirstAuthorField(USHORT& nPos, String& rToFill)const;
    BOOL                    GetNextAuthorField(USHORT& nPos, String& rToFill)const;
    inline const String&    GetAuthorField(ToxAuthorityField ePos)const;
    inline void             SetAuthorField(ToxAuthorityField ePos,
                                            const String& rField);

    void            AddRef()                { ++nRefCount; }
    void            RemoveRef()             { --nRefCount; }
    USHORT          GetRefCount()           { return nRefCount; }
};
/* -----------------20.10.99 16:49-------------------

 --------------------------------------------------*/
struct SwTOXSortKey
{
	ToxAuthorityField	eField;
	BOOL				bSortAscending;
	SwTOXSortKey() :
		eField(AUTH_FIELD_END),
		bSortAscending(TRUE){}
};

/* -----------------14.09.99 16:15-------------------

 --------------------------------------------------*/

class SwAuthorityField;
class SortKeyArr;
class SwAuthorityFieldType : public SwFieldType
{
    SwDoc*          m_pDoc;
    SwAuthDataArr*  m_pDataArr;
    SvLongs*        m_pSequArr;
    SortKeyArr*     m_pSortKeyArr;
    sal_Unicode     m_cPrefix;
    sal_Unicode     m_cSuffix;
    BOOL            m_bIsSequence :1;
    BOOL            m_bSortByDocument :1;
    LanguageType    m_eLanguage;
    String          m_sSortAlgorithm;

    const SwAuthorityFieldType& operator=( const SwAuthorityFieldType& );

public:
    SwAuthorityFieldType(SwDoc* pDoc);
    SwAuthorityFieldType( const SwAuthorityFieldType& );
    ~SwAuthorityFieldType();

    virtual SwFieldType* Copy()    const;
    virtual void        Modify( SfxPoolItem *pOld, SfxPoolItem *pNew );

    virtual BOOL        QueryValue( ::com::sun::star::uno::Any& rVal, BYTE nMId ) const;
    virtual BOOL        PutValue( const ::com::sun::star::uno::Any& rVal, BYTE nMId );

    inline SwDoc*   GetDoc() const                      { return m_pDoc; }
    inline void     SetDoc(SwDoc* pNewDoc)              { m_pDoc = pNewDoc; }

    void                RemoveField(long nHandle);
    long                AddField(const String& rFieldContents);
    BOOL                AddField(long nHandle);
    void                DelSequenceArray()
                        {
                            m_pSequArr->Remove(0, m_pSequArr->Count());
                        }

    const SwAuthEntry*  GetEntryByHandle(long nHandle) const;

    // import interface
    USHORT              AppendField(const SwAuthEntry& rInsert);
    long                GetHandle(USHORT nPos);
    USHORT              GetPosition(long nHandle);

    USHORT              GetEntryCount() const;
    const SwAuthEntry*  GetEntryByPosition(USHORT nPos) const;

    USHORT              GetSequencePos(long nHandle);

    BOOL                IsSequence() const      {return m_bIsSequence;}
    void                SetSequence(BOOL bSet)
                            {
                                DelSequenceArray();
                                m_bIsSequence = bSet;
                            }

    void                SetPreSuffix( sal_Unicode cPre, sal_Unicode cSuf)
                            {
                                m_cPrefix = cPre;
                                m_cSuffix = cSuf;
                            }
    sal_Unicode         GetPrefix() const { return m_cPrefix;}
    sal_Unicode         GetSuffix() const { return m_cSuffix;}

    BOOL                IsSortByDocument() const {return m_bSortByDocument;}
    void                SetSortByDocument(BOOL bSet)
                            {
                                DelSequenceArray();
                                m_bSortByDocument = bSet;
                            }

    USHORT              GetSortKeyCount() const ;
    const SwTOXSortKey* GetSortKey(USHORT nIdx) const ;
    void                SetSortKeys(USHORT nKeyCount, SwTOXSortKey nKeys[]);
    static const String&    GetAuthTypeName(ToxAuthorityType eType);

    LanguageType    GetLanguage() const {return m_eLanguage;}
    void            SetLanguage(LanguageType nLang)  {m_eLanguage = nLang;}

    const String&   GetSortAlgorithm()const {return m_sSortAlgorithm;}
    void            SetSortAlgorithm(const String& rSet) {m_sSortAlgorithm = rSet;}

};
/* -----------------14.09.99 16:15-------------------

 --------------------------------------------------*/
class SwAuthorityField : public SwField
{
    long    nHandle;

public:
    SwAuthorityField(SwAuthorityFieldType* pType, const String& rFieldContents);
    SwAuthorityField(SwAuthorityFieldType* pType, long nHandle);
    ~SwAuthorityField();

    const String&       GetFieldText(ToxAuthorityField eField) const;

    virtual String      Expand() const;
    virtual SwField*    Copy() const;
    virtual void        SetPar1(const String& rStr);
    virtual SwFieldType* ChgTyp( SwFieldType* );

    virtual BOOL        QueryValue( ::com::sun::star::uno::Any& rVal, BYTE nMId ) const;
    virtual BOOL        PutValue( const ::com::sun::star::uno::Any& rVal, BYTE nMId );

    long                GetHandle() const       { return nHandle; }

	//import interface
    USHORT              GetHandlePosition() const;
};

// --- inlines -----------------------------------------------------------
inline const String&    SwAuthEntry::GetAuthorField(ToxAuthorityField ePos)const
{
    DBG_ASSERT(AUTH_FIELD_END > ePos, "wrong index");
    return aAuthFields[ePos];
}
inline void SwAuthEntry::SetAuthorField(ToxAuthorityField ePos, const String& rField)
{
    DBG_ASSERT(AUTH_FIELD_END > ePos, "wrong index");
    if(AUTH_FIELD_END > ePos)
        aAuthFields[ePos] = rField;
}

} //namespace binfilter
#endif

