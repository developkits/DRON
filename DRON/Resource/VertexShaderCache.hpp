/**
 *  Resource/VertexShaderCache.hpp
 *  (c) Jonathan Capps
 *  Created 15 Sept. 2011
 */

#ifndef VERTEX_SHADER_CACHE_HPP
#define VERTEX_SHADER_CACHE_HPP

#include <map>
#include <string>
#include <D3D11.h>

class GFXDevice;
class VertexShaderResource;
class VertexShaderCache
{
	public:
		VertexShaderCache( GFXDevice& device );
		~VertexShaderCache();
		
		VertexShaderResource& Request( const std::string& filename,
									   const std::string& shader );

		ID3D11InputLayout* GetInputLayout() const { return _input_layout; }

	protected:
		VertexShaderCache( const VertexShaderCache& );
		VertexShaderCache& operator=( const VertexShaderCache& );

		VertexShaderResource& Load( const std::string& filename,
									const std::string& shader );

		ID3DBlob* CompileVertexShaderFromFile(
			const std::string& filename,
			const std::string& shader );

		void CreateInputLayout( ID3DBlob* );

		GFXDevice&			_device;
		ID3D11InputLayout*	_input_layout;

		typedef std::map< std::string, VertexShaderResource* > ResourceMap;
		ResourceMap _resources;
};

#endif //VERTEX_SHADER_CACHE_HPP
