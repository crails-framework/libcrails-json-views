#ifndef  CRAILS_JSON_RENDERER_HPP
# define CRAILS_JSON_RENDERER_HPP

# include <crails/renderer.hpp>

namespace Crails
{
  class JsonRenderer : public Renderer
  {
  public:
    bool can_render(const std::string& accept_header, const std::string& view) const override;
    void render_template(const std::string& view, RenderTarget& response, SharedVars& vars) const override;
  };
}

#endif
