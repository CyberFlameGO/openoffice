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



package ifc.drawing;

import lib.MultiPropertyTest;


/**
* Testing <code>com.sun.star.drawing.GenericDrawPage</code>
* service properties :
* <ul>
*  <li><code> BorderBottom</code></li>
*  <li><code> BorderLeft</code></li>
*  <li><code> BorderRight</code></li>
*  <li><code> BorderTop</code></li>
*  <li><code> Height</code></li>
*  <li><code> Width</code></li>
*  <li><code> Number</code></li>
*  <li><code> Orientation</code></li>
* </ul> <p>
* Properties testing is automated by <code>lib.MultiPropertyTest</code>.
* @see com.sun.star.drawing.GenericDrawPage
*/
public class _GenericDrawPage extends MultiPropertyTest {

    public void _Number() {
        log.println("Number started");
        boolean res = false;
        log.println("trying to get the value");
        try {
            Short wat = (Short) oObj.getPropertyValue("Number");
            if (wat == null) {
                log.println("it is null");
            } else {
                log.println("it isn't null");
                res=true;
            }
        } catch (com.sun.star.beans.UnknownPropertyException e) {
            log.println("an UnknownPropertyException occured");

        } catch (com.sun.star.lang.WrappedTargetException e) {
            log.println("an WrappedTargetException occured");
        }

        tRes.tested("Number",res);
    }

    public void _UserDefinedAttributes() {
        log.println("Userdefined started");
        boolean res = false;
        log.println("trying to get the value");
        try {
            Object wat = oObj.getPropertyValue("UserDefinedAttributes");
            if (wat == null) {
                log.println("it is null");
            } else {
                log.println("it isn't null");
                res=true;
            }
        } catch (com.sun.star.beans.UnknownPropertyException e) {
            log.println("an UnknownPropertyException occured");

        } catch (com.sun.star.lang.WrappedTargetException e) {
            log.println("an WrappedTargetException occured");
        }

        tRes.tested("UserDefinedAttributes",res);
    }

} // end of GenericDrawPage

