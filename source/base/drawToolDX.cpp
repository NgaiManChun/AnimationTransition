// =======================================================
// drawToolDX.cpp
// 
// DirectX環境のスプライトツール
// 
// 作者：魏文俊（ガイ　マンチュン）　2024/11/06
// =======================================================
#include "drawToolDX.h"
#include "rendererDX.h"

namespace MG {

	static ID3D11Buffer* g_QuadVertexBuffer = NULL;
	static ID3D11Buffer* g_CubeVertexBuffer = NULL;
	static ID3D11Buffer* g_CubeIndexBuffer = NULL;
	static ID3D11Buffer* g_PolygonVertexBuffer = NULL;
	static constexpr const int POLYGON_VERTEX_BUFFER_LENGTH = 1000;

	void DrawToolDX::DrawModelNode(ModelDX* model, MODEL_NODE* const node, const std::map<MODEL_NODE*, M4x4>& nodeWorldTransforms)
	{
		if (strcmp(node->instance, "")) return;

		string name = node->name;

		M4x4 worldM4x4 = nodeWorldTransforms.at(node);

		/*if (animationApplicants) {
			F3 size = node->scale;
			F3 position = node->position;
			Quaternion rotate = { node->rotate.x, node->rotate.y, node->rotate.z, node->rotate.w };
			for (const ANIMATION_APPLICANT& applicant : *animationApplicants) {
				auto itr = applicant.animation->modelNodeChannels.find(node->name);
				if (itr != applicant.animation->modelNodeChannels.end()) {
					ANIMATION_CHANNEL* animationChannel = itr->second;
					ApplyAnimation(animationChannel, applicant.currentFrame, size, position, rotate);
					break;
				}
			}
			worldM4x4 = M4x4::ScalingMatrix(size) * M4x4::RotatingMatrix(rotate) * M4x4::TranslatingMatrix(position) * parentWorld;
		}*/

		XMMATRIX worldMatrix = { 
			worldM4x4._v00, worldM4x4._v10, worldM4x4._v20, worldM4x4._v30,
			worldM4x4._v01, worldM4x4._v11, worldM4x4._v21, worldM4x4._v31,
			worldM4x4._v02, worldM4x4._v12, worldM4x4._v22, worldM4x4._v32,
			worldM4x4._v03, worldM4x4._v13, worldM4x4._v23, worldM4x4._v33
		};

		ID3D11DeviceContext* context = renderer->GetDeviceContext();

		

		for (int i = 0; i < node->meshNum; i++) {
			unsigned int meshIndex = node->meshIndexes[i];
			MESH* mesh = (model->rawModel->meshes + meshIndex);

			if (mesh->primitiveType != PRIMITIVE_TYPE_TRIANGLE) {
				continue; // 三角形以外は描画せず
			}

			if (mesh->primitiveType == PRIMITIVE_TYPE_TRIANGLE) {
				context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			}
			else if (mesh->primitiveType == PRIMITIVE_TYPE_LINE) {
				context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
			}
			else {
				continue;
			}

			ID3D11Buffer* vertexBuffer = model->vertexBuffers[mesh];
			ID3D11Buffer* indexBuffer = model->indexBuffers[mesh];
			Texture* texture = model->meshTextures[mesh];

			UINT offset = 0;
			UINT stride = sizeof(VERTEX);
			context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

			
			

			if (mesh->boneNum > 0) {
				ID3D11Buffer* boneWeightBuffer = model->boneWeightBuffers[mesh];
				std::vector<MESH_BONE> meshBones(model->meshBones[mesh]);
				for (int b = 0; b < mesh->boneNum; b++) {
					meshBones[b].world = nodeWorldTransforms.at(meshBones[b].node);
				}

				UINT offset = 0;
				UINT stride = sizeof(VERTEX_BONE_WEIGHT);
				renderer->SetUseBone(true);
				renderer->SetBones(meshBones.data(), meshBones.size());
				context->IASetVertexBuffers(1, 1, &boneWeightBuffer, &stride, &offset);

				M4x4 skinM4x4 = model->nodeWorldTransforms.at(node);
				XMMATRIX skinMatrix = {
					skinM4x4._v00, skinM4x4._v10, skinM4x4._v20, skinM4x4._v30,
					skinM4x4._v01, skinM4x4._v11, skinM4x4._v21, skinM4x4._v31,
					skinM4x4._v02, skinM4x4._v12, skinM4x4._v22, skinM4x4._v32,
					skinM4x4._v03, skinM4x4._v13, skinM4x4._v23, skinM4x4._v33
				};
				renderer->SetWorldMatrix(skinMatrix);
			}
			else {
				renderer->SetUseBone(false);

				renderer->SetWorldMatrix(worldMatrix);
			}

			renderer->UpdateConstant();
			renderer->UpdateLight();

			context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);


			
			

			ID3D11ShaderResourceView* resourceView = ((TextureDX*)texture)->resourceView;
			context->PSSetShaderResources(0, 1, &resourceView);

			context->DrawIndexed(mesh->vertexIndexNum, 0, 0);
		}

		for (int i = 0; i < node->childrenNum; i++) {
			DrawModelNode(model, (node->children + i), nodeWorldTransforms);
		}
	}

	DrawToolDX::DrawToolDX(RendererDX* renderer) : renderer(renderer)
	{
		ID3D11Device* pDevice = renderer->GetDevice();

		// Quad
		{
			D3D11_BUFFER_DESC bd = {};
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.ByteWidth = sizeof(VERTEX) * 4;
			bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bd.CPUAccessFlags = 0;

			VERTEX vertex[] = {
			{ { -0.5f,  0.5f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f } },
			{ {  0.5f,  0.5f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 0.0f } },
			{ { -0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f } },
			{ {  0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f } }
			};

			D3D11_SUBRESOURCE_DATA data;
			data.pSysMem = vertex;
			data.SysMemPitch = 0;
			data.SysMemSlicePitch = 0;

			pDevice->CreateBuffer(&bd, &data, &g_QuadVertexBuffer);

		}

		// Cube
		{
			D3D11_BUFFER_DESC bd = {};
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.ByteWidth = sizeof(VERTEX) * 24;
			bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bd.CPUAccessFlags = 0;

			VERTEX vertex[] = {
			{ { -0.5f,  0.5f, -0.5f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.0f, 0.33f } },
			{ {  0.5f,  0.5f, -0.5f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 0.33f } },
			{ { -0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.0f, 0.66f } },
			{ {  0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f, -1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 0.66f } },

			{ {  0.5f,  0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.0f, 0.33f } },
			{ {  0.5f,  0.5f,  0.5f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 0.33f } },
			{ {  0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.0f, 0.66f } },
			{ {  0.5f, -0.5f,  0.5f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 0.66f } },

			{ {  0.5f,  0.5f,  0.5f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.0f, 0.33f } },
			{ { -0.5f,  0.5f,  0.5f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 0.33f } },
			{ {  0.5f, -0.5f,  0.5f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.0f, 0.66f } },
			{ { -0.5f, -0.5f,  0.5f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 0.66f } },

			{ { -0.5f,  0.5f,  0.5f }, { -1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.0f, 0.33f } },
			{ { -0.5f,  0.5f, -0.5f }, { -1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 0.33f } },
			{ { -0.5f, -0.5f,  0.5f }, { -1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.0f, 0.66f } },
			{ { -0.5f, -0.5f, -0.5f }, { -1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 0.66f } },

			{ { -0.5f,  0.5f,  0.5f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.0f,  0.0f } },
			{ {  0.5f,  0.5f,  0.5f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f,  0.0f } },
			{ { -0.5f,  0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.0f, 0.33f } },
			{ {  0.5f,  0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 0.33f } },

			{ { -0.5f, -0.5f, -0.5f }, { 0.0f, -1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.0f, 0.66f } },
			{ {  0.5f, -0.5f, -0.5f }, { 0.0f, -1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 0.66f } },
			{ { -0.5f, -0.5f,  0.5f }, { 0.0f, -1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 0.0f,  1.0f } },
			{ {  0.5f, -0.5f,  0.5f }, { 0.0f, -1.0f, 0.0f }, { 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f,  1.0f } }
			};

			D3D11_SUBRESOURCE_DATA data;
			data.pSysMem = vertex;
			data.SysMemPitch = 0;
			data.SysMemSlicePitch = 0;

			pDevice->CreateBuffer(&bd, &data, &g_CubeVertexBuffer);

			unsigned short indexes[] = {
				 0,  1,  2,  2,  1,  3,
				 4,  5,  6,  6,  5,  7,
				 8,  9, 10, 10,  9, 11,
				12, 13, 14, 14, 13, 15,
				16, 17, 18, 18, 17, 19,
				20, 21, 22, 22, 21, 23
			};

			// インデックスバッファ生成
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.ByteWidth = sizeof(unsigned short) * 36;
			bd.BindFlags = D3D11_BIND_INDEX_BUFFER;

			data.pSysMem = indexes;
			data.SysMemPitch = 0;
			data.SysMemSlicePitch = 0;

			pDevice->CreateBuffer(&bd, &data, &g_CubeIndexBuffer);
		}

		{
			unsigned int vertexBufferLength = GetCommonInt("POLYGON_VERTEX_BUFFER_LENGTH");
			if (vertexBufferLength < 1) {
				vertexBufferLength = POLYGON_VERTEX_BUFFER_LENGTH;
			}
			D3D11_BUFFER_DESC bd = {};
			bd.Usage = D3D11_USAGE_DYNAMIC;
			bd.ByteWidth = sizeof(VERTEX) * vertexBufferLength;
			bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			pDevice->CreateBuffer(&bd, NULL, &g_PolygonVertexBuffer);
		}

	}

	DrawToolDX::~DrawToolDX()
	{
		if (g_QuadVertexBuffer)
		{
			g_QuadVertexBuffer->Release();
			g_QuadVertexBuffer = NULL;
		}

		if (g_CubeVertexBuffer)
		{
			g_CubeVertexBuffer->Release();
			g_CubeVertexBuffer = NULL;
		}

		if (g_CubeIndexBuffer)
		{
			g_CubeIndexBuffer->Release();
			g_CubeIndexBuffer = NULL;
		}
	}

	void DrawToolDX::DrawQuad(const Texture* texture, const F3& position, const F3& size, const Quaternion& rotate, const F4& color, const F2& uvOffset, const F2& uvRange)
	{
		ID3D11DeviceContext* context = renderer->GetDeviceContext();
		context->PSSetShaderResources(0, 1, &((TextureDX*)texture)->resourceView);

		// 頂点バッファ設定
		UINT stride = sizeof(VERTEX);
		UINT offset = 0;

		context->IASetVertexBuffers(0, 1, &g_QuadVertexBuffer, &stride, &offset);

		// プリミティブトポロジ設定
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		renderer->SetWorldMatrix(
			XMMatrixScaling(size.x, size.y, size.z) *
			XMMatrixRotationQuaternion({ rotate.x, rotate.y, rotate.z, rotate.w }) *
			XMMatrixTranslation(position.x, position.y, position.z)
		);

		renderer->SetColor(color);
		renderer->SetUVOffset(uvOffset);
		renderer->SetUVRange(uvRange);
		renderer->UpdateConstant();
		renderer->UpdateLight();
		renderer->SetUseBone(false);

		// ポリゴン描画
		context->Draw(4, 0);
	}

	void DrawToolDX::DrawCube(const Texture* texture, const F3& position, const F3& size, const Quaternion& rotate, const F4& color)
	{

		ID3D11DeviceContext* context = renderer->GetDeviceContext();

		context->PSSetShaderResources(0, 1, &((TextureDX*)texture)->resourceView);

		// 頂点バッファ設定
		UINT stride = sizeof(VERTEX);
		UINT offset = 0;

		context->IASetVertexBuffers(0, 1, &g_CubeVertexBuffer, &stride, &offset);
		context->IASetIndexBuffer(g_CubeIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

		// プリミティブトポロジ設定
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		renderer->SetWorldMatrix(
			XMMatrixScaling(size.x, size.y, size.z) *
			XMMatrixRotationQuaternion({ rotate.x, rotate.y, rotate.z, rotate.w }) *
			XMMatrixTranslation(position.x, position.y, position.z)
		);
		renderer->SetColor(color);
		renderer->SetUVOffset({});
		renderer->SetUVRange({ 1.0f, 1.0f });
		renderer->UpdateConstant();
		renderer->UpdateLight();
		renderer->SetUseBone(false);

		// ポリゴン描画
		context->DrawIndexed(36, 0, 0);
	}

	void DrawToolDX::DrawModel(const Model* model, const F3& position, const F3& size, const Quaternion& rotate, const F4& color, const F2& uvOffset, const F2& uvRange)
	{
		ID3D11DeviceContext* context = renderer->GetDeviceContext();
		M4x4 world = M4x4::ScalingMatrix(size) * M4x4::RotatingMatrix(rotate) * M4x4::TranslatingMatrix(position);
		renderer->SetColor(color);
		renderer->SetUVOffset(uvOffset);
		renderer->SetUVRange(uvRange);

		ModelDX* modelDX = (ModelDX*)model;

		std::map<MODEL_NODE*, M4x4> nodeWorldTransforms(modelDX->nodeWorldTransforms);

		LoadNodeWorldTransforms(model->rawModel->rootNode, world, nodeWorldTransforms);

		DrawModelNode(modelDX, model->rawModel->rootNode, nodeWorldTransforms);
	}

	void DrawToolDX::DrawModel(const Model* model, const std::vector<ANIMATION_APPLICANT>& animationApplicants, const F3& position, const F3& size, const Quaternion& rotate, const F4& color)
	{
		ID3D11DeviceContext* context = renderer->GetDeviceContext();
		M4x4 world = M4x4::ScalingMatrix(size) * M4x4::RotatingMatrix(rotate) * M4x4::TranslatingMatrix(position);
		renderer->SetColor(color);
		renderer->SetUVOffset({});
		renderer->SetUVRange({ 1.0f, 1.0f });

		ModelDX* modelDX = (ModelDX*)model;

		std::map<MODEL_NODE*, M4x4> nodeWorldTransforms(modelDX->nodeWorldTransforms);
		LoadNodeWorldTransforms(model->rawModel->rootNode, world, nodeWorldTransforms, animationApplicants);

		DrawModelNode(modelDX, model->rawModel->rootNode, nodeWorldTransforms);
	}

	void DrawToolDX::DrawModel(const Model* model, const std::vector<ANIMATION_APPLICANT>& animationApplicants0, const std::vector<ANIMATION_APPLICANT>& animationApplicants1, const float animTransitionT, const F3& position, const F3& size, const Quaternion& rotate, const F4& color)
	{
		ID3D11DeviceContext* context = renderer->GetDeviceContext();
		M4x4 world = M4x4::ScalingMatrix(size) * M4x4::RotatingMatrix(rotate) * M4x4::TranslatingMatrix(position);
		renderer->SetColor(color);
		renderer->SetUVOffset({});
		renderer->SetUVRange({ 1.0f, 1.0f });

		ModelDX* modelDX = (ModelDX*)model;

		std::map<MODEL_NODE*, M4x4> nodeWorldTransforms(modelDX->nodeWorldTransforms);
		LoadNodeWorldTransforms(model->rawModel->rootNode, world, nodeWorldTransforms, animationApplicants0, animationApplicants1, animTransitionT);

		DrawModelNode(modelDX, model->rawModel->rootNode, nodeWorldTransforms);
	}

	void DrawToolDX::DrawModel(const Model* model, const std::map<MODEL_NODE*, M4x4>& transforms, const F4& color) {
		ModelDX* modelDX = (ModelDX*)model;
		renderer->SetColor(color);
		renderer->SetUVOffset({});
		renderer->SetUVRange({ 1.0f, 1.0f });
		DrawModelNode(modelDX, model->rawModel->rootNode, transforms);
	}

	void DrawToolDX::DrawPolygon(const Texture* texture, const VERTEX* vertices, size_t length, TOPOLOGY topology, const F3& position, const F3& size, const Quaternion& rotate, const F4& color, const F2& uvOffset, const F2& uvRange)
	{
		ID3D11DeviceContext* context = renderer->GetDeviceContext();

		//頂点の作成
		D3D11_MAPPED_SUBRESOURCE msr;
		context->Map(g_PolygonVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		memcpy(msr.pData, vertices, sizeof(VERTEX) * length);

		context->Unmap(g_PolygonVertexBuffer, 0);

		// TO-DO: テクスチャなしシェーダでやる
		if (texture) {
			context->PSSetShaderResources(0, 1, &((TextureDX*)texture)->resourceView);
		}

		// 頂点バッファ設定
		UINT stride = sizeof(VERTEX);
		UINT offset = 0;

		context->IASetVertexBuffers(0, 1, &g_PolygonVertexBuffer, &stride, &offset);

		// TO-DO: トポロジ判定
		// プリミティブトポロジ設定
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		renderer->SetWorldMatrix(
			XMMatrixScaling(size.x, size.y, size.z) *
			XMMatrixRotationQuaternion({ rotate.x, rotate.y, rotate.z, rotate.w }) *
			XMMatrixTranslation(position.x, position.y, position.z)
		);

		renderer->SetColor(color);
		renderer->SetUVOffset(uvOffset);
		renderer->SetUVRange(uvRange);
		renderer->UpdateConstant();
		renderer->UpdateLight();
		renderer->SetUseBone(false);

		// ポリゴン描画
		context->Draw(length, 0);
	}

	void DrawToolDX::DrawCube(const M4x4& matrix, const F4& color)
	{
		ID3D11DeviceContext* context = renderer->GetDeviceContext();
		XMMATRIX _matrix = XMMATRIX(
			matrix._v00, matrix._v01, matrix._v02, matrix._v03,
			matrix._v10, matrix._v11, matrix._v12, matrix._v13,
			matrix._v20, matrix._v21, matrix._v22, matrix._v23,
			matrix._v30, matrix._v31, matrix._v32, matrix._v33
		);

		// 頂点バッファ設定
		UINT stride = sizeof(VERTEX);
		UINT offset = 0;

		context->IASetVertexBuffers(0, 1, &g_CubeVertexBuffer, &stride, &offset);
		context->IASetIndexBuffer(g_CubeIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

		// プリミティブトポロジ設定
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		renderer->SetWorldMatrix(_matrix);
		renderer->SetColor(color);
		renderer->SetUVOffset({});
		renderer->SetUVRange({ 1.0f, 1.0f });
		renderer->UpdateConstant();
		renderer->UpdateLight();
		//renderer->SetRasterizerState(RASTERIZER_STATE_WIREFRAME);

		// ポリゴン描画
		context->DrawIndexed(36, 0, 0);

		//renderer->SetRasterizerState(RASTERIZER_STATE_CULL_NONE);
	}

} // namespace MG