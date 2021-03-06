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


#ifndef _TESTAPP_HXX
#define _TESTAPP_HXX

#include <vcl/svapp.hxx>
#ifndef _DIALOG_HXX //autogen
#include <vcl/dialog.hxx>
#endif
#ifndef _BUTTON_HXX //autogen
#include <vcl/button.hxx>
#endif
#ifndef _FIELD_HXX //autogen
#include <vcl/field.hxx>
#endif
#ifndef _EDIT_HXX //autogen
#include <vcl/edit.hxx>
#endif
#ifndef _GROUP_HXX //autogen
#include <vcl/group.hxx>
#endif
#ifndef _COMBOBOX_HXX //autogen
#include <vcl/combobox.hxx>
#endif
#ifndef _FIXED_HXX //autogen
#include <vcl/fixed.hxx>
#endif
#ifndef _MENU_HXX //autogen
#include <vcl/menu.hxx>
#endif
#ifndef _WRKWIN_HXX //autogen
#include <vcl/wrkwin.hxx>
#endif
#include <svl/poolitem.hxx>


#include "editwin.hxx"


#define EXECUTE_NO					0
#define EXECUTE_POSSIBLE            1
#define EXECUTE_YES                 2
#define EXECUTEMODE_ASYNCHRON		1
#define EXECUTEMODE_DIALOGASYNCHRON 2


class MyApp;
class MainWindow : public WorkWindow
{
	MyApp *pApp;

public:
	MainWindow(MyApp *pAppl);
	virtual sal_Bool Close(); // derived

	void FileExit();
	void FileOpen();
	void TestGross();
	void Tree(GHEditWindow *aEditWin, Window *pBase, sal_uInt16 Indent);
	void WinTree();
	void SysDlg();
	DECL_LINK(MenuSelectHdl,MenuBar*);

};
#define PlugInDispatcher MyDispatcher
class MyDispatcher
{
	MainWindow  *pMainWin;

public:
	MyDispatcher(MainWindow  *MainWin) : pMainWin(MainWin) {};
	virtual ~MyDispatcher() {};
	virtual sal_uInt16 ExecuteFunction( sal_uInt16 nSID, SfxPoolItem** ppArgs = 0, sal_uInt16 nMode = 0);
	virtual void SetExecuteMode( sal_uInt16 nMode )
    {
        (void) nMode; /* avoid warning about unused parameter */
    };	// Ist hier sowieso egal
};

class MyApp : public Application
{
	PopupMenu  *MyMenu;
	Timer aCommandTimer;
	PlugInDispatcher *pMyDispatcher;

public:
	MyApp();
	void Main();

	virtual void				Property( ApplicationProperty& );
	virtual PlugInDispatcher*	GetDispatcher();

	MainWindow  *pMainWin;
};

// -----------------------------------------------------------------------

extern MyApp aApp;

#endif

