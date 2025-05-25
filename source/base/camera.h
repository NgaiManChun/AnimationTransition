#ifndef _H_CAMERA
#define _H_CAMERA

#include "MGCommon.h"
namespace MG {

	class Camera {
	protected:
		float m_near;
		float m_far;
		Camera(float _near = 0.01f, float _far = 1.0f);
	public:
		// 継承専用
		Camera() = delete;
		virtual ~Camera() = 0;
		float GetNear() const;
		float GetFar() const;
		void SetNear(const float _near);
		void SetFar(const float _far);
	};

	class Camera2D : public Camera {
	protected:
		F2 offset;
	public:
		Camera2D(const F2& offset = {}, float _near = -1000.0f, float _far = 1000.0f);
		void SetOffset(const F2& offset);
		F2 GetOffset() const;
	};

	class Camera3D : public Camera {
	protected:
		F3 front;
		F3 upper;
		float pitch;	// 縦
		float roll;		// 横
		float yaw;		// ツイスト
		Quaternion rotate;

		float angle;
	public:
		F3 position;
		Camera3D(
			const F3& position = { 0.0f, 0.0f, -5.0f },
			const Quaternion& rotate = Quaternion::Identity(),
			float angle = 60.0f,
			float _near = 0.01f,
			float _far = 1000.0f
		);



		F3 GetPosition() const;
		F3 GetFront() const;
		F3 GetUpper() const;
		F3 GetPitchYawRoll() const;
		float GetPitch() const;
		float GetYaw() const;
		float GetRoll() const;
		
		Quaternion GetRotate() const;
		float GetAngle() const;
		
		void SetPosition(const F3& position);
		void SetAngle(const float degree);
		void SetRotate(const Quaternion& r);
		void SetPitch(float p);
		void SetYaw(float y);
		void SetRoll(float r);
		void SetPitchYawRoll(const F3& pyr);
		
		void SetFront(const F3& front, const F3& upper = { 0.0f, 1.0f, 0.0f });

		F2 GetScreenPosition(const F3& worldPosition, const F2& viewSize = GetScreenSize()) const;

		//F3& Position();
		/*void SetFront(const F3& vector);
		void SetUpper(const F3& vector);*/
		
	};
}




#endif