#ifndef CLIENT_EVENT_SYSTEM_H
#define CLIENT_EVENT_SYSTEM_H

#include "ECS.h"
#include "EventHandler.h"
#include "Netcode/Client.h"
#include "Timer.h"
#include <array>

struct ClientSideClientInfo : ClientInfo {
  ECS::EntityID ecs_ID = -1;
  Vector2 last_position, lerp_position;
};

// Monolithic system that handles inputs and network code
class ClientEventSystem : public ECS::ISystem {
  // using ClientInfoEx = std::pair<ECS::EntityID, ClientInfo>;

public:
  explicit ClientEventSystem(const std::string &ip, std::uint16_t port);
  ~ClientEventSystem() { net_client_.disconnect(3.0); }
  void init(ECS::Scene &scene) override;
  void update(ECS::Scene &scene, float delta) override;

  [[nodiscard]] ECS::EntityID getThisClientID() {
    return network_ID_to_ecs_ID_[net_client_.getID()].ecs_ID;
  }

private:
  void syncGame(const std::string &packet_data);
  void onReceivePacket(const PacketData &packet);
  void interpolateEntities();

  [[nodiscard]] std::uint16_t getKeyboardBits();

private:
  static constexpr float net_tick_rate_ = 60.f;
  NetClient net_client_;
  std::array<ClientSideClientInfo, 10> network_ID_to_ecs_ID_;
  Timer lerp_timer_;
  Timer network_tick_timer_;
  ECS::Scene *current_scene_;
  // network config variables
  const std::string preloaded_ip;
  const std::uint16_t preloaded_port;
};

#endif
