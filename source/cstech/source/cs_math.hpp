#pragma once

#include "cs_define.hpp"

#define CSM_API CS_API
#define CSM_INLINE CS_INLINE
#define CSM_CAST(t,x) CS_CAST(t,x)
#define CSM_ASSERT(e,msg) CS_ASSERT(e,msg)

#include <csmath/csm_util.hpp>
#include <csmath/csm_vec2.hpp>
#include <csmath/csm_vec3.hpp>
#include <csmath/csm_vec4.hpp>
#include <csmath/csm_mat2.hpp>
#include <csmath/csm_mat3.hpp>
#include <csmath/csm_mat4.hpp>

// Typedef the math types into our namespace for convenience.
CS_PUBLIC_SCOPE
{
    typedef csm::Vec2 Vec2;
    typedef csm::Vec2i Vec2i;
    typedef csm::Vec3 Vec3;
    typedef csm::Vec3i Vec3i;
    typedef csm::Vec4 Vec4;
    typedef csm::Vec4i Vec4i;
    typedef csm::Mat2 Mat2;
    typedef csm::Mat2i Mat2i;
    typedef csm::Mat3 Mat3;
    typedef csm::Mat3i Mat3i;
    typedef csm::Mat4 Mat4;
    typedef csm::Mat4i Mat4i;
}
