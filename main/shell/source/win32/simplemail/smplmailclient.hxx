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



#ifndef _SMPLMAILCLIENT_HXX_
#define _SMPLMAILCLIENT_HXX_

#include <cppuhelper/compbase1.hxx>
#include <com/sun/star/lang/XServiceInfo.hpp>

#ifndef _COM_SUN_STAR_SYS_SHELL_XSYSTEMSHELLEXECUTE_HPP_
#include <com/sun/star/system/XSimpleMailClient.hpp>
#endif

namespace css = ::com::sun::star;

class CSmplMailClient : public cppu::WeakImplHelper1<css::system::XSimpleMailClient>
{
public:    
    virtual css::uno::Reference<css::system::XSimpleMailMessage> SAL_CALL createSimpleMailMessage() 
        throw (css::uno::RuntimeException);

    virtual void SAL_CALL sendSimpleMailMessage(const css::uno::Reference<css::system::XSimpleMailMessage>& xSimpleMailMessage, sal_Int32 aFlag) 
        throw (css::lang::IllegalArgumentException, css::uno::Exception, css::uno::RuntimeException);

private:    
    void validateParameter(const css::uno::Reference<css::system::XSimpleMailMessage>& xSimpleMailMessage, sal_Int32 aFlag);        
    void assembleCommandLine(const css::uno::Reference<css::system::XSimpleMailMessage>& xSimpleMailMessage, sal_Int32 aFlag, std::vector<rtl::OUString>& rCommandArgs);    
}; 

#endif 
