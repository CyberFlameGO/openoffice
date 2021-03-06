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



#ifndef SC_NOTESUNO_HXX
#define SC_NOTESUNO_HXX

#ifndef SC_SCGLOB_HXX
#include "global.hxx"			// ScRange, ScAddress
#endif

#ifndef _SFXLSTNER_HXX //autogen
#include <bf_svtools/lstner.hxx>
#endif

#ifndef _COM_SUN_STAR_SHEET_XSHEETANNOTATION_HPP_
#include <com/sun/star/sheet/XSheetAnnotation.hpp>
#endif
#ifndef _COM_SUN_STAR_LANG_XSERVICEINFO_HPP_
#include <com/sun/star/lang/XServiceInfo.hpp>
#endif
#ifndef _COM_SUN_STAR_CONTAINER_XCHILD_HPP_
#include <com/sun/star/container/XChild.hpp>
#endif
#ifndef _COM_SUN_STAR_TEXT_XSIMPLETEXT_HPP_
#include <com/sun/star/text/XSimpleText.hpp>
#endif

#ifndef _CPPUHELPER_IMPLBASE4_HXX_
#include <cppuhelper/implbase4.hxx>
#endif
namespace binfilter {


class ScDocShell;
class SvxUnoText;


class ScAnnotationObj : public cppu::WeakImplHelper4<
							::com::sun::star::container::XChild,
							::com::sun::star::text::XSimpleText,
							::com::sun::star::sheet::XSheetAnnotation,
							::com::sun::star::lang::XServiceInfo >,
						public SfxListener
{
private:
	ScDocShell*				pDocShell;
	ScAddress				aCellPos;
	SvxUnoText*				pUnoText;

private:
	SvxUnoText&	GetUnoText();

public:
							ScAnnotationObj(ScDocShell* pDocSh, const ScAddress& rPos);
	virtual					~ScAnnotationObj();

	virtual void			Notify( SfxBroadcaster& rBC, const SfxHint& rHint );

							// XChild
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::uno::XInterface > SAL_CALL
							getParent() throw(::com::sun::star::uno::RuntimeException);
	virtual void SAL_CALL	setParent( const ::com::sun::star::uno::Reference<
										::com::sun::star::uno::XInterface >& Parent )
									throw(::com::sun::star::lang::NoSupportException,
											::com::sun::star::uno::RuntimeException);

							// XSimpleText
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextCursor > SAL_CALL
							createTextCursor() throw(::com::sun::star::uno::RuntimeException);
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextCursor > SAL_CALL
							createTextCursorByRange( const ::com::sun::star::uno::Reference<
										::com::sun::star::text::XTextRange >& aTextPosition )
									throw(::com::sun::star::uno::RuntimeException);
	virtual void SAL_CALL	insertString( const ::com::sun::star::uno::Reference<
										::com::sun::star::text::XTextRange >& xRange,
										const ::rtl::OUString& aString, sal_Bool bAbsorb )
									throw(::com::sun::star::uno::RuntimeException);
	virtual void SAL_CALL	insertControlCharacter( const ::com::sun::star::uno::Reference<
										::com::sun::star::text::XTextRange >& xRange,
										sal_Int16 nControlCharacter, sal_Bool bAbsorb )
									throw(::com::sun::star::lang::IllegalArgumentException,
										::com::sun::star::uno::RuntimeException);

							// XTextRange
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::text::XText > SAL_CALL
							getText() throw(::com::sun::star::uno::RuntimeException);
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextRange > SAL_CALL
							getStart() throw(::com::sun::star::uno::RuntimeException);
	virtual ::com::sun::star::uno::Reference< ::com::sun::star::text::XTextRange > SAL_CALL
							getEnd() throw(::com::sun::star::uno::RuntimeException);
	virtual ::rtl::OUString SAL_CALL getString() throw(::com::sun::star::uno::RuntimeException);
	virtual void SAL_CALL	setString( const ::rtl::OUString& aString )
									throw(::com::sun::star::uno::RuntimeException);

							// XSheetAnnotation
	virtual ::com::sun::star::table::CellAddress SAL_CALL getPosition()
									throw(::com::sun::star::uno::RuntimeException);
	virtual ::rtl::OUString SAL_CALL getAuthor() throw(::com::sun::star::uno::RuntimeException);
	virtual ::rtl::OUString SAL_CALL getDate() throw(::com::sun::star::uno::RuntimeException);
	virtual sal_Bool SAL_CALL getIsVisible() throw(::com::sun::star::uno::RuntimeException);
	virtual void SAL_CALL setIsVisible( sal_Bool bIsVisible )
									throw(::com::sun::star::uno::RuntimeException);

							// XServiceInfo
	virtual ::rtl::OUString SAL_CALL getImplementationName()
								throw(::com::sun::star::uno::RuntimeException);
	virtual sal_Bool SAL_CALL supportsService( const ::rtl::OUString& ServiceName )
								throw(::com::sun::star::uno::RuntimeException);
	virtual ::com::sun::star::uno::Sequence< ::rtl::OUString > SAL_CALL getSupportedServiceNames()
								throw(::com::sun::star::uno::RuntimeException);
};


} //namespace binfilter
#endif

