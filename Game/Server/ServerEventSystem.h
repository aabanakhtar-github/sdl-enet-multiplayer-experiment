#ifndef SERVER_EVENT_SYSTEM_H
#define SERVER_EVENT_SYSTEM_H

#include "ECS.h"
#include "Netcode/Server.h"
#include "Timer.h"
#include <array>
#include <string>

class ServerEventSystem : public ECS::ISystem {
public:
  explicit ServerEventSystem(std::uint16_t port);     
  ServerEventSystem() 
    : current_scene_(nullptr), net_server_(), preloaded_port_() {}
  ~ServerEventSystem();

  void init(ECS::Scene &scene) override;
  void update(ECS::Scene &scene, float delta) override;

  [[nodiscard]] const std::array<ECS::EntityID, 10> &
  getClientToECS_IDArray() const {
    return client_to_ecs_ID_;
  }

public:
  // std::pair is <anim_state, facing_left>
  std::array<std::pair<std::string, bool>, 10> anim_states;

private:
  void onRecievePacket(const PacketData &packet);
  void setupServer();
private:
  static constexpr float net_tick_rate_ = 60.f;
  static constexpr float player_x_speed_ = 300.f;
  static constexpr float player_jump_speed_ = 500.f;
  ECS::Scene *current_scene_;
  Timer net_tick_timer_;
  NetServer net_server_;
  std::array<ECS::EntityID, 10> client_to_ecs_ID_;
  // Network config variables
  const std::uint16_t preloaded_port_;  
};

#endif
