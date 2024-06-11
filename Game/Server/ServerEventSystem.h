#ifndef SERVER_EVENT_SYSTEM_H
#define SERVER_EVENT_SYSTEM_H

#include "ECS.h"
#include "Netcode/Server.h"
#include <array>
#include <vector>
#include <string>
#include "Timer.h" 

class ServerEventSystem : public ECS::ISystem {
public:
    explicit ServerEventSystem(ECS::Scene& scene) : current_scene_(&scene) { }
    ServerEventSystem() : client_to_ecs_ID_(), current_scene_(nullptr) {}
    ~ServerEventSystem(); 

    void init(ECS::Scene &scene) override;
    void update(ECS::Scene &scene, float delta) override;
    // TODO: bind IP
    void setupServer(std::uint16_t port);
public:
    std::vector<std::string> anim_states{10};
private:
    void onRecievePacket(const PacketData& packet);
private:
    static constexpr float net_tick_rate_ = 60.f;
    static constexpr float player_accel_x_ = 400.f;
    static constexpr float player_accel_y_ = 1300.f;
    ECS::Scene* current_scene_;
    Timer net_tick_timer_;
    NetServer net_server_;
    std::array<ECS::EntityID, 10> client_to_ecs_ID_ ;
}; 

#endif