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



#ifndef _XMLOFF_FORMS_CONTROLELEMENT_HXX_
#define _XMLOFF_FORMS_CONTROLELEMENT_HXX_

#ifndef _SAL_TYPES_H_
#include <sal/types.h>
#endif
namespace binfilter {

//.........................................................................
namespace xmloff
{
//.........................................................................

	//=====================================================================
	//= OControlElement
	//=====================================================================
	/** helper for translating between control types and XML tags
	*/
	class OControlElement
	{
	public:
		enum ElementType
		{
			TEXT = 0,
			TEXT_AREA,
			PASSWORD,
			FILE,
			FORMATTED_TEXT,
			FIXED_TEXT,
			COMBOBOX,
			LISTBOX,
			BUTTON,
			IMAGE,
			CHECKBOX,
			RADIO,
			FRAME,
			IMAGE_FRAME,
			HIDDEN,
			GRID,
			GENERIC_CONTROL,

			UNKNOWN	// must be the last element
		};

	protected:
		/** ctor.
			<p>This default constructor is protected, 'cause this class is not intended to be instantiated
			directly. Instead, the derived classes should be used.</p>
		*/
		OControlElement() { }

	public:
		/** retrieves the tag name to be used to describe a control of the given type

			<p>The retuned string is the pure element name, without any namespace.</p>
			
			@param	_eType
				the element type
		*/
		static const sal_Char* getElementName(ElementType _eType);
	};

//.........................................................................
}	// namespace xmloff
//.........................................................................

}//end of namespace binfilter
#endif // _XMLOFF_FORMS_CONTROLELEMENT_HXX_

