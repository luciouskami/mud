//  Copyright (c) 2018 Hugo Amiard hugo.amiard@laposte.net
//  This software is provided 'as-is' under the zlib License, see the LICENSE.txt file.
//  This notice and the license may not be removed or altered from any source distribution.

#include <gfx/Cpp20.h>
#ifndef MUD_CPP_20
#include <fstream>
#endif

#ifdef MUD_MODULES
module mud.gfx;
#else
#include <infra/File.h>
#include <srlz/Serial.h>
#include <gfx/Types.h>
#include <gfx/Importer.h>
#include <gfx/Prefab.h>
#include <gfx/Mesh.h>
#include <gfx/Assets.h>
#include <gfx/GfxSystem.h>
#endif

namespace mud
{
	Import::Import(GfxSystem& gfx_system, const string& filepath, const ImportConfig& config)
		: m_gfx_system(gfx_system),m_config(config)
	{
		m_path = file_directory(filepath);
		m_file = file_name(filepath);
	}

	bool ImportConfig::filter_element(const string& name) const
	{
		for(const string& filter : m_exclude_elements)
			if(name.find(filter) != string::npos)
			{
				return true;
			}

		for(const string& filter : m_include_elements)
			if(name.find(filter) == string::npos)
			{
				return true;
			}

		return false;
	}

	bool ImportConfig::filter_material(const string& name) const
	{
		for(const string& filter : m_exclude_materials)
			if(name.find(filter) != string::npos)
			{
				return true;
			}

		for(const string& filter : m_include_materials)
			if(name.find(filter) == string::npos)
			{
				return true;
			}

		return false;
	}

	ImportConfig load_model_config(cstring path, cstring model_name)
	{
		ImportConfig config = {};

		string config_path = file_directory(path) + "/" + model_name + ".cfg";
		if(file_exists(config_path.c_str()))
			unpack_json_file(Ref(&config), config_path);

		config.m_transform = bxSRT(config.m_scale, config.m_rotation, config.m_position);

		return config;
	}

	void import_to_prefab(GfxSystem& gfx_system, Prefab& prefab, Import& state)
	{
		prefab.m_nodes.reserve(state.m_items.size());
		prefab.m_items.reserve(state.m_items.size());

		prefab.m_aabb = {};

		for(Import::Item& item : state.m_items)
		{
			Model& model = *item.model;
			prefab.m_nodes.push_back({ item.transform });
			prefab.m_items.push_back({ prefab.m_nodes.back(), model });

			prefab.m_aabb.mergeSafe(transform_aabb(model.m_aabb, item.transform));
		}
	}
}
