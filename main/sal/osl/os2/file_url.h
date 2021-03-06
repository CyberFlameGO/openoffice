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


 
 /***************************************************
  * Internal header file, declares all functions 
  * that are not part of the offical API and are 
  * not defined in the osl header files
  **************************************************/
 
 #ifndef _FILE_URL_H_
 #define _FILE_URL_H_
 
 #ifndef _FILE_H_
 #include <osl/file.h>
 #endif
 

/***************************************************
 * constants
 **************************************************/

#define _tcslen(a)		wcslen((const wchar_t *) a)
#define _tcsspn(a,b)	wcsspn((const wchar_t *) a, (const wchar_t *) b)
#define _istalpha(a)	iswalpha((wint_t) a)

const sal_Unicode UNICHAR_SLASH = ((sal_Unicode)'/');
const sal_Unicode UNICHAR_COLON = ((sal_Unicode)':');
const sal_Unicode UNICHAR_DOT   = ((sal_Unicode)'.');

#define ELEMENTS_OF_ARRAY(arr) (sizeof(arr)/(sizeof((arr)[0])))

#if OSL_DEBUG_LEVEL > 0
#define OSL_ENSURE_FILE( cond, msg, file ) ( (cond) ?  (void)0 : _osl_warnFile( msg, file ) )
#else
#define OSL_ENSURE_FILE( cond, msg, file ) ((void)0)
#endif

typedef sal_Unicode            				TCHAR;
typedef sal_Unicode           				*LPTSTR;
typedef const sal_Unicode     				*LPCTSTR;
typedef wchar_t           				*LPWSTR;
typedef const wchar_t     				*LPCWSTR;
typedef sal_Unicode							DWORD;
#define WINAPI

#define	CHARSET_SEPARATOR					L"\\/"
#define WSTR_SYSTEM_ROOT_PATH				L"\\\\.\\"


/******************************************************************************
 *
 *                  Data Type Definition
 *
 ******************************************************************************/

#define PATHTYPE_ERROR						0
#define PATHTYPE_RELATIVE					1
#define PATHTYPE_ABSOLUTE_UNC				2
#define PATHTYPE_ABSOLUTE_LOCAL				3
#define PATHTYPE_MASK_TYPE					0xFF
#define PATHTYPE_IS_VOLUME					0x0100
#define PATHTYPE_IS_SERVER					0x0200

#define VALIDATEPATH_NORMAL					0x0000
#define VALIDATEPATH_ALLOW_WILDCARDS		0x0001
#define VALIDATEPATH_ALLOW_ELLIPSE			0x0002
#define VALIDATEPATH_ALLOW_RELATIVE			0x0004
#define VALIDATEPATH_ALLOW_UNC				0x0008

typedef struct {
	UINT			uType;
    rtl_uString* 	ustrDrive;
    rtl_uString* 	ustrFilePath;	/* holds native directory path */
	int				d_attr;			/* OS/2 file attributes */
	int				nRefCount;
}DirectoryItem_Impl;

#define	DIRECTORYTYPE_LOCALROOT	    0
#define	DIRECTORYTYPE_NETROOT		1
#define	DIRECTORYTYPE_NETRESORCE	2
#define	DIRECTORYTYPE_FILESYSTEM	3

#define DIRECTORYITEM_DRIVE		0
#define DIRECTORYITEM_FILE		1
#define DIRECTORYITEM_SERVER	2

typedef struct
{
	UINT		 uType;
    rtl_uString* ustrPath;           /* holds native directory path */
    DIR*         pDirStruct;
	ULONG		 ulDriveMap;
	ULONG		 ulNextDrive;
	ULONG		 ulNextDriveMask;
} DirectoryImpl;

/* Different types of paths */
typedef enum _PATHTYPE
{
	PATHTYPE_SYNTAXERROR = 0,
	PATHTYPE_NETROOT,
	PATHTYPE_NETSERVER,
	PATHTYPE_VOLUME,
	PATHTYPE_FILE
} PATHTYPE;

DWORD WINAPI IsValidFilePath(LPCTSTR, LPCTSTR*, DWORD);


typedef struct _oslVolumeDeviceHandleImpl
{
    sal_Char pszMountPoint[PATH_MAX];
    sal_Char pszFilePath[PATH_MAX];
    sal_Char pszDevice[PATH_MAX];
    sal_Char ident[4];
    sal_uInt32   RefCount;
} oslVolumeDeviceHandleImpl;

/* OS error to errno values mapping table */
struct errentry {
	unsigned long oscode;	/* OS return value */
	int errnocode;			/* System V error code */
};

#ifdef __cplusplus
extern "C" 
{
#endif
 
/**************************************************
* _osl_getSystemPathFromFileURL
*************************************************/

#define FURL_ALLOW_RELATIVE sal_True
#define FURL_DENY_RELATIVE  sal_False

oslFileError osl_getSystemPathFromFileURL_Ex(rtl_uString *ustrFileURL, rtl_uString **pustrSystemPath, sal_Bool bAllowRelative);

/**************************************************
* FileURLToPath
*************************************************/

oslFileError FileURLToPath(char * buffer, size_t bufLen, rtl_uString* ustrFileURL);


#ifdef __cplusplus
}
#endif


#endif /* #define _FILE_URL_H_ */

