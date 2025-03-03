#ifndef POST_MANAGER_HPP
#define POST_MANAGER_HPP

#include "../Models/post_storage.hpp"
#include <userver/components/component_fwd.hpp>

namespace post_service {
class PostManager final : public userver::components::ComponentBase {
public:
  static constexpr std::string_view kName = "post-manager";
  PostManager(const userver::components::ComponentConfig &config,
              const userver::components::ComponentContext &context);
  bool CreatePost(int user_id, const std::string &content);
  bool DeletePost(int post_id);
  bool EditPost(int post_id, const std::string &edited_content);
  std::optional<post_service::models::Post> FindPostById(int post_id);

private:
  post_service::PostStorage &PostStorage_;
};
} // namespace post_service

#endif

