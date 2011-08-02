#ifndef OBJECT_H_INCLUDED
#define OBJECT_H_INCLUDED

#include <set>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

class Player;

class Object {
  public:
    Object(sf::Uint16 type, sf::String name, sf::Vector2f pos, sf::Vector2f vel, float rot, float rot_vel);
    virtual ~Object();

    virtual void Update(float time);

    // Network Handlers
    virtual void FillPartialPacket(sf::Packet& p);
    virtual void FillFullPacket(sf::Packet& p);
    virtual void HandlePacket(sf::Packet& p) = 0;

    // ID Handlers
    inline sf::Uint16 GetId() { return id; }
    inline void SetName( sf::String name ) { this->name = name; }
    inline std::string GetName() { return name; }

    // Dirty flag Handlers
    inline bool IsDirty() { return dirty; }
    inline void SetDirty() { dirty = true; }
    inline void ClearDirty() { dirty = false; }

    inline bool IsFresh() { return fresh; }
    inline void SetFresh() { fresh = true; }
    inline void ClearFresh() { fresh = false; }

    // Delete Handlers
    inline void Delete() { delete_me = true; }
    inline bool IsDeleted() { return delete_me; }

    inline sf::Vector2f GetPosition() { return position; }
    inline void SetPosition(sf::Vector2f p) { position = p; }

  private:
    sf::Uint16 id;
    sf::Uint16 type;
    sf::String name;

    bool dirty;
    bool fresh;

    bool delete_me;
  protected:
    sf::Vector2f position;
    sf::Vector2f velocity;

    float rotation;
    float rotational_velocity;
};

enum packet_server_object{
  OBJECT_UPDATE = 0,
  OBJECT_STATE,
  OBJECT_REMOVE
};

#endif // OBJECT_H_INCLUDED
