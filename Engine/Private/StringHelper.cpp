#include "StringHelper.h"

void CStringHelper::ConvertWideToUTF(const WCHAR* _wstr, char* _Outstr)
{
    int iLength = WideCharToMultiByte(CP_UTF8, 0, _wstr, -1, NULL, 0, NULL, NULL);
    
    memset(_Outstr, 0, sizeof(char) * iLength);
    if (_Outstr)
        WideCharToMultiByte(CP_ACP, 0, _wstr, -1, _Outstr, iLength, NULL, NULL);
}

void CStringHelper::ConvertUTFToWide(const char* _str, WCHAR* _outwstr)
{
    int iLength = MultiByteToWideChar(CP_UTF8, 0, _str, -1, NULL, NULL);

    memset(_outwstr, 0, sizeof(WCHAR) * iLength);
    if (_outwstr)
        MultiByteToWideChar(CP_ACP, 0, _str, -1, _outwstr, iLength);
    
}
