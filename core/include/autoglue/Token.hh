#ifndef AUTOGLUE_TOKEN_HH
#define AUTOGLUE_TOKEN_HH

#include <string_view>

namespace ag
{

/// Extracts a token from the start until whitespace. Removes trailing whitespace.
///
/// \param str The string to extract from. The extracted portion is removed.
/// \return The extracted portion.
std::wstring_view extractNextToken(std::wstring_view& str);

/// Extracts a token from the start to given character. Removes trailing whitespace.
///
/// \param str The string to extract from. The extracted portion is removed.
/// \param ch The character to extract until.
/// \return The extracted portion.
std::wstring_view extractUntil(std::wstring_view& str, wchar_t ch);

std::wstring_view trim(std::wstring_view str);
std::wstring_view trimLeft(std::wstring_view str);
std::wstring_view trimRight(std::wstring_view str);

}

#endif
