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



#ifndef _XMLOFF_NUMBERSTYLESIMPORT_HXX
#define _XMLOFF_NUMBERSTYLESIMPORT_HXX

#ifndef _XMLOFF_XMLNUMFI_HXX
#include "xmlnumfi.hxx"
#endif

#ifndef _COM_SUN_STAR_CONTAINER_XNAMEACCESS_HPP_ 
#include <com/sun/star/container/XNameAccess.hpp>
#endif
namespace binfilter {

//////////////////////////////////////////////////////////////////////////////
// presentations:animations

struct SdXMLFixedDataStyle;
class SdXMLImport;

class SdXMLNumberFormatImportContext : public SvXMLNumFormatContext
{
private:
	friend class SdXMLNumberFormatMemberImportContext;

	SdXMLImport& mrImport;

	sal_Bool	mbTimeStyle;
	sal_Bool	mbAutomatic;
	sal_uInt8	mnElements[8];
	sal_Int16	mnIndex;

	sal_Int32	mnKey;

	sal_Bool compareStyle( const SdXMLFixedDataStyle* pStyle ) const;

protected:
	void add( ::rtl::OUString& rNumberStyle, sal_Bool bLong, sal_Bool bTextual, sal_Bool	bDecimal02, ::rtl::OUString& rText );

public:
	TYPEINFO();

	SdXMLNumberFormatImportContext( SdXMLImport& rImport, 
		sal_uInt16 nPrfx,
		const ::rtl::OUString& rLocalName, 
		SvXMLNumImpData* pNewData, sal_uInt16 nNewType,
		const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList>& xAttrList,
		SvXMLStylesContext& rStyles);
	virtual ~SdXMLNumberFormatImportContext();

	virtual void EndElement();

	virtual SvXMLImportContext * CreateChildContext( USHORT nPrefix, const ::rtl::OUString& rLocalName,
		const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList>& xAttrList );

	sal_Int32 GetDrawKey() const { return mnKey; }
};

}//end of namespace binfilter
#endif	//  _XMLOFF_NUMBERSTYLESIMPORT_HXX

