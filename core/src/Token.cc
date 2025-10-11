#include <autoglue/Token.hh>

#include <cctype>

namespace ag
{

std::wstring_view extractNextToken(std::wstring_view& str)
{
	auto trimmed = trimLeft(str);
	if (trimmed.empty())
	{
		return L"";
	}

	size_t index = 0;
	while (index < trimmed.size() && !std::isspace(trimmed[index]))
	{
		index++;
	}

	auto token = trimmed.substr(0, index);
	str = trimmed.substr(index);
	return token;
}

std::wstring_view extractUntil(std::wstring_view& str, wchar_t ch)
{
	auto trimmed = trimLeft(str);
	if (trimmed.empty())
	{
		return L"";
	}

	size_t index = trimmed.find(ch);
	if (index == std::wstring_view::npos)
	{
		return L"";
	}

	auto token = trimmed.substr(0, index);
	str = trimmed.substr(index + 1);
	return trimRight(token);
}

std::wstring_view trim(std::wstring_view str)
{
	return trimRight(trimLeft(str));
}

std::wstring_view trimLeft(std::wstring_view str)
{
	size_t index = 0;
	while (index < str.size() && std::isspace(str[index]))
	{
		index++;
	}

	return str.substr(index);
}

std::wstring_view trimRight(std::wstring_view str)
{
	if (str.empty())
	{
		return str;
	}

	size_t index = str.size() - 1;
	while (index > 0 && std::isspace(str[index]))
	{
		index--;
	}

	return str.substr(0, index + 1);
}

}
