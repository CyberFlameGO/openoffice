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



#include <tools/debug.hxx>

#include "xmlscripti.hxx"
#include "xmlnmspe.hxx"
#include "xmlimp.hxx"
#include "nmspmap.hxx"
#include "XMLEventsImportContext.hxx"
#include "xmlbasici.hxx"

#include <com/sun/star/script/XStarBasicAccess.hpp>
#include <com/sun/star/document/XEventsSupplier.hpp>
namespace binfilter {

using namespace rtl;
using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using namespace ::com::sun::star::lang;
using namespace ::com::sun::star::frame;
using namespace ::com::sun::star::script;
using namespace ::com::sun::star::document;
using namespace ::com::sun::star::xml::sax;
using namespace ::binfilter::xmloff::token;


//-------------------------------------------------------------------------

class XMLScriptElementContext;

class XMLScriptModuleContext : public SvXMLImportContext
{
private:
	XMLScriptElementContext&	mrParent;
	OUString					msSource;
	Reference<XStarBasicAccess> mxBasicAccess;

	OUString					msLibName;
	OUString					msModuleName;
	OUString					msLanguage;

public:
	XMLScriptModuleContext( SvXMLImport& rImport, sal_uInt16 nPrfx,
							const OUString& rLName, const OUString& aLibName,
							const Reference<XAttributeList>& xAttrList,
							XMLScriptElementContext& rParentContext,
							Reference<XStarBasicAccess> xBasicAccess );

	virtual ~XMLScriptModuleContext();

	virtual SvXMLImportContext *CreateChildContext( sal_uInt16 nPrefix,
								 const OUString& rLName,
								 const Reference<XAttributeList>& xAttrList );
	virtual void EndElement();
	virtual void Characters( const ::rtl::OUString& rChars );
};

//-------------------------------------------------------------------------

class XMLScriptElementContext : public SvXMLImportContext
{
private:
	XMLScriptContext&			mrParent;
	OUString					msLName;
	OUString					msContent;
	Reference<XStarBasicAccess> mxBasicAccess;

	OUString					msLibName;

public:
	XMLScriptElementContext( SvXMLImport& rImport, sal_uInt16 nPrfx,
							const OUString& rLName,
							const Reference<XAttributeList>& xAttrList,
							XMLScriptContext& rParentContext,
							Reference<XStarBasicAccess> xBasicAccess );
	virtual ~XMLScriptElementContext();

	virtual SvXMLImportContext *CreateChildContext( sal_uInt16 nPrefix,
								 const OUString& rLName,
								 const Reference<XAttributeList>& xAttrList );
	virtual void EndElement();
	virtual void Characters( const ::rtl::OUString& rChars );
};


//-------------------------------------------------------------------------

XMLScriptElementContext::~XMLScriptElementContext()
{
	mrParent.ReleaseRef();
}


SvXMLImportContext* XMLScriptElementContext::CreateChildContext( sal_uInt16 nPrefix,
									 const OUString& rLName,
									 const Reference<XAttributeList>& xAttrList )
{
	SvXMLImportContext* pContext = NULL;

	if ( XML_NAMESPACE_SCRIPT == nPrefix)
	{
		if( IsXMLToken( msLName, XML_LIBRARY_EMBEDDED ) )
		{
			if( IsXMLToken( rLName, XML_MODULE ) )
			{
				pContext = new XMLScriptModuleContext( GetImport(), nPrefix,
					rLName, msLibName, xAttrList, *this, mxBasicAccess );
			}
			//else if( IsXMLToken( rLName, XML_DIALOG ) )
			//{
				//pContext = new XMLScriptDialogContext( GetImport(),
					//nPrefix, rLName, xAttrList, *this, mxBasicAccess );
			//}
		}
	}
	// else: unknown namespace: ignore

	if ( !pContext )
	{
		//	default context to ignore unknown elements
		pContext = new SvXMLImportContext( GetImport(), nPrefix, rLName );
	}
	return pContext;
}

void XMLScriptElementContext::EndElement()
{
}

void XMLScriptElementContext::Characters( const ::rtl::OUString& rChars )
{
	msContent += rChars;
}

//-------------------------------------------------------------------------

XMLScriptModuleContext::XMLScriptModuleContext( SvXMLImport& rImport, sal_uInt16 nPrfx,
									const OUString& rLName, const OUString& aLibName,
									const Reference<XAttributeList>& xAttrList,
									XMLScriptElementContext& rParentContext,
									Reference<XStarBasicAccess> xBasicAccess )
	: SvXMLImportContext( rImport, nPrfx, rLName )
	, msLibName( aLibName )
	, mrParent( rParentContext )
	, mxBasicAccess( xBasicAccess )
{
	mrParent.AddRef();

	sal_Int16 nAttrCount = xAttrList.is() ? xAttrList->getLength() : 0;
	for( sal_Int16 i = 0 ; i < nAttrCount ; i++ )
	{
		OUString sFullAttrName = xAttrList->getNameByIndex( i );
		OUString sAttrName;
		sal_Int16 nAttrPrefix =
			GetImport().GetNamespaceMap().GetKeyByAttrName( sFullAttrName,
															&sAttrName );

		if( (XML_NAMESPACE_SCRIPT == nAttrPrefix) &&
			IsXMLToken( sAttrName, XML_NAME ) )
		{
			msModuleName = xAttrList->getValueByIndex( i );
		}
		else if( (XML_NAMESPACE_SCRIPT == nAttrPrefix) &&
                 IsXMLToken( sAttrName, XML_LANGUAGE ) )
		{
			msLanguage = xAttrList->getValueByIndex( i );
		}
	}
}

XMLScriptModuleContext::~XMLScriptModuleContext()
{
	mrParent.ReleaseRef();
}

SvXMLImportContext* XMLScriptModuleContext::CreateChildContext( sal_uInt16 nPrefix,
									 const OUString& rLName,
									 const Reference<XAttributeList>& xAttrList )
{
	SvXMLImportContext* pContext = new SvXMLImportContext( GetImport(), nPrefix, rLName );
	return pContext;
}

void XMLScriptModuleContext::EndElement()
{
	mxBasicAccess->addModule( msLibName, msModuleName, msLanguage, msSource );
}

void XMLScriptModuleContext::Characters( const ::rtl::OUString& rChars )
{
	msSource += rChars;
}

// =============================================================================
// XMLScriptChildContext: context for <office:script-data> element
// =============================================================================

class XMLScriptChildContext : public SvXMLImportContext
{
private:
    ::com::sun::star::uno::Reference< ::com::sun::star::frame::XModel > m_xModel;
    ::rtl::OUString m_aLanguage;

public:
    XMLScriptChildContext( SvXMLImport& rImport, USHORT nPrfx, const ::rtl::OUString& rLName,
        const ::com::sun::star::uno::Reference< ::com::sun::star::frame::XModel>& rxModel,
        const ::rtl::OUString& rLanguage );
    virtual ~XMLScriptChildContext();

    virtual SvXMLImportContext* CreateChildContext( USHORT nPrefix, const ::rtl::OUString& rLocalName,
        const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList >& xAttrList );

	virtual void EndElement();
};

// -----------------------------------------------------------------------------

XMLScriptChildContext::XMLScriptChildContext( SvXMLImport& rImport, USHORT nPrfx, const ::rtl::OUString& rLName,
        const Reference< frame::XModel >& rxModel, const ::rtl::OUString& rLanguage )
    :SvXMLImportContext( rImport, nPrfx, rLName )
    ,m_xModel( rxModel )
    ,m_aLanguage( rLanguage )
{
}

// -----------------------------------------------------------------------------

XMLScriptChildContext::~XMLScriptChildContext()
{
}

// -----------------------------------------------------------------------------

SvXMLImportContext* XMLScriptChildContext::CreateChildContext( 
    USHORT nPrefix, const ::rtl::OUString& rLocalName,
    const Reference< xml::sax::XAttributeList >& xAttrList )
{
    SvXMLImportContext* pContext = NULL;

    ::rtl::OUString aBasic( RTL_CONSTASCII_USTRINGPARAM( "Basic" ) );

    if ( m_aLanguage == aBasic && nPrefix == XML_NAMESPACE_SCRIPT && IsXMLToken( rLocalName, XML_LIBRARIES ) )
        pContext = new XMLBasicImportContext( GetImport(), nPrefix, rLocalName, m_xModel );

    if ( !pContext )
        pContext = SvXMLImportContext::CreateChildContext( nPrefix, rLocalName, xAttrList );
	
    return pContext;
}

// -----------------------------------------------------------------------------

void XMLScriptChildContext::EndElement()
{
}

// =============================================================================
// XMLScriptContext: context for <office:script> element
// =============================================================================

XMLScriptContext::XMLScriptContext( SvXMLImport& rImport, sal_uInt16 nPrfx, const OUString& rLName,
        const Reference<XModel>& rDocModel )
    :SvXMLImportContext( rImport, nPrfx, rLName )
    ,m_xModel( rDocModel )
{
}

// -----------------------------------------------------------------------------

XMLScriptContext::~XMLScriptContext()
{
}

// -----------------------------------------------------------------------------

SvXMLImportContext* XMLScriptContext::CreateChildContext( 
    sal_uInt16 nPrefix, const OUString& rLName,
    const Reference<XAttributeList>& xAttrList )
{
	SvXMLImportContext* pContext = NULL;

    if ( nPrefix == XML_NAMESPACE_OFFICE )
    {
        if ( IsXMLToken( rLName, XML_EVENTS ) )
        {
            Reference< XEventsSupplier > xSupplier( GetImport().GetModel(), UNO_QUERY );
            pContext = new XMLEventsImportContext( GetImport(), nPrefix, rLName, xSupplier );
        }
        else if ( IsXMLToken( rLName, XML_SCRIPT_DATA ) )
        {
            ::rtl::OUString aAttrName( GetImport().GetNamespaceMap().GetPrefixByKey( XML_NAMESPACE_SCRIPT ) );
            aAttrName += ::rtl::OUString( RTL_CONSTASCII_USTRINGPARAM( ":language" ) );
            if ( xAttrList.is() )
            {
                ::rtl::OUString aLanguage = xAttrList->getValueByName( aAttrName );
                pContext = new XMLScriptChildContext( GetImport(), nPrefix, rLName, m_xModel, aLanguage );
            }
        }
	}

    if ( !pContext )
        pContext = SvXMLImportContext::CreateChildContext( nPrefix, rLName, xAttrList);
	
	return pContext;
}

// -----------------------------------------------------------------------------

void XMLScriptContext::EndElement()
{
}

// -----------------------------------------------------------------------------

}//end of namespace binfilter
