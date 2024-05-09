#ifndef SERVER_EVENT_SYSTEM_H
#define SERVER_EVENT_SYSTEM_H

#include "ECS.h"
#include "Netcode/Server.h"
#include <array> 
#include "Timer.h" 

class ServerEventSystem : public ECS::ISystem 
{
public: 
    explicit ServerEventSystem(ECS::Scene& scene) : m_NetworkSequenceNumber(0) {}
    ~ServerEventSystem(); 

    virtual void Init(ECS::Scene& scene) override; 
    virtual void Update(ECS::Scene& scene, float delta) override; 

    void SetupServer(const std::uint16_t port); 
private:
    void OnRecievePacket(const PacketData& packet);
private:
    static constexpr float m_NetTickRate = 20.f;
    static constexpr float m_PlayerAccelX = 5.f; 
    static constexpr float m_PlayerAccelY = 5.f; 
    Timer m_NetTickTimer; 
    std::uint64_t m_NetworkSequenceNumber;
    NetServer m_NetServer;
    ECS::Scene* m_CurrentScene;
    std::array<ECS::EntityID, 10> m_ClientToECS_ID; 
}; 

#endif