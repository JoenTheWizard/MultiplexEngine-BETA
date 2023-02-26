#ifndef PHYSICS_HPP__
#define PHYSICS_HPP__

#include <glm/detail/type_vec.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>


class Physics
{
public:
   typedef struct {
      glm::vec3& Position;
      glm::vec3& Velocity;
      glm::vec3& Force;
      float mass;
   }Object;
   
public:
   Physics();
   ~Physics();
   void AddObject(Object* object);
   void RemoveObject(Object* object);
   void Step(float dt);

private:
   std::vector<Object*> m_objects;
   glm::vec3 m_gravity = glm::vec3(0, -9.81f, 0);

public:
   Object obj_;
      
};

#endif

