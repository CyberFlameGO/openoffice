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



// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_shell.hxx"
#include <osl/diagnose.h>
#include <osl/process.h>
#include <rtl/bootstrap.hxx>
#include "smplmailclient.hxx"
#include "smplmailmsg.hxx"
#include <com/sun/star/system/SimpleMailClientFlags.hpp>
#include <osl/file.hxx>

#define WIN32_LEAN_AND_MEAN
#if defined _MSC_VER
#pragma warning(push, 1)
#endif
#include <windows.h>
#include <mapi.h>
#if defined _MSC_VER
#pragma warning(pop)
#endif 

#include <process.h>
#include <vector>

using css::uno::Reference;
using css::uno::Exception;
using css::uno::RuntimeException;
using css::uno::Sequence;
using css::lang::IllegalArgumentException;

using css::system::XSimpleMailClient;
using css::system::XSimpleMailMessage;
using css::system::SimpleMailClientFlags::NO_USER_INTERFACE;
using css::system::SimpleMailClientFlags::NO_LOGON_DIALOG;

typedef std::vector<rtl::OUString> StringList_t;
typedef StringList_t::const_iterator StringListIterator_t;

const rtl::OUString TO = rtl::OUString::createFromAscii("--to");
const rtl::OUString CC = rtl::OUString::createFromAscii("--cc");
const rtl::OUString BCC = rtl::OUString::createFromAscii("--bcc");
const rtl::OUString FROM = rtl::OUString::createFromAscii("--from");
const rtl::OUString SUBJECT = rtl::OUString::createFromAscii("--subject");
const rtl::OUString BODY = rtl::OUString::createFromAscii("--body");
const rtl::OUString ATTACH = rtl::OUString::createFromAscii("--attach");
const rtl::OUString FLAG_MAPI_DIALOG = rtl::OUString::createFromAscii("--mapi-dialog");
const rtl::OUString FLAG_MAPI_LOGON_UI = rtl::OUString::createFromAscii("--mapi-logon-ui");

namespace /* private */
{        
    /** @internal 
        look if an alternative program is configured 
        which should be used as senddoc executable */
    rtl::OUString getAlternativeSenddocUrl()
    {
        rtl::OUString altSenddocUrl;
        HKEY hkey;
        LONG lret = RegOpenKeyW(HKEY_CURRENT_USER, L"Software\\OpenOffice.org\\SendAsEMailClient", &hkey);
        if (lret == ERROR_SUCCESS)
        {
            wchar_t buff[MAX_PATH];
            LONG sz = sizeof(buff);
            lret = RegQueryValueW(hkey, NULL, buff, &sz);            
            if (lret == ERROR_SUCCESS)
            {                
                osl::FileBase::getFileURLFromSystemPath(reinterpret_cast<const sal_Unicode*>(buff), altSenddocUrl);
            }            
            RegCloseKey(hkey);
        }
        return altSenddocUrl;
    }
    
    /**
        Returns the absolute file Url of the senddoc executable.
        
        @returns
        the absolute file Url of the senddoc executable. In case
        of an error an empty string will be returned.
    */
    rtl::OUString getSenddocUrl()
    {
        rtl::OUString senddocUrl = getAlternativeSenddocUrl();
        
        if (senddocUrl.getLength() == 0)
        {
            senddocUrl = rtl::OUString(
                RTL_CONSTASCII_USTRINGPARAM(
                    "$OOO_BASE_DIR/program/senddoc.exe"));
            rtl::Bootstrap::expandMacros(senddocUrl); //TODO: detect failure
        }                    
        return senddocUrl;    
    }
    
    /**
        Execute Senddoc.exe which a MAPI wrapper.
        
        @param rCommandArgs
        [in] the arguments to be passed to Senddoc.exe
        
        @returns
        <TRUE/> on success.
    */
    bool executeSenddoc(const StringList_t& rCommandArgs)
    {    
        rtl::OUString senddocUrl = getSenddocUrl();
        if (senddocUrl.getLength() == 0)
            return false;

        oslProcess proc;    
        oslProcessError err = osl_Process_E_Unknown;
                        
        /* for efficiency reasons we are using a 'bad' cast here 
        as a vector or rtl::OUStrings is nothing else than
        an array of pointers to rtl_uString's */
        err = osl_executeProcess(
            senddocUrl.pData,
            (rtl_uString**)&rCommandArgs[0], 
            rCommandArgs.size(),
            osl_Process_WAIT | osl_Process_DETACHED,
            NULL,
            NULL,
            NULL,
            0,
            &proc);
            
        if (err != osl_Process_E_None)
            return false;
                
        oslProcessInfo procInfo;
        procInfo.Size = sizeof(oslProcessInfo);    
        osl_getProcessInfo(proc, osl_Process_EXITCODE, &procInfo);
        osl_freeProcessHandle(proc);   
        return (procInfo.Code == SUCCESS_SUCCESS);            
    }
} // namespace private 

Reference<XSimpleMailMessage> SAL_CALL CSmplMailClient::createSimpleMailMessage() 
    throw (RuntimeException)
{
    return Reference<XSimpleMailMessage>(new CSmplMailMsg());
}

/**
    Assemble a command line for SendDoc.exe out of the members
    of the supplied SimpleMailMessage.
    
    @param xSimpleMailMessage
    [in] the mail message.
    
    @param aFlags
    [in] different flags to be used with the simple mail service.
    
    @param rCommandArgs
    [in|out] a buffer for the command line arguments. The buffer
    is assumed to be empty.    
    
    @throws com::sun::star::lang::IllegalArgumentException
    if an invalid file URL has been detected in the attachment list.
*/
void CSmplMailClient::assembleCommandLine(
    const Reference<XSimpleMailMessage>& xSimpleMailMessage, 
    sal_Int32 aFlag, StringList_t& rCommandArgs)
{
    OSL_ENSURE(rCommandArgs.size() == 0, "Provided command argument buffer not empty");
    
    rtl::OUString to = xSimpleMailMessage->getRecipient();
    if (to.getLength() > 0)
    {
        rCommandArgs.push_back(TO);
        rCommandArgs.push_back(to);
    }
    
    Sequence<rtl::OUString> ccRecipients = xSimpleMailMessage->getCcRecipient();
    for (int i = 0; i < ccRecipients.getLength(); i++)
    {
        rCommandArgs.push_back(CC);
        rCommandArgs.push_back(ccRecipients[i]);
    }
    
    Sequence<rtl::OUString> bccRecipients = xSimpleMailMessage->getBccRecipient();
    for (int i = 0; i < bccRecipients.getLength(); i++)
    {
        rCommandArgs.push_back(BCC);
        rCommandArgs.push_back(bccRecipients[i]);
    }
    
    rtl::OUString from = xSimpleMailMessage->getOriginator();
    if (from.getLength() > 0)
    {
        rCommandArgs.push_back(FROM);
        rCommandArgs.push_back(from);
    }
    
    rtl::OUString subject = xSimpleMailMessage->getSubject();
    if (subject.getLength() > 0)
    {
        rCommandArgs.push_back(SUBJECT);
        rCommandArgs.push_back(subject);
    }
    
    Sequence<rtl::OUString> attachments = xSimpleMailMessage->getAttachement();    
    for (int i = 0; i < attachments.getLength(); i++)
    {
        rtl::OUString sysPath;
        osl::FileBase::RC err = osl::FileBase::getSystemPathFromFileURL(attachments[i], sysPath);
        if (err != osl::FileBase::E_None)
            throw IllegalArgumentException(
                rtl::OUString::createFromAscii("Invalid attachment file URL"),
                static_cast<XSimpleMailClient*>(this), 
                1);
                
        rCommandArgs.push_back(ATTACH);
        rCommandArgs.push_back(sysPath);        
    }
            
    if (!(aFlag & NO_USER_INTERFACE))
        rCommandArgs.push_back(FLAG_MAPI_DIALOG);        

    if (!(aFlag & NO_LOGON_DIALOG))
        rCommandArgs.push_back(FLAG_MAPI_LOGON_UI);                
}

void SAL_CALL CSmplMailClient::sendSimpleMailMessage(
    const Reference<XSimpleMailMessage>& xSimpleMailMessage, sal_Int32 aFlag) 
    throw (IllegalArgumentException, Exception, RuntimeException)
{    
    validateParameter(xSimpleMailMessage, aFlag);    

    StringList_t senddocParams;    
    assembleCommandLine(xSimpleMailMessage, aFlag, senddocParams);
    
    if (!executeSenddoc(senddocParams))
        throw Exception(
            rtl::OUString::createFromAscii("Send email failed"), 
            static_cast<XSimpleMailClient*>(this));                                                
}

void CSmplMailClient::validateParameter( 
    const Reference<XSimpleMailMessage>& xSimpleMailMessage, sal_Int32 aFlag ) 
{
    if (!xSimpleMailMessage.is())
        throw IllegalArgumentException(
            rtl::OUString::createFromAscii("Empty mail message reference"),
            static_cast<XSimpleMailClient*>(this),
            1);
 
    // #93077# 
    OSL_ENSURE(!(aFlag & NO_LOGON_DIALOG), "Flag NO_LOGON_DIALOG has currently no effect");
               
    // check the flags, the allowed range is 0 - (2^n - 1)
    if (aFlag < 0 || aFlag > 3)
        throw IllegalArgumentException(
            rtl::OUString::createFromAscii("Invalid flag value"),
            static_cast<XSimpleMailClient*>(this),
            2);

    // check if a recipient is specified of the flags NO_USER_INTERFACE is specified
    if ((aFlag & NO_USER_INTERFACE) && !xSimpleMailMessage->getRecipient().getLength())
        throw IllegalArgumentException(
            rtl::OUString::createFromAscii("No recipient specified"),
            static_cast<XSimpleMailClient*>(this),
            1);
}
