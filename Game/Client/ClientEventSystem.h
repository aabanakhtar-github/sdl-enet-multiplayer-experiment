#ifndef CLIENT_EVENT_SYSTEM_H 
#define CLIENT_EVENT_SYSTEM_H 

#include "ECS.h"
#include "Util.h"
#include "SDL.h"
#include "EventHandler.h"
#include "Netcode/Client.h"
#include <array> 

class ClientEventSystem : public ECS::ISystem
{
public:
    ClientEventSystem() = default;
    ~ClientEventSystem() { m_NetClient.Disconnect(3.0); } 
    virtual void Init(ECS::Scene& scene) override; 
    virtual void Update(ECS::Scene& scene, float deltatime) override; 
private:
    void OnRecievePacket(const PacketData& packet);
    void PredictClientState(); 
    void ReconcileWithServer();  
    void InterpolateEntities(); 
private:
    static constexpr float m_NetTickRate = 30.f; 
    std::uint64_t m_NetworkSequenceNumber = 0; 
    EventHandler m_EventHandler; 
    NetClient m_NetClient;
    std::array<ClientInfo, 10> m_Players; 
    std::array<std::pair<ClientUpdatePayload, ClientInfo>, 1024> m_RollbackBuffer; 
};

#endif