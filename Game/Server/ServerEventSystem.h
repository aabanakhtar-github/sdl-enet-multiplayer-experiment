#ifndef SERVER_EVENT_SYSTEM_H
#define SERVER_EVENT_SYSTEM_H

#include "ECS.h"
#include "Netcode/Server.h"
#include <array> 

class ServerEventSystem : public ECS::ISystem 
{
public: 
    explicit ServerEventSystem() = default; 
    ~ServerEventSystem(); 

    virtual void Init(ECS::Scene& scene) override; 
    virtual void Update(ECS::Scene& scene, float delta) override; 

    void SetupServer(const std::uint16_t port); 
private:
    void OnRecievePacket(const PacketData& packet); 
    void SendHandshakeChallenge(); 
    bool VerifyHandshakeChallenge(const std::uint32_t result) const; 
    void FilterSilliness();
    void SendHandshakeAcceptReject(bool accept_reject);
    void SendWorldState(); 
    void FillClient(std::uint32_t ID); 
private:
    static constexpr float m_NetTickRate = 30.f;
    std::uint64_t m_NetworkSequenceNumber = 0;
    NetServer m_NetServer;
    std::array<ClientInfo, 10> m_Clients; 
    std::array<std::array<ClientInfo, 1024>, 10> PastClientStates; 
}; 

#endif