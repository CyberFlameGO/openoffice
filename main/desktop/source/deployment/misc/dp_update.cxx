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
#include "precompiled_desktop.hxx"


#include "dp_update.hxx"
#include "dp_version.hxx"
#include "dp_identifier.hxx"
#include "dp_descriptioninfoset.hxx"

#include "rtl/bootstrap.hxx"

using namespace ::com::sun::star;
using namespace ::com::sun::star::uno;
using ::rtl::OUString;
using ::rtl::OString;


namespace dp_misc {
namespace {

int determineHighestVersion(
    ::rtl::OUString const & userVersion,
    ::rtl::OUString const & sharedVersion,
    ::rtl::OUString const & bundledVersion,
    ::rtl::OUString const & onlineVersion)
{
    int index = 0;
    OUString  greatest = userVersion;
    if (dp_misc::compareVersions(sharedVersion, greatest) == dp_misc::GREATER)
    {
        index = 1;
        greatest = sharedVersion;
    }
    if (dp_misc::compareVersions(bundledVersion, greatest) == dp_misc::GREATER)
    {
        index = 2;
        greatest = bundledVersion;
    }
    if (dp_misc::compareVersions(onlineVersion, greatest) == dp_misc::GREATER)
    {
        index = 3;
    }
    return index;
}

Sequence< Reference< xml::dom::XElement > >
getUpdateInformation( Reference<deployment::XUpdateInformationProvider > const & updateInformation,
                      Sequence< OUString > const & urls,
                      OUString const & identifier,
                      uno::Any & out_error)
{
    try {
        return updateInformation->getUpdateInformation(urls, identifier);
    } catch (uno::RuntimeException &) {
        throw;
    } catch (ucb::CommandFailedException & e) {
        out_error = e.Reason;
    } catch (ucb::CommandAbortedException &) {
    } catch (uno::Exception & e) {
        out_error = uno::makeAny(e);
    }
    return
        Sequence<Reference< xml::dom::XElement > >();
}

void getOwnUpdateInfos(
        Reference<uno::XComponentContext> const & xContext,
        Reference<deployment::XUpdateInformationProvider > const &  updateInformation,
        UpdateInfoMap& inout_map, std::vector<std::pair<Reference<deployment::XPackage>, uno::Any> > & out_errors,
        bool & out_allFound)
{
    bool allHaveOwnUpdateInformation = true;
    for (UpdateInfoMap::iterator i = inout_map.begin(); i != inout_map.end(); i++)
    {
        OSL_ASSERT(i->second.extension.is());
        Sequence<OUString> urls(i->second.extension->getUpdateInformationURLs());
        if (urls.getLength())
        {
            const OUString id =  dp_misc::getIdentifier(i->second.extension);
            uno::Any anyError;
            //It is unclear from the idl if there can be a null reference returned.
            //However all valid information should be the same
            Sequence<Reference< xml::dom::XElement > >
                infos(getUpdateInformation(updateInformation, urls, id, anyError));
            if (anyError.hasValue())
                out_errors.push_back(std::make_pair(i->second.extension, anyError));
            
            for (sal_Int32 j = 0; j < infos.getLength(); ++j)
            {
                dp_misc::DescriptionInfoset infoset(
                    xContext,
                    Reference< xml::dom::XNode >(infos[j], UNO_QUERY_THROW));
                if (!infoset.hasDescription())
                    continue;
                boost::optional< OUString > id2(infoset.getIdentifier());
                if (!id2) 
                    continue;
                OSL_ASSERT(*id2 == id);
                if (*id2 == id)
                {
                    i->second.version = infoset.getVersion();
                    i->second.info = Reference< xml::dom::XNode >(
                        infos[j], UNO_QUERY_THROW);
                }
                break;
            }
        }
        else
        {
            allHaveOwnUpdateInformation &= false;
        }
    }
    out_allFound = allHaveOwnUpdateInformation;
}

void getDefaultUpdateInfos(
    Reference<uno::XComponentContext> const & xContext,
    Reference<deployment::XUpdateInformationProvider > const &  updateInformation,
    UpdateInfoMap& inout_map,
     std::vector<std::pair<Reference<deployment::XPackage>, uno::Any> > & out_errors)
{
    const rtl::OUString sDefaultURL(dp_misc::getExtensionDefaultUpdateURL());
    OSL_ASSERT(sDefaultURL.getLength());

    Any anyError;
    Sequence< Reference< xml::dom::XElement > >
        infos(
            getUpdateInformation(
                updateInformation, 
                Sequence< OUString >(&sDefaultURL, 1), OUString(), anyError));
    if (anyError.hasValue())
        out_errors.push_back(std::make_pair(Reference<deployment::XPackage>(), anyError));
    for (sal_Int32 i = 0; i < infos.getLength(); ++i)
    {
        Reference< xml::dom::XNode > node(infos[i], UNO_QUERY_THROW);
        dp_misc::DescriptionInfoset infoset(xContext, node);
        boost::optional< OUString > id(infoset.getIdentifier());
        if (!id) {
            continue;
        }
        UpdateInfoMap::iterator j = inout_map.find(*id);
        if (j != inout_map.end())
        {
            //skip those extension which provide its own update urls
            if (j->second.extension->getUpdateInformationURLs().getLength())
                continue;
            OUString v(infoset.getVersion());
            //look for the highest version in the online repository
            if (dp_misc::compareVersions(v, j->second.version) ==
                dp_misc::GREATER)
            {
                j->second.version = v;
                j->second.info = node;
            }
        }
    }
}

bool containsBundledOnly(Sequence<Reference<deployment::XPackage> > const & sameIdExtensions)
{
    OSL_ASSERT(sameIdExtensions.getLength() == 3);
    if (!sameIdExtensions[0].is() && !sameIdExtensions[1].is() && sameIdExtensions[2].is())
        return true;
    else
        return false;
}
/** Returns true if the list of extensions are bundled extensions and there are no
    other extensions with the same identifier in the shared or user repository.
    If extensionList is NULL, then it is checked if there are only bundled extensions.
*/
bool onlyBundledExtensions(
    Reference<deployment::XExtensionManager> const & xExtMgr,
    std::vector< Reference<deployment::XPackage > > const * extensionList)
{
    OSL_ASSERT(xExtMgr.is());
    bool onlyBundled = true;
    if (extensionList)
    {
        typedef std::vector<Reference<deployment::XPackage > >::const_iterator CIT;
        for (CIT i = extensionList->begin(); i != extensionList->end(); i++)
        {
            Sequence<Reference<deployment::XPackage> > seqExt = xExtMgr->getExtensionsWithSameIdentifier(
                dp_misc::getIdentifier(*i), (*i)->getName(), Reference<ucb::XCommandEnvironment>());

            if (!containsBundledOnly(seqExt))
            {
                onlyBundled = false;
                break;
            }
            
        }
    }
    else
    {
        const uno::Sequence< uno::Sequence< Reference<deployment::XPackage > > > seqAllExt =
            xExtMgr->getAllExtensions(Reference<task::XAbortChannel>(), Reference<ucb::XCommandEnvironment>());
        
        for (int pos = seqAllExt.getLength(); pos --; )
        {
            if (!containsBundledOnly(seqAllExt[pos]))
            {
                onlyBundled = false;
                break;
            }
        }
    }
    return onlyBundled;
}

} // anon namespace


OUString getExtensionDefaultUpdateURL()
{
    ::rtl::OUString sUrl(
        RTL_CONSTASCII_USTRINGPARAM(
        "${$BRAND_BASE_DIR/program/" SAL_CONFIGFILE("version")
        ":Version:ExtensionUpdateURL}"));
    ::rtl::Bootstrap::expandMacros(sUrl);
    return sUrl;
}

/* returns the index of the greatest version, starting with 0

 */
UPDATE_SOURCE isUpdateUserExtension(
    bool bReadOnlyShared,
    ::rtl::OUString const & userVersion,
    ::rtl::OUString const & sharedVersion,
    ::rtl::OUString const & bundledVersion,
    ::rtl::OUString const & onlineVersion)
{
    UPDATE_SOURCE retVal = UPDATE_SOURCE_NONE;
    if (bReadOnlyShared)
    {
        if (userVersion.getLength())
        {
            int index = determineHighestVersion(
                userVersion, sharedVersion, bundledVersion, onlineVersion);
            if (index == 1)
                retVal = UPDATE_SOURCE_SHARED;
            else if (index == 2)
                retVal = UPDATE_SOURCE_BUNDLED;
            else if (index == 3)
                retVal = UPDATE_SOURCE_ONLINE;
        }
        else if (sharedVersion.getLength())
        {
            int index = determineHighestVersion(
                OUString(), sharedVersion, bundledVersion, onlineVersion);
            if (index == 2)
                retVal = UPDATE_SOURCE_BUNDLED;
            else if (index == 3)
                retVal = UPDATE_SOURCE_ONLINE;
            
        }
        //No update for bundled extensions, they are updated only by the setup
        //else if (bundledVersion.getLength())
        //{
        //    int index = determineHighestVersion(
        //        OUString(), OUString(), bundledVersion, onlineVersion);
        //    if (index == 3)
        //        retVal = UPDATE_SOURCE_ONLINE;
        //}
    }
    else
    {
        if (userVersion.getLength())
        {
            int index = determineHighestVersion(
                userVersion, sharedVersion, bundledVersion, onlineVersion);
            if (index == 1)
                retVal = UPDATE_SOURCE_SHARED;
            else if (index == 2)
                retVal = UPDATE_SOURCE_BUNDLED;
            else if (index == 3)
                retVal = UPDATE_SOURCE_ONLINE;
        }
    }
        
    return retVal;
}

UPDATE_SOURCE isUpdateSharedExtension(
    bool bReadOnlyShared,
    ::rtl::OUString const & sharedVersion,
    ::rtl::OUString const & bundledVersion,
    ::rtl::OUString const & onlineVersion)
{
    if (bReadOnlyShared)
        return UPDATE_SOURCE_NONE;
    UPDATE_SOURCE retVal = UPDATE_SOURCE_NONE;
    
    if (sharedVersion.getLength())
    {
        int index = determineHighestVersion(
            OUString(), sharedVersion, bundledVersion, onlineVersion);
        if (index == 2)
            retVal = UPDATE_SOURCE_BUNDLED;
        else if (index == 3)
            retVal = UPDATE_SOURCE_ONLINE;
    }
    //No update for bundled extensions, they are updated only by the setup
    //else if (bundledVersion.getLength())
    //{
    //    int index = determineHighestVersion(
    //        OUString(), OUString(), bundledVersion, onlineVersion);
    //    if (index == 3)
    //        retVal = UPDATE_SOURCE_ONLINE;
    //}
    return retVal;
}

Reference<deployment::XPackage>
getExtensionWithHighestVersion(
    Sequence<Reference<deployment::XPackage> > const & seqExt)
{
    if (seqExt.getLength() == 0)
        return Reference<deployment::XPackage>();

    Reference<deployment::XPackage> greatest;
    sal_Int32 len = seqExt.getLength();

    for (sal_Int32 i = 0; i < len; i++)
    {
        if (!greatest.is())
        {
            greatest = seqExt[i];
            continue;
        }
        Reference<deployment::XPackage> const & current = seqExt[i];
        //greatest has a value
        if (! current.is())
            continue;
        
        if (dp_misc::compareVersions(current->getVersion(), greatest->getVersion()) == dp_misc::GREATER)
            greatest = current;   
    }
    return greatest;
}

UpdateInfo::UpdateInfo( Reference< deployment::XPackage> const & ext):
extension(ext)
{
}



UpdateInfoMap getOnlineUpdateInfos(
    Reference<uno::XComponentContext> const &xContext, 
    Reference<deployment::XExtensionManager> const & xExtMgr,
    Reference<deployment::XUpdateInformationProvider > const & updateInformation,
    std::vector<Reference<deployment::XPackage > > const * extensionList,
    std::vector<std::pair< Reference<deployment::XPackage>, uno::Any> > & out_errors)
{
    OSL_ASSERT(xExtMgr.is());
    UpdateInfoMap infoMap;
    if (!xExtMgr.is() || onlyBundledExtensions(xExtMgr, extensionList))
        return infoMap;

    if (!extensionList)
    {
        const uno::Sequence< uno::Sequence< Reference<deployment::XPackage > > > seqAllExt =  xExtMgr->getAllExtensions(
            Reference<task::XAbortChannel>(), Reference<ucb::XCommandEnvironment>());
        
        //fill the UpdateInfoMap. key = extension identifier, value = UpdateInfo
        for (int pos = seqAllExt.getLength(); pos --; )
        {
            uno::Sequence<Reference<deployment::XPackage> > const &   seqExt = seqAllExt[pos];
            
            Reference<deployment::XPackage> extension = getExtensionWithHighestVersion(seqExt);
            OSL_ASSERT(extension.is());
            
            std::pair<UpdateInfoMap::iterator, bool> insertRet = infoMap.insert(
                UpdateInfoMap::value_type(
                    dp_misc::getIdentifier(extension), UpdateInfo(extension)));
            OSL_ASSERT(insertRet.second == true);
        }
    }
    else
    {
        typedef std::vector<Reference<deployment::XPackage > >::const_iterator CIT;
        for (CIT i = extensionList->begin(); i != extensionList->end(); i++)
        {
            OSL_ASSERT(i->is());
            std::pair<UpdateInfoMap::iterator, bool> insertRet = infoMap.insert(
                UpdateInfoMap::value_type(
                    dp_misc::getIdentifier(*i), UpdateInfo(*i)));
            OSL_ASSERT(insertRet.second == true);            
        }
    }

    //Now find the update information for the extensions which provide their own
    //URLs to update information.
    bool allInfosObtained = false;
    getOwnUpdateInfos(xContext, updateInformation, infoMap, out_errors, allInfosObtained);

    if (!allInfosObtained)
        getDefaultUpdateInfos(xContext, updateInformation, infoMap, out_errors);
    return infoMap;
}
OUString getHighestVersion(
    ::rtl::OUString const & userVersion,
    ::rtl::OUString const & sharedVersion,
    ::rtl::OUString const & bundledVersion,
    ::rtl::OUString const & onlineVersion)
{
    int index = determineHighestVersion(userVersion, sharedVersion, bundledVersion, onlineVersion);
    switch (index)
    {
    case 0: return userVersion;
    case 1: return sharedVersion;
    case 2: return bundledVersion;
    case 3: return onlineVersion;
    default: OSL_ASSERT(0);
    }
        
    return OUString();
}
} //namespace dp_misc
