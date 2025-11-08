#include "XmlParser.h"

#include "HUDLayer.h"

void CXmlParser::Load_From_XML(XML::XMLElement* element)
{
}

void CXmlParser::Save_To_XML(XML::XMLDocument& doc, XML_DESC* pDesc, XML::XMLElement* parent)
{
    XML::XMLElement* elem = doc.NewElement(pDesc->szEltag.c_str());

    if (pDesc->szText != "")
        elem->SetText(pDesc->szText.c_str());

    // 加己 历厘
    for (auto& iter : pDesc->attr)
    {
        elem->SetAttribute(iter.first.c_str(), iter.second.c_str());
    }

    //// 磊侥 历厘
    //for (auto* child : children)
    //    child->SaveToXML(doc, elem);

    if (parent)
        parent->InsertEndChild(elem);
    else
        doc.InsertFirstChild(elem);
}
