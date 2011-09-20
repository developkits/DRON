/**
 *  Resource/MeshCache.cpp
 *  (c) Jonathan Capps
 *  Created 19 Sept. 2011
 */

#include "MeshCache.hpp"
#include <Windows.h>
#include <D3D11.h>
#include <assimp.hpp>
#include <aiScene.h>
#include <aiPostProcess.h>
#include "MeshResource.hpp"
#include "../Utility/DXUtil.hpp"
#include "../Utility/Geometry.hpp"
#include "../Utility/StringHelper.hpp"

MeshCache::MeshCache()
	: _device( 0 )
{
	MeshResource* invalid_resource = new MeshResource();
	invalid_resource->SetValid( false );
	_resources.insert(
		std::make_pair( "invalid_resource", invalid_resource ) );
}

MeshCache::~MeshCache()
{
	ResourceMap::iterator iter = _resources.begin();
	while( iter != _resources.end() )
	{
		delete ( *iter ).second->_data;
		++iter;
	}
}

void MeshCache::Initialize( ID3D11Device* device )
{
	if( !_device )
		_device = device;
}

MeshResource& MeshCache::Request( const std::string& filename )
{
	ResourceMap::iterator iter = _resources.find( filename );
	if( iter != _resources.end() ) return *( *iter ).second;

	MeshResource& mr = Load( filename );
	return mr;
}

MeshResource& MeshCache::Load( const std::string& filename )
{
	Assimp::Importer importer;
	std::string file_and_path( "Data//Meshes//" + filename );

	const aiScene* scene_ptr = importer.ReadFile( file_and_path,
		aiProcess_JoinIdenticalVertices |
		aiProcess_MakeLeftHanded        |
		aiProcess_Triangulate           |
		aiProcess_ImproveCacheLocality  |
		aiProcess_SortByPType           |
		aiProcess_OptimizeMeshes        |
		aiProcess_OptimizeGraph );

	if( !scene_ptr )
	{
		std::wstring ws = StringToWString( importer.GetErrorString() );
		OutputDebugString( ws.c_str() );

		return *( *_resources.find( "invalid_resource" ) ).second;
	}

	if( !scene_ptr->HasMeshes() )
	{
		OutputDebugString( L"The scene has no meshes!" );
		return *( *_resources.find( "invalid_resource" ) ).second;
	}

	/**************************************************************************
	 * TODO: Need to copy the mesh data, because the Importer keeps ownership
	 *       of the scene data, and destroys it when it goes out of scope.
	 */
	MeshResource* mr_ptr = new MeshResource();
	mr_ptr->_data = BuildMesh( scene_ptr );
	mr_ptr->SetValid( true );
	_resources.insert( std::make_pair( filename, mr_ptr ) );

	return *mr_ptr;
}

Mesh* MeshCache::BuildMesh( const aiScene* scene_ptr )
{
	aiMesh* aim = scene_ptr->mMeshes[ 0 ];
	Mesh* mesh = new Mesh;

    D3D11_BUFFER_DESC bd;
	bd.ByteWidth = sizeof( aiVector3D ) * aim->mNumVertices;
    bd.Usage = D3D11_USAGE_IMMUTABLE;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;
    bd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA dsd;
	ZeroMemory( &dsd, sizeof( dsd ) );
	dsd.pSysMem = aim->mVertices;

	HR( _device->CreateBuffer( &bd, &dsd, &mesh->_vertex_buffer ) );
	mesh->_num_vertices = aim->mNumVertices;

	std::vector< unsigned int > index_vector;
	index_vector.reserve( aim->mNumFaces * 3 );

	for( unsigned int i = 0; i < aim->mNumFaces; ++i )
	{
		index_vector.push_back( aim->mFaces[ i ].mIndices[ 0 ] );
		index_vector.push_back( aim->mFaces[ i ].mIndices[ 1 ] );
		index_vector.push_back( aim->mFaces[ i ].mIndices[ 2 ] );
	}

	bd.ByteWidth = sizeof( unsigned int ) * aim->mNumFaces * 3;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;

	dsd.pSysMem = &index_vector[ 0 ];

	HR( _device->CreateBuffer( &bd, &dsd, &mesh->_index_buffer ) );
	mesh->_num_indices = aim->mNumFaces * 3;

	return mesh;
}