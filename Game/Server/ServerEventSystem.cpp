#include "ServerEventSystem.h"
#include "EventHandler.h"
#include "Components.h"
#include "CreateScenes.h"
#include <sstream> 
#include <functional> 

using UserData = std::pair<ECS::EntityID, std::array<std::tuple<ClientInfo, ClientUpdatePayload, std::shared_ptr<ECS::EntityID>>, 1024>>;

void ServerEventSystem::init(ECS::Scene &scene) {
    for (auto&[client, data] : net_server_.GetClients()) {
        auto temp = UserData();
        temp.first = -1; 
        data.user_data = std::move(temp); 
    }

    EventHandler::get().bindEvent(SDL_QUIT, [&](SDL_Event& e) -> void {
        GlobalAppState::get().setAppState(AppState::AS_QUIT);
    });

    for (auto& i : client_to_ecs_ID_) {
        i = makeEntity(scene, Proto::PLAYER, Vector2(0, 0));
        auto& component = scene.getComponent<PhysicsBodyComponent>(i);
        component.SimulatesPhysics = true;
        scene.setEntityActive(i, false);
    }
}

void ServerEventSystem::setupServer(std::uint16_t port) {
    net_server_ = NetServer(port, 10, [this](const PacketData& packet) -> void {
        this->onRecievePacket(packet);
    });

    if (!net_server_.getValid()) {
        GlobalAppState::get().setAppState(AppState::AS_FAIL, "Cannot initialize network!"); 
    }
}


// Most un-ECSy update method ever
// probably won't update it because it's too complex to wrap into an ECS ( + scope creep )
void ServerEventSystem::update(ECS::Scene &scene, float delta) {
    current_scene_ = &scene;

    if (net_server_.getValid()) {
        net_server_.updateNetwork();

        if (net_tick_timer_.getDelta() >= 1.0f / net_tick_rate_) {
            // send an update packet
            PacketData update_packet;
            ServerUpdatePayload payload;
           
            update_packet.type = PT_GAME_UPDATE; 
            auto& clients = net_server_.GetClients();
            payload.clients_size = clients.size(); 
            payload.client_states.resize(payload.clients_size); 

            // connected clients
            // used to determine which clients to disconnect on line 76
            std::vector<std::size_t> connected_list;
            connected_list.reserve(10); 

            int i = 0;
            for (auto &client : clients) {
                // save this client's Server ID for later
                connected_list.push_back(client.first); 
                Vector2 client_position;
                // send the position data etc
                auto& component = scene.getComponent<PhysicsBodyComponent>(client_to_ecs_ID_[client.first]);
                client_position = { static_cast<float>(component.BoundingBox.x), static_cast<float>(component.BoundingBox.y) };

                // set the animation state and position
                payload.client_states[i] = ClientInfo { static_cast<int>(client.first), client_position, anim_states[client.second.ID]};
                ++i;
            }

            // Determine which client's to disable using their Server ID -> ECS::EntityID
            for (std::size_t k = 0; k < client_to_ecs_ID_.size(); ++k) {
                // is it in the connected list? if so, make the entity active again
                // find the client if it is connected using Server ID through connected_list
                auto it = std::find(connected_list.begin(), connected_list.end(), k);
                // this client isn't connected
                if (it == connected_list.end()) {
                    // make this client disappear and reset their position by finding their ECS::EntityID
                    scene.setEntityActive(client_to_ecs_ID_[k], false);
                    // reset the entity
                    auto& component = scene.getComponent<PhysicsBodyComponent>(client_to_ecs_ID_[k]);
                    component = PhysicsBodyComponent { .BoundingBox = component.BoundingBox };
                    component.SimulatesPhysics = true; 
                } else {
                    // this server ID entity is active, set their ECS::EntityID to active
                    scene.setEntityActive(client_to_ecs_ID_[k]);
                } 
            }

            update_packet.data = payloadToString<ServerUpdatePayload>(payload); 
            update_packet.data_size = update_packet.data.size() + 1; 

            net_server_.broadcastPacket(update_packet, 0, false);
            net_tick_timer_.reset();
        }
    } 

    EventHandler::get().update();
}

void ServerEventSystem::onRecievePacket(const PacketData& packet) {
    switch (packet.type) {
    case PT_GAME_UPDATE: {
        auto payload = payloadFromString<ClientUpdatePayload>(packet.data); 
        ECS::EntityID client = client_to_ecs_ID_[packet.ID];
        auto& component = current_scene_->getComponent<PhysicsBodyComponent>(client);
        //w = 15, a = 14, s = 13, d = 12
        std::bitset<16> inputs = payload.input_bits;
        float x_scale = 0.0f, y_scale = 0.0f;

        if (inputs[14]) {
            x_scale = -1; 
        } else if (inputs[13]) {
            x_scale = 1; 
        } else {
            x_scale = 0;
        }
       
        component.Velocity.x = x_scale * player_accel_x_;
        break;
    }
    case PT_CLIENT_JUMP: {
        ECS::EntityID client = client_to_ecs_ID_[packet.ID];
        auto& component = current_scene_->getComponent<PhysicsBodyComponent>(client);

        if (component.grounded) {
            component.Velocity.y = -player_accel_y_;
        }

        break;
    }
    }
}

ServerEventSystem::~ServerEventSystem() {
}