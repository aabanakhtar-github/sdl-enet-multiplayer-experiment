#ifndef CLIENT_EVENT_SYSTEM_H 
#define CLIENT_EVENT_SYSTEM_H 

#include "ECS.h"
#include "Util.h"
#include "SDL.h"
#include "EventHandler.h"
#include "Netcode/Client.h"
#include "Timer.h"
#include <array>  


struct ClientView : ClientInfo {
    ECS::EntityID ID = -1;
    Vector2 last_position, lerp_position;
};

class ClientEventSystem : public ECS::ISystem
{
    using ClientInfoEx = std::pair<ECS::EntityID, ClientInfo>;
    using ClientRollbackInfo = std::pair<ClientInfo, ClientUpdatePayload>; 
public:
    ClientEventSystem() : current_scene_(nullptr) {}
    ~ClientEventSystem() { net_client_.disconnect(3.0); }
    void init(ECS::Scene &scene) override;
    void update(ECS::Scene &scene, float delta) override;
private:
    void syncGame(const std::string& packet_data);
    void onReceivePacket(const PacketData& packet);
    void predictClientState();
    void interpolateEntities();

    [[nodiscard]] std::uint16_t getKeyboardBits();
private:
    static constexpr float net_tick_rate_ = 30.f;
    bool input_bit_ = false;
    ECS::EntityID ID_ = -1;
    NetClient net_client_;
    std::array<ClientView, 10> other_peers_;
    std::array<ClientRollbackInfo, 1024> rollback_buffer_;
    Timer lerp_timer_;
    ECS::Scene* current_scene_;
};

#endif