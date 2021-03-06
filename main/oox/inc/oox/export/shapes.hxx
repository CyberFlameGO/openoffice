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



#ifndef _OOX_EXPORT_SHAPES_HXX_
#define _OOX_EXPORT_SHAPES_HXX_

#include <oox/dllapi.h>
#include <com/sun/star/uno/XReference.hpp>
#include <oox/export/drawingml.hxx>
#include <sax/fshelper.hxx>
#include <vcl/mapmod.hxx>
#include <hash_map>

namespace com { namespace sun { namespace star {
namespace beans {
    class XPropertySet;
}
namespace drawing {
    class XShape;
    class XShapes;
}
}}}

namespace oox { namespace drawingml {

class OOX_DLLPUBLIC ShapeExport : public DrawingML {
private:
    sal_Int32           mnXmlNamespace;
    sal_Int32           mnShapeIdMax, mnPictureIdMax;
    Fraction            maFraction;
    MapMode             maMapModeSrc, maMapModeDest;

    ::com::sun::star::awt::Size MapSize( const ::com::sun::star::awt::Size& ) const;

    struct ShapeCheck
    {
        bool operator()( const ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape> s1, const ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape> s2 ) const
        {
            return s1 == s2;
        }
    };

    struct ShapeHash
    {
        std::hash<const char*> maHashFunction;

        size_t operator()( const ::com::sun::star::uno::Reference < ::com::sun::star::drawing::XShape > ) const;
    };

    typedef std::hash_map< const ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape>, sal_Int32, ShapeHash, ShapeCheck> ShapeHashMap;
    ShapeHashMap maShapeMap;

public:
    ShapeExport( sal_Int32 nXmlNamespace, ::sax_fastparser::FSHelperPtr pFS, ::oox::core::XmlFilterBase* pFB = NULL, DocumentType eDocumentType = DOCUMENT_PPTX );
    virtual ~ShapeExport() {}

    sal_Int32           GetXmlNamespace() const;
    ShapeExport&        SetXmlNamespace( sal_Int32 nXmlNamespace );

    static sal_Bool     NonEmptyText( ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape > xShape );

    virtual ShapeExport&
                        WriteBezierShape( ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape > xShape, sal_Bool bClosed );
    virtual ShapeExport&
                        WriteClosedBezierShape( ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape > xShape );
    virtual ShapeExport&
                        WriteConnectorShape( ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape > xShape );
    virtual ShapeExport&
                        WriteCustomShape( ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape > xShape );
    virtual ShapeExport&
                        WriteEllipseShape( ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape > xShape );
    virtual ShapeExport&
                        WriteFill( ::com::sun::star::uno::Reference< ::com::sun::star::beans::XPropertySet > xPropSet );
    virtual ShapeExport&
                        WriteGraphicObjectShape( ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape > xShape );
    virtual ShapeExport&
                        WriteLineShape( ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape > xShape );
    virtual ShapeExport&
                        WriteNonVisualDrawingProperties( ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape > xShape, const char* sName );
    virtual ShapeExport&
                        WriteNonVisualProperties( ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape > xShape );
    virtual ShapeExport&
                        WriteOpenBezierShape( ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape > xShape );
    virtual ShapeExport&
                        WriteRectangleShape( ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape > xShape );

    /**
     * Write the DrawingML for a particular shape.
     *
     * <p>This is the member function you want.  It performs the type lookup and
     * invokes the appropriate corresponding Write*() method for the specific
     * type.</p>
     *
     * <p>To write an XShape, XShape::getShapeType() is called to determine
     * the shape type, and the corresponding method in this table is
     * invoked:</p>
     *
     * <table>
     *   <tr><th>Shape Type</th><th>Method</th></tr>
     *   <tr><td><tt>com.sun.star.drawing.ClosedBezierShape</tt></td>    <td>ShapeExport::WriteClosedBezierShape</td></tr>
     *   <tr><td><tt>com.sun.star.drawing.CustomShape</tt></td>          <td>ShapeExport::WriteCustomShape</td></tr>
     *   <tr><td><tt>com.sun.star.drawing.EllipseShape</tt></td>         <td>ShapeExport::WriteEllipseShape</td></tr>
     *   <tr><td><tt>com.sun.star.drawing.GraphicObjectShape</tt></td>   <td>ShapeExport::WriteGraphicObjectShape</td></tr>
     *   <tr><td><tt>com.sun.star.drawing.LineShape</tt></td>            <td>ShapeExport::WriteLineShape</td></tr>
     *   <tr><td><tt>com.sun.star.drawing.OpenBezierShape</tt></td>      <td>ShapeExport::WriteOpenBezierShape</td></tr>
     *   <tr><td><tt>com.sun.star.drawing.RectangleShape</tt></td>       <td>ShapeExport::WriteRectangleShape</td></tr>
     *   <tr><td><tt>com.sun.star.drawing.TextShape</tt></td>            <td>ShapeExport::WriteTextShape</td></tr>
     *   <tr><td><tt>com.sun.star.presentation.DateTimeShape</tt></td>   <td>ShapeExport::WriteTextShape</td></tr>
     *   <tr><td><tt>com.sun.star.presentation.FooterShape</tt></td>     <td>ShapeExport::WriteTextShape</td></tr>
     *   <tr><td><tt>com.sun.star.presentation.HeaderShape</tt></td>     <td>ShapeExport::WriteTextShape</td></tr>
     *   <tr><td><tt>com.sun.star.presentation.NotesShape</tt></td>      <td>ShapeExport::WriteTextShape</td></tr>
     *   <tr><td><tt>com.sun.star.presentation.OutlinerShape</tt></td>   <td>ShapeExport::WriteTextShape</td></tr>
     *   <tr><td><tt>com.sun.star.presentation.SlideNumberShape</tt></td><td>ShapeExport::WriteTextShape</td></tr>
     *   <tr><td><tt>com.sun.star.presentation.TitleTextShape</tt></td>  <td>ShapeExport::WriteTextShape</td></tr>
     * </table>
     *
     * <p>If the shape type is not recognized, then
     * <tt>ShapeExport::WriteUnknownShape</tt> is called.</p>
     *
     * @param xShape    The shape to export as DrawingML.
     * @return   <tt>*this</tt>
     */
    virtual ShapeExport&
                        WriteShape( ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape > xShape );
    virtual ShapeExport&
                        WriteTextBox( ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape > xShape );
    virtual ShapeExport&
                        WriteTextShape( ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape > xShape );
    virtual ShapeExport&
                        WriteUnknownShape( ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape > xShape );

    sal_Int32 GetNewShapeID( const ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape > rShape );
    sal_Int32 GetShapeID( const ::com::sun::star::uno::Reference< ::com::sun::star::drawing::XShape > rShape );
};

}}

#endif /* ndef _OOX_EXPORT_SHAPES_HXX_ */
