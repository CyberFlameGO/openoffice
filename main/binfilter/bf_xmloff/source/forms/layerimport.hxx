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



#ifndef _XMLOFF_FORMS_LAYERIMPORT_HXX_
#define _XMLOFF_FORMS_LAYERIMPORT_HXX_

#ifndef _COM_SUN_STAR_XML_SAX_XATTRIBUTELIST_HPP_
#include <com/sun/star/xml/sax/XAttributeList.hpp>
#endif
#include <com/sun/star/container/XNameContainer.hpp>
#ifndef _VOS_REF_HXX_
#include <vos/ref.hxx>
#endif
#ifndef _XMLOFF_FORMATTRIBUTES_HXX_
#include "formattributes.hxx"
#endif
#ifndef _XMLOFF_FORMS_CALLBACKS_HXX_
#include "callbacks.hxx"
#endif
#ifndef _XMLOFF_FORMS_EVENTIMPORT_HXX_
#include "eventimport.hxx"
#endif
#ifndef _REF_HXX
#include <tools/ref.hxx>
#endif
namespace binfilter {

class SvXMLImport;
class SvXMLImportContext;
class XMLPropertyHandlerFactory;
class SvXMLImportPropertyMapper;
class XMLPropStyleContext;

SV_DECL_REF( SvXMLStylesContext ) //STRIP007;
	// unfortunately, we can't put this into our namespace, as the macro expands to (amongst others) a forward
	// declaration of the class name, which then would be in the namespace, too

//.........................................................................
namespace xmloff
{
//.........................................................................

	class OAttribute2Property;

	//=====================================================================
	//= ControlReference
	//=====================================================================
	/// a structure containing a property set (the referred control) and a string (the list of referring controls)
/*	struct ControlReference
	{
		::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >
					xReferredControl;
		::rtl::OUString
					sReferringControls;

		ControlReference(
				const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >& _rxReferredControl,
				const ::rtl::OUString& _rReferringControls)
			:xReferredControl(_rxReferredControl)
			,sReferringControls(_rReferringControls)
		{
		}
	};
*/

	//=====================================================================
	//= OFormLayerXMLImport_Impl
	//=====================================================================
	class OFormLayerXMLImport_Impl
				:public OAttributeMetaData
				,public IControlIdMap
				,public IFormsImportContext
				,public ODefaultEventAttacherManager
	{
		friend class OFormLayerXMLImport;

	protected:
		SvXMLImport&						m_rImporter;
		OAttribute2Property					m_aAttributeMetaData;
		::com::sun::star::uno::Reference< ::com::sun::star::container::XNameContainer >
											m_xForms;	// the forms of the currently imported page
		SvXMLStylesContext*					m_pAutoStyles;

	protected:
		// style handling
		::vos::ORef< XMLPropertyHandlerFactory >	m_xPropertyHandlerFactory;
		::vos::ORef< SvXMLImportPropertyMapper >	m_xImportMapper;

		DECLARE_STL_USTRINGACCESS_MAP( ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >, MapString2PropertySet );
		DECLARE_STL_MAP( ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XDrawPage >, MapString2PropertySet, ODrawPageCompare, MapDrawPage2Map);

		MapDrawPage2Map			m_aControlIds;			// ids of the controls on all known page
		MapDrawPage2MapIterator	m_aCurrentPageIds;		// ifs of the controls on the current page

        typedef ::std::pair< ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >, ::rtl::OUString >
                                ModelStringPair;
        ::std::vector< ModelStringPair >
                                m_aControlReferences;	// control reference descriptions for current page
        ::std::vector< ModelStringPair >
                                m_aCellValueBindings;   // information about controls bound to spreadsheet cells
        ::std::vector< ModelStringPair >
                                m_aCellRangeListSources;// information about controls bound to spreadsheet cell range list sources

	public:
		// IControlIdMap
		virtual void	registerControlId(
			const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >& _rxControl,
			const ::rtl::OUString& _rId);
		virtual void	registerControlReferences(
			const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >& _rxControl,
			const ::rtl::OUString& _rReferringControls);

		// IFormsImportContext
		virtual IControlIdMap&				getControlIdMap();
		virtual OAttribute2Property&		getAttributeMap();
		virtual ::com::sun::star::uno::Reference< ::com::sun::star::lang::XMultiServiceFactory >
											getServiceFactory();
		virtual SvXMLImport&				getGlobalContext();
		const SvXMLStyleContext*			getStyleElement(const ::rtl::OUString& _rStyleName) const;
		virtual void						enterEventContext();
		virtual void						leaveEventContext();
		void                                applyControlNumberStyle(
			const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >& _rxControlModel,
			const ::rtl::OUString& _rControlNumerStyleName
		);
        virtual void                        registerCellValueBinding(
            const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >& _rxControlModel,
            const ::rtl::OUString& _rCellAddress
        );

        virtual void                        registerCellRangeListSource(
            const ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >& _rxControlModel,
            const ::rtl::OUString& _rCellRangeAddress
        );

	protected:
		OFormLayerXMLImport_Impl(SvXMLImport& _rImporter);
		~OFormLayerXMLImport_Impl();

		/** retrieves the property mapper form form related auto styles.
		*/
		::vos::ORef< SvXMLImportPropertyMapper > getStylePropertyMapper() const;

		/** start importing the forms of the given page
		*/
		void startPage(
			const ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XDrawPage >& _rxDrawPage);

		/** end importing the forms of the current page
		*/
		void endPage();

		/** creates an import context for the office:forms element
		*/
		SvXMLImportContext* createOfficeFormsContext(
			SvXMLImport& _rImport,
			sal_uInt16 _nPrefix,
			const ::rtl::OUString& _rLocalName);

		/** create an <type>SvXMLImportContext</type> instance which is able to import the &lt;form:form&gt;
			element.
		*/
		SvXMLImportContext* createContext(
			const sal_uInt16 _nPrefix,
			const ::rtl::OUString& _rLocalName,
			const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList >& _rxAttribs);

		/**
		*/
		XMLPropStyleContext* createControlStyleContext(
			sal_uInt16 _nPrefix, 
			const ::rtl::OUString& _rLocalName,
			const ::com::sun::star::uno::Reference< ::com::sun::star::xml::sax::XAttributeList >& _rxAttrList,
			SvXMLStylesContext& _rParentStyles,
			sal_uInt16 _nFamily = 0,
		    sal_Bool _bDefaultStyle = sal_False
		);

		/** get the control with the given id
		*/
		::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet >
				lookupControlId(const ::rtl::OUString& _rControlId);

		/** announces the auto-style context to the form importer
		*/
		void setAutoStyleContext(SvXMLStylesContext* _pNewContext);

        /** to be called when the document has been completely imported

            <p>For some documents (currently: only some spreadsheet documents) it's necessary
            do to a post processing, since not all information from the file can be processed
            if the document is not completed, yet.</p>
        */
        void documentDone( );
	};

//.........................................................................
}	// namespace xmloff
//.........................................................................

}//end of namespace binfilter
#endif // _XMLOFF_FORMS_LAYERIMPORT_HXX_

