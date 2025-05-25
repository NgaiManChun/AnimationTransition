#include "camera.h"
#include "camera.h"

namespace MG {
    Camera::Camera(float _near, float _far) : m_near(_near), m_far(_far)
    {
    }

    Camera::~Camera() {}

    float Camera::GetNear() const
    {
        return m_near;
    }

    float Camera::GetFar() const
    {
        return m_far;
    }

    void Camera::SetNear(const float _near)
    {
        if (_near <= 0) {
            m_near = 0.01f;
        }
        else if (_near > m_far) {
            m_near = m_far;
        }
        else {
            m_near = _near;
        }
    }

    void Camera::SetFar(const float _far)
    {
        if (m_near > _far) {
            m_far = m_near;
        }
        else {
            m_far = _far;
        }
    }

    Camera2D::Camera2D(const F2& offset, float _near, float _far) : offset(offset), Camera(_near, _far)
    {
    }

    void Camera2D::SetOffset(const F2& offset)
    {
        this->offset = offset;
    }

    F2 Camera2D::GetOffset() const
    {
        return offset;
    }

    /*Camera3D::Camera3D(F3 position, F3 front, F3 upper, float angle, float _near, float _far)
        : position(position), front(front), upper(upper), angle(angle), Camera(_near, _far)
    {
    }*/

    Camera3D::Camera3D(const F3& position, const Quaternion& rotate, float angle, float _near, float _far)
        : position(position), rotate(rotate), angle(angle), Camera(_near, _far)
    {
        front = Rotate({ 0.0f, 0.0f, 1.0f }, rotate);
        upper = Rotate({ 0.0f, 1.0f, 0.0f }, rotate);
        F3 pitchYawRoll = rotate.GetPitchYawRoll();
        pitch = pitchYawRoll.x;
        yaw = pitchYawRoll.y;
        roll = pitchYawRoll.z;
    }

    F3 Camera3D::GetPosition() const
    {
        return position;
    }

    F3 Camera3D::GetFront() const
    {
        //return Rotate(Rotate({ 0.0f, 0.0f, 1.0f }, Quaternion::AxisXDegree(rotateV)), Quaternion::AxisYDegree(rotateH));
        //return Rotate({ 0.0f, 0.0f, 1.0f }, rotate);
        return front;
    }

    F3 Camera3D::GetUpper() const
    {
        //return Rotate(Rotate({ 0.0f, 1.0f, 0.0f }, Quaternion::AxisXDegree(rotateV)), Quaternion::AxisYDegree(rotateH));
        //return Rotate({ 0.0f, 1.0f, 0.0f }, rotate);
        return upper;
    }

    F3 Camera3D::GetPitchYawRoll() const
    {
        return { pitch, yaw, roll };
    }

    float Camera3D::GetPitch() const
    {
        return pitch;
    }

    float Camera3D::GetYaw() const
    {
        return yaw;
    }

    float Camera3D::GetRoll() const
    {
        return roll;
    }

    Quaternion Camera3D::GetRotate() const
    {
        return rotate;
    }

    float Camera3D::GetAngle() const
    {
        return angle;
    }

    void Camera3D::SetPosition(const F3& position)
    {
        this->position = position;
    }

    void Camera3D::SetAngle(const float degree)
    {
        angle = degree;
    }

    void Camera3D::SetRotate(const Quaternion& r)
    {
        rotate = r;
        front = Rotate({ 0.0f, 0.0f, 1.0f }, rotate);
        upper = Rotate({ 0.0f, 1.0f, 0.0f }, rotate);
        F3 pyr = rotate.GetPitchYawRoll();
        pitch = pyr.x;
        yaw = pyr.y;
        roll = pyr.z;
    }

    void Camera3D::SetPitch(float p)
    {
        pitch = p;
        rotate = Normalize(Quaternion::AxisYRadian(yaw) * Quaternion::AxisXRadian(pitch) * Quaternion::AxisZRadian(roll));
        front = Rotate({ 0.0f, 0.0f, 1.0f }, rotate);
        upper = Rotate({ 0.0f, 1.0f, 0.0f }, rotate);
    }

    void Camera3D::SetYaw(float y)
    {
        yaw = y;
        rotate = Normalize(Quaternion::AxisYRadian(yaw) * Quaternion::AxisXRadian(pitch) * Quaternion::AxisZRadian(roll));
        front = Rotate({ 0.0f, 0.0f, 1.0f }, rotate);
        upper = Rotate({ 0.0f, 1.0f, 0.0f }, rotate);
    }

    void Camera3D::SetRoll(float r)
    {
        roll = r;
        rotate = Normalize(Quaternion::AxisYRadian(yaw) * Quaternion::AxisXRadian(pitch) * Quaternion::AxisZRadian(roll));
        front = Rotate({ 0.0f, 0.0f, 1.0f }, rotate);
        upper = Rotate({ 0.0f, 1.0f, 0.0f }, rotate);
    }

    void Camera3D::SetPitchYawRoll(const F3& pyr)
    {
        pitch = pyr.x;
        yaw = pyr.y;
        roll = pyr.z;
        rotate = Normalize(Quaternion::AxisYRadian(yaw) * Quaternion::AxisXRadian(pitch) * Quaternion::AxisZRadian(roll));
        front = Rotate({ 0.0f, 0.0f, 1.0f }, rotate);
        upper = Rotate({ 0.0f, 1.0f, 0.0f }, rotate);
    }

    void Camera3D::SetFront(const F3& f, const F3& u)
    {
        SetRotate(Quaternion(f, u));
    }

    F2 Camera3D::GetScreenPosition(const F3& worldPosition, const F2& viewSize) const
    {
        F3 pos = Rotate(worldPosition - position, rotate.Inverse());
        float length = tanf(angle * 0.5f / 180.0f * PI) * pos.z * 2.0f;
        pos.x = (pos.x / length) * viewSize.x;
        pos.y = (pos.y / length) * viewSize.y;
        return F2{ pos.x, pos.y };
    }


    /*F3& Camera3D::Position()
    {
        return position;
    }*/

    /*void Camera3D::SetFront(const F3& vector)
    {
        front = Normalize(vector);
    }

    void Camera3D::SetUpper(const F3& vector)
    {
        upper = vector;
    }*/

    

    

}