#include <codecvt>
#include "wstring.h"

namespace bee::fish::server
{
   std::wstring s2ws(const std::string& str)
   {
      std::wstring wstr;
      for (char c : str)
         wstr += (wchar_t)c;
      return wstr;
   }

   std::string ws2s(const std::wstring& wstr)
   {
      std::string str;
      for (wchar_t c : wstr)
      {
         //str += (unsigned char)
         //   ((c & 0xFF00) >> 8);
         str += (unsigned char)
             (c & 0x00FF);
      }
      return str;

   }
}