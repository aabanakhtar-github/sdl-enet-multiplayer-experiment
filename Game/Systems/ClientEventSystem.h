#ifndef CLIENT_EVENT_SYSTEM_H 
#define CLIENT_EVENT_SYSTEM_H 

#include "ECS.h"
#include "Util.h"
#include "SDL.h"
#include "EventHandler.h"
#include "Netcode/Client.h"
 
class ClientEventSystem : public ECS::ISystem
{
public:
    ClientEventSystem() = default;
    ~ClientEventSystem() { m_NetClient.Disconnect(3.0); } 
    virtual void Init(ECS::Scene& scene) override; 
    virtual void Update(ECS::Scene& scene, float deltatime) override; 
private:
    void OnRecievePacket(const PacketData& packet); 
private:
    EventHandler m_EventHandler; 
    NetClient m_NetClient;
};

#endif