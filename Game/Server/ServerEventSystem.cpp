#include "ServerEventSystem.h"
#include "EventHandler.h"
#include "Components.h"
#include "CreateScenes.h"
#include <sstream> 
#include <functional> 

namespace { 
    int up = 0; 
    int down = 0; 
}

using UserData = std::pair<ECS::EntityID, std::array<std::tuple<ClientInfo, ClientUpdatePayload, std::shared_ptr<ECS::EntityID>>, 1024>>;
void ServerEventSystem::Init(ECS::Scene& scene) 
{
    for (auto&[client, data] : m_NetServer.GetClients())
    {
        auto temp = UserData();
        temp.first = -1; 
        data.UserData = std::move(temp); 
    }

    EventHandler::Get().BindEvent(SDL_QUIT, [&](SDL_Event& e) -> void 
    {
        GlobalAppState::Get().SetAppState(AppState::AS_QUIT);
    });
}

void ServerEventSystem::SetupServer(const std::uint16_t port)
{
    m_NetServer = NetServer(port, 10, std::bind(&ServerEventSystem::OnRecievePacket, this, std::placeholders::_1), std::bind(&ServerEventSystem::OnConnect, this, std::placeholders::_1), 
        std::bind(&ServerEventSystem::OnDisconnect, this, std::placeholders::_1));

    if (!m_NetServer.GetValid()) 
    {
        GlobalAppState::Get().SetAppState(AppState::AS_FAIL, "Cannot initialize network!"); 
    }
}

void ServerEventSystem::Update(ECS::Scene& scene, float delta)
{
    PacketData output_packet = PacketData { 
        .ID = 0, 
        .Type = PT_GAME_UPDATE, 
    }; 

    std::ostringstream output_ss; 
    output_ss << scene.GetComponent<PhysicsBodyComponent>(0).BoundingBox.x << " "; 
    output_ss <<  scene.GetComponent<PhysicsBodyComponent>(0).BoundingBox.y << " "; 
   
    output_packet.Data = output_ss.str(); 
    output_packet.DataLength = output_ss.str().size() + 1;  

    if (m_NetServer.GetValid())
    {
        m_NetServer.UpdateNetwork(); 
    } 

    scene.GetComponent<PhysicsBodyComponent>(0).Velocity.Y = 15 * up * delta; 
    scene.GetComponent<PhysicsBodyComponent>(0).Velocity.X = 15 * down * delta; 

    EventHandler::Get().Update(); 
}

void ServerEventSystem::OnRecievePacket(const PacketData& packet) 
{
}


// clear the input buffers on client disconnect / reconnect
void ServerEventSystem::OnConnect(std::size_t ID)
{
    auto& clients = m_NetServer.GetClients(); 
    auto temp = std::move(UserData());
    temp.first = m_Scene.CreateEntity(); 
    BuildPlayer(m_Scene, temp.first); 
    clients[ID].UserData = std::move(temp); 
}

void ServerEventSystem::OnDisconnect(std::size_t ID)
{
    auto& clients = m_NetServer.GetClients(); 
    m_Scene.DestroyEntity(std::any_cast<UserData>(clients[ID].UserData).first);
    clients[ID].UserData = std::move(UserData());
}

ServerEventSystem::~ServerEventSystem() 
{

}