#include "PhysicsWorld.h"
PhysicsWorld::PhysicsWorld() 
{
}

PhysicsWorld::~PhysicsWorld()
{
}

void PhysicsWorld::AddObject(Object* obj)
{
   m_objects.push_back(obj);
}

void PhysicsWorld::RemoveObject(Object* obj)
{
   if (!obj) return;
   auto itr = std::find(m_objects.begin(), m_objects.end(), obj);
   if (itr == m_objects.end()) return;
   m_objects.erase(itr);
}

void PhysicsWorld::Step(float dt)
{
   for (Object* obj : m_objects)
   {
      obj->Force += obj->Mass * m_gravity;

      obj->Velocity += obj->Force / obj->Mass * dt;
      obj->Position += obj->Velocity * dt;

      obj->Force = glm::vec3(0, 0, 0);
   }
}