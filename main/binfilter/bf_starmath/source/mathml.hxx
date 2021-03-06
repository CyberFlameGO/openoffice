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


#ifndef MATHML_HXX
#define MATHML_HXX
#ifndef _SDXMLIMP_HXX
#include <bf_xmloff/xmlimp.hxx>
#endif
#ifndef _SDXMLEXP_HXX
#include <bf_xmloff/xmlexp.hxx>
#endif
#ifndef _XMLOFF_DOCUMENTSETTINGSCONTEXT_HXX
#include <bf_xmloff/DocumentSettingsContext.hxx>
#endif
#ifndef _XMLOFF_XMLTOKEN_HXX
#include <bf_xmloff/xmltoken.hxx>
#endif

#ifndef NODE_HXX
#include <node.hxx>
#endif
namespace com { namespace sun { namespace star {
	namespace io {
		class XInputStream;
		class XOutputStream; }
		namespace beans { 
		class XPropertySet; }
} } }
namespace binfilter {

class SvStorage; 
class SfxMedium;

//STRIP008 namespace com { namespace sun { namespace star {
//STRIP008 	namespace io {
//STRIP008 		class XInputStream;
//STRIP008 		class XOutputStream; }
//STRIP008 		namespace beans { XPropertySet; }
//STRIP008 } } }


class SmXMLWrapper
{
public:
	SmXMLWrapper(::com::sun::star::uno::Reference< ::com::sun::star::frame::XModel> &rRef)
		: xModel(rRef), bFlat(sal_True) {}
    ULONG Import(SfxMedium &rMedium);
	sal_Bool Export(SfxMedium &rMedium);
	void SetFlat(sal_Bool bIn) {bFlat = bIn;}
private:
	::com::sun::star::uno::Reference< ::com::sun::star::frame::XModel> xModel;
	sal_Bool bFlat;		//set true for export to flat .mml, set false for
						//export to a .sxm (or whatever) package

    ULONG ReadThroughComponent(
		::com::sun::star::uno::Reference< ::com::sun::star::io::XInputStream > xInputStream,
		::com::sun::star::uno::Reference< ::com::sun::star::lang::XComponent > xModelComponent,
		::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > & rFactory,
        const sal_Char* pFilterName,
        sal_Bool bEncrypted );

    ULONG ReadThroughComponent(
		SvStorage* pStorage,
		::com::sun::star::uno::Reference< ::com::sun::star::lang::XComponent > xModelComponent,
		const sal_Char* pStreamName,
		const sal_Char* pCompatibilityStreamName,
		::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > & rFactory,
		const sal_Char* pFilterName );

	sal_Bool WriteThroughComponent(
		::com::sun::star::uno::Reference< ::com::sun::star::io::XOutputStream >
            xOutputStream,
		::com::sun::star::uno::Reference< ::com::sun::star::lang::XComponent > 
            xComponent,
		::com::sun::star::uno::Reference< 
            ::com::sun::star::lang::XMultiServiceFactory > & rFactory,
		::com::sun::star::uno::Reference< 
            ::com::sun::star::beans::XPropertySet > & rPropSet,
		const sal_Char* pComponentName );
	sal_Bool WriteThroughComponent(
		SvStorage* pStorage,
		::com::sun::star::uno::Reference< ::com::sun::star::lang::XComponent > 
            xComponent,
		const sal_Char* pStreamName,
		::com::sun::star::uno::Reference< 
            ::com::sun::star::lang::XMultiServiceFactory > & rFactory,
		::com::sun::star::uno::Reference< 
            ::com::sun::star::beans::XPropertySet > & rPropSet,
		const sal_Char* pComponentName,
		sal_Bool bCompress=sal_True );
};


class SmXMLImport : public SvXMLImport
{
public:
	// #110680#
	SmXMLImport(
		const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > xServiceFactory,
		sal_uInt16 nImportFlags=IMPORT_ALL);

    // XServiceInfo (override parent method)
    ::rtl::OUString SAL_CALL getImplementationName()
        throw( ::com::sun::star::uno::RuntimeException );
    
    void SAL_CALL endDocument(void)
		throw( ::com::sun::star::xml::sax::SAXException,
		::com::sun::star::uno::RuntimeException );
	sal_Int64 SAL_CALL getSomething( const ::com::sun::star::uno::Sequence<
		sal_Int8 >& rId ) throw(::com::sun::star::uno::RuntimeException);
	static const ::com::sun::star::uno::Sequence< sal_Int8 > & getUnoTunnelId()
		throw();
	SvXMLImportContext *CreateContext(sal_uInt16 nPrefix,
		const ::rtl::OUString &rLocalName,
		const ::com::sun::star::uno::Reference <
		::com::sun::star::xml::sax::XAttributeList> &xAttrList);
	SvXMLImportContext *CreateMathContext(sal_uInt16 nPrefix,
		const ::rtl::OUString &rLocalName,
		const ::com::sun::star::uno::Reference <
		::com::sun::star::xml::sax::XAttributeList> &xAttrList);
	SvXMLImportContext *CreateRowContext(sal_uInt16 nPrefix,
		const ::rtl::OUString &rLocalName,
		const ::com::sun::star::uno::Reference <
		::com::sun::star::xml::sax::XAttributeList> &xAttrList);
	SvXMLImportContext *CreateFracContext(sal_uInt16 nPrefix,
		const ::rtl::OUString &rLocalName,
		const ::com::sun::star::uno::Reference <
		::com::sun::star::xml::sax::XAttributeList> &xAttrList);
	SvXMLImportContext *CreateNumberContext(sal_uInt16 nPrefix,
		const ::rtl::OUString &rLocalName,
		const ::com::sun::star::uno::Reference <
		::com::sun::star::xml::sax::XAttributeList> &xAttrList);
	SvXMLImportContext *CreateTextContext(sal_uInt16 nPrefix,
		const ::rtl::OUString &rLocalName,
		const ::com::sun::star::uno::Reference <
		::com::sun::star::xml::sax::XAttributeList> &xAttrList);
	SvXMLImportContext *CreateAnnotationContext(sal_uInt16 nPrefix,
		const ::rtl::OUString &rLocalName,
		const ::com::sun::star::uno::Reference <
		::com::sun::star::xml::sax::XAttributeList> &xAttrList);
	SvXMLImportContext *CreateStringContext(sal_uInt16 nPrefix,
		const ::rtl::OUString &rLocalName,
		const ::com::sun::star::uno::Reference <
		::com::sun::star::xml::sax::XAttributeList> &xAttrList);
	SvXMLImportContext *CreateIdentifierContext(sal_uInt16 nPrefix,
		const ::rtl::OUString &rLocalName,
		const ::com::sun::star::uno::Reference <
		::com::sun::star::xml::sax::XAttributeList> &xAttrList);
	SvXMLImportContext *CreateOperatorContext(sal_uInt16 nPrefix,
		const ::rtl::OUString &rLocalName,
		const ::com::sun::star::uno::Reference <
		::com::sun::star::xml::sax::XAttributeList> &xAttrList);
	SvXMLImportContext *CreateSpaceContext(sal_uInt16 nPrefix,
		const ::rtl::OUString &rLocalName,
		const ::com::sun::star::uno::Reference <
		::com::sun::star::xml::sax::XAttributeList> &xAttrList);
	SvXMLImportContext *CreateSqrtContext(sal_uInt16 nPrefix,
		const ::rtl::OUString &rLocalName,
		const ::com::sun::star::uno::Reference <
		::com::sun::star::xml::sax::XAttributeList> &xAttrList);
	SvXMLImportContext *CreateRootContext(sal_uInt16 nPrefix,
		const ::rtl::OUString &rLocalName,
		const ::com::sun::star::uno::Reference <
		::com::sun::star::xml::sax::XAttributeList> &xAttrList);
	SvXMLImportContext *CreateStyleContext(sal_uInt16 nPrefix,
		const ::rtl::OUString &rLocalName,
		const ::com::sun::star::uno::Reference <
		::com::sun::star::xml::sax::XAttributeList> &xAttrList);
	SvXMLImportContext *CreatePaddedContext(sal_uInt16 nPrefix,
		const ::rtl::OUString &rLocalName,
		const ::com::sun::star::uno::Reference <
		::com::sun::star::xml::sax::XAttributeList> &xAttrList);
	SvXMLImportContext *CreatePhantomContext(sal_uInt16 nPrefix,
		const ::rtl::OUString &rLocalName,
		const ::com::sun::star::uno::Reference <
		::com::sun::star::xml::sax::XAttributeList> &xAttrList);
	SvXMLImportContext *CreateFencedContext(sal_uInt16 nPrefix,
		const ::rtl::OUString &rLocalName,
		const ::com::sun::star::uno::Reference <
		::com::sun::star::xml::sax::XAttributeList> &xAttrList);
	SvXMLImportContext *CreateErrorContext(sal_uInt16 nPrefix,
		const ::rtl::OUString &rLocalName,
		const ::com::sun::star::uno::Reference <
		::com::sun::star::xml::sax::XAttributeList> &xAttrList);
	SvXMLImportContext *CreateSubContext(sal_uInt16 nPrefix,
		const ::rtl::OUString &rLocalName,
		const ::com::sun::star::uno::Reference <
		::com::sun::star::xml::sax::XAttributeList> &xAttrList);
	SvXMLImportContext *CreateSupContext(sal_uInt16 nPrefix,
		const ::rtl::OUString &rLocalName,
		const ::com::sun::star::uno::Reference <
		::com::sun::star::xml::sax::XAttributeList> &xAttrList);
	SvXMLImportContext *CreateSubSupContext(sal_uInt16 nPrefix,
		const ::rtl::OUString &rLocalName,
		const ::com::sun::star::uno::Reference <
		::com::sun::star::xml::sax::XAttributeList> &xAttrList);
	SvXMLImportContext *CreateUnderContext(sal_uInt16 nPrefix,
		const ::rtl::OUString &rLocalName,
		const ::com::sun::star::uno::Reference <
		::com::sun::star::xml::sax::XAttributeList> &xAttrList);
	SvXMLImportContext *CreateOverContext(sal_uInt16 nPrefix,
		const ::rtl::OUString &rLocalName,
		const ::com::sun::star::uno::Reference <
		::com::sun::star::xml::sax::XAttributeList> &xAttrList);
	SvXMLImportContext *CreateUnderOverContext(sal_uInt16 nPrefix,
		const ::rtl::OUString &rLocalName,
		const ::com::sun::star::uno::Reference <
		::com::sun::star::xml::sax::XAttributeList> &xAttrList);
	SvXMLImportContext *CreateMultiScriptsContext(sal_uInt16 nPrefix,
		const ::rtl::OUString &rLocalName,
		const ::com::sun::star::uno::Reference <
		::com::sun::star::xml::sax::XAttributeList> &xAttrList);
	SvXMLImportContext *CreateNoneContext(sal_uInt16 nPrefix,
		const ::rtl::OUString &rLocalName,
		const ::com::sun::star::uno::Reference <
		::com::sun::star::xml::sax::XAttributeList> &xAttrList);
	SvXMLImportContext *CreatePrescriptsContext(sal_uInt16 nPrefix,
		const ::rtl::OUString &rLocalName,
		const ::com::sun::star::uno::Reference <
		::com::sun::star::xml::sax::XAttributeList> &xAttrList);
	SvXMLImportContext *CreateTableContext(sal_uInt16 nPrefix,
		const ::rtl::OUString &rLocalName,
		const ::com::sun::star::uno::Reference <
		::com::sun::star::xml::sax::XAttributeList> &xAttrList);
	SvXMLImportContext *CreateTableRowContext(sal_uInt16 nPrefix,
		const ::rtl::OUString &rLocalName,
		const ::com::sun::star::uno::Reference <
		::com::sun::star::xml::sax::XAttributeList> &xAttrList);
	SvXMLImportContext *CreateTableCellContext(sal_uInt16 nPrefix,
		const ::rtl::OUString &rLocalName,
		const ::com::sun::star::uno::Reference <
		::com::sun::star::xml::sax::XAttributeList> &xAttrList);
	SvXMLImportContext *CreateAlignGroupContext(sal_uInt16 nPrefix,
		const ::rtl::OUString &rLocalName,
		const ::com::sun::star::uno::Reference <
		::com::sun::star::xml::sax::XAttributeList> &xAttrList);
	SvXMLImportContext *CreateActionContext(sal_uInt16 nPrefix,
		const ::rtl::OUString &rLocalName,
		const ::com::sun::star::uno::Reference <
		::com::sun::star::xml::sax::XAttributeList> &xAttrList);
//   const SvXMLTokenMap &GetMathElemTokenMap();
	const SvXMLTokenMap &GetPresLayoutElemTokenMap();
	const SvXMLTokenMap &GetPresLayoutAttrTokenMap();
	const SvXMLTokenMap &GetFencedAttrTokenMap();
	const SvXMLTokenMap &GetOperatorAttrTokenMap();
	const SvXMLTokenMap &GetAnnotationAttrTokenMap();
	const SvXMLTokenMap &GetPresElemTokenMap();
	const SvXMLTokenMap &GetPresScriptEmptyElemTokenMap();
	const SvXMLTokenMap &GetPresTableElemTokenMap();
	const SvXMLTokenMap &GetColorTokenMap();
	virtual ~SmXMLImport() throw ();
	SmNodeStack & GetNodeStack() {return aNodeStack;}
	SmNode *GetTree() { return aNodeStack.Pop();}
	sal_Bool GetSuccess() { return bSuccess; }
	String &GetText() { return aText;}

	virtual void SetViewSettings(const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue>& aViewProps);
	virtual void SetConfigurationSettings(const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue>& aViewProps);

private:
        SvXMLTokenMap *pMathElemTokenMap;
        SvXMLTokenMap *pPresLayoutElemTokenMap;
        SvXMLTokenMap *pPresLayoutAttrTokenMap;
        SvXMLTokenMap *pFencedAttrTokenMap;
        SvXMLTokenMap *pOperatorAttrTokenMap;
        SvXMLTokenMap *pAnnotationAttrTokenMap;
        SvXMLTokenMap *pPresElemTokenMap;
        SvXMLTokenMap *pPresScriptEmptyElemTokenMap;
        SvXMLTokenMap *pPresTableElemTokenMap;
        SvXMLTokenMap *pColorTokenMap;

		SmNodeStack aNodeStack;
		sal_Bool bSuccess;
		String aText;
};

enum SmXMLMathElemTokenMap
{
    XML_TOK_MATH
};

enum SmXMLPresLayoutElemTokenMap
{
	XML_TOK_SEMANTICS,
	XML_TOK_MSTYLE,
	XML_TOK_MERROR,
	XML_TOK_MPHANTOM,
	XML_TOK_MROW,
	XML_TOK_MFRAC,
	XML_TOK_MSQRT,
	XML_TOK_MROOT,
	XML_TOK_MSUB,
	XML_TOK_MSUP,
	XML_TOK_MSUBSUP,
	XML_TOK_MMULTISCRIPTS,
	XML_TOK_MUNDER,
	XML_TOK_MOVER,
	XML_TOK_MUNDEROVER,
	XML_TOK_MTABLE,
	XML_TOK_MACTION,
	XML_TOK_MFENCED,
	XML_TOK_MPADDED
};

enum SmXMLPresLayoutAttrTokenMap
{
	XML_TOK_FONTWEIGHT,
	XML_TOK_FONTSTYLE,
	XML_TOK_FONTSIZE,
	XML_TOK_FONTFAMILY,
	XML_TOK_COLOR
};


enum SmXMLFencedAttrTokenMap
{
	XML_TOK_OPEN,
	XML_TOK_CLOSE
};


enum SmXMLPresTableElemTokenMap
{
	XML_TOK_MTR,
	XML_TOK_MTD
};

enum SmXMLPresElemTokenMap
{
	XML_TOK_ANNOTATION,
	XML_TOK_MI,
	XML_TOK_MN,
	XML_TOK_MO,
	XML_TOK_MTEXT,
	XML_TOK_MSPACE,
	XML_TOK_MS,
	XML_TOK_MALIGNGROUP
};

enum SmXMLPresScriptEmptyElemTokenMap
{
	XML_TOK_MPRESCRIPTS,
	XML_TOK_NONE
};

enum SmXMLOperatorAttrTokenMap
{
	XML_TOK_STRETCHY
};

enum SmXMLAnnotationAttrTokenMap
{
	XML_TOK_ENCODING
};


class SmXMLExport : public SvXMLExport
{
public:
	// #110680#
	SmXMLExport(
		const ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory > xServiceFactory,
		sal_uInt16 nExportFlags=EXPORT_ALL);
	
	virtual ~SmXMLExport() {};
    
    // XServiceInfo (override parent method)
    ::rtl::OUString SAL_CALL getImplementationName()
        throw( ::com::sun::star::uno::RuntimeException );
    
    void _ExportAutoStyles() {}
	void _ExportMasterStyles() {}
	void _ExportContent();
	sal_uInt32 exportDoc(enum ::binfilter::xmloff::token::XMLTokenEnum eClass);

    virtual void GetViewSettings(::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue>& aProps);
    virtual void GetConfigurationSettings(::com::sun::star::uno::Sequence< ::com::sun::star::beans::PropertyValue>& aProps);

	sal_Int64 SAL_CALL getSomething( const ::com::sun::star::uno::Sequence<
		sal_Int8 >& rId ) throw(::com::sun::star::uno::RuntimeException);
	static const ::com::sun::star::uno::Sequence< sal_Int8 > & getUnoTunnelId()
		throw();

	sal_Bool GetSuccess() {return bSuccess;}

protected:
	void ExportNodes(const SmNode *pIn,int nLevel);
	void ExportTable(const SmNode *pIn,int nLevel);
	void ExportLine(const SmNode *pNode,int nLevel);
	void ExportExpression(const SmNode *pNode,int nLevel);
	void ExportText(const SmNode *pNode, int nLevel);
	void ExportMath(const SmNode *pNode, int nLevel);
	void ExportPolygon(const SmNode *pNode, int nLevel);
	void ExportBinaryHorizontal(const SmNode *pNode,int nLevel);
	void ExportUnaryHorizontal(const SmNode *pNode,int nLevel);
	void ExportBrace(const SmNode *pNode, int nLevel);
	void ExportBinaryVertical(const SmNode *pNode,int nLevel);
	void ExportSubSupScript(const SmNode *pNode,int nLevel);
	void ExportRoot(const SmNode *pNode, int nLevel);
	void ExportOperator(const SmNode *pNode, int nLevel);
	void ExportAttributes(const SmNode *pNode, int nLevel);
	void ExportFont(const SmNode *pNode, int nLevel);
	void ExportVerticalBrace(const SmNode *pNode, int nLevel);
	void ExportMatrix(const SmNode *pNode, int nLevel);
private:
	const SmNode *pTree;
    String        aText;
	sal_Bool bSuccess;
};
} //namespace binfilter
#endif
