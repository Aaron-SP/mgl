/* Copyright [2013-2016] [Aaron Springstroh, Minimal Graphics Library]

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/
#ifndef __PHYSICS__
#define __PHYSICS__

// The physics equations governing rigid bodies are listed below
// V = linear_velocity
// omega = angular_velocity
// F = Force
// P = position
// C = contact_position
// G = Gravity
// m = mass
// I = inertia
// k = damping parameter
// 1.) dV/dt = a = (F - k*V + G) / m
// 2.) domega/dt = alpha = ((C - P) x F - k*omega) / I

// These equations are solved using RK4
// dy/dt = f(t, y)
// y_n+1 = y_n + (dt / 6) * (k_1 + 2*k_2 + 2*k_3 + k_4)
// t_n+1 = t + dt
// k1 = f(t_n, y_n)
// k2 = f(t_n + 0.5*dt, y_n + 0.5*k1*dt)
// k3 = f(t_n + 0.5*dt, y_n + 0.5*k2*dt)
// k4 = f(t_n + dt, y_n + k3*dt)

#include <cmath>
#include <min/aabbox.h>
#include <min/intersect.h>
#include <min/mat2.h>
#include <min/quat.h>
#include <min/sphere.h>
#include <min/vec2.h>
#include <min/vec3.h>
#include <min/vec4.h>
#include <stdexcept>
#include <vector>

namespace min
{

template <typename T, template <typename> class vec>
class body_base
{
  protected:
    std::vector<vec<T>> _forces;
    vec<T> _position; // This is at the center of mass
    vec<T> _linear_velocity;
    T _mass;
    T _inv_mass;

  public:
    body_base(const vec<T> &center, const T mass) : _position(center), _mass(mass), _inv_mass(1.0 / mass) {}
    inline void add_force(const vec<T> &f)
    {
        _forces.push_back(f);
    }
    inline void clear_force()
    {
        _forces.clear();
    }
    inline const vec<T> get_linear_acceleration(const vec<T> &linear_velocity, const vec<T> &gravity, const T damping) const
    {
        // Gravity = -mg
        vec<T> sum = gravity * _mass;

        // Sum all forces on this object
        for (const auto &force : _forces)
        {
            sum += force;
        }

        // Calculate the acceleration
        return (sum - linear_velocity * damping) * _inv_mass;
    }
    inline const vec<T> &get_linear_velocity() const
    {
        return _linear_velocity;
    }
    inline void set_linear_velocity(const vec<T> &v)
    {
        _linear_velocity = v;
    }
    inline const T get_mass() const
    {
        return _mass;
    }
    inline const T get_inv_mass() const
    {
        return _inv_mass;
    }
    inline const vec<T> &get_position() const
    {
        return _position;
    }
    inline void set_position(const vec<T> &p)
    {
        _position = p;
    }
    inline const void move_offset(const vec<T> &offset)
    {
        _position += offset;
    }
    inline void update_position(const vec<T> &linear_velocity, const T time_step, const vec<T> &min, const vec<T> &max)
    {
        // Update position from velocity
        _position += linear_velocity * time_step;

        // Clamp position to wall of physics world
        vec<T> direction = _position.clamp_direction(min, max);

        // Reverses linear velocity if hit edge of world
        _linear_velocity = linear_velocity * direction;
    }
};

template <typename T, template <typename> class vec>
class body : public body_base<T, vec>
{
};

// Partial specialization for resolving the type of angular_velocity for vec2 = T
template <typename T>
class body<T, vec2> : public body_base<T, vec2>
{
  protected:
    std::vector<T> _torques;
    mat2<T> _rotation;
    T _angular_velocity;
    T _inertia;
    T _inv_inertia;

  public:
    body(const vec2<T> &center, const T mass, const T inertia) : body_base<T, vec2>(center, mass), _angular_velocity(0.0), _inertia(inertia), _inv_inertia(1.0 / inertia) {}
    inline void add_torque(const T torque)
    {
        _torques.push_back(torque);
    }
    inline void add_torque(const vec2<T> &force, const vec2<T> &contact)
    {
        _torques.push_back((contact - this->_position).cross(force));
    }
    inline void clear_torque()
    {
        _torques.clear();
    }
    inline const T get_angular_acceleration(const T angular_velocity, const T damping) const
    {
        T sum = 0.0;

        // Sum all torques around this object's center of mass
        for (const auto &torque : _torques)
        {
            sum += torque;
        }

        // Calculate the acceleration
        return (sum - angular_velocity * damping) * this->_inv_inertia;
    }
    inline const T &get_angular_velocity() const
    {
        return _angular_velocity;
    }
    inline void set_angular_velocity(const T w)
    {
        _angular_velocity = w;
    }
    inline const T get_inertia() const
    {
        return _inertia;
    }
    inline const T &get_inv_inertia() const
    {
        return _inv_inertia;
    }
    inline const mat2<T> &get_rotation() const
    {
        return _rotation;
    }
    inline mat2<T> update_rotation(const T angular_velocity, const T time_step)
    {
        _angular_velocity = angular_velocity;

        // Rotation is around the Z axis in euler angles
        const mat2<T> out(_angular_velocity * time_step);

        // Transform the absolute rotation
        _rotation *= out;

        // return the relative rotation
        return out;
    }
};

// Partial specialization for resolving the type of angular_velocity for vec3 = vec3<T>
template <typename T>
class body<T, vec3> : public body_base<T, vec3>
{
  protected:
    std::vector<vec3<T>> _torques;
    quat<T> _rotation;
    vec3<T> _angular_velocity;
    vec3<T> _inertia;
    vec3<T> _inv_inertia;

  public:
    body(const vec3<T> &center, const T mass, const vec3<T> inertia) : body_base<T, vec3>(center, mass), _inertia(inertia), _inv_inertia(inertia.inverse()) {}
    inline void add_torque(const vec3<T> &torque)
    {
        _torques.push_back(torque);
    }
    inline void add_torque(const vec3<T> &force, const vec3<T> &contact)
    {
        _torques.push_back((contact - this->_position).cross(force));
    }
    inline void clear_torque()
    {
        _torques.clear();
    }
    inline const vec3<T> get_angular_acceleration(const vec3<T> &angular_velocity, const T damping) const
    {
        vec3<T> sum;

        // Sum all torques around this object's center of mass
        for (const auto &torque : _torques)
        {
            sum += torque;
        }

        // Calculate the acceleration
        return (sum - angular_velocity * damping) * _inv_inertia;
    }
    inline const vec3<T> &get_angular_velocity() const
    {
        return _angular_velocity;
    }
    inline void set_angular_velocity(const vec3<T> &w)
    {
        _angular_velocity = w;
    }
    inline const vec3<T> get_inertia() const
    {
        return _inertia.inverse();
    }
    inline const vec3<T> &get_inv_inertia() const
    {
        return _inv_inertia;
    }
    inline const quat<T> &get_rotation() const
    {
        return _rotation;
    }
    inline quat<T> update_rotation(const vec3<T> &angular_velocity, const T time_step)
    {
        _angular_velocity = angular_velocity;

        // Calculate rotation for this timestep
        const vec3<T> rotation = _angular_velocity * time_step;

        // Creation quaternion rotation
        const T angle = rotation.magnitude();
        const quat<T> q(rotation, angle);

        // Transform the absolute rotation
        _rotation *= q;

        // Normalize the rotation vector to avoid accumulation of rotational energy
        _rotation.normalize();

        // Update rotational inertia axis of rotation
        _inv_inertia = _rotation.transform(_inertia).inverse();

        // return the relative rotation
        return q;
    }
};

// Partial specialization for resolving the type of angular_velocity for vec4 = vec4<T>
template <typename T>
class body<T, vec4> : public body_base<T, vec4>
{
  protected:
    std::vector<vec4<T>> _torques;
    quat<T> _rotation;
    vec4<T> _angular_velocity;
    vec4<T> _inertia;
    vec4<T> _inv_inertia;

  public:
    body(const vec4<T> &center, const T mass, const vec4<T> inertia) : body_base<T, vec4>(center, mass), _inertia(inertia), _inv_inertia(inertia.inverse()) {}
    inline void add_torque(const vec4<T> &torque)
    {
        _torques.push_back(torque);
    }
    inline void add_torque(const vec4<T> &force, const vec4<T> &contact)
    {
        _torques.push_back((contact - this->_position).cross(force));
    }
    inline void clear_torque()
    {
        _torques.clear();
    }
    inline const vec4<T> get_angular_acceleration(const vec4<T> &angular_velocity, const T damping) const
    {
        vec4<T> sum;

        // Sum all torques around this object's center of mass
        for (const auto &torque : _torques)
        {
            sum += torque;
        }

        // Calculate the acceleration
        return (sum - angular_velocity * damping) * _inv_inertia;
    }
    inline const vec4<T> &get_angular_velocity() const
    {
        return _angular_velocity;
    }
    inline void set_angular_velocity(const vec4<T> &w)
    {
        _angular_velocity = w;
    }
    inline const vec4<T> get_inertia() const
    {
        return _inertia.inverse();
    }
    inline const vec4<T> &get_inv_inertia() const
    {
        return _inv_inertia;
    }
    inline const quat<T> &get_rotation() const
    {
        return _rotation;
    }
    inline quat<T> update_rotation(const vec4<T> &angular_velocity, const T time_step)
    {
        _angular_velocity = angular_velocity;

        // Calculate rotation for this timestep
        const vec3<T> rotation = (_angular_velocity * time_step).xyz();

        // Creation quaternion rotation
        const T angle = rotation.magnitude();
        const quat<T> q(rotation, angle);

        // Transform the absolute rotation
        _rotation *= q;

        // Normalize the rotation vector to avoid accumulation of rotational energy
        _rotation.normalize();

        // Update rotational inertia axis of rotation
        _inv_inertia = _rotation.transform(_inertia).inverse();

        // return the relative rotation
        return q;
    }
};

template <typename T>
inline T get_inertia(const aabbox<T, vec2> &box, const T mass)
{
    // Iz = (1/12) * (x^2 + y^2)
    const vec3<T> &b = box.get_extent();
    return (b.x() * b.x() + b.y() * b.y()) * 0.0833;
}

template <typename T>
inline vec3<T> get_inertia(const aabbox<T, vec3> &box, const T mass)
{
    // Ix = (1/12) * (y^2 + z^2)
    // Iy = (1/12) * (x^2 + z^2)
    // Iz = (1/12) * (x^2 + y^2)
    const vec3<T> &b = box.get_extent();
    const T x2 = b.x() * b.x();
    const T y2 = b.y() * b.y();
    const T z2 = b.z() * b.z();
    return vec3<T>(y2 + z2, x2 + z2, x2 + y2) * 0.0833;
}

template <typename T>
inline vec4<T> get_inertia(const aabbox<T, vec4> &box, const T mass)
{
    // Ix = (1/12) * (y^2 + z^2)
    // Iy = (1/12) * (x^2 + z^2)
    // Iz = (1/12) * (x^2 + y^2)
    const vec3<T> &b = box.get_extent();
    const T x2 = b.x() * b.x();
    const T y2 = b.y() * b.y();
    const T z2 = b.z() * b.z();
    return vec3<T>(y2 + z2, x2 + z2, x2 + y2) * 0.0833;
}

template <typename T>
inline T get_inertia(const sphere<T, vec2> &s, const T mass)
{
    // Iz = m*r^2/2
    return mass * s.get_square_radius() * 0.25;
}

template <typename T>
inline vec3<T> get_inertia(const sphere<T, vec3> &s, const T mass)
{
    // Ixyz = (2.0/5.0)*m*r^2
    T inertia = s.get_square_radius() * mass * 0.4;
    return vec3<T>().set_all(inertia);
}

template <typename T>
inline vec4<T> get_inertia(const sphere<T, vec4> &s, const T mass)
{
    // Ixyz = (2.0/5.0)*m*r^2
    T inertia = s.get_square_radius() * mass * 0.4;
    return vec4<T>().set_all(inertia);
}

// Can't rotate an AABB reliably so the default implementation is empty
template <typename T>
inline void rotate(aabbox<T, vec2> &box, const mat2<T> &mat) {}

template <typename T>
inline void rotate(aabbox<T, vec3> &box, const quat<T> &q) {}

template <typename T>
inline void rotate(aabbox<T, vec4> &box, const quat<T> &q) {}

// No need to rotate sphere shape so these are empty on purpose
template <typename T>
inline void rotate(const sphere<T, vec2> &s, const mat2<T> &mat) {}

template <typename T>
inline void rotate(const sphere<T, vec3> &s, const quat<T> &q) {}

template <typename T>
inline void rotate(const sphere<T, vec4> &s, const quat<T> &q) {}

template <typename T>
inline T inverse(const T x)
{
    return 1.0 / x;
}

template <typename T>
inline vec3<T> inverse(const vec3<T> &v)
{
    T x = 1.0 / v.x();
    T y = 1.0 / v.y();
    T z = 1.0 / v.z();

    return vec3<T>(x, y, z);
}

template <typename T>
inline vec4<T> inverse(const vec4<T> &v)
{
    T x = 1.0 / v.x();
    T y = 1.0 / v.y();
    T z = 1.0 / v.z();

    return vec4<T>(x, y, z, 1.0);
}

template <typename T>
inline vec2<T> cross(const T w, const vec2<T> &r)
{
    return r.orthogonal() * w;
}

template <typename T>
inline vec3<T> cross(const vec3<T> &w, const vec3<T> &r)
{
    return w.cross(r);
}

template <typename T>
inline vec4<T> cross(const vec4<T> &w, const vec4<T> &r)
{
    return w.cross(r);
}

template <typename T>
inline T dot(const T v1, const T v2)
{
    return v1 * v2;
}

template <typename T>
inline T dot(const vec3<T> &v1, const vec3<T> &v2)
{
    return v1.dot(v2);
}

template <typename T>
inline T dot(const vec4<T> &v1, const vec4<T> &v2)
{
    return v1.dot(v2);
}

template <typename T, typename K, typename L, template <typename> class vec, template <typename, template <typename> class> class cell, template <typename, template <typename> class> class shape,
          template <typename, typename, typename, template <typename> class, template <typename, template <typename> class> class, template <typename, template <typename> class> class> class spatial>
class physics
{
  private:
    spatial<T, K, L, vec, cell, shape> _spatial;
    std::vector<shape<T, vec>> _shapes;
    std::vector<body<T, vec>> _bodies;
    vec<T> _lower_bound;
    vec<T> _upper_bound;
    vec<T> _gravity;
    T _elasticity;

    static constexpr T _collision_tolerance = 1E-1;

    inline void add_force(body<T, vec> &b, const vec<T> &force)
    {
        b.add_force(force);
    }
    inline void add_torque(body<T, vec> &b, const vec<T> &force, const vec<T> &contact)
    {
        b.add_torque(force, contact);
    }

    inline void collide(const size_t index1, const size_t index2)
    {
        // Get shapes from spatial index
        const shape<T, vec> &s1 = _shapes[index1];
        const shape<T, vec> &s2 = _shapes[index2];

        // Calculate...
        // 1) the collision normal vector that points toward b1
        // 2) the two closest points between bodies
        // 3) the offset to resolve the collision

        vec<T> collision_normal;
        vec<T> intersection;
        vec<T> offset = resolve<T, vec>(s1, s2, collision_normal, intersection, _collision_tolerance);

        // Get rigid bodies to solve energy equations
        body<T, vec> &b1 = _bodies[index1];
        body<T, vec> &b2 = _bodies[index2];

        // Solve linear and angular momentum conservation equations
        solve_energy_conservation(b1, b2, collision_normal, intersection);

        // Resolve collision and resolve penetration depth
        b1.move_offset(offset);
    }

    // The normal axis is defined to be the vector between b1 and b2, pointing towards b1
    // n = C1 - C2
    // These equations yield an impulse J, which is a change in linear momentum
    // This impulse at point P creates a change in angular momentum
    // J1 = n * m1(v1' - v1)
    // J2 = n * m2(v2' - v2)
    // F = J/dt
    // dL1 = (P - C1) x J1
    // dL2 = (P - C2) x J2

    // Intersection point intersect
    inline void solve_energy_conservation(body<T, vec> &b1, body<T, vec> &b2, const vec<T> &n, const vec<T> &intersect)
    {
        // Get velocities of bodies
        const T v1n = b1.get_linear_velocity().dot(n);
        const T v2n = b2.get_linear_velocity().dot(n);

        // If objects are moving very slowly, skip calculation
        // If objects are moving away from each other, skip calculation
        if (v1n >= -_collision_tolerance && v2n <= _collision_tolerance)
        {
            return;
        }

        // If objects are moving in the same direction, skip calculation
        if (std::abs(v1n - v2n) <= _collision_tolerance)
        {
            return;
        }

        // Get inverse masses of bodies
        const T inv_m1 = b1.get_inv_mass();
        const T inv_m2 = b2.get_inv_mass();

        // Get velocities of bodies
        const vec<T> v1 = b1.get_linear_velocity();
        const vec<T> v2 = b2.get_linear_velocity();

        // Get inverse inertia of bodies
        const auto inv_I1 = b1.get_inv_inertia();
        const auto inv_I2 = b2.get_inv_inertia();

        // Get angular velocities of bodies
        const auto w1 = b1.get_angular_velocity();
        const auto w2 = b2.get_angular_velocity();

        // Calculate the vector from the intersection point and object center
        const vec<T> r1 = (intersect - b1.get_position()).normalize();
        const vec<T> r2 = (intersect - b2.get_position()).normalize();

        // Calculate the relative velocity between b1 and b2
        const vec<T> v12 = (v1 + cross<T>(w1, r1)) - (v2 + cross<T>(w2, r2));

        const auto r1n = r1.cross(n);
        const auto r2n = r2.cross(n);
        const auto r1i = r1n * inv_I1;
        const auto r2i = r2n * inv_I2;

        // (A x B)^2 = (A X B) * (A X B)
        const T r1r = dot<T>(r1i, r1n);
        const T r2r = dot<T>(r2i, r2n);

        // Calculate the kinetic resistance of the object
        const T resistance = inv_m1 + inv_m2 + r1r + r2r;

        // Calculate the impulse
        const T j = -(1.0 + _elasticity) * (v12.dot(n) / resistance);

        // Calculate the impulse vector
        const vec<T> impulse = n * j;

        // Calculate linear velocity vectors
        const vec<T> v1_out = v1 + impulse * inv_m1;
        const vec<T> v2_out = v2 - impulse * inv_m2;

        // Calculate angular velocity vectors
        const auto w1_out = w1 + r1i * j;
        const auto w2_out = w2 - r2i * j;

        // Update body linear velocity
        b1.set_linear_velocity(v1_out);
        b2.set_linear_velocity(v2_out);

        // Update body angular velocity
        b1.set_angular_velocity(w1_out);
        b2.set_angular_velocity(w2_out);
    }

  public:
    physics(const cell<T, vec> &world, const vec<T> &gravity) : _spatial(world),
                                                                _lower_bound(world.get_min() + vec<T>().set_all(1.0)),
                                                                _upper_bound(world.get_max() - vec<T>().set_all(1.0)),
                                                                _gravity(gravity), _elasticity(1.0) {}
    inline size_t add_body(const shape<T, vec> &s, const T mass)
    {
        // Add shape to shape vector
        _shapes.push_back(s);

        // Create rigid body for this shape
        _bodies.emplace_back(s.get_center(), mass, get_inertia(s, mass));

        // return the body id
        return _bodies.size() - 1;
    }
    inline void add_force(size_t index, const vec<T> &f, const vec<T> &contact)
    {
        add_force(_bodies[index], f);
        add_torque(_bodies[index], f, contact);
    }
    inline const body<T, vec> &get_body(size_t index) const
    {
        return _bodies[index];
    }
    inline const std::vector<body<T, vec>> &get_bodies() const
    {
        return _bodies;
    }
    inline const shape<T, vec> &get_shape(size_t index) const
    {
        return _shapes[index];
    }
    inline void reserve(const size_t N)
    {
        // Reserve memory for shapes and bodies
        _shapes.reserve(N);
        _bodies.reserve(N);
    }
    inline void solve(const T dt, const T damping)
    {
        if (_shapes.size() > 0)
        {
            // Create the spatial partitioning structure based off rigid bodies
            // This reorders the shapes vector so we need to reorganize the shape and body data to reflect this!
            std::vector<size_t> map = _spatial.insert(_shapes);

            // Determine intersecting shapes for contact resolution
            const std::vector<std::pair<K, K>> &collisions = _spatial.get_collisions();

            // Handle all collisions between objects
            for (const auto &c : collisions)
            {
                collide(map[c.first], map[c.second]);
            }

            // Precalculate time constants
            const T dt2 = dt * 0.5;
            const T dt6 = dt * 0.16667;

            if (_bodies.size() != _shapes.size())
            {
                throw std::runtime_error("physics: body and shape sizes are disjoint");
            }

            // Solve the first order initial value problem differential equations with Runge-Kutta4
            const size_t size = _bodies.size();
            for (size_t i = 0; i < size; i++)
            {
                body<T, vec> &b = _bodies[i];
                shape<T, vec> &s = _shapes[i];

                // Solve for angular velocity
                const auto &w_n = b.get_angular_velocity();

                // Evaluate the derivative at different angular velocities
                const auto wk1 = b.get_angular_acceleration(w_n, damping);
                const auto wk2 = b.get_angular_acceleration(w_n + wk1 * dt2, damping);
                const auto wk3 = b.get_angular_acceleration(w_n + wk2 * dt2, damping);
                const auto wk4 = b.get_angular_acceleration(w_n + wk3 * dt, damping);

                // Calculate the angular velocity at this time step
                const auto w_n1 = w_n + (wk1 + (wk2 * 2.0) + (wk3 * 2.0) + wk4) * dt6;

                // Solve for linear velocity
                const auto v_n = b.get_linear_velocity();

                // Evaluate the derivative at different linear velocities
                const auto vk1 = b.get_linear_acceleration(v_n, _gravity, damping);
                const auto vk2 = b.get_linear_acceleration(v_n + vk1 * dt2, _gravity, damping);
                const auto vk3 = b.get_linear_acceleration(v_n + vk2 * dt2, _gravity, damping);
                const auto vk4 = b.get_linear_acceleration(v_n + vk3 * dt, _gravity, damping);

                // Calculate the linear velocity at this time step
                const auto v_n1 = v_n + (vk1 + (vk2 * 2.0) + (vk3 * 2.0) + vk4) * dt6;

                // Update the body position at this timestep
                b.update_position(v_n1, dt, _lower_bound, _upper_bound);

                // Update the body rotation at this timestep
                const auto rel_rotation = b.update_rotation(w_n1, dt);

                // Clear any acting forces on this object
                b.clear_force();
                b.clear_torque();

                // Update the shapes position
                s.set_position(b.get_position());

                // Rotate the shapes by the relative rotation
                rotate<T>(s, rel_rotation);
            }
        }
    }
    inline T get_total_energy() const
    {
        T KE2 = 0.0;
        T PE = 0.0;
        T AE = 0.0;

        for (auto &b : _bodies)
        {
            // Calculate the double kinetic energy = mv^2
            const vec<T> &v = b.get_linear_velocity();
            const T m = b.get_mass();
            KE2 += m * v.dot(v);

            // Calculate the potential energy = -mgh
            PE += m * _gravity.dot(_lower_bound - b.get_position());

            // Calculate the rotational energy
            const auto I = b.get_inertia();
            const auto w = b.get_angular_velocity();
            AE += dot<T>(I * w, w);
        }

        return 0.5 * KE2 + PE + AE;
    }
    void set_elasticity(const T e)
    {
        _elasticity = e;
    }
};
}

#endif