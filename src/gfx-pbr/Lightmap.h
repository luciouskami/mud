

#pragma once

#ifndef MUD_MODULES
#include <math/ImageAtlas.h>
#include <gfx/Renderer.h>
#include <gfx/Light.h>
#endif
#include <gfx-pbr/Forward.h>

namespace mud
{
	export_ class refl_ MUD_GFX_PBR_EXPORT LightmapItem
	{
	public:
		size_t m_item = SIZE_MAX;
		bgfx::TextureHandle m_lightmap = BGFX_INVALID_HANDLE;
		bgfx::VertexBufferHandle m_light_uvs = BGFX_INVALID_HANDLE;
	};

	export_ class refl_ MUD_GFX_PBR_EXPORT Lightmap
	{
	public:
		Lightmap(uint32_t size);

		uint32_t m_size = 0;
		float m_density = 1.f;
		bool m_dirty = false;

		TextureAtlas m_atlas;

		bgfx::TextureHandle m_texture = BGFX_INVALID_HANDLE;

		std::vector<LightmapItem> m_items;

		void add_item(size_t index, Item& item, bgfx::VertexBufferHandle uvs);
		bgfx::VertexBufferHandle create_uv_buffer(const std::vector<vec2>& uvs);
	};

	export_ class refl_ MUD_GFX_PBR_EXPORT LightmapAtlas : public NonCopy
	{
	public:
		LightmapAtlas(uint32_t size, float density);
		~LightmapAtlas();

		Lightmap& add_lightmap() { m_layers.emplace_back(make_unique<Lightmap>(m_size)); return *m_layers.back(); }

		uint32_t m_size;
		float m_density = 1.f;
		bool m_dirty = false;
		string m_save_path;

		std::vector<unique_ptr<Lightmap>> m_layers;
	};

	export_ MUD_GFX_PBR_EXPORT void export_lightmaps(GfxSystem& gfx_system, const string& path, GIProbe& gi_probe);
	export_ MUD_GFX_PBR_EXPORT void import_lightmaps(GfxSystem& gfx_system, const string& path, Scene& scene, GIProbe& gi_probe);

	struct LightmapRenderer : public Renderer
	{
		LightmapRenderer(GfxSystem& gfx_system, Pipeline& pipeline);
	};

	export_ class MUD_GFX_PBR_EXPORT PassLightmap : public DrawPass
	{
	public:
		PassLightmap(GfxSystem& gfx_system, BlockLightmap& block_lightmap);

		BlockLightmap& m_block_lightmap;

		virtual void next_draw_pass(Render& render, Pass& render_pass) final;
		virtual void queue_draw_element(Render& render, DrawElement& element) final;
	};

	export_ class refl_ MUD_GFX_PBR_EXPORT BlockLightmap : public DrawBlock
	{
	public:
		BlockLightmap(GfxSystem& gfx_system, BlockLight& block_light, BlockGIBake& block_gi_bake);

		BlockLight& m_block_light;
		BlockGIBake& m_block_gi_bake;

		virtual void init_block() override;
		virtual void begin_frame(const RenderFrame& frame) override;

		virtual void begin_render(Render& render) override;
		virtual void begin_pass(Render& render) override;

		virtual void begin_draw_pass(Render& render) override;

		virtual void options(Render& render, ShaderVersion& shader_version) const override;
		virtual void submit(Render& render, const Pass& render_pass) const override;
		virtual void submit(Render& render, const DrawElement& element, const Pass& render_pass) const override;

		void bake_geometry(array<Item*> items, const mat4& transform, LightmapAtlas& lightmaps);
		void bake_lightmaps(Scene& scene, const mat4& transform, const vec3& extents, LightmapAtlas& lightmaps);

		struct VoxelGIUniform
		{
			void createUniforms()
			{
				s_lightmap = bgfx::createUniform("s_lightmap",        bgfx::UniformType::Int1);
			}

			bgfx::UniformHandle s_lightmap;

		} u_lightmap;

		Program* m_lightmap;

		struct BakeProbe { Scene* m_scene; GIProbe* m_probe; };
		std::vector<BakeProbe> m_bake_queue;
	};
}
