#include "json_template.hpp"
#include <cmath>

using namespace Crails;

void JsonTemplate::json(std::function<void()> object)
{
  stream << '{';
  object();
  stream << '}';
}

std::string JsonTemplate::apply_post_render_filters(const std::string& data)
{
  if (data[0] != '{' && data[0] != '[') {
    auto pos_comma = data.find(',');
    auto pos_colon = data.find(':');

    if (pos_colon < 0 || (pos_comma >= 0 && pos_comma < pos_colon))
      return '[' + data + ']';
    return '{' + data + '}';
  }
  return data;
}

void JsonTemplate::add_value_with_key(const std::string& key, std::function<void()> callback)
{
  add_separator();
  add_key(key);
  first_item_in_object = true;
  callback();
  first_item_in_object = false;
}

void JsonTemplate::inline_partial(const std::string& view, SharedVars vars)
{
  std::string str = Template::partial(view, vars);

  if (str.find('"') != std::string::npos)
  {
    if (!first_item_in_object)
      stream << ',';
    stream << str.substr(1, str.size() - 2);
    first_item_in_object = false;
  }
}

void JsonTemplate::partial(const std::string& key, const std::string& view, SharedVars vars)
{
  add_separator();
  add_key(key);
  partial(view, vars);
}

void JsonTemplate::partial(const std::string& view, SharedVars vars)
{
  stream << Template::partial(view, vars);
}

void JsonTemplate::json_array(Data value)
{
  stream << '[';
  value.each([this](Data item) -> bool
  {
    if (first_item_in_object == false)
      stream << ',';
    stream << item.to_json();
    first_item_in_object = false;
    return true;
  });
  stream << ']';
}

void JsonTemplate::json_array(const std::string& key, Data value)
{
  add_value_with_key(key, [this, value]() { json_array(value); });
}

std::string JsonTemplate::javascript_escape(const std::string& input) const
{
  std::string output;

  output.reserve(input.length());
  for (std::string::size_type i = 0; i < input.length(); ++i)
  {
    switch (input[i])
    {
      case '"':  output += "\\\""; break;
      case '/':  output += "\\/";  break;
      case '\b': output += "\\b";  break;
      case '\f': output += "\\f";  break;
      case '\n': output += "\\n";  break;
      case '\r': output += "\\r";  break;
      case '\t': output += "\\t";  break;
      case '\\': output += "\\\\"; break;
      default:
        output += input[i];
        break;
    }
  }
  return output;
}

void JsonTemplate::add_separator()
{
  if (!first_item_in_object)
    stream << ',';
  first_item_in_object = false;
}

void JsonTemplate::add_key(const std::string& key)
{
  stream << '"' << javascript_escape(key) << "\":";
}

void JsonTemplate::add_key(unsigned long number)
{
  stream << '"' << number << "\":";
}

void JsonTemplate::add_object(std::function<void()> func)
{
  stream << '{';
  first_item_in_object = true;
  func();
  stream << '}';
  first_item_in_object = false;
}

namespace Crails
{
  template<>
  void JsonTemplate::add_value<const char*>(const char* val)
  {
    stream << '"' << javascript_escape(val) << '"';
  }

  template<>
  void JsonTemplate::add_value<std::string>(const std::string val)
  {
    stream << '"' << javascript_escape(val) << '"';
  }

  template<>
  void JsonTemplate::add_value<float>(float val)
  {
    if (std::isnan(val))
      stream << "null";
    else
      stream << val;
  }

  template<>
  void JsonTemplate::add_value<double>(double val)
  {
    if (std::isnan(val))
      stream << "null";
    else
      stream << val;
  }

  template<>
  void JsonTemplate::json<std::function<void()> >(const std::string& key, const std::function<void()> func)
  {
    add_separator();
    add_key(key);
    add_object(func);
  }

  template<>
  void JsonTemplate::add_value<bool>(bool value)
  {
    if (value)
      stream << "true";
    else
      stream << "false";
  }

  template<>
  void JsonTemplate::add_value<Data>(const Data value)
  {
    value.output(stream);
  }

  template<>
  void JsonTemplate::add_value<char>(char value)
  {
    stream << (short)value;
  }

  template<>
  void JsonTemplate::add_value<unsigned char>(unsigned char value)
  {
    stream << (unsigned short)value;
  }
}
