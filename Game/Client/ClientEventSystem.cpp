#include "ClientEventSystem.h"
#include "Components.h"
#include "CreateScenes.h"
#include <cstdint>
#include <iostream>

// TODO left:
// - Client server view synchronization (Clients see same as server) /
// correction
// - Client inputs sent to server
// - Server response to client input
// - Interpolation (depends)

void ClientEventSystem::init(ECS::Scene &scene) {
  net_client_ = NetClient([this](const PacketData &packet) -> void {
    this->onReceivePacket(packet);
  });

  if (!net_client_.getValid()) {
    GlobalAppState::get().setAppState(AppState::AS_FAIL,
                                      "Cannot initialize network!");
  }

  // TODO refactor into reusable function that allows game to persist across
  // multiple servers
  net_client_.connectTo("127.0.0.1", 7777, 3.0);

  EventHandler::get().bindEvent(SDL_KEYDOWN, [&, this](SDL_Event &e) -> void {
    switch (e.key.keysym.sym) {
    case SDLK_SPACE: {
      if (!net_client_.getConnected()) {
        return;
      }

      PacketData packet;
      packet.type = PT_CLIENT_JUMP;
      packet.data_size = 0;
      net_client_.sendPacket(packet, 0, true);
      break;
    }
    default:
      break;
    }
  });

  EventHandler::get().bindEvent(SDL_QUIT, [&](SDL_Event &e) -> void {
    GlobalAppState::get().setAppState(AppState::AS_QUIT);
  });

  // Create Players
  for (auto &other_peer : network_ID_to_ecs_ID_) {
    other_peer.ecs_ID = makeEntity(scene, Proto::PLAYER, Vector2(0, 0));
    scene.setEntityActive(other_peer.ecs_ID, false);
  }
}

#if false
void ClientEventSystem::Update(ECS::Scene& scene, float delta)
{
    auto& component = scene.GetComponent<PhysicsBodyComponent>(0);
    
    input_packet.DataLength = output.str().size() + 1; 
    input_packet.Data = output.str(); 

    EventHandler::get().Update();

    if (m_NetClient.getConnected()) 
    { 
        m_NetClient.UpdateNetwork();
        m_NetClient.SendPacket(input_packet, 0, false);  
    }
}
#endif

void ClientEventSystem::update(ECS::Scene &scene, float delta) {
  EventHandler::get().update();
  current_scene_ = &scene;

  std::uint16_t inputs = getKeyboardBits();
  // Predict Client Movement
  predictClientState();
  interpolateEntities();

  if (net_client_.getConnected() &&
      network_tick_timer_.getDelta() > 1.0f / net_tick_rate_) {
    net_client_.updateNetwork();
    PacketData packet;
    packet.type = PT_GAME_UPDATE;

    ClientUpdatePayload payload;
    payload.input_bits = inputs;
    packet.data = payloadToString<ClientUpdatePayload>(payload);

    packet.data_size = packet.data.size() + 1;
    net_client_.sendPacket(packet, 0, false);
  }
}

void ClientEventSystem::syncGame(const std::string &packet_data) {
  if (current_scene_ == nullptr) {
    return;
  }

  ECS::Scene &scene = *current_scene_;
  auto payload = payloadFromString<ServerUpdatePayload>(packet_data);
  // list of connected clients which will be filled by the server update
  std::vector<std::size_t> connected_list;

  for (auto &client : payload.client_states) {
    // save the ID for removing clients later
    connected_list.push_back(client.network_ID);

    ECS::EntityID client_ecs_ID =
        network_ID_to_ecs_ID_[client.network_ID].ecs_ID;
    // update their state
    auto &physics_component =
        scene.getComponent<PhysicsBodyComponent>(client_ecs_ID);
    auto &anim_component =
        scene.getComponent<AnimationStateMachineComponent>(client_ecs_ID);
    auto &texture_component =
        scene.getComponent<TextureComponent>(client_ecs_ID);
    auto &player_component = scene.getComponent<PlayerComponent>(client_ecs_ID);

    auto &view = network_ID_to_ecs_ID_[client.network_ID];

    // update their position
    view.last_position = {static_cast<float>(physics_component.AABB.x),
                          static_cast<float>(physics_component.AABB.y)};
    view.lerp_position = client.position;

    // update the animation
    anim_component.state = client.anim_state;
    player_component.facing_left = client.facing_left;
    // make sure it is active because we know it is connected now
    scene.setEntityActive(client_ecs_ID);
  }

  for (std::size_t i = 0; i < network_ID_to_ecs_ID_.size(); ++i) {
    // is it in the connected list? if not, make the entity inactive again
    auto it = std::find(connected_list.begin(), connected_list.end(), i);

    if (it == connected_list.end()) {
      // make this client disappear and reset their position
      ECS::EntityID client_ecs_ID = network_ID_to_ecs_ID_[i].ecs_ID;
      scene.setEntityActive(client_ecs_ID, false);
      auto &component = scene.getComponent<PhysicsBodyComponent>(client_ecs_ID);
      component = PhysicsBodyComponent{.AABB = component.AABB};
    }
  }
}

void ClientEventSystem::onReceivePacket(const PacketData &packet) {
#pragma clang diagnostic push // Linter is annoying
#pragma clang diagnostic ignored "-Wswitch"
  switch (packet.type) {
  case PT_GAME_UPDATE:
    // re-sync / reconcile / interpolate
    lerp_timer_.reset(); // re start the lerp timer from 0 so that it's smooth
    syncGame(packet.data);
    break;
  }
#pragma clang diagnostic pop
}

void ClientEventSystem::predictClientState() {
  // TODO: implement when have time
}

std::uint16_t ClientEventSystem::
    getKeyboardBits() /* NOLINT(*-convert-member-functions-to-static) */ {
  const std::uint8_t *keyboard_state = SDL_GetKeyboardState(nullptr);
  std::uint16_t bits = 0;
  bits |= (keyboard_state[SDL_SCANCODE_A]) << 14;
  bits |= (keyboard_state[SDL_SCANCODE_D]) << 13;
  return bits;
}

void ClientEventSystem::
    interpolateEntities() /* NOLINT(*-convert-member-functions-to-static) */ {
  static constexpr float smooth = 1.3f;

  for (auto &client : network_ID_to_ecs_ID_) {
    auto &component =
        current_scene_->getComponent<PhysicsBodyComponent>(client.ecs_ID);

    // shouldn't exceed 1, as lerp is at 1 when it is the maximum value, 0 when
    // minimum otherwise interpolate within ~1/10 of a second when delta = 0 ->
    // t=0 when delta = 1 -> t=10 when delta = 1/10 -> t=1
    const float T = (std::min)(1.0f, lerp_timer_.getDelta() * 10) * smooth;

    Vector2 new_position = {
        std::lerp(client.position.x, client.lerp_position.x, T),
        std::lerp(client.position.y, client.lerp_position.y, T)};

    client.position = new_position;
    component.AABB.x = new_position.x;
    component.AABB.y = new_position.y;
  }
}
