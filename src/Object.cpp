#include "Object.h"

Object::Object(ObjectType t, MaterialType mt, const glm::vec3& c)
    : type(t), materialType(mt), color(c) {}

ObjectType Object::getType() const {
    return type;
}

MaterialType Object::getMaterialType() const {
    return materialType;
}

void Object::setColor(const glm::vec3& c) {
    color = c;
}
