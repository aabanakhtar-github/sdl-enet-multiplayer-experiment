#include "ClientEventSystem.h"
#include "Components.h"
#include <cstdint>
#include <sstream> 
#include <iostream>
#include "CreateScenes.h"

// TODO left: 
// - Client server view synchronization (Clients see same as server) / correction
// - Client inputs sent to server
// - Server response to client input
// - Interpolation (depends)

void ClientEventSystem::init(ECS::Scene &scene) {
    net_client_ = NetClient([this](const PacketData& packet) -> void {
        this->onReceivePacket(packet);
    });
    
    if (!net_client_.getValid()) {
        GlobalAppState::get().setAppState(AppState::AS_FAIL, "Cannot initialize network!"); 
    }

    // TODO refactor into reusable function that allows game to persist across multiple servers
    net_client_.connectTo("127.0.0.1", 7777, 3.0);

    EventHandler::get().bindEvent(SDL_KEYDOWN, [&, this](SDL_Event& e) -> void {
        switch (e.key.keysym.sym) {
        case SDLK_SPACE: {
            if (!net_client_.getConnected()) return;
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

    EventHandler::get().bindEvent(SDL_QUIT, [&](SDL_Event& e) -> void {
        GlobalAppState::get().setAppState(AppState::AS_QUIT);
    });

    // Create Players 
    for (auto& other_peer : other_peers_) {
        other_peer.ID = makeEntity(scene, Proto::PLAYER, Vector2(0, 0));
        scene.setEntityActive(other_peer.ID, false);
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

    if (net_client_.getConnected()) {
        // TODO: work on this, unimplemented
        net_client_.updateNetwork();
        PacketData packet; 
        packet.type = PT_GAME_UPDATE;
      
        ClientUpdatePayload payload; 
        payload.input_bits = inputs;
        packet.data = payloadToString<ClientUpdatePayload>(payload);

        packet.data_size = packet.data.size() + 1;
        net_client_.sendPacket(packet, 0, false);
    }

    if (net_client_.getID() == -1) {
        return; // haven't connected, and thus this player doesn't exist
    }
}

void ClientEventSystem::syncGame(const std::string &packet_data) {
    if (current_scene_ == nullptr) {
        return;
    }

    ECS::Scene& scene = *current_scene_;
    auto payload = payloadFromString<ServerUpdatePayload>(packet_data);
    // update the client positions 
    std::vector<std::size_t> connected_list; 

    for (auto &client : payload.client_states) {
        connected_list.push_back(client.ID); 
        auto& physics_component = scene.getComponent<PhysicsBodyComponent>(other_peers_[client.ID].ID);
        auto& anim_component = scene.getComponent<AnimationStateMachineComponent>(other_peers_[client.ID].ID);
        auto& view = other_peers_[client.ID];

        view.last_position = { static_cast<float>(physics_component.BoundingBox.x), static_cast<float>(physics_component.BoundingBox.y) };
        view.lerp_position = client.position;

        anim_component.state = client.anim_state;

        scene.setEntityActive(other_peers_[client.ID].ID);
    }

    for (std::size_t i = 0; i < other_peers_.size(); ++i) {
        // is it in the connected list? if so, make the entity active again
        auto it = std::find(connected_list.begin(), connected_list.end(), i); 

        if (it == connected_list.end()) {
            // make this client dissappear and reset their position 
            scene.setEntityActive(other_peers_[i].ID, false);
            auto& component = scene.getComponent<PhysicsBodyComponent>(other_peers_[i].ID);
            component = PhysicsBodyComponent { .BoundingBox = component.BoundingBox };
        }
    }
}

void ClientEventSystem::onReceivePacket(const PacketData &packet) {
    switch (packet.type) {
    case PT_GAME_UPDATE: 
        // re-sync / reconcile / interpolate
        lerp_timer_.reset();
        syncGame(packet.data);
        break;
    }
}

void ClientEventSystem::predictClientState() {
    // TODO: implement when have time
}

std::uint16_t ClientEventSystem::getKeyboardBits() /* NOLINT(*-convert-member-functions-to-static) */ {
    const std::uint8_t* keyboard_state = SDL_GetKeyboardState(nullptr); 
    std::uint16_t bits = 0;
    bits |= (keyboard_state[SDL_SCANCODE_A]) << 14; 
    bits |= (keyboard_state[SDL_SCANCODE_D]) << 13;
    return bits;
}

void ClientEventSystem::interpolateEntities() /* NOLINT(*-convert-member-functions-to-static) */ {
    static constexpr float smooth = 1.3f;

    for (auto& client : other_peers_) {
        auto& component = current_scene_->getComponent<PhysicsBodyComponent>(client.ID);
        const float T = (std::min)(1.0f, lerp_timer_.getDelta() / (1.0f / 10.0f)) * smooth;

        Vector2 new_position = {
            std::lerp(client.position.x, client.lerp_position.x, T),
            std::lerp(client.position.y, client.lerp_position.y, T)
        };

        client.position = new_position;
        component.BoundingBox.x = static_cast<int>(new_position.x);
        component.BoundingBox.y = static_cast<int>(new_position.y);
    }
}
