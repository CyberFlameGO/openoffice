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


#ifndef INCLUDED_unotools_LOCALISATIONOPTIONS_HXX
#define INCLUDED_unotools_LOCALISATIONOPTIONS_HXX

//_________________________________________________________________________________________________________________
//	includes
//_________________________________________________________________________________________________________________

#include <sal/types.h>
#include "unotools/unotoolsdllapi.h"
#include <osl/mutex.hxx>
#include <unotools/options.hxx>

//_________________________________________________________________________________________________________________
//	forward declarations
//_________________________________________________________________________________________________________________

/*-************************************************************************************************************//**
	@short			forward declaration to our private date container implementation
	@descr			We use these class as internal member to support small memory requirements.
					You can create the container if it is neccessary. The class which use these mechanism
					is faster and smaller then a complete implementation!
*//*-*************************************************************************************************************/

class SvtLocalisationOptions_Impl;

//_________________________________________________________________________________________________________________
//	declarations
//_________________________________________________________________________________________________________________

/*-************************************************************************************************************//**
	@short			collect informations about localisation features
	@descr          -

	@implements		-
	@base			-

	@devstatus		ready to use
*//*-*************************************************************************************************************/

class UNOTOOLS_DLLPUBLIC SvtLocalisationOptions: public utl::detail::Options
{
	//-------------------------------------------------------------------------------------------------------------
	//	public methods
	//-------------------------------------------------------------------------------------------------------------

	public:

		//---------------------------------------------------------------------------------------------------------
		//	constructor / destructor
		//---------------------------------------------------------------------------------------------------------

		/*-****************************************************************************************************//**
			@short		standard constructor and destructor
			@descr		This will initialize an instance with default values.
						We implement these class with a refcount mechanism! Every instance of this class increase it
						at create and decrease it at delete time - but all instances use the same data container!
						He is implemented as a static member ...

			@seealso	member m_nRefCount
			@seealso	member m_pDataContainer

			@param		-
			@return		-

			@onerror	-
		*//*-*****************************************************************************************************/

         SvtLocalisationOptions();
        virtual ~SvtLocalisationOptions();

		//---------------------------------------------------------------------------------------------------------
		//	interface
		//---------------------------------------------------------------------------------------------------------

		/*-****************************************************************************************************//**
			@short		interface methods to get and set value of config key "org.openoffice.Office.Common/View/Localisation/AutoMnemonic"
			@descr		These value specifies if shortcuts should be assigned automatically.

			@seealso	-

			@param		"bState", new value to set it in configuration.
			@return		The value which represent current state of internal variable.

			@onerror	No error should occurre!
		*//*-*****************************************************************************************************/

		sal_Bool	IsAutoMnemonic	(					) const	;
		void		SetAutoMnemonic	( sal_Bool bState	)		;

		/*-****************************************************************************************************//**
			@short		interface methods to get and set value of config key "org.openoffice.Office.Common/View/Localisation/DialogScale"
			@descr		These value specifies the factor for increasing controls.
						Value from [0..100] are allowed.

			@ATTENTION	These methods don't check for valid or invalid values!
						Our configuration server can do it ... but these implementation don't get any notifications
						about wrong commits ...!
						=> If you set an invalid value - nothing will be changed. The information will lost.

			@seealso	baseclass ConfigItem

			@param		"nScale" new value to set it in configuration.
			@return		The value which represent current state of internal variable.

			@onerror	No error should occurre!
		*//*-*****************************************************************************************************/

		sal_Int32	GetDialogScale(						) const	;
		void		SetDialogScale( sal_Int32 nScale	)		;

	//-------------------------------------------------------------------------------------------------------------
	//	private methods
	//-------------------------------------------------------------------------------------------------------------

	private:

		/*-****************************************************************************************************//**
			@short		return a reference to a static mutex
			@descr		These class is partially threadsafe (for de-/initialization only).
						All access methods are'nt safe!
						We create a static mutex only for one ime and use at different times.

			@seealso	-

			@param		-
			@return		A reference to a static mutex member.

			@onerror	-
		*//*-*****************************************************************************************************/

        UNOTOOLS_DLLPRIVATE static ::osl::Mutex& GetOwnStaticMutex();

	//-------------------------------------------------------------------------------------------------------------
	//	private member
	//-------------------------------------------------------------------------------------------------------------

	private:

		/*Attention

			Don't initialize these static member in these header!
			a) Double dfined symbols will be detected ...
			b) and unresolved externals exist at linking time.
			Do it in your source only.
		 */

    	static SvtLocalisationOptions_Impl*	m_pDataContainer	;	/// impl. data container as dynamic pointer for smaller memory requirements!
		static sal_Int32					m_nRefCount			;	/// internal ref count mechanism

};		// class SvtLocalisationOptions

#endif  // #ifndef INCLUDED_unotools_LOCALISATIONOPTIONS_HXX
