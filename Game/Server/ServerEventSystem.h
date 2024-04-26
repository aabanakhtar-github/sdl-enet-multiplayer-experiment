#ifndef SERVER_EVENT_SYSTEM_H
#define SERVER_EVENT_SYSTEM_H

#include "ECS.h"
#include "Netcode/Server.h"
#include <array> 

class ServerEventSystem : public ECS::ISystem 
{
public: 
    explicit ServerEventSystem(ECS::Scene& scene) : m_Scene(scene), m_NetServer() {}
    ~ServerEventSystem(); 

    virtual void Init(ECS::Scene& scene) override; 
    virtual void Update(ECS::Scene& scene, float delta) override; 

    void SetupServer(const std::uint16_t port); 
private:
    void OnRecievePacket(const PacketData& packet);
    void OnConnect(std::size_t ID);
    void OnDisconnect(std::size_t ID);

    void SendWorldState(); 
private:
    static constexpr float m_NetTickRate = 30.f;
    std::uint64_t m_NetworkSequenceNumber = 0;
    NetServer m_NetServer;
    ECS::Scene& m_Scene; 
}; 

#endif