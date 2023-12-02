#include "json.hpp"

using namespace Crails;

const std::vector<std::string>& JsonRenderer::get_mimetypes() const
{
  static const std::vector<std::string> mimetypes{
    "application/json", "text/json", "application/feed+json"
  };
  return mimetypes;
}

void JsonRenderer::render_template(const std::string& view, RenderTarget& target, SharedVars& vars) const
{
  auto tpl = templates.find(view);

  target.set_header("Content-Type", "application/json");
  (*tpl).second(*this, target, vars);
}
