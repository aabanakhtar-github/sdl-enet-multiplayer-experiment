#ifndef CLIENT_EVENT_SYSTEM_H
#define CLIENT_EVENT_SYSTEM_H

#include "ECS.h"
#include "EventHandler.h"
#include "Netcode/Client.h"
#include "SDL.h"
#include "Timer.h"
#include "Util.h"
#include <array>

struct ClientSideClientInfo : ClientInfo {
  ECS::EntityID ecs_ID = -1;
  Vector2 last_position, lerp_position;
};

// Monolithic system that handles inputs and network code in a slightly hackish
// way
class ClientEventSystem : public ECS::ISystem {
  using ClientInfoEx = std::pair<ECS::EntityID, ClientInfo>;
  using ClientRollbackInfo = std::pair<ClientInfo, ClientUpdatePayload>;

public:
  ClientEventSystem() : current_scene_(nullptr) {}
  ~ClientEventSystem() { net_client_.disconnect(3.0); }
  void init(ECS::Scene &scene) override;
  void update(ECS::Scene &scene, float delta) override;

  [[nodiscard]] ECS::EntityID getThisClientID() {
    return network_ID_to_ecs_ID_[net_client_.getID()].ecs_ID;
  }

private:
  void syncGame(const std::string &packet_data);
  void onReceivePacket(const PacketData &packet);
  void predictClientState();
  void interpolateEntities();

  [[nodiscard]] std::uint16_t getKeyboardBits();

private:
  static constexpr float net_tick_rate_ = 60.f;
  NetClient net_client_;
  std::array<ClientSideClientInfo, 10> network_ID_to_ecs_ID_;
  Timer lerp_timer_;
  Timer network_tick_timer_;
  ECS::Scene *current_scene_;
};

#endif
