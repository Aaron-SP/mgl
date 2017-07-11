#include <min/aabbox.h>
#include <min/intersect.h>
#include <min/oobbox.h>
#include <min/sphere.h>
#include <min/vec2.h>
#include <min/vec3.h>

// intersection 2D
// RAY
template bool min::intersect<float>(const min::sphere<float, min::vec2> &s, const ray<float, min::vec2> &ray, min::vec2<float> &p);
template bool min::intersect<float>(const min::aabbox<float, min::vec2> &box, const ray<float, min::vec2> &ray, min::vec2<float> &p);

// SPHERE-SPHERE
template bool min::intersect<float>(const min::sphere<float, min::vec2> &s1, const min::sphere<float, min::vec2> &s2);
template bool min::intersect<float>(const min::sphere<float, min::vec2> &s1, const min::sphere<float, min::vec2> &s2, min::vec2<float> &p);

// AABB-AABB
template bool min::intersect<float>(const min::aabbox<float, min::vec2> &box1, const min::aabbox<float, min::vec2> &box2);
template bool min::intersect<float>(const min::aabbox<float, min::vec2> &box1, const min::aabbox<float, min::vec2> &box2, min::vec2<float> &p);

// OOBB-OOBB
template bool min::intersect<float>(const min::oobbox<float, min::vec2> &b1, const min::oobbox<float, min::vec2> &b2);
template bool min::intersect<float>(const min::oobbox<float, min::vec2> &b1, const min::oobbox<float, min::vec2> &b2, min::vec2<float> &p);

// AABB-SPHERE
template bool min::intersect<float>(const min::sphere<float, min::vec2> &s, const min::aabbox<float, min::vec2> &box);
template bool min::intersect<float>(const min::aabbox<float, min::vec2> &box, const min::sphere<float, min::vec2> &s);
template bool min::intersect<float>(const min::sphere<float, min::vec2> &s, const min::aabbox<float, min::vec2> &box, min::vec2<float> &p);
template bool min::intersect<float>(const min::aabbox<float, min::vec2> &box, const min::sphere<float, min::vec2> &s, min::vec2<float> &p);

// OOBB-AABB
template bool min::intersect<float>(const min::oobbox<float, min::vec2> &s, const min::aabbox<float, min::vec2> &box);
template bool min::intersect<float>(const min::aabbox<float, min::vec2> &box, const min::oobbox<float, min::vec2> &s);
template bool min::intersect<float>(const min::oobbox<float, min::vec2> &s, const min::aabbox<float, min::vec2> &box, min::vec2<float> &p);
template bool min::intersect<float>(const min::aabbox<float, min::vec2> &box, const min::oobbox<float, min::vec2> &s, min::vec2<float> &p);

// SPHERE-OOBB
template bool min::intersect<float>(const min::sphere<float, min::vec2> &s, const min::oobbox<float, min::vec2> &box);
template bool min::intersect<float>(const min::oobbox<float, min::vec2> &box, const min::sphere<float, min::vec2> &s);
template bool min::intersect<float>(const min::sphere<float, min::vec2> &s, const min::oobbox<float, min::vec2> &box, min::vec2<float> &p);
template bool min::intersect<float>(const min::oobbox<float, min::vec2> &box, const min::sphere<float, min::vec2> &s, min::vec2<float> &p);

// intersection 3D
// RAY
template bool min::intersect<float>(const min::sphere<float, min::vec3> &s, const ray<float, min::vec3> &ray, min::vec3<float> &p);
template bool min::intersect<float>(const min::aabbox<float, min::vec3> &box, const ray<float, min::vec3> &ray, min::vec3<float> &p);

// SPHERE-SPHERE
template bool min::intersect<float>(const min::sphere<float, min::vec3> &s1, const min::sphere<float, min::vec3> &s2);
template bool min::intersect<float>(const min::sphere<float, min::vec3> &s1, const min::sphere<float, min::vec3> &s2, min::vec3<float> &p);

// AABB-AABB
template bool min::intersect<float>(const min::aabbox<float, min::vec3> &box1, const min::aabbox<float, min::vec3> &box2);
template bool min::intersect<float>(const min::aabbox<float, min::vec3> &box1, const min::aabbox<float, min::vec3> &box2, min::vec3<float> &p);

// OOBB-OOBB
template bool min::intersect<float>(const min::oobbox<float, min::vec3> &b1, const min::oobbox<float, min::vec3> &b2);
template bool min::intersect<float>(const min::oobbox<float, min::vec3> &b1, const min::oobbox<float, min::vec3> &b2, min::vec3<float> &p);

// AABB-SPHERE
template bool min::intersect<float>(const min::sphere<float, min::vec3> &s, const min::aabbox<float, min::vec3> &box);
template bool min::intersect<float>(const min::aabbox<float, min::vec3> &box, const min::sphere<float, min::vec3> &s);
template bool min::intersect<float>(const min::sphere<float, min::vec3> &s, const min::aabbox<float, min::vec3> &box, min::vec3<float> &p);
template bool min::intersect<float>(const min::aabbox<float, min::vec3> &box, const min::sphere<float, min::vec3> &s, min::vec3<float> &p);

// OOBB-AABB
template bool min::intersect<float>(const min::oobbox<float, min::vec3> &s, const min::aabbox<float, min::vec3> &box);
template bool min::intersect<float>(const min::aabbox<float, min::vec3> &box, const min::oobbox<float, min::vec3> &s);
template bool min::intersect<float>(const min::oobbox<float, min::vec3> &s, const min::aabbox<float, min::vec3> &box, min::vec3<float> &p);
template bool min::intersect<float>(const min::aabbox<float, min::vec3> &box, const min::oobbox<float, min::vec3> &s, min::vec3<float> &p);

// SPHERE-OOBB
template bool min::intersect<float>(const min::sphere<float, min::vec3> &s, const min::oobbox<float, min::vec3> &box);
template bool min::intersect<float>(const min::oobbox<float, min::vec3> &box, const min::sphere<float, min::vec3> &s);
template bool min::intersect<float>(const min::sphere<float, min::vec3> &s, const min::oobbox<float, min::vec3> &box, min::vec3<float> &p);
template bool min::intersect<float>(const min::oobbox<float, min::vec3> &box, const min::sphere<float, min::vec3> &s, min::vec3<float> &p);

// FRUSTUM
template bool min::intersect<float>(const frustum<float> &f, const min::sphere<float, min::vec3> &s);
template bool min::intersect<float>(const frustum<float> &f, const min::sphere<float, min::vec3> &s, min::vec3<float> &p);
template bool min::intersect<float>(const frustum<float> &f, const min::aabbox<float, min::vec3> &box);
template bool min::intersect<float>(const frustum<float> &f, const min::aabbox<float, min::vec3> &box, min::vec3<float> &p);
