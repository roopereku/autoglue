#ifndef AUTOGLUE_TOKEN_HH
#define AUTOGLUE_TOKEN_HH

#include <string_view>

namespace ag
{

/// Extracts a token from the start until whitespace. Removes trailing whitespace.
///
/// \param str The string to extract from. The extracted portion is removed.
/// \return The extracted portion.
std::string_view extractNextToken(std::string_view& str);

/// Extracts a token from the start to given character. Removes trailing whitespace.
///
/// \param str The string to extract from. The extracted portion is removed.
/// \param ch The character to extract until.
/// \return The extracted portion.
std::string_view extractUntil(std::string_view& str, wchar_t ch);

/// Extracts a token from the start to given character. If the character is not found,
/// the remainder is returned. Removes trailing whitespace.
///
/// \param str The string to extract from. The extracted portion is removed.
/// \param ch The character to extract until.
/// \param ch Output flag determining whether the delimiter was found.
/// \return The extracted portion.
std::string_view extractUntilOrNextToken(std::string_view& str, wchar_t ch, bool& foundDelimiter);

std::string_view trim(std::string_view str);
std::string_view trimLeft(std::string_view str);
std::string_view trimRight(std::string_view str);

}

#endif
