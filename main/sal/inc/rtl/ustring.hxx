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



#ifndef _RTL_USTRING_HXX_
#define _RTL_USTRING_HXX_

#ifdef __cplusplus

#ifndef _RTL_DIAGNOSE_H_
#include "osl/diagnose.h"
#endif
#include <rtl/ustring.h>
#include <rtl/string.hxx>
#include <rtl/memory.h>

#if defined EXCEPTIONS_OFF
#include <stdlib.h>
#else
#include <new>
#endif

namespace rtl
{
/* ======================================================================= */

/**
  This String class provide base functionality for C++ like Unicode
  character array handling. The advantage of this class is, that it
  handle all the memory managament for you - and it do it
  more efficient. If you assign a string to another string, the
  data of both strings are shared (without any copy operation or
  memory allocation) as long as you do not change the string. This class
  stores also the length of the string, so that many operations are
  faster as the C-str-functions.

  This class provide only readonly string handling. So you could create
  a string and you could only query the content from this string.
  It provide also functionality to change the string, but this results
  in every case in a new string instance (in the most cases with an
  memory allocation). You don't have functionality to change the
  content of the string. If you want change the string content, than
  you should us the OStringBuffer class, which provide these
  functionality and avoid to much memory allocation.

  The design of this class is similar to the string classes in Java
  and so more people should have fewer understanding problems when they
  use this class.
*/

class OUString
{
public:
    /** @internal */
    rtl_uString * pData;

private:
    /** @internal */
    class DO_NOT_ACQUIRE{};

    /** @internal */
    OUString( rtl_uString * value, DO_NOT_ACQUIRE * )
    {
        pData = value;
    }

public:
    /**
      New string containing no characters.
    */
    OUString() SAL_THROW(())
    {
        pData = 0;
        rtl_uString_new( &pData );
    }

    /**
      New string from OUString.

      @param    str         a OUString.
    */
    OUString( const OUString & str ) SAL_THROW(())
    {
        pData = str.pData;
        rtl_uString_acquire( pData );
    }

    /**
      New string from OUString data.

      @param    str         a OUString data.
    */
    OUString( rtl_uString * str )  SAL_THROW(())
    {
        pData = str;
        rtl_uString_acquire( pData );
    }
    /** New OUString from OUString data without acquiring it.  Takeover of ownership.

        @param str
               OUString data
        @param dummy
               SAL_NO_ACQUIRE to distinguish from other ctors
    */
    inline OUString( rtl_uString * str, __sal_NoAcquire ) SAL_THROW( () )
        { pData = str; }


    /**
      New string from a single Unicode character.

      @param    value       a Unicode character.
    */
	explicit OUString( sal_Unicode value ) SAL_THROW(())
        : pData (0)
	{
        rtl_uString_newFromStr_WithLength( &pData, &value, 1 );
	}

    /**
      New string from a Unicode character buffer array.

      @param    value       a NULL-terminated Unicode character array.
    */
    OUString( const sal_Unicode * value ) SAL_THROW(())
    {
        pData = 0;
        rtl_uString_newFromStr( &pData, value );
    }

    /**
      New string from a Uniocde character buffer array.

      @param    value       a Unicode character array.
      @param    length      the number of character which should be copied.
                            The character array length must be greater or
                            equal than this value.
    */
    OUString( const sal_Unicode * value, sal_Int32 length ) SAL_THROW(())
    {
        pData = 0;
        rtl_uString_newFromStr_WithLength( &pData, value, length );
    }

    /**
      New string from a 8-Bit character buffer array.

      @param    value           a 8-Bit character array.
      @param    length          the number of character which should be converted.
                                The 8-Bit character array length must be
                                greater or equal than this value.
      @param    encoding        the text encoding from which the 8-Bit character
                                sequence should be converted.
      @param    convertFlags    flags which controls the conversion.
                                see RTL_TEXTTOUNICODE_FLAGS_...

      @exception std::bad_alloc is thrown if an out-of-memory condition occurs
    */
    OUString( const sal_Char * value, sal_Int32 length,
              rtl_TextEncoding encoding,
              sal_uInt32 convertFlags = OSTRING_TO_OUSTRING_CVTFLAGS )
    {
        pData = 0;
        rtl_string2UString( &pData, value, length, encoding, convertFlags );
#if defined EXCEPTIONS_OFF
        OSL_ASSERT(pData != NULL);
#else
        if (pData == 0) {
            throw std::bad_alloc();
        }
#endif
    }

    /** Create a new string from an array of Unicode code points.

        @param codePoints
        an array of at least codePointCount code points, which each must be in
        the range from 0 to 0x10FFFF, inclusive.  May be null if codePointCount
        is zero.

        @param codePointCount
        the non-negative number of code points.

        @exception std::bad_alloc
        is thrown if either an out-of-memory condition occurs or the resulting
        number of UTF-16 code units would have been larger than SAL_MAX_INT32.

        @since UDK 3.2.7
    */
    inline explicit OUString(
        sal_uInt32 const * codePoints, sal_Int32 codePointCount):
        pData(NULL)
    {
        rtl_uString_newFromCodePoints(&pData, codePoints, codePointCount);
        if (pData == NULL) {
#if defined EXCEPTIONS_OFF
            abort();
#else
            throw std::bad_alloc();
#endif
        }
    }

    /**
      Release the string data.
    */
    ~OUString() SAL_THROW(())
    {
        rtl_uString_release( pData );
    }
    
    /** Provides an OUString const & passing a storage pointer of an
        rtl_uString * handle.
        It is more convenient to use C++ OUString member functions when dealing
        with rtl_uString * handles.  Using this function avoids unnecessary
        acquire()/release() calls for a temporary OUString object.
        
        @param ppHandle
               pointer to storage
        @return
               OUString const & based on given storage
    */
    static inline OUString const & unacquired( rtl_uString * const * ppHandle )
        { return * reinterpret_cast< OUString const * >( ppHandle ); }
    
    /**
      Assign a new string.

      @param    str         a OUString.
    */
    OUString & operator=( const OUString & str ) SAL_THROW(())
    {
        rtl_uString_assign( &pData, str.pData );
        return *this;
    }

    /**
      Append a string to this string.

      @param    str         a OUString.
    */
    OUString & operator+=( const OUString & str ) SAL_THROW(())
    {
        rtl_uString_newConcat( &pData, pData, str.pData );
        return *this;
    }

    /**
      Returns the length of this string.

      The length is equal to the number of Unicode characters in this string.

      @return   the length of the sequence of characters represented by this
                object.
    */
    sal_Int32 getLength() const SAL_THROW(()) { return pData->length; }

    /**
      Returns a pointer to the Unicode character buffer from this string.

      It isn't necessarily NULL terminated.

      @return   a pointer to the Unicode characters buffer from this object.
    */
    operator const sal_Unicode *() const SAL_THROW(()) { return pData->buffer; }

    /**
      Returns a pointer to the Unicode character buffer from this string.

      It isn't necessarily NULL terminated.

      @return   a pointer to the Unicode characters buffer from this object.
    */
    const sal_Unicode * getStr() const SAL_THROW(()) { return pData->buffer; }

    /**
      Compares two strings.

      The comparison is based on the numeric value of each character in
      the strings and return a value indicating their relationship.
      This function can't be used for language specific sorting.

      @param    str         the object to be compared.
      @return   0 - if both strings are equal
                < 0 - if this string is less than the string argument
                > 0 - if this string is greater than the string argument
    */
    sal_Int32 compareTo( const OUString & str ) const SAL_THROW(())
    {
        return rtl_ustr_compare_WithLength( pData->buffer, pData->length,
                                            str.pData->buffer, str.pData->length );
    }

    /**
      Compares two strings with an maximum count of characters.

      The comparison is based on the numeric value of each character in
      the strings and return a value indicating their relationship.
      This function can't be used for language specific sorting.

      @param    str         the object to be compared.
      @param    maxLength   the maximum count of characters to be compared.
      @return   0 - if both strings are equal
                < 0 - if this string is less than the string argument
                > 0 - if this string is greater than the string argument
    */
    sal_Int32 compareTo( const OUString & str, sal_Int32 maxLength ) const SAL_THROW(())
    {
        return rtl_ustr_shortenedCompare_WithLength( pData->buffer, pData->length,
                                                     str.pData->buffer, str.pData->length, maxLength );
    }

    /**
      Compares two strings in reverse order.

      The comparison is based on the numeric value of each character in
      the strings and return a value indicating their relationship.
      This function can't be used for language specific sorting.

      @param    str         the object to be compared.
      @return   0 - if both strings are equal
                < 0 - if this string is less than the string argument
                > 0 - if this string is greater than the string argument
    */
    sal_Int32 reverseCompareTo( const OUString & str ) const SAL_THROW(())
    {
        return rtl_ustr_reverseCompare_WithLength( pData->buffer, pData->length,
                                                   str.pData->buffer, str.pData->length );
    }

    /**
      Perform a comparison of two strings.

      The result is true if and only if second string
      represents the same sequence of characters as the first string.
      This function can't be used for language specific comparison.

      @param    str         the object to be compared.
      @return   sal_True if the strings are equal;
                sal_False, otherwise.
    */
    sal_Bool equals( const OUString & str ) const SAL_THROW(())
    {
        if ( pData->length != str.pData->length )
            return sal_False;
        if ( pData == str.pData )
            return sal_True;
        return rtl_ustr_reverseCompare_WithLength( pData->buffer, pData->length,
                                                   str.pData->buffer, str.pData->length ) == 0;
    }

    /**
      Perform a ASCII lowercase comparison of two strings.

      The result is true if and only if second string
      represents the same sequence of characters as the first string,
      ignoring the case.
      Character values between 65 and 90 (ASCII A-Z) are interpreted as
      values between 97 and 122 (ASCII a-z).
      This function can't be used for language specific comparison.

      @param    str         the object to be compared.
      @return   sal_True if the strings are equal;
                sal_False, otherwise.
    */
    sal_Bool equalsIgnoreAsciiCase( const OUString & str ) const SAL_THROW(())
    {
        if ( pData->length != str.pData->length )
            return sal_False;
        if ( pData == str.pData )
            return sal_True;
        return rtl_ustr_compareIgnoreAsciiCase_WithLength( pData->buffer, pData->length,
                                                           str.pData->buffer, str.pData->length ) == 0;
    }

    /**
      Match against a substring appearing in this string.

      The result is true if and only if the second string appears as a substring
      of this string, at the given position.
      This function can't be used for language specific comparison.

      @param    str         the object (substring) to be compared.
      @param    fromIndex   the index to start the comparion from.
                            The index must be greater or equal than 0
                            and less or equal as the string length.
      @return   sal_True if str match with the characters in the string
                at the given position;
                sal_False, otherwise.
    */
    sal_Bool match( const OUString & str, sal_Int32 fromIndex = 0 ) const SAL_THROW(())
    {
        return rtl_ustr_shortenedCompare_WithLength( pData->buffer+fromIndex, pData->length-fromIndex,
                                                     str.pData->buffer, str.pData->length, str.pData->length ) == 0;
    }

    /**
      Match against a substring appearing in this string, ignoring the case of
      ASCII letters.

      The result is true if and only if the second string appears as a substring
      of this string, at the given position.
      Character values between 65 and 90 (ASCII A-Z) are interpreted as
      values between 97 and 122 (ASCII a-z).
      This function can't be used for language specific comparison.

      @param    str         the object (substring) to be compared.
      @param    fromIndex   the index to start the comparion from.
                            The index must be greater or equal than 0
                            and less or equal as the string length.
      @return   sal_True if str match with the characters in the string
                at the given position;
                sal_False, otherwise.
    */
    sal_Bool matchIgnoreAsciiCase( const OUString & str, sal_Int32 fromIndex = 0 ) const SAL_THROW(())
    {
        return rtl_ustr_shortenedCompareIgnoreAsciiCase_WithLength( pData->buffer+fromIndex, pData->length-fromIndex,
                                                                    str.pData->buffer, str.pData->length,
                                                                    str.pData->length ) == 0;
    }

    /**
      Compares two strings.

      The comparison is based on the numeric value of each character in
      the strings and return a value indicating their relationship.
      Since this method is optimized for performance, the ASCII character
      values are not converted in any way. The caller has to make sure that
      all ASCII characters are in the allowed range between 0 and
      127. The ASCII string must be NULL-terminated.
      This function can't be used for language specific sorting.

      @param  asciiStr      the 8-Bit ASCII character string to be compared.
      @return   0 - if both strings are equal
                < 0 - if this string is less than the string argument
                > 0 - if this string is greater than the string argument
    */
    sal_Int32 compareToAscii( const sal_Char* asciiStr ) const SAL_THROW(())
    {
        return rtl_ustr_ascii_compare_WithLength( pData->buffer, pData->length, asciiStr );
    }

    /**
      Compares two strings with an maximum count of characters.

      The comparison is based on the numeric value of each character in
      the strings and return a value indicating their relationship.
      Since this method is optimized for performance, the ASCII character
      values are not converted in any way. The caller has to make sure that
      all ASCII characters are in the allowed range between 0 and
      127. The ASCII string must be NULL-terminated.
      This function can't be used for language specific sorting.

      @param  asciiStr          the 8-Bit ASCII character string to be compared.
      @param  maxLength         the maximum count of characters to be compared.
      @return   0 - if both strings are equal
                < 0 - if this string is less than the string argument
                > 0 - if this string is greater than the string argument
    */
    sal_Int32 compareToAscii( const sal_Char * asciiStr, sal_Int32 maxLength ) const SAL_THROW(())
    {
        return rtl_ustr_ascii_shortenedCompare_WithLength( pData->buffer, pData->length,
                                                           asciiStr, maxLength );
    }

    /**
      Compares two strings in reverse order.

      This could be useful, if normally both strings start with the same
      content. The comparison is based on the numeric value of each character
      in the strings and return a value indicating their relationship.
      Since this method is optimized for performance, the ASCII character
      values are not converted in any way. The caller has to make sure that
      all ASCII characters are in the allowed range between 0 and
      127. The ASCII string must be NULL-terminated and must be greater or
      equal as asciiStrLength.
      This function can't be used for language specific sorting.

      @param    asciiStr        the 8-Bit ASCII character string to be compared.
      @param    asciiStrLength  the length of the ascii string
      @return   0 - if both strings are equal
                < 0 - if this string is less than the string argument
                > 0 - if this string is greater than the string argument
    */
    sal_Int32 reverseCompareToAsciiL( const sal_Char * asciiStr, sal_Int32 asciiStrLength ) const SAL_THROW(())
    {
        return rtl_ustr_asciil_reverseCompare_WithLength( pData->buffer, pData->length,
                                                          asciiStr, asciiStrLength );
    }

    /**
      Perform a comparison of two strings.

      The result is true if and only if second string
      represents the same sequence of characters as the first string.
      Since this method is optimized for performance, the ASCII character
      values are not converted in any way. The caller has to make sure that
      all ASCII characters are in the allowed range between 0 and
      127. The ASCII string must be NULL-terminated.
      This function can't be used for language specific comparison.

      @param    asciiStr        the 8-Bit ASCII character string to be compared.
      @return   sal_True if the strings are equal;
                sal_False, otherwise.
    */
    sal_Bool equalsAscii( const sal_Char* asciiStr ) const SAL_THROW(())
    {
        return rtl_ustr_ascii_compare_WithLength( pData->buffer, pData->length,
                                                  asciiStr ) == 0;
    }

    /**
      Perform a comparison of two strings.

      The result is true if and only if second string
      represents the same sequence of characters as the first string.
      Since this method is optimized for performance, the ASCII character
      values are not converted in any way. The caller has to make sure that
      all ASCII characters are in the allowed range between 0 and
      127. The ASCII string must be NULL-terminated and must be greater or
      equal as asciiStrLength.
      This function can't be used for language specific comparison.

      @param    asciiStr         the 8-Bit ASCII character string to be compared.
      @param    asciiStrLength   the length of the ascii string
      @return   sal_True if the strings are equal;
                sal_False, otherwise.
    */
    sal_Bool equalsAsciiL( const sal_Char* asciiStr, sal_Int32 asciiStrLength ) const SAL_THROW(())
    {
        if ( pData->length != asciiStrLength )
            return sal_False;

        return rtl_ustr_asciil_reverseEquals_WithLength( 
					pData->buffer, asciiStr, asciiStrLength );
    }

    /**
      Perform a ASCII lowercase comparison of two strings.

      The result is true if and only if second string
      represents the same sequence of characters as the first string,
      ignoring the case.
      Character values between 65 and 90 (ASCII A-Z) are interpreted as
      values between 97 and 122 (ASCII a-z).
      Since this method is optimized for performance, the ASCII character
      values are not converted in any way. The caller has to make sure that
      all ASCII characters are in the allowed range between 0 and
      127. The ASCII string must be NULL-terminated.
      This function can't be used for language specific comparison.

      @param    asciiStr        the 8-Bit ASCII character string to be compared.
      @return   sal_True if the strings are equal;
                sal_False, otherwise.
    */
    sal_Bool equalsIgnoreAsciiCaseAscii( const sal_Char * asciiStr ) const SAL_THROW(())
    {
        return rtl_ustr_ascii_compareIgnoreAsciiCase_WithLength( pData->buffer, pData->length, asciiStr ) == 0;
    }

    /**
      Perform a ASCII lowercase comparison of two strings.

      The result is true if and only if second string
      represents the same sequence of characters as the first string,
      ignoring the case.
      Character values between 65 and 90 (ASCII A-Z) are interpreted as
      values between 97 and 122 (ASCII a-z).
      Since this method is optimized for performance, the ASCII character
      values are not converted in any way. The caller has to make sure that
      all ASCII characters are in the allowed range between 0 and
      127. The ASCII string must be NULL-terminated and must be greater or
      equal as asciiStrLength.
      This function can't be used for language specific comparison.

      @param    asciiStr        the 8-Bit ASCII character string to be compared.
      @param    asciiStrLength  the length of the ascii string
      @return   sal_True if the strings are equal;
                sal_False, otherwise.
    */
    sal_Bool equalsIgnoreAsciiCaseAsciiL( const sal_Char * asciiStr, sal_Int32 asciiStrLength ) const SAL_THROW(())
    {
        if ( pData->length != asciiStrLength )
            return sal_False;

        return rtl_ustr_ascii_compareIgnoreAsciiCase_WithLength( pData->buffer, pData->length, asciiStr ) == 0;
    }

    /**
      Match against a substring appearing in this string.

      The result is true if and only if the second string appears as a substring
      of this string, at the given position.
      Since this method is optimized for performance, the ASCII character
      values are not converted in any way. The caller has to make sure that
      all ASCII characters are in the allowed range between 0 and
      127. The ASCII string must be NULL-terminated and must be greater or
      equal as asciiStrLength.
      This function can't be used for language specific comparison.

      @param    str         the object (substring) to be compared.
      @param    fromIndex   the index to start the comparion from.
                            The index must be greater or equal than 0
                            and less or equal as the string length.
      @return   sal_True if str match with the characters in the string
                at the given position;
                sal_False, otherwise.
    */
    sal_Bool matchAsciiL( const sal_Char* asciiStr, sal_Int32 asciiStrLength, sal_Int32 fromIndex = 0 ) const SAL_THROW(())
    {
        return rtl_ustr_ascii_shortenedCompare_WithLength( pData->buffer+fromIndex, pData->length-fromIndex,
                                                           asciiStr, asciiStrLength ) == 0;
    }

    /**
      Match against a substring appearing in this string, ignoring the case of
      ASCII letters.

      The result is true if and only if the second string appears as a substring
      of this string, at the given position.
      Character values between 65 and 90 (ASCII A-Z) are interpreted as
      values between 97 and 122 (ASCII a-z).
      Since this method is optimized for performance, the ASCII character
      values are not converted in any way. The caller has to make sure that
      all ASCII characters are in the allowed range between 0 and
      127. The ASCII string must be NULL-terminated and must be greater or
      equal as asciiStrLength.
      This function can't be used for language specific comparison.

      @param    asciiStr        the 8-Bit ASCII character string to be compared.
      @param    asciiStrLength  the length of the ascii string
      @param    fromIndex       the index to start the comparion from.
                                The index must be greater or equal than 0
                                and less or equal as the string length.
      @return   sal_True if str match with the characters in the string
                at the given position;
                sal_False, otherwise.
    */
    sal_Bool matchIgnoreAsciiCaseAsciiL( const sal_Char* asciiStr, sal_Int32 asciiStrLength, sal_Int32 fromIndex = 0 ) const SAL_THROW(())
    {
        return rtl_ustr_ascii_shortenedCompareIgnoreAsciiCase_WithLength( pData->buffer+fromIndex, pData->length-fromIndex,
                                                                          asciiStr, asciiStrLength ) == 0;
    }

    /**
      Check whether this string ends with a given ASCII string.

      @param asciiStr a sequence of at least asciiStrLength ASCII characters
          (bytes in the range 0x00--0x7F)
      @param asciiStrLen the length of asciiStr; must be non-negative
      @return true if this string ends with asciiStr; otherwise, false is
      returned

      @since UDK 3.2.7
     */
    inline bool endsWithAsciiL(char const * asciiStr, sal_Int32 asciiStrLength)
        const
    {
        return asciiStrLength <= pData->length
            && rtl_ustr_asciil_reverseEquals_WithLength(
                pData->buffer + pData->length - asciiStrLength, asciiStr,
                asciiStrLength);
    }

    /**
      Check whether this string ends with a given ASCII string, ignoring the
      case of ASCII letters.

      @param asciiStr a sequence of at least asciiStrLength ASCII characters
          (bytes in the range 0x00--0x7F)
      @param asciiStrLen the length of asciiStr; must be non-negative
      @return true if this string ends with asciiStr, ignoring the case of ASCII
      letters ("A"--"Z" and "a"--"z"); otherwise, false is returned
     */
    inline bool endsWithIgnoreAsciiCaseAsciiL(
        char const * asciiStr, sal_Int32 asciiStrLength) const
    {
        return asciiStrLength <= pData->length
            && (rtl_ustr_ascii_compareIgnoreAsciiCase_WithLengths(
                    pData->buffer + pData->length - asciiStrLength,
                    asciiStrLength, asciiStr, asciiStrLength)
                == 0);
    }

    friend sal_Bool     operator == ( const OUString& rStr1,    const OUString& rStr2 ) SAL_THROW(())
                        { return rStr1.getLength() == rStr2.getLength() && rStr1.compareTo( rStr2 ) == 0; }
    friend sal_Bool     operator == ( const OUString& rStr1,    const sal_Unicode * pStr2 ) SAL_THROW(())
                        { return rStr1.compareTo( pStr2 ) == 0; }
    friend sal_Bool     operator == ( const sal_Unicode * pStr1,    const OUString& rStr2 ) SAL_THROW(())
                        { return OUString( pStr1 ).compareTo( rStr2 ) == 0; }

    friend sal_Bool     operator != ( const OUString& rStr1,        const OUString& rStr2 ) SAL_THROW(())
                        { return !(operator == ( rStr1, rStr2 )); }
    friend sal_Bool     operator != ( const OUString& rStr1,    const sal_Unicode * pStr2 ) SAL_THROW(())
                        { return !(operator == ( rStr1, pStr2 )); }
    friend sal_Bool     operator != ( const sal_Unicode * pStr1,    const OUString& rStr2 ) SAL_THROW(())
                        { return !(operator == ( pStr1, rStr2 )); }

    friend sal_Bool     operator <  ( const OUString& rStr1,    const OUString& rStr2 ) SAL_THROW(())
                        { return rStr1.compareTo( rStr2 ) < 0; }
    friend sal_Bool     operator >  ( const OUString& rStr1,    const OUString& rStr2 ) SAL_THROW(())
                        { return rStr1.compareTo( rStr2 ) > 0; }
    friend sal_Bool     operator <= ( const OUString& rStr1,    const OUString& rStr2 ) SAL_THROW(())
                        { return rStr1.compareTo( rStr2 ) <= 0; }
    friend sal_Bool     operator >= ( const OUString& rStr1,    const OUString& rStr2 ) SAL_THROW(())
                        { return rStr1.compareTo( rStr2 ) >= 0; }

    /**
      Returns a hashcode for this string.

      @return   a hash code value for this object.

      @see rtl::OUStringHash for convenient use of STLPort's hash_map
    */
    sal_Int32 hashCode() const SAL_THROW(())
    {
        return rtl_ustr_hashCode_WithLength( pData->buffer, pData->length );
    }

    /**
      Returns the index within this string of the first occurrence of the
      specified character, starting the search at the specified index.

      @param    ch          character to be located.
      @param    fromIndex   the index to start the search from.
                            The index must be greater or equal than 0
                            and less or equal as the string length.
      @return   the index of the first occurrence of the character in the
                character sequence represented by this string that is
                greater than or equal to fromIndex, or
                -1 if the character does not occur.
    */
    sal_Int32 indexOf( sal_Unicode ch, sal_Int32 fromIndex = 0 ) const SAL_THROW(())
    {
        sal_Int32 ret = rtl_ustr_indexOfChar_WithLength( pData->buffer+fromIndex, pData->length-fromIndex, ch );
        return (ret < 0 ? ret : ret+fromIndex);
    }

    /**
      Returns the index within this string of the last occurrence of the
      specified character, searching backward starting at the end.

      @param    ch          character to be located.
      @return   the index of the last occurrence of the character in the
                character sequence represented by this string, or
                -1 if the character does not occur.
    */
    sal_Int32 lastIndexOf( sal_Unicode ch ) const SAL_THROW(())
    {
        return rtl_ustr_lastIndexOfChar_WithLength( pData->buffer, pData->length, ch );
    }

    /**
      Returns the index within this string of the last occurrence of the
      specified character, searching backward starting before the specified
      index.

      @param    ch          character to be located.
      @param    fromIndex   the index before which to start the search.
      @return   the index of the last occurrence of the character in the
                character sequence represented by this string that
                is less than fromIndex, or -1
                if the character does not occur before that point.
    */
    sal_Int32 lastIndexOf( sal_Unicode ch, sal_Int32 fromIndex ) const SAL_THROW(())
    {
        return rtl_ustr_lastIndexOfChar_WithLength( pData->buffer, fromIndex, ch );
    }

    /**
      Returns the index within this string of the first occurrence of the
      specified substring, starting at the specified index.

      If str doesn't include any character, always -1 is
      returned. This is also the case, if both strings are empty.

      @param    str         the substring to search for.
      @param    fromIndex   the index to start the search from.
      @return   If the string argument occurs one or more times as a substring
                within this string at the starting index, then the index
                of the first character of the first such substring is
                returned. If it does not occur as a substring starting
                at fromIndex or beyond, -1 is returned.
    */
    sal_Int32 indexOf( const OUString & str, sal_Int32 fromIndex = 0 ) const SAL_THROW(())
    {
        sal_Int32 ret = rtl_ustr_indexOfStr_WithLength( pData->buffer+fromIndex, pData->length-fromIndex,
                                                        str.pData->buffer, str.pData->length );
        return (ret < 0 ? ret : ret+fromIndex);
    }

    /**
       Returns the index within this string of the first occurrence of the
       specified ASCII substring, starting at the specified index.

       @param str
       the substring to be searched for.  Need not be null-terminated, but must
       be at least as long as the specified len.  Must only contain characters
       in the ASCII range 0x00--7F.

       @param len
       the length of the substring; must be non-negative.

       @param fromIndex
       the index to start the search from.  Must be in the range from zero to
       the length of this string, inclusive.

       @return
       the index (starting at 0) of the first character of the first occurrence
       of the substring within this string starting at the given fromIndex, or
       -1 if the substring does not occur.  If len is zero, -1 is returned.

       @since UDK 3.2.7
    */
    sal_Int32 indexOfAsciiL(
        char const * str, sal_Int32 len, sal_Int32 fromIndex = 0) const
        SAL_THROW(())
    {
        sal_Int32 ret = rtl_ustr_indexOfAscii_WithLength(
            pData->buffer + fromIndex, pData->length - fromIndex, str, len);
        return ret < 0 ? ret : ret + fromIndex;
    }

    /**
      Returns the index within this string of the last occurrence of
      the specified substring, searching backward starting at the end.

      The returned index indicates the starting index of the substring
      in this string.
      If str doesn't include any character, always -1 is
      returned. This is also the case, if both strings are empty.

      @param    str         the substring to search for.
      @return   If the string argument occurs one or more times as a substring
                within this string, then the index of the first character of
                the last such substring is returned. If it does not occur as
                a substring, -1 is returned.
    */
    sal_Int32 lastIndexOf( const OUString & str ) const SAL_THROW(())
    {
        return rtl_ustr_lastIndexOfStr_WithLength( pData->buffer, pData->length,
                                                   str.pData->buffer, str.pData->length );
    }

    /**
      Returns the index within this string of the last occurrence of
      the specified substring, searching backward starting before the specified
      index.

      The returned index indicates the starting index of the substring
      in this string.
      If str doesn't include any character, always -1 is
      returned. This is also the case, if both strings are empty.

      @param    str         the substring to search for.
      @param    fromIndex   the index before which to start the search.
      @return   If the string argument occurs one or more times as a substring
                within this string before the starting index, then the index
                of the first character of the last such substring is
                returned. Otherwise, -1 is returned.
    */
    sal_Int32 lastIndexOf( const OUString & str, sal_Int32 fromIndex ) const SAL_THROW(())
    {
        return rtl_ustr_lastIndexOfStr_WithLength( pData->buffer, fromIndex,
                                                   str.pData->buffer, str.pData->length );
    }

    /**
       Returns the index within this string of the last occurrence of the
       specified ASCII substring.

       @param str
       the substring to be searched for.  Need not be null-terminated, but must
       be at least as long as the specified len.  Must only contain characters
       in the ASCII range 0x00--7F.

       @param len
       the length of the substring; must be non-negative.

       @return
       the index (starting at 0) of the first character of the last occurrence
       of the substring within this string, or -1 if the substring does not
       occur.  If len is zero, -1 is returned.

       @since UDK 3.2.7
    */
    sal_Int32 lastIndexOfAsciiL(char const * str, sal_Int32 len) const
        SAL_THROW(())
    {
        return rtl_ustr_lastIndexOfAscii_WithLength(
            pData->buffer, pData->length, str, len);
    }

    /**
      Returns a new string that is a substring of this string.

      The substring begins at the specified beginIndex.  It is an error for
      beginIndex to be negative or to be greater than the length of this string.

      @param     beginIndex   the beginning index, inclusive.
      @return    the specified substring.
    */
    OUString copy( sal_Int32 beginIndex ) const SAL_THROW(())
    {
        OSL_ASSERT(beginIndex >= 0 && beginIndex <= getLength());
        if ( beginIndex == 0 )
            return *this;
        else
        {
            rtl_uString* pNew = 0;
            rtl_uString_newFromStr_WithLength( &pNew, pData->buffer+beginIndex, getLength()-beginIndex );
            return OUString( pNew, (DO_NOT_ACQUIRE*)0 );
        }
    }

    /**
      Returns a new string that is a substring of this string.

      The substring begins at the specified beginIndex and contains count
      characters.  It is an error for either beginIndex or count to be negative,
      or for beginIndex + count to be greater than the length of this string.

      @param     beginIndex   the beginning index, inclusive.
      @param     count        the number of characters.
      @return    the specified substring.
    */
    OUString copy( sal_Int32 beginIndex, sal_Int32 count ) const SAL_THROW(())
    {
        OSL_ASSERT(beginIndex >= 0 && beginIndex <= getLength()
                   && count >= 0 && count <= getLength() - beginIndex);
        if ( (beginIndex == 0) && (count == getLength()) )
            return *this;
        else
        {
            rtl_uString* pNew = 0;
            rtl_uString_newFromStr_WithLength( &pNew, pData->buffer+beginIndex, count );
            return OUString( pNew, (DO_NOT_ACQUIRE*)0 );
        }
    }

    /**
      Concatenates the specified string to the end of this string.

      @param    str   the string that is concatenated to the end
                      of this string.
      @return   a string that represents the concatenation of this string
                followed by the string argument.
    */
    OUString concat( const OUString & str ) const SAL_THROW(())
    {
        rtl_uString* pNew = 0;
        rtl_uString_newConcat( &pNew, pData, str.pData );
        return OUString( pNew, (DO_NOT_ACQUIRE*)0 );
    }

    friend OUString operator+( const OUString& rStr1, const OUString& rStr2  ) SAL_THROW(())
    {
        return rStr1.concat( rStr2 );
    }

    /**
      Returns a new string resulting from replacing n = count characters
      from position index in this string with newStr.

      @param  index   the replacing index in str.
                      The index must be greater or equal as 0 and
                      less or equal as the length of the string.
      @param  count   the count of charcters that will replaced
                      The count must be greater or equal as 0 and
                      less or equal as the length of the string minus index.
      @param  newStr  the new substring.
      @return the new string.
    */
    OUString replaceAt( sal_Int32 index, sal_Int32 count, const OUString& newStr ) const SAL_THROW(())
    {
        rtl_uString* pNew = 0;
        rtl_uString_newReplaceStrAt( &pNew, pData, index, count, newStr.pData );
        return OUString( pNew, (DO_NOT_ACQUIRE*)0 );
    }

    /**
      Returns a new string resulting from replacing all occurrences of
      oldChar in this string with newChar.

      If the character oldChar does not occur in the character sequence
      represented by this object, then the string is assigned with
      str.

      @param    oldChar     the old character.
      @param    newChar     the new character.
      @return   a string derived from this string by replacing every
                occurrence of oldChar with newChar.
    */
    OUString replace( sal_Unicode oldChar, sal_Unicode newChar ) const SAL_THROW(())
    {
        rtl_uString* pNew = 0;
        rtl_uString_newReplace( &pNew, pData, oldChar, newChar );
        return OUString( pNew, (DO_NOT_ACQUIRE*)0 );
    }

    /**
      Converts from this string all ASCII uppercase characters (65-90)
      to ASCII lowercase characters (97-122).

      This function can't be used for language specific conversion.
      If the string doesn't contain characters which must be converted,
      then the new string is assigned with str.

      @return   the string, converted to ASCII lowercase.
    */
    OUString toAsciiLowerCase() const SAL_THROW(())
    {
        rtl_uString* pNew = 0;
        rtl_uString_newToAsciiLowerCase( &pNew, pData );
        return OUString( pNew, (DO_NOT_ACQUIRE*)0 );
    }

    /**
      Converts from this string all ASCII lowercase characters (97-122)
      to ASCII uppercase characters (65-90).

      This function can't be used for language specific conversion.
      If the string doesn't contain characters which must be converted,
      then the new string is assigned with str.

      @return   the string, converted to ASCII uppercase.
    */
    OUString toAsciiUpperCase() const SAL_THROW(())
    {
        rtl_uString* pNew = 0;
        rtl_uString_newToAsciiUpperCase( &pNew, pData );
        return OUString( pNew, (DO_NOT_ACQUIRE*)0 );
    }

    /**
      Returns a new string resulting from removing white space from both ends
      of the string.

      All characters that have codes less than or equal to
      32 (the space character) are considered to be white space.
      If the string doesn't contain white spaces at both ends,
      then the new string is assigned with str.

      @return   the string, with white space removed from the front and end.
    */
    OUString trim() const SAL_THROW(())
    {
        rtl_uString* pNew = 0;
        rtl_uString_newTrim( &pNew, pData );
        return OUString( pNew, (DO_NOT_ACQUIRE*)0 );
    }

    /**
      Returns a token in the string.

      Example:
        sal_Int32 nIndex = 0;
        do
        {
            ...
            OUString aToken = aStr.getToken( 0, ';', nIndex );
            ...
        }
        while ( nIndex >= 0 );

      @param    token       the number of the token to return
      @param    cTok        the character which seperate the tokens.
      @param    index       the position at which the token is searched in the
                            string.
                            The index must not be greater than the length of the
                            string.
                            This param is set to the position of the
                            next token or to -1, if it is the last token.
      @return   the token; if either token or index is negative, an empty token
                is returned (and index is set to -1)
    */
    OUString getToken( sal_Int32 token, sal_Unicode cTok, sal_Int32& index ) const SAL_THROW(())
    {
        rtl_uString * pNew = 0;
        index = rtl_uString_getToken( &pNew, pData, token, cTok, index );
        return OUString( pNew, (DO_NOT_ACQUIRE *)0 );
    }

    /**
      Returns the Boolean value from this string.

      This function can't be used for language specific conversion.

      @return   sal_True, if the string is 1 or "True" in any ASCII case.
                sal_False in any other case.
    */
    sal_Bool toBoolean() const SAL_THROW(())
    {
        return rtl_ustr_toBoolean( pData->buffer );
    }

    /**
      Returns the first character from this string.

      @return   the first character from this string or 0, if this string
                is emptry.
    */
    sal_Unicode toChar() const SAL_THROW(())
    {
        return pData->buffer[0];
    }

    /**
      Returns the int32 value from this string.

      This function can't be used for language specific conversion.

      @param    radix       the radix (between 2 and 36)
      @return   the int32 represented from this string.
                0 if this string represents no number.
    */
    sal_Int32 toInt32( sal_Int16 radix = 10 ) const SAL_THROW(())
    {
        return rtl_ustr_toInt32( pData->buffer, radix );
    }

    /**
      Returns the int64 value from this string.

      This function can't be used for language specific conversion.

      @param    radix       the radix (between 2 and 36)
      @return   the int64 represented from this string.
                0 if this string represents no number.
    */
    sal_Int64 toInt64( sal_Int16 radix = 10 ) const SAL_THROW(())
    {
        return rtl_ustr_toInt64( pData->buffer, radix );
    }

    /**
      Returns the float value from this string.

      This function can't be used for language specific conversion.

      @return   the float represented from this string.
                0.0 if this string represents no number.
    */
    float toFloat() const SAL_THROW(())
    {
        return rtl_ustr_toFloat( pData->buffer );
    }

    /**
      Returns the double value from this string.

      This function can't be used for language specific conversion.

      @return   the double represented from this string.
                0.0 if this string represents no number.
    */
    double toDouble() const SAL_THROW(())
    {
        return rtl_ustr_toDouble( pData->buffer );
    }


    /**
       Return a canonical representation for a string.

       A pool of strings, initially empty is maintained privately
       by the string class. On invocation, if present in the pool
       the original string will be returned. Otherwise this string,
       or a copy thereof will be added to the pool and returned.

       @return
       a version of the string from the pool.

       @exception std::bad_alloc is thrown if an out-of-memory condition occurs

       @since UDK 3.2.7
    */
    OUString intern() const
    {
        rtl_uString * pNew = 0;
        rtl_uString_intern( &pNew, pData );
#if defined EXCEPTIONS_OFF
        OSL_ASSERT(pNew != NULL);
#else
        if (pNew == 0) {
            throw std::bad_alloc();
        }
#endif
        return OUString( pNew, (DO_NOT_ACQUIRE *)0 );
    }

    /**
       Return a canonical representation for a converted string.

       A pool of strings, initially empty is maintained privately
       by the string class. On invocation, if present in the pool
       the original string will be returned. Otherwise this string,
       or a copy thereof will be added to the pool and returned.

       @param    value           a 8-Bit character array.
       @param    length          the number of character which should be converted.
                                 The 8-Bit character array length must be
                                 greater or equal than this value.
       @param    encoding        the text encoding from which the 8-Bit character
                                 sequence should be converted.
       @param    convertFlags    flags which controls the conversion.
                                 see RTL_TEXTTOUNICODE_FLAGS_...
       @param    pInfo           pointer to return conversion status or NULL.

       @return
       a version of the converted string from the pool.

       @exception std::bad_alloc is thrown if an out-of-memory condition occurs

       @since UDK 3.2.7
    */
    static OUString intern( const sal_Char * value, sal_Int32 length,
                            rtl_TextEncoding encoding,
                            sal_uInt32 convertFlags = OSTRING_TO_OUSTRING_CVTFLAGS,
                            sal_uInt32 *pInfo = NULL )
    {
        rtl_uString * pNew = 0;
        rtl_uString_internConvert( &pNew, value, length, encoding,
                                   convertFlags, pInfo );
#if defined EXCEPTIONS_OFF
        OSL_ASSERT(pNew != NULL);
#else
        if (pNew == 0) {
            throw std::bad_alloc();
        }
#endif
        return OUString( pNew, (DO_NOT_ACQUIRE *)0 );
    }

    /**
      Converts to an OString, signalling failure.

      @param pTarget
      An out parameter receiving the converted OString.  Must not be null; the
      contents are not modified if conversion fails (convertToOString returns
      false).

      @param nEncoding
      The text encoding to convert into.  Must be an octet encoding (i.e.,
      rtl_isOctetTextEncoding(nEncoding) must return true).

      @param nFlags
      A combination of RTL_UNICODETOTEXT_FLAGS that detail how to do the
      conversion (see rtl_convertUnicodeToText).  RTL_UNICODETOTEXT_FLAGS_FLUSH
      need not be included, it is implicitly assumed.  Typical uses are either
      RTL_UNICODETOTEXT_FLAGS_UNDEFINED_ERROR |
      RTL_UNICODETOTEXT_FLAGS_INVALID_ERROR (fail if a Unicode character cannot
      be converted to the target nEncoding) or OUSTRING_TO_OSTRING_CVTFLAGS
      (make a best efforts conversion).

      @return
      True if the conversion succeeded, false otherwise.
     */
    inline bool convertToString(OString * pTarget, rtl_TextEncoding nEncoding,
                                sal_uInt32 nFlags) const
    {
        return rtl_convertUStringToString(&pTarget->pData, pData->buffer,
                                            pData->length, nEncoding, nFlags);
    }

    /** Iterate through this string based on code points instead of UTF-16 code
        units.

        See Chapter 3 of The Unicode Standard 5.0 (Addison--Wesley, 2006) for
        definitions of the various terms used in this description.

        This string is interpreted as a sequence of zero or more UTF-16 code
        units.  For each index into this sequence (from zero to one less than
        the length of the sequence, inclusive), a code point represented
        starting at the given index is computed as follows:

        - If the UTF-16 code unit addressed by the index constitutes a
        well-formed UTF-16 code unit sequence, the computed code point is the
        scalar value encoded by that UTF-16 code unit sequence.

        - Otherwise, if the index is at least two UTF-16 code units away from
        the end of the sequence, and the sequence of two UTF-16 code units
        addressed by the index constitutes a well-formed UTF-16 code unit
        sequence, the computed code point is the scalar value encoded by that
        UTF-16 code unit sequence.

        - Otherwise, the computed code point is the UTF-16 code unit addressed
        by the index.  (This last case catches unmatched surrogates as well as
        indices pointing into the middle of surrogate pairs.)

        @param indexUtf16
        pointer to a UTF-16 based index into this string; must not be null.  On
        entry, the index must be in the range from zero to the length of this
        string (in UTF-16 code units), inclusive.  Upon successful return, the
        index will be updated to address the UTF-16 code unit that is the given
        incrementCodePoints away from the initial index.

        @param incrementCodePoints
        the number of code points to move the given *indexUtf16.  If
        non-negative, moving is done after determining the code point at the
        index.  If negative, moving is done before determining the code point
        at the (then updated) index.  The value must be such that the resulting
        UTF-16 based index is in the range from zero to the length of this
        string (in UTF-16 code units), inclusive.

        @return
        the code point (an integer in the range from 0 to 0x10FFFF, inclusive)
        that is represented within this string starting at the index computed as
        follows:  If incrementCodePoints is non-negative, the index is the
        initial value of *indexUtf16; if incrementCodePoints is negative, the
        index is the updated value of *indexUtf16.  In either case, the computed
        index must be in the range from zero to one less than the length of this
        string (in UTF-16 code units), inclusive.

        @since UDK 3.2.7
    */
    inline sal_uInt32 iterateCodePoints(
        sal_Int32 * indexUtf16, sal_Int32 incrementCodePoints = 1) const
    {
        return rtl_uString_iterateCodePoints(
            pData, indexUtf16, incrementCodePoints);
    }

    /**
      Returns the string representation of the sal_Bool argument.

      If the sal_Bool is true, the string "true" is returned.
      If the sal_Bool is false, the string "false" is returned.
      This function can't be used for language specific conversion.

      @param    b   a sal_Bool.
      @return   a string with the string representation of the argument.
    */
    static OUString valueOf( sal_Bool b ) SAL_THROW(())
    {
        sal_Unicode aBuf[RTL_USTR_MAX_VALUEOFBOOLEAN];
        rtl_uString* pNewData = 0;
        rtl_uString_newFromStr_WithLength( &pNewData, aBuf, rtl_ustr_valueOfBoolean( aBuf, b ) );
        return OUString( pNewData, (DO_NOT_ACQUIRE*)0 );
    }

    /**
      Returns the string representation of the char argument.

      @param    c   a character.
      @return   a string with the string representation of the argument.
    */
    static OUString valueOf( sal_Unicode c ) SAL_THROW(())
    {
        return OUString( &c, 1 );
    }

    /**
      Returns the string representation of the int argument.

      This function can't be used for language specific conversion.

      @param    i           a int32.
      @param    radix       the radix (between 2 and 36)
      @return   a string with the string representation of the argument.
    */
    static OUString valueOf( sal_Int32 i, sal_Int16 radix = 10 ) SAL_THROW(())
    {
        sal_Unicode aBuf[RTL_USTR_MAX_VALUEOFINT32];
        rtl_uString* pNewData = 0;
        rtl_uString_newFromStr_WithLength( &pNewData, aBuf, rtl_ustr_valueOfInt32( aBuf, i, radix ) );
        return OUString( pNewData, (DO_NOT_ACQUIRE*)0 );
    }

    /**
      Returns the string representation of the long argument.

      This function can't be used for language specific conversion.

      @param    ll          a int64.
      @param    radix       the radix (between 2 and 36)
      @return   a string with the string representation of the argument.
    */
    static OUString valueOf( sal_Int64 ll, sal_Int16 radix = 10 ) SAL_THROW(())
    {
        sal_Unicode aBuf[RTL_USTR_MAX_VALUEOFINT64];
        rtl_uString* pNewData = 0;
        rtl_uString_newFromStr_WithLength( &pNewData, aBuf, rtl_ustr_valueOfInt64( aBuf, ll, radix ) );
        return OUString( pNewData, (DO_NOT_ACQUIRE*)0 );
    }

    /**
      Returns the string representation of the float argument.

      This function can't be used for language specific conversion.

      @param    f           a float.
      @return   a string with the string representation of the argument.
    */
    static OUString valueOf( float f ) SAL_THROW(())
    {
        sal_Unicode aBuf[RTL_USTR_MAX_VALUEOFFLOAT];
        rtl_uString* pNewData = 0;
        rtl_uString_newFromStr_WithLength( &pNewData, aBuf, rtl_ustr_valueOfFloat( aBuf, f ) );
        return OUString( pNewData, (DO_NOT_ACQUIRE*)0 );
    }

    /**
      Returns the string representation of the double argument.

      This function can't be used for language specific conversion.

      @param    d           a double.
      @return   a string with the string representation of the argument.
    */
    static OUString valueOf( double d ) SAL_THROW(())
    {
        sal_Unicode aBuf[RTL_USTR_MAX_VALUEOFDOUBLE];
        rtl_uString* pNewData = 0;
        rtl_uString_newFromStr_WithLength( &pNewData, aBuf, rtl_ustr_valueOfDouble( aBuf, d ) );
        return OUString( pNewData, (DO_NOT_ACQUIRE*)0 );
    }

    /**
      Returns a OUString copied without conversion from an ASCII
      character string.

      Since this method is optimized for performance, the ASCII character
      values are not converted in any way. The caller has to make sure that
      all ASCII characters are in the allowed range between 0 and
      127. The ASCII string must be NULL-terminated.

      @param    value       the 8-Bit ASCII character string
      @return   a string with the string representation of the argument.
     */
    static OUString createFromAscii( const sal_Char * value ) SAL_THROW(())
    {
        rtl_uString* pNew = 0;
        rtl_uString_newFromAscii( &pNew, value );
        return OUString( pNew, (DO_NOT_ACQUIRE*)0 );
    }
};

/* ======================================================================= */

/** A helper to use OUStrings with hash maps.

    Instances of this class are unary function objects that can be used as
    hash function arguments to STLPort's hash_map and similar constructs.
 */
struct OUStringHash
{
    /** Compute a hash code for a string.

        @param rString
        a string.

        @return
        a hash code for the string.  This hash code should not be stored
        persistently, as its computation may change in later revisions.
     */
    size_t operator()(const rtl::OUString& rString) const
        { return (size_t)rString.hashCode(); }
};

/* ======================================================================= */

/** Convert an OString to an OUString, using a specific text encoding.

    The lengths of the two strings may differ (e.g., for double-byte
    encodings, UTF-7, UTF-8).

    @param rStr
    an OString to convert.

    @param encoding
    the text encoding to use for conversion.

    @param convertFlags
    flags which control the conversion.  Either use
    OSTRING_TO_OUSTRING_CVTFLAGS, or see
    <http://udk.openoffice.org/cpp/man/spec/textconversion.html> for more
    details.
 */
inline OUString OStringToOUString( const OString & rStr,
                                   rtl_TextEncoding encoding,
                                   sal_uInt32 convertFlags = OSTRING_TO_OUSTRING_CVTFLAGS )
{
    return OUString( rStr.getStr(), rStr.getLength(), encoding, convertFlags );
}

/** Convert an OUString to an OString, using a specific text encoding.

    The lengths of the two strings may differ (e.g., for double-byte
    encodings, UTF-7, UTF-8).

    @param rStr
    an OUString to convert.

    @param encoding
    the text encoding to use for conversion.

    @param convertFlags
    flags which control the conversion.  Either use
    OUSTRING_TO_OSTRING_CVTFLAGS, or see
    <http://udk.openoffice.org/cpp/man/spec/textconversion.html> for more
    details.
 */
inline OString OUStringToOString( const OUString & rUnicode,
                                  rtl_TextEncoding encoding,
                                  sal_uInt32 convertFlags = OUSTRING_TO_OSTRING_CVTFLAGS )
{
    return OString( rUnicode.getStr(), rUnicode.getLength(), encoding, convertFlags );
}

/* ======================================================================= */

} /* Namespace */

#endif /* __cplusplus */

#endif /* _RTL_USTRING_HXX */
