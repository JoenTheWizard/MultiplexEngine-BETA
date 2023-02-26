#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

struct Object {
   glm::vec3 Position;
   glm::vec3 Velocity;
   glm::vec3 Force;
   float Mass;
};

class PhysicsWorld
{
public:
   PhysicsWorld();
   ~PhysicsWorld();
   Object object_;
   void AddObject(Object* obj);
   void RemoveObject(Object* obj);
   void Step(float dt);

private:
   std::vector<Object*> m_objects;
   glm::vec3 m_gravity = glm::vec3(0, -9.81f, 0);
};

