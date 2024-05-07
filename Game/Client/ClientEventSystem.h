#ifndef CLIENT_EVENT_SYSTEM_H 
#define CLIENT_EVENT_SYSTEM_H 

#include "ECS.h"
#include "Util.h"
#include "SDL.h"
#include "EventHandler.h"
#include "Netcode/Client.h"
#include <array>  


class ClientClientInfo : ClientInfo 
{
    ECS::EntityID ID; 
    Vector2 LastPosition; 
};

class ClientEventSystem : public ECS::ISystem
{
    using ClientInfoEx = std::pair<ECS::EntityID, ClientInfo>;
    using ClientRollbackInfo = std::pair<ClientInfo, ClientUpdatePayload>; 
public:
    ClientEventSystem() : m_CurrentScene(nullptr) {}
    ~ClientEventSystem() { m_NetClient.Disconnect(3.0); } 
    virtual void Init(ECS::Scene& scene) override; 
    virtual void Update(ECS::Scene& scene, float deltatime) override; 
private:
    void UpdateGame(const std::string& packet_data);
    void OnRecievePacket(const PacketData& packet);
    void PredictClientState(); 
    void ReconcileWithServer();  
    std::uint16_t GetKeyboardBits();
    void InterpolateEntities(); 
private:
    static constexpr float m_NetTickRate = 30.f; 
    std::uint64_t m_InputSequenceNumber = 0; 
    EventHandler m_EventHandler; 
    NetClient m_NetClient;
    std::array<ClientInfoEx, 10> m_OtherPeers; 
    std::array<ClientRollbackInfo, 1024> m_RollbackBuffer; 
    ECS::EntityID m_ID; 
    ECS::Scene* m_CurrentScene;
};

#endif