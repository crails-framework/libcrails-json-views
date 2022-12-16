#ifndef  CRAILS_JSON_TEMPLATE_HPP
# define CRAILS_JSON_TEMPLATE_HPP

# include <functional>
# include <sstream>
# include <string>
# include <crails/template.hpp>
# include <crails/utils/string.hpp>
# include <crails/datatree.hpp>

namespace Crails
{
  class JsonTemplate : public Template
  {
  public:
    JsonTemplate(const Renderer& renderer, RenderTarget& target, SharedVars& vars) :
      Template(renderer, target, vars),
      first_item_in_object(true)
    {
    }

    template<typename T>
    void json(const std::string& key, const T val)
    {
      add_separator();
      add_key(key);
      add_value(val);
    }

    void json(std::function<void()> object);

    //
    // BEGIN ARRAYS
    //
    template<typename ARRAY>
    void json_array(ARRAY& array) { json_array(array.begin(), array.end()); }

    template<typename ARRAY, typename T = typename ARRAY::value_type>
    void json_array(ARRAY& array, std::function<void(T)> func)
    {
      json_array<ARRAY, T>(array.begin(), array.end(), func);
    }

    template<typename ITERATOR>
    void json_array(ITERATOR beg, ITERATOR end)
    {
      add_array<ITERATOR>(beg, end, [this](ITERATOR iterator) { add_value(*iterator); });
    }

    template<typename ITERATOR, typename T>
    void json_array(ITERATOR beg, ITERATOR end, std::function<void (T)> func)
    {
      add_array<ITERATOR>(beg, end, [&](ITERATOR iterator)
      {
        add_object([this, func, iterator]() { func(*iterator); });
      });
    }

    template<typename ITERATOR>
    void json_array(ITERATOR beg, ITERATOR end, const std::string& partial_view, std::string var_key = "")
    {
      add_array<ITERATOR>(beg, end, [&](ITERATOR iterator)
      {
        partial(partial_view, {{var_key, &(*iterator)}});
      });
    }

    void json_array(Data value);

    template<typename ARRAY>
    void json_array(const std::string& key, ARRAY& array) { json_array(key, array.begin(), array.end()); }

    template<typename ARRAY, typename T = typename ARRAY::value_type>
    void json_array(const std::string& key, ARRAY& array, std::function<void (T)> func) { json_array(key, array.begin(), array.end(), func); }

    template<typename ITERATOR>
    void json_array(const std::string& key, ITERATOR beg, ITERATOR end)
    {
      add_value_with_key(key, std::bind(&JsonTemplate::json_array<ITERATOR>, this, beg, end));
    }

    template<typename ITERATOR, typename T>
    void json_array(const std::string& key,
                    ITERATOR beg,
                    ITERATOR end,
                    std::function<void (T)> func)
    {
      add_value_with_key(key, [&]() { json_array<ITERATOR, T>(beg, end, func); });
                              //std::bind(&JsonTemplate::json_array<ITERATOR, T>, this, beg, end, func));
    }

    template<typename ITERATOR>
    void json_array(const std::string& key,
                    ITERATOR beg,
                    ITERATOR end,
                    const std::string& partial_view,
                    std::string var_key = "")
    {
      if (var_key == "")
        var_key = singularize(key);
      add_value_with_key(key, std::bind(&JsonTemplate::json_array<ITERATOR>, this, beg, end, partial_view, var_key));
    }

    void json_array(const std::string& key, Data value);
    //
    // END ARRAYS
    //

    //
    // BEGIN MAPS
    //
    template<typename MAP>
    void json_map(const MAP& map)
    {
      stream << '{';
      for (auto item : map)
      {
        add_separator();
        add_key(item.first);
        add_value(item.second);
      }
      stream << '}';
    }

    template<typename MAP>
    void json_map(const MAP& map, std::function<void (typename MAP::mapped_type)> functor, bool use_braces = true)
    {
      stream << '{';
      for (auto item : map)
      {
        add_separator();
        add_key(item.first);
        if (use_braces)
          add_object([this, functor, item]() { functor(item.second); });
        else
          functor(item.second);
      }
      stream << '}';
    }

    template<typename MAP>
    void json_map(const MAP& map, const std::string& partial_view, std::string& var_key)
    {
      stream << '{';
      for (auto item : map)
      {
        add_separator();
        add_key(item.first);
        partial(partial_view, { {var_key, &(item.second)} });
      }
      stream << '}';
    }

    template<typename MAP>
    void json_map(const std::string& key, const MAP& map)
    {
      add_value_with_key(key, [&]() { json_map<MAP>(map); });
                            //std::bind(&JsonTemplate::json_map<MAP>, this, map));
    }

    template<typename MAP>
    void json_map(const std::string& key, const MAP& map, std::function<void (typename MAP::mapped_type)> functor, bool use_braces = true)
    {
      add_value_with_key(key, std::bind(&JsonTemplate::json_map<MAP>, this, map, functor, use_braces));
    }

    template<typename MAP>
    void json_map(const std::string& key, const MAP& map, const std::string& partial_view, std::string var_key = "")
    {
      if (var_key == "")
        var_key = singularize(key);
      add_value_with_key(key, std::bind(&JsonTemplate::json_map<MAP>, this, map, partial_view, var_key));
    }
    //
    // END MAPS
    //

    void partial(const std::string& view, SharedVars vars = {});
    void partial(const std::string& key, const std::string& view, SharedVars vars = {});
    void inline_partial(const std::string& view, SharedVars vars = {});

  protected:
    std::stringstream stream;

  private:
    template<typename T>
    void        add_value(const T val)
    {
      stream << val;
    }

    template<typename ITERATOR>
    void        add_array(ITERATOR beg, ITERATOR end, std::function<void(ITERATOR)> renderer)
    {
      stream << '[';
      while (beg != end)
      {
        add_separator();
        renderer(beg);
        ++beg;
      }
      stream << ']';
    }

    void        add_value_with_key(const std::string& key, std::function<void()> callback);
    void        add_separator();
    void        add_key(const std::string& key);
    void        add_key(unsigned long number);
    void        add_object(std::function<void()> func);
    std::string javascript_escape(const std::string& val) const;

    bool first_item_in_object;
  };
  template<> void JsonTemplate::json<std::function<void()> >(const std::string& key, const std::function<void()> func);
  template<> void JsonTemplate::add_value<bool>(bool value);
  template<> void JsonTemplate::add_value<char>(char value);
  template<> void JsonTemplate::add_value<unsigned char>(unsigned char value);
  template<> void JsonTemplate::add_value<float>(float value);
  template<> void JsonTemplate::add_value<double>(double value);
  template<> void JsonTemplate::add_value<const char*>(const char* val);
  template<> void JsonTemplate::add_value<std::string>(const std::string val);
  template<> void JsonTemplate::add_value<Data>(const Data value);
}

#endif
