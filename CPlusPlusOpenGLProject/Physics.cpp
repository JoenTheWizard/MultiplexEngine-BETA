#include "Physics.h"

Physics::Physics()
{

}

Physics::~Physics()
{}


void Physics::AddObject(Object* object)
{
   m_objects.push_back(object);
}


void Physics::RemoveObject(Object* object)
{
   if (!object) return;
   auto itr = std::find(m_objects.begin(), m_objects.end(), object);
   if (itr == m_objects.end()) return;
   m_objects.erase(itr);
}


void Physics::Step(float dt)
{
   for (Object* obj : m_objects)
   {
      obj->Force += obj->mass * m_gravity;

      obj->Velocity += obj->Force / obj->mass * dt;
      obj->Position += obj->Velocity * dt;

      obj->Force = glm::vec3(0, 0, 0);
   }
}
