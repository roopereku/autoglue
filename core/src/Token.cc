#include <autoglue/Token.hh>

#include <cctype>

namespace ag
{

std::string_view extractNextToken(std::string_view& str)
{
	auto trimmed = trimLeft(str);
	if (trimmed.empty())
	{
		return "";
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

std::string_view extractUntil(std::string_view& str, wchar_t ch)
{
	auto trimmed = trimLeft(str);
	if (trimmed.empty())
	{
		return "";
	}

	size_t index = trimmed.find(ch);
	if (index == std::string_view::npos)
	{
		return "";
	}

	auto token = trimmed.substr(0, index);
	str = trimmed.substr(index + 1);
	return trimRight(token);
}

std::string_view trim(std::string_view str)
{
	return trimRight(trimLeft(str));
}

std::string_view trimLeft(std::string_view str)
{
	size_t index = 0;
	while (index < str.size() && std::isspace(str[index]))
	{
		index++;
	}

	return str.substr(index);
}

std::string_view trimRight(std::string_view str)
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
