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



package ifc.awt;


import lib.MultiMethodTest;

import com.sun.star.awt.XButton;

/**
* Testing <code>com.sun.star.awt.XButton</code>
* interface methods :
* <ul>
*  <li><code> addActionListener()</code></li>
*  <li><code> removeActionListener()</code></li>
*  <li><code> setLabel()</code></li>
*  <li><code> setActionCommand()</code></li>
* </ul> <p>
* Test is <b> NOT </b> multithread compilant. <p>
* @see com.sun.star.awt.XButton
*/
public class _XButton extends MultiMethodTest {

    public XButton oObj = null;

    /**
    * Listener implementation which sets flags on appropriate method calls
    */
    protected class TestActionListener implements com.sun.star.awt.XActionListener {
        public boolean disposingCalled = false ;
        public boolean actionPerformedCalled = false ;

        public void disposing(com.sun.star.lang.EventObject e) {
            disposingCalled = true ;
        }

        public void actionPerformed(com.sun.star.awt.ActionEvent e) {
            actionPerformedCalled = true ;
        }

    }

    TestActionListener listener = new TestActionListener() ;

    /**
    * !!! Can be checked only interactively !!!
    */
    public void _addActionListener() {

        boolean result = true ;
        oObj.addActionListener(listener) ;

        tRes.tested("addActionListener()", result) ;
    }

    /**
    * !!! Can be checked only interactively !!!
    */
    public void _removeActionListener() {

        boolean result = true ;
        oObj.removeActionListener(listener) ;

        tRes.tested("removeActionListener()", result) ;
    }

    /**
    * Just sets some text for label. <p>
    * Has <b> OK </b> status if no runtime exceptions occured
    */
    public void _setLabel() {

        boolean result = true ;
        oObj.setLabel("XButton Label") ;

        tRes.tested("setLabel()", result) ;
    }

    /**
    * Just sets some command for button. <p>
    * Has <b> OK </b> status if no runtime exceptions occured
    */
    public void _setActionCommand() {

        boolean result = true ;
        oObj.setActionCommand("XButtonComand") ;

        tRes.tested("setActionCommand()", result) ;
    }

}


