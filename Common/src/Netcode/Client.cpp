#include "enet/enet.h"
#define ENET_IMPLEMENTATION
#include "Netcode/Client.h"
#include "Util.h"
#include <iostream>

NetClient::NetClient()
    : recv_callback_(), client_(nullptr), server_{nullptr, {}} {}

NetClient::NetClient(const std::string& ip, std::uint16_t port, std::function<void(const PacketData &)> recv_callback)
    : recv_callback_(std::move(recv_callback)), client_(nullptr),
      server_{nullptr, {}} {

  client_ = enet_host_create(nullptr, 1, 1, 0, 0);
  if (client_ == nullptr) {
    GlobalAppState::get().setAppState(AppState::AS_FAIL,
                                      "Cannot initialize network!");
    return;
  }

  valid_ = true;
  std::cout << "Created a client successfully!" << std::endl;
}

NetClient::NetClient(NetClient &&other) noexcept { swap(*this, other); }

NetClient &NetClient::operator=(NetClient &&other) noexcept {
  swap(*this, other);
  return *this;
}

void NetClient::connectTo(const std::string &server_addr,
                          const std::uint16_t port, const float timeout) {
  if (client_ == nullptr) {
    std::cerr << "Failed to connect to server! Client was nullptr."
              << std::endl;
    return;
  }

  ENetAddress server_ip;
  enet_address_set_host_ip(&server_ip, server_addr.c_str());

  server_ip.port = port;
  server_.address = server_ip;
  server_.server = enet_host_connect(client_, &server_.address, 1, 0);

  if (server_.server == nullptr) {
    std::cerr << "Failed to connect to server!" << std::endl;
    connected_ = false;
    return;
  }

  // check for connection by peeping into the event queue
  ENetEvent event;

  if (enet_host_service(client_, &event, static_cast<int>(timeout * 1000.f)) >
          0 &&
      event.type == ENET_EVENT_TYPE_CONNECT) {
    std::cout << "Successfully connected!" << std::endl;
  } else {
    std::cerr << "Failed to connect to server!" << std::endl;
    // server is not available...
    enet_peer_reset(server_.server);
    connected_ = false;
    return;
  }

  connected_ = true;
}

void NetClient::disconnect(const float timeout) {
  // connection does not exist ...
  if (server_.server != nullptr) {
    enet_peer_disconnect(server_.server, 0);
  }

  connected_ = false;
  updateNetwork(1.0);
}

void NetClient::sendPacket(PacketData &packet, const int channel,
                           const bool reliable) {
  if (server_.server == nullptr) {
    std::cerr << "Attempted packet send to non existent server!" << std::endl;
    return;
  }

  packet.ID = ID_;
  packet.salt = client_salt_ ^ server_salt_;
  ENetPacket *netpacket =
      packetDataToEnetPacket(packet, reliable ? ENET_PACKET_FLAG_RELIABLE
                                              : ENET_PACKET_FLAG_UNSEQUENCED);
  enet_peer_send(server_.server, channel, netpacket);
}

void NetClient::updateNetwork(const float block_time,
                              const bool disconnection) {
  if (client_ == nullptr) {
    return;
  }

  ENetEvent event;

  while (getConnected() && enet_host_service(client_, &event,
                                             static_cast<std::uint32_t>(
                                                 block_time * 1000.0f)) > 0) {
    switch (event.type) {
    case ENET_EVENT_TYPE_CONNECT:
      std::cout << "Connection Succeeded!" << std::endl;
      break;

    case ENET_EVENT_TYPE_RECEIVE: {
      PacketData parsed_packet = packetDataFromEnetPacket(event.packet);

      switch (parsed_packet.type) {
      case PT_HANDSHAKE: {
        std::cout << parsed_packet.data << std::endl;
        auto payload =
            payloadFromString<HandshakeChallengePayload>(parsed_packet.data);
        int response = payload.client_salt ^ payload.server_salt;

        HandshakeResponsePayload response_payload;
        response_payload.response = response;

        PacketData response_packet;
        response_packet.type = PT_HANDSHAKE_RESULT;
        response_packet.data =
            payloadToString<HandshakeResponsePayload>(response_payload);
        response_packet.data_size = response_packet.data.size() + 1;

        server_salt_ = payload.server_salt;
        client_salt_ = payload.client_salt;

        sendPacket(response_packet, 0, true);
        break;
      }
      case PT_HANDSHAKE_RESULT: {
        auto payload =
            payloadFromString<HandshakeAcceptRejectPayload>(parsed_packet.data);

        if (payload.accepted) {
          ID_ = payload.new_ID;
        } else {
          // reset this client and invalidate state
          enet_peer_reset(server_.server);
          connected_ = false;
        }

        break;
      }
      default:
        if (!disconnection &&
            parsed_packet.salt == (client_salt_ ^ server_salt_)) {
          recv_callback_(parsed_packet);
        }
        break;
      }

      enet_packet_destroy(event.packet);
      break;
    }
    case ENET_EVENT_TYPE_DISCONNECT:
      enet_peer_reset(server_.server);
      event.peer->data = nullptr; // enet requires this as the peer is no longer
                                  // garbage collected :)
      std::cout << "Disconnection Succeeded!" << std::endl;
      connected_ = false;
      break;
    }
  }
}

NetClient::~NetClient() {
  if (client_ != nullptr) {
    enet_host_destroy(client_);
  }

  client_ = nullptr;
}
