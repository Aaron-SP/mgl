/* Copyright [2013-2018] [Aaron Springstroh, Minimal Graphics Library]

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
#include <functional>
#include <min/intersect.h>
#include <min/template_math.h>
#include <stdexcept>
#include <vector>

namespace min
{

union body_data {
    size_t index;
    void *ptr;
    int32_t sign;
    body_data() : ptr(nullptr) {}
    body_data(const size_t i) : index(i) {}
    body_data(void *const p) : ptr(p) {}
    body_data(const int32_t i) : sign(i) {}
};

template <typename T, template <typename> class vec, class angular, template <typename> class rot>
class body_base
{
  protected:
    vec<T> _force;
    angular _torque;
    vec<T> _position; // This is at the center of mass
    rot<T> _rotation;
    vec<T> _linear_velocity;
    angular _angular_velocity;
    T _mass;
    T _inv_mass;
    angular _inertia;
    angular _inv_inertia;
    size_t _id;
    body_data _data;
    bool _dead;

  public:
    body_base(const vec<T> &center, const vec<T> &gravity, const T mass, const angular &inertia, const size_t id, const body_data data)
        : _force(gravity * mass), _torque{}, _position(center), _angular_velocity{},
          _mass(mass), _inv_mass(1.0 / mass), _inertia(inertia), _inv_inertia(inverse<T>(inertia)),
          _id(id), _data(data), _dead(false) {}

    inline void add_force(const vec<T> &force)
    {
        // Add force to force vector
        _force += force;
    }
    inline void add_torque(const vec<T> &local_torque)
    {
        // Add local torque to torque vector
        _torque += local_torque;
    }
    inline void add_torque(const vec<T> &force, const vec<T> &contact)
    {
        // Calculate the torque in world space
        const auto torque = (contact - _position).cross(force);

        // Convert the world space torque to object space
        const auto local_torque = min::align<T>(torque, _rotation);

        // Add local torque to torque vector
        _torque += local_torque;
    }
    inline vec<T> align(const vec<T> &v) const
    {
        // Transform the point in object space
        return _rotation.inverse().transform(v);
    }
    inline void clear_force(const vec<T> &gravity)
    {
        // Gravity = mg
        _force = gravity * _mass;
    }
    inline void clear_torque()
    {
        _torque = angular{};
    }
    inline void clear_no_force()
    {
        // Set no force on this object
        _force = vec<T>();

        // Clear all linear velocity
        _linear_velocity = vec<T>();

        // Clear all torques
        _torque = angular{};

        // Clear all rotational  velocity
        _angular_velocity = angular{};
    }
    inline const angular get_angular_acceleration(const angular angular_velocity, const T damping) const
    {
        // Calculate the acceleration
        return (_torque - angular_velocity * damping) * _inv_inertia;
    }
    inline const angular &get_angular_velocity() const
    {
        return _angular_velocity;
    }
    inline body_data get_data() const
    {
        return _data;
    }
    inline size_t get_id() const
    {
        return _id;
    }
    inline const vec<T> get_linear_acceleration(const vec<T> &linear_velocity, const T damping) const
    {
        // Calculate the acceleration
        return (_force - linear_velocity * damping) * _inv_mass;
    }
    inline const vec<T> &get_linear_velocity() const
    {
        return _linear_velocity;
    }
    inline const T get_mass() const
    {
        return _mass;
    }
    inline const T get_inv_mass() const
    {
        return _inv_mass;
    }
    inline const angular &get_inertia() const
    {
        // In object coordinates
        return _inertia;
    }
    inline const angular &get_inv_inertia() const
    {
        // In object coordinates
        return _inv_inertia;
    }
    inline const rot<T> &get_rotation() const
    {
        return _rotation;
    }
    inline const vec<T> &get_position() const
    {
        return _position;
    }
    inline bool is_dead() const
    {
        return _dead;
    }
    inline void kill()
    {
        _dead = true;
    }
    inline void set_angular_velocity(const angular w)
    {
        _angular_velocity = w;
    }
    inline void set_data(const body_data data)
    {
        _data = data;
    }
    inline void set_linear_velocity(const vec<T> &v)
    {
        _linear_velocity = v;
    }
    inline void set_no_move()
    {
        // Make the object's mass infinite
        _inv_mass = 0.0;
        _mass = 0.0;
    }
    inline void set_no_rotate()
    {
        // Make the object's inertia infinite
        _inv_inertia = angular{};
        _inertia = angular{};
    }
    inline void set_position(const vec<T> &p)
    {
        _position = p;
    }
    inline void set_rotation(const rot<T> &r)
    {
        _rotation = r;
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
        const vec<T> direction = _position.clamp_direction(min, max);

        // Reverses linear velocity if hit edge of world
        _linear_velocity = linear_velocity * direction;
    }
};

template <typename T, template <typename> class vec>
class body : public body_base<T, vec, vec<T>, quat>
{
};

// Partial specialization for resolving the type of angular_velocity for vec2 = T
template <typename T>
class body<T, vec2> : public body_base<T, vec2, T, mat2>
{
  private:
    std::function<void(body<T, vec2> &, body<T, vec2> &)> _f;

  public:
    body(const vec2<T> &center, const vec2<T> &gravity, const T mass, const T inertia, const size_t id, const body_data data)
        : body_base<T, vec2, T, mat2>(center, gravity, mass, inertia, id, data), _f(nullptr) {}

    inline void callback(body<T, vec2> &b2)
    {
        // If we registered a callback
        if (this->_f)
        {
            this->_f(*this, b2);
        }
    }
    inline void register_callback(const std::function<void(body<T, vec2> &, body<T, vec2> &)> &f)
    {
        this->_f = f;
    }
    inline mat2<T> update_rotation(const T angular_velocity, const T time_step)
    {
        this->_angular_velocity = angular_velocity;

        // Rotation is around the Z axis in euler angles
        const mat2<T> out(this->_angular_velocity * time_step);

        // Transform the absolute rotation
        this->_rotation *= out;

        // return the relative rotation
        return this->_rotation;
    }
};

// Partial specialization for resolving the type of angular_velocity for vec3 = vec3<T>
template <typename T>
class body<T, vec3> : public body_base<T, vec3, vec3<T>, quat>
{
  private:
    std::function<void(body<T, vec3> &, body<T, vec3> &)> _f;

  public:
    body(const vec3<T> &center, const vec3<T> &gravity, const T mass, const vec3<T> &inertia, const size_t id, const body_data data)
        : body_base<T, vec3, vec3<T>, quat>(center, gravity, mass, inertia, id, data), _f(nullptr) {}

    inline void callback(body<T, vec3> &b2)
    {
        // If we registered a callback
        if (this->_f)
        {
            this->_f(*this, b2);
        }
    }
    inline void register_callback(const std::function<void(body<T, vec3> &, body<T, vec3> &)> &f)
    {
        this->_f = f;
    }
    inline quat<T> update_rotation(const vec3<T> &angular_velocity, const T time_step)
    {
        this->_angular_velocity = angular_velocity;

        // Calculate rotation for this timestep
        vec3<T> rotation = this->_angular_velocity * time_step;

        // Calculate rotation angle for angular velocity
        const T angle = rotation.magnitude();
        if (angle > var<T>::TOL_REL)
        {
            // Normalize rotation axis
            const T inv_angle = 1.0 / angle;
            rotation *= inv_angle;

            // Create quaternion rotation with angle
            const quat<T> q(rotation, angle);

            // Transform the absolute rotation
            this->_rotation *= q;

            // Normalize the rotation vector to avoid accumulation of rotational energy
            this->_rotation.normalize();
        }

        // return the absolute rotation
        return this->_rotation;
    }
};

// Partial specialization for resolving the type of angular_velocity for vec4 = vec4<T>
template <typename T>
class body<T, vec4> : public body_base<T, vec4, vec4<T>, quat>
{
  private:
    std::function<void(body<T, vec4> &, body<T, vec4> &)> _f;

  public:
    body(const vec4<T> &center, const vec4<T> &gravity, const T mass, const vec4<T> &inertia, const size_t id, const body_data data)
        : body_base<T, vec4, vec4<T>, quat>(center, gravity, mass, inertia, id, data), _f(nullptr) {}

    inline void callback(body<T, vec4> &b2)
    {
        // If we registered a callback
        if (this->_f)
        {
            this->_f(*this, b2);
        }
    }
    inline void register_callback(const std::function<void(body<T, vec4> &, body<T, vec4> &)> &f)
    {
        this->_f = f;
    }
    inline quat<T> update_rotation(const vec4<T> &angular_velocity, const T time_step)
    {
        this->_angular_velocity = angular_velocity;

        // Calculate rotation for this timestep
        vec3<T> rotation = this->_angular_velocity * time_step;

        // Calculate rotation angle for angular velocity
        const T angle = rotation.magnitude();
        if (angle > var<T>::TOL_REL)
        {
            // Normalize rotation axis
            const T inv_angle = 1.0 / angle;
            rotation *= inv_angle;

            // Create quaternion rotation with angle
            const quat<T> q(rotation, angle);

            // Transform the absolute rotation
            this->_rotation *= q;

            // Normalize the rotation vector to avoid accumulation of rotational energy
            this->_rotation.normalize();
        }

        // return the absolute rotation
        return this->_rotation;
    }
};

template <typename T, typename K, typename L, template <typename> class vec, template <typename, template <typename> class> class cell, template <typename, template <typename> class> class shape,
          template <typename, typename, typename, template <typename> class, template <typename, template <typename> class> class, template <typename, template <typename> class> class> class spatial>
class physics
{
  private:
    spatial<T, K, L, vec, cell, shape> _spatial;
    std::vector<shape<T, vec>> _shapes;
    std::vector<body<T, vec>> _bodies;
    std::vector<size_t> _dead;
    vec<T> _lower_bound;
    vec<T> _upper_bound;
    vec<T> _gravity;
    T _elasticity;
    bool _clean;

    static constexpr T _collision_tolerance = 1E-4;

    inline void collide(const size_t index1, const size_t index2)
    {
        // Get rigid bodies to solve energy equations
        body<T, vec> &b1 = _bodies[index1];
        body<T, vec> &b2 = _bodies[index2];

        // Check if either body has died
        if (b1.is_dead() || b2.is_dead())
        {
            return;
        }

        // Get shapes from spatial index
        const shape<T, vec> &s1 = _shapes[index1];
        const shape<T, vec> &s2 = _shapes[index2];

        // Calculate...
        // 1) the collision normal vector that points toward b1
        // 2) the intersection point between bodies
        // 3) the offset to resolve the collision

        vec<T> collision_normal;
        vec<T> intersection;
        const vec<T> offset = resolve<T, vec>(s1, s2, collision_normal, intersection, _collision_tolerance);

        // Do the collision callback function
        b1.callback(b2);
        b2.callback(b1);

        // Solve linear and angular momentum conservation equations
        solve_energy_conservation(b1, b2, collision_normal, intersection);

        // If an object has infinite mass, inv_mass = 0
        // Move each object based off inv_mass
        // Treat this as a parallel circuit, 1/R = 1/R_1 + 1/R_2
        // For this case V = 1.0
        // The current through this circuit is V/R = I, or 1.0*(1/R_1 + 1/R_2) = 1/R_1 + 1/R_2 = I
        // The percentage split can be calculated from (I - 1/R_x) / I

        // Split offset in half and move b1 and b2 in opposite directions
        const T total = b1.get_inv_mass() + b2.get_inv_mass();
        if (total > var<T>::TOL_ZERO)
        {
            const T inv_total = 1.0 / total;
            const vec<T> half_offset1 = offset * (total - b2.get_inv_mass()) * inv_total;
            const vec<T> half_offset2 = offset * (b1.get_inv_mass() - total) * inv_total;

            // Resolve collision and resolve penetration depth
            b1.move_offset(half_offset1);
            b2.move_offset(half_offset2);
        }
    }
    inline bool collide_static(const size_t index, const shape<T, vec> &s2)
    {
        // Get rigid bodies to solve energy equations
        body<T, vec> &b = _bodies[index];

        // Check if body has died
        if (b.is_dead())
        {
            return false;
        }

        // Get shapes from spatial index
        const shape<T, vec> &s1 = _shapes[index];

        // Test if the body is intersecting the static shape
        const bool collide = intersect(s1, s2);
        if (collide)
        {
            // Calculate...
            // 1) the collision normal vector that points toward body
            // 2) the intersection point between body and static shape
            // 3) the offset to resolve the collision

            vec<T> collision_normal;
            vec<T> intersection;
            const vec<T> offset = resolve<T, vec>(s1, s2, collision_normal, intersection, _collision_tolerance);

            // Solve linear and angular momentum conservation equations
            solve_energy_conservation_static(b, collision_normal, intersection);

            // Move based off calculated offfset
            b.move_offset(offset);
        }

        // return if we collided
        return collide;
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
        // Get velocities of bodies in world space
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

        // Get velocities of bodies in world space
        const vec<T> &v1 = b1.get_linear_velocity();
        const vec<T> &v2 = b2.get_linear_velocity();

        // Get inverse inertia of bodies in object space
        const auto &inv_I1 = b1.get_inv_inertia();
        const auto &inv_I2 = b2.get_inv_inertia();

        // Get angular velocities of bodies in object space
        const auto &w1_local = b1.get_angular_velocity();
        const auto &w2_local = b2.get_angular_velocity();

        // convert angular velocity to world space
        const auto w1_world = transform<T>(w1_local, b1.get_rotation());
        const auto w2_world = transform<T>(w2_local, b2.get_rotation());

        // Calculate the vector from the intersection point and object center in object coordinates
        const vec<T> r1 = (intersect - b1.get_position()).normalize_safe(vec<T>());
        const vec<T> r2 = (intersect - b2.get_position()).normalize_safe(vec<T>());

        // Calculate the relative velocity between b1 and b2 in world space
        const vec<T> v12 = (v1 + cross<T>(w1_world, r1)) - (v2 + cross<T>(w2_world, r2));

        // Convert cross product into object space since inertia is in object space
        const auto r1n = align<T>(r1.cross(n), b1.get_rotation());
        const auto r2n = align<T>(r2.cross(n), b2.get_rotation());
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
        const auto w1_out = w1_local + r1i * j;
        const auto w2_out = w2_local - r2i * j;

        // Update body linear velocity
        b1.set_linear_velocity(v1_out);
        b2.set_linear_velocity(v2_out);

        // Update body angular velocity
        b1.set_angular_velocity(w1_out);
        b2.set_angular_velocity(w2_out);
    }

    // Collision with object of infinite mass
    inline void solve_energy_conservation_static(body<T, vec> &b, const vec<T> &n, const vec<T> &intersect)
    {
        // Get velocities of bodies in world space
        const T v1n = b.get_linear_velocity().dot(n);

        // If objects are moving very slowly, skip calculation
        // If objects are moving away from each other, skip calculation
        if (v1n >= -_collision_tolerance)
        {
            return;
        }

        // If object is not moving, skip calculation
        if (std::abs(v1n) <= _collision_tolerance)
        {
            return;
        }

        // Get inverse masses of bodies
        const T inv_m = b.get_inv_mass();

        // Get velocities of bodies in world space
        const vec<T> &v = b.get_linear_velocity();

        // Get inverse inertia of bodies in object space
        const auto &inv_I = b.get_inv_inertia();

        // Get angular velocities of bodies in object space
        const auto &w_local = b.get_angular_velocity();

        // convert angular velocity to world space
        const auto w_world = transform<T>(w_local, b.get_rotation());

        // Calculate the vector from the intersection point and object center in object coordinates
        const vec<T> r = (intersect - b.get_position()).normalize_safe(vec<T>());

        // Calculate the relative velocity of body
        const vec<T> v_rel = (v + cross<T>(w_world, r));

        // Convert cross product into object space since inertia is in object space
        const auto rn = align<T>(r.cross(n), b.get_rotation());
        const auto ri = rn * inv_I;

        // (A x B)^2 = (A X B) * (A X B)
        const T rr = dot<T>(ri, rn);

        // Calculate the kinetic resistance of the object
        const T resistance = inv_m + rr;

        // Calculate the impulse
        const T j = -(1.0 + _elasticity) * (v_rel.dot(n) / resistance);

        // Calculate the impulse vector
        const vec<T> impulse = n * j;

        // Calculate linear velocity vectors
        const vec<T> v_out = v + impulse * inv_m;

        // Calculate angular velocity vectors
        const auto w_out = w_local + ri * j;

        // Update body linear velocity
        b.set_linear_velocity(v_out);

        // Update body angular velocity
        b.set_angular_velocity(w_out);
    }
    inline void solve_integrals(const size_t index, const T dt, const T damping)
    {
        // Check if body has died
        body<T, vec> &b = _bodies[index];
        if (b.is_dead())
        {
            return;
        }

        // Precalculate time constants
        const T dt2 = dt * 0.5;
        const T dt6 = dt * 0.16667;

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
        const auto vk1 = b.get_linear_acceleration(v_n, damping);
        const auto vk2 = b.get_linear_acceleration(v_n + vk1 * dt2, damping);
        const auto vk3 = b.get_linear_acceleration(v_n + vk2 * dt2, damping);
        const auto vk4 = b.get_linear_acceleration(v_n + vk3 * dt, damping);

        // Calculate the linear velocity at this time step
        const auto v_n1 = v_n + (vk1 + (vk2 * 2.0) + (vk3 * 2.0) + vk4) * dt6;

        // Update the body position at this timestep
        b.update_position(v_n1, dt, _lower_bound, _upper_bound);

        // Update the body rotation at this timestep
        const auto abs_rotation = b.update_rotation(w_n1, dt);

        // Clear any acting forces on this object
        b.clear_force(_gravity);
        b.clear_torque();

        // Update the shapes position
        shape<T, vec> &s = _shapes[index];
        s.set_position(b.get_position());

        // Rotate the shapes by the relative rotation
        rotate<T>(s, abs_rotation);
    }
    inline void solve_integrals(const T dt, const T damping)
    {
        // Solve the first order initial value problem differential equations with Runge-Kutta4
        const size_t size = _bodies.size();
        for (size_t i = 0; i < size; i++)
        {
            solve_integrals(i, dt, damping);
        }
    }

  public:
    physics(const cell<T, vec> &world, const vec<T> &gravity)
        : _spatial(world),
          _lower_bound(world.get_min() + vec<T>().set_all(1.0)),
          _upper_bound(world.get_max() - vec<T>().set_all(1.0)),
          _gravity(gravity), _elasticity(1.0), _clean(true) {}

    inline size_t add_body(const shape<T, vec> &s, const T mass, const size_t id = 0, const body_data data = nullptr)
    {
        // If bodies can be recycled
        if (_dead.size() > 0)
        {
            // Get recycle index
            const size_t index = _dead.back();
            _dead.pop_back();

            // Flag clean if no more dead bodies
            if (_dead.size() == 0)
            {
                _clean = true;
            }

            // Recycle shape
            _shapes[index] = s;

            // Recycle body
            _bodies[index] = body<T, vec>(s.get_center(), _gravity, mass, get_inertia(s, mass), id, data);

            // Return recycled index
            return index;
        }

        // Add shape to shape vector
        _shapes.push_back(s);

        // Create rigid body for this shape
        _bodies.emplace_back(s.get_center(), _gravity, mass, get_inertia(s, mass), id, data);

        // return the body id
        return _bodies.size() - 1;
    }
    inline void clear_body(const size_t index)
    {
        // Flag this body for destruction
        _bodies[index].kill();

        // Add body index to the dead list
        _dead.push_back(index);

        // Flag that we dirtied up
        _clean = false;
    }
    inline void clear()
    {
        // Clear out the shapes
        _shapes.clear();

        // Clear out the bodies
        _bodies.clear();
    }
    inline bool collide(const size_t index, const shape<T, vec> &s)
    {
        // return whether we collided or not
        return collide_static(index, s);
    }
    inline const body<T, vec> &get_body(const size_t index) const
    {
        return _bodies[index];
    }
    inline body<T, vec> &get_body(const size_t index)
    {
        return _bodies[index];
    }
    inline const std::vector<body<T, vec>> &get_bodies() const
    {
        return _bodies;
    }
    inline std::vector<body<T, vec>> &get_bodies()
    {
        return _bodies;
    }
    inline const shape<T, vec> &get_shape(const size_t index) const
    {
        return _shapes[index];
    }
    inline void prune_after(const size_t index)
    {
        // Check if we need to prune
        if (!_clean)
        {
            prune_after_force(index);
        }
    }
    inline void prune_after_force(const size_t index)
    {
        // Clear all dead bodies
        _dead.clear();

        // Shorten shape and body buffers
        const size_t size = index + 1;
        _shapes.resize(size);
        _bodies.resize(size);

        // Scan for dead bodies in remnants
        for (size_t i = 0; i < size; i++)
        {
            if (_bodies.is_dead())
            {
                _dead.push_back(i);
            }
        }

        // Flag that we cleaned up
        _clean = true;
    }
    inline void register_callback(const size_t index, const std::function<void(body<T, vec> &, body<T, vec> &)> &f)
    {
        _bodies[index].register_callback(f);
    }
    inline void reserve(const size_t size)
    {
        // Reserve memory for shapes and bodies
        _shapes.reserve(size);
        _bodies.reserve(size);
        _dead.reserve(size);
    }
    inline void solve(const T dt, const T damping)
    {
        if (_shapes.size() > 0)
        {
            // Create the spatial partitioning structure based off rigid bodies
            // This reorders the shapes vector so we need to reorganize the shape and body data to reflect this!
            _spatial.insert(_shapes);

            // Get the index map for reordering
            const std::vector<size_t> &map = _spatial.get_index_map();

            // Determine intersecting shapes for contact resolution
            const std::vector<std::pair<K, K>> &collisions = _spatial.get_collisions();

            // Handle all collisions between objects
            for (const auto &c : collisions)
            {
                collide(map[c.first], map[c.second]);
            }

            // Solve the simulation
            solve_integrals(dt, damping);
        }
    }
    inline void solve_no_collide(const T dt, const T damping)
    {
        // Solve the simulation
        solve_integrals(dt, damping);
    }
    inline void solve_no_sort(const T dt, const T damping)
    {
        if (_shapes.size() > 0)
        {
            // Create the spatial partitioning structure based off rigid bodies
            // This doesn't reorder the shapes vector
            _spatial.insert_no_sort(_shapes);

            // Determine intersecting shapes for contact resolution
            const std::vector<std::pair<K, K>> &collisions = _spatial.get_collisions();

            // Handle all collisions between objects
            for (const auto &c : collisions)
            {
                collide(c.first, c.second);
            }

            // Solve the simulation
            solve_integrals(dt, damping);
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
    inline void set_elasticity(const T e)
    {
        _elasticity = e;
    }
};
}

#endif
