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


#ifndef _XMLOFF_TXTSTYLI_HXX_
#define _XMLOFF_TXTSTYLI_HXX_

#ifndef _XMLOFF_PRSTYLEI_HXX_
#include <bf_xmloff/prstylei.hxx>
#endif
namespace binfilter {

class SvXMLTokenMap;
class XMLEventsImportContext;

class XMLTextStyleContext : public XMLPropStyleContext
{
	::rtl::OUString				sListStyleName;
	::rtl::OUString				sCategoryVal;
	::rtl::OUString				sDropCapTextStyleName;
	::rtl::OUString				sMasterPageName;
	const ::rtl::OUString		sIsAutoUpdate;
	const ::rtl::OUString		sCategory;
	const ::rtl::OUString		sNumberingStyleName;
public:
	const ::rtl::OUString		sDropCapCharStyleName;
private:
	const ::rtl::OUString		sPageDescName;

	sal_Bool	bAutoUpdate : 1;
	sal_Bool	bHasMasterPageName : 1;

	sal_Bool bHasCombinedCharactersLetter : 1;

	XMLEventsImportContext* pEventContext;

protected:

	virtual void SetAttribute( sal_uInt16 nPrefixKey,
							   const ::rtl::OUString& rLocalName,
							   const ::rtl::OUString& rValue );

public:

	TYPEINFO();

	XMLTextStyleContext( SvXMLImport& rImport, sal_uInt16 nPrfx,
			const ::rtl::OUString& rLName,
			const ::com::sun::star::uno::Reference<
				::com::sun::star::xml::sax::XAttributeList > & xAttrList,
			SvXMLStylesContext& rStyles, sal_uInt16 nFamily,
		    sal_Bool bDefaultStyle = sal_False );
	virtual ~XMLTextStyleContext();

	virtual SvXMLImportContext *CreateChildContext(
			sal_uInt16 nPrefix,
			const ::rtl::OUString& rLocalName,
			const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList > & xAttrList );

	sal_Bool IsAutoUpdate() const { return bAutoUpdate; }

	const ::rtl::OUString& GetListStyle() const { return sListStyleName; }
	const ::rtl::OUString& GetMasterPageName() const { return sMasterPageName; }
	sal_Bool HasMasterPageName() const { return bHasMasterPageName; }
	const ::rtl::OUString& GetDropCapStyleName() const { return sDropCapTextStyleName; }

	virtual void CreateAndInsert( sal_Bool bOverwrite );
	virtual void Finish( sal_Bool bOverwrite );
	virtual void SetDefaults();

	// overload FillPropertySet, so we can get at the combined characters
	virtual void FillPropertySet(
			const ::com::sun::star::uno::Reference<
				::com::sun::star::beans::XPropertySet > & rPropSet );

	inline sal_Bool HasCombinedCharactersLetter() 
		{ return bHasCombinedCharactersLetter; }

#ifdef CONV_STAR_FONTS
	const ::std::vector< XMLPropertyState > & _GetProperties() { return GetProperties(); }
#endif
};

}//end of namespace binfilter
#endif
