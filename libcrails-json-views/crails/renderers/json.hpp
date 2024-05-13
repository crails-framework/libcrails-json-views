#ifndef  CRAILS_JSON_RENDERER_HPP
# define CRAILS_JSON_RENDERER_HPP

# include <crails/renderer.hpp>

namespace Crails
{
  class JsonRenderer : public Renderer
  {
  public:
    std::string_view get_name() const override { return "crails/json"; }
    const std::vector<std::string>& get_mimetypes() const override;
    void render_template(const std::string& view, RenderTarget& response, SharedVars& vars) const override;
  };
}

#endif
