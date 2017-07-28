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
#ifndef __MD5_MODEL__
#define __MD5_MODEL__

#include <min/md5_anim.h>
#include <min/md5_mesh.h>
#include <min/model.h>
#include <vector>

namespace min
{

template <typename T, typename K, template <typename> class vec, template <typename, template <typename> class> class bound>
class md5_model : public model<T, K, vec, bound>
{
  protected:
    std::vector<mat4<T>> _inverse_bp;
    mutable std::vector<mat4<T>> _bones;
    std::vector<md5_anim<T>> _animations;
    size_t _current;

    void check_bones()
    {
        // Check size of animated components
        for (const auto &m : this->_mesh)
        {
            const size_t size = m.vertex.size();
            if (m.bone_index.size() != size || m.bone_weight.size() != size)
            {
                throw std::runtime_error("md5_model: bone parameters are not of appropriate length");
            }
        }
    }
    void make_bind_pose(const std::vector<md5_joint<T>> &joints)
    {
        // Allocate bone matrices
        _inverse_bp.reserve(joints.size());
        _bones.resize(joints.size());

        // Calculate the inverse bind pose matrices
        for (const auto &joint : joints)
        {
            // Create inverse transformation matrix
            mat4<T> bone(joint.get_position(), joint.get_rotation());

            // Check if matrix has an inverse
            if (!bone.invert())
            {
                throw std::runtime_error("md5_model: uninvertable matrix found while calculating inverse bind-pose");
            }

            // Save the inverse matrix for interpolation
            _inverse_bp.push_back(bone);
        }
    }

  public:
    // This will steal data from provider
    md5_model(md5_mesh<T, K> &&model) : model<T, K, vec, bound>(std::move(model.get_meshes()))
    {
        // Joints are thrown away after this
        make_bind_pose(model.get_joints());

        // Check the mesh bone dimensions
        check_bones();
    }
    md5_model(const md5_mesh<T, K> &model) : model<T, K, vec, bound>(model.get_meshes())
    {
        // Joints are thrown away after this
        make_bind_pose(model.get_joints());

        // Check the mesh bone dimensions
        check_bones();
    }
    inline const std::vector<mat4<T>> &get_bones() const
    {
        return _bones;
    }
    const md5_anim<T> &get_current_animation() const
    {
        return _animations[_current];
    }
    inline void load_animation(const std::string &file)
    {
        // Load animation in place
        _animations.emplace_back(file);

        // Update the current animation
        _current = _animations.size() - 1;

        // Get current frame of animation
        const std::vector<mat4<T>> &frame = _animations[_current].get_current_frame();

        // Validate that the animation frame size matches the model bones
        const size_t size = frame.size();
        if (_bones.size() != size || _inverse_bp.size() != size)
        {
            throw std::runtime_error("md5_model: animation is not compatible with model");
        }
    }
    inline void set_current_animation(const size_t animation)
    {
        // Set animation with index
        _current = animation;
    }
    inline void step(const T time) const
    {
        // Check if we loaded an animation
        if (_animations.size() == 0)
        {
            throw std::runtime_error("md5_model: no animations are loaded");
        }

        // Get the current animation
        const md5_anim<T> &anim = _animations[_current];

        // Step the animation
        anim.step(time);

        // Get current frame of animation
        const std::vector<mat4<T>> &frame = anim.get_current_frame();

        // Check frame size, consider removing since we checked on load
        if (frame.size() != _bones.size())
        {
            throw std::runtime_error("md5_model: animation is not compatible with model");
        }

        // Update the model bones
        const size_t size = frame.size();
        for (size_t i = 0; i < size; i++)
        {
            // Transform the bone based off animation frame
            _bones[i] = _inverse_bp[i] * frame[i];
        }
    }
};
}

#endif
