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



#include <WW8Sttbf.hxx>
#include <resources.hxx>

namespace writerfilter {
namespace doctok
{

WW8Sttbf::WW8Sttbf(WW8Stream & rStream, sal_uInt32 nOffset, sal_uInt32 nCount)
: WW8StructBase(rStream, nOffset, nCount)
{
    sal_uInt32 nComplexOffset = 0;
    if (getU16(0) == 0xffff)
    {
        mbComplex = true;
        nComplexOffset = 2;
    }

    mnCount = getU16(nComplexOffset);
    mnExtraDataCount = getU16(nComplexOffset + 2);

    nOffset = (mbComplex ? 2 : 0) + 4;

    for (sal_uInt32 n = 0; n < mnCount; ++n)
    {
        mEntryOffsets.push_back(nOffset);

        sal_uInt32 nStringLength = getU16(nOffset);
        
        nOffset += 2 + nStringLength * (mbComplex ? 2 : 1);

        mExtraOffsets.push_back(nOffset);

        nOffset += mnExtraDataCount;
    }
}

sal_uInt32 WW8Sttbf::getEntryOffset(sal_uInt32 nPos) const
{
    return mEntryOffsets[nPos];
}

sal_uInt32 WW8Sttbf::getExtraOffset(sal_uInt32 nPos) const
{
    return mExtraOffsets[nPos];
}

sal_uInt32 WW8Sttbf::getEntryCount() const
{
    return mnCount;
}

rtl::OUString WW8Sttbf::getEntry(sal_uInt32 nPos) const
{
    return getString(getEntryOffset(nPos));
}

WW8StructBase::Pointer_t WW8Sttbf::getExtraData(sal_uInt32 nPos)
{
    return WW8StructBase::Pointer_t
        (new WW8StructBase(*this, getExtraOffset(nPos), mnExtraDataCount));
}

WW8SttbTableResource::WW8SttbTableResource(WW8Sttbf::Pointer_t pSttbf)
: mpSttbf(pSttbf)
{
}

WW8SttbTableResource::~WW8SttbTableResource()
{
}

void WW8SttbTableResource::resolve(Table & rTable)
{
    sal_uInt32 nCount = mpSttbf->getEntryCount();
    
    for (sal_uInt32 n = 0; n < nCount; n++)
    {
        WW8StringValue::Pointer_t pVal(new WW8StringValue(mpSttbf->getEntry(n)));
        ::writerfilter::Reference<Properties>::Pointer_t pProps(new WW8StringProperty(0, pVal));
        
        rTable.entry(n, pProps);
    }
}

string WW8SttbTableResource::getType() const
{
    return "WW8SttbTableResource";
}

WW8StringProperty::WW8StringProperty(sal_uInt32 nId, WW8StringValue::Pointer_t pValue)
: mnId(nId), mpValue(pValue)
{
}

WW8StringProperty::~WW8StringProperty()
{
}

void WW8StringProperty::resolve(Properties & rProperties)
{
    rProperties.attribute(mnId, *mpValue);
}

string WW8StringProperty::getType() const
{
    return "WW8StringProperty";
}

sal_uInt32 WW8SttbRgtplc::getEntryCount()
{
    return getU16(2);
}

::writerfilter::Reference<Properties>::Pointer_t 
WW8SttbRgtplc::getEntry(sal_uInt32 nIndex)
{
    ::writerfilter::Reference<Properties>::Pointer_t pResult;
    
    sal_uInt32 nOffset = 6;
    
    while (nIndex > 0)
    {
        sal_uInt16 nCount = getU16(nOffset);
        
        nOffset = nOffset + 2 + nCount;
        ++nIndex;
    }
    
    sal_uInt16 nCount = getU16(nOffset);
    
    if (nCount > 0)
    {
        WW8Tplc * pTplc = new WW8Tplc(*this, nOffset + 2, nCount);
        
        pResult.reset(pTplc);
    }
    
    return pResult;
}

}}
