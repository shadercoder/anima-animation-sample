#pragma once

#include <vector>
#include <set>

#include "aiTypes.h"

struct aiScene;
struct aiNode;
class SkeletonFactory;

class Skeleton
{
	std::vector<int> mParents;
	std::vector<aiMatrix4x4> mTransforms;
	std::vector<std::string> mNodeNames;

public:
	Skeleton(void);
	~Skeleton(void);

	void addTransform( int parent, const aiMatrix4x4& transform, const std::string& name );

	const aiMatrix4x4& getLocalTransform( int bone ) const;
	aiMatrix4x4 getWorldTransform( int bone ) const;
	void setLocalTransform( int bone, const aiMatrix4x4& transform );
	
	char getParent( const int bone ) const;

	int getBoneIndex( const std::string& name ) const;
	int getBoneCount() const;

	friend class SkeletonFactory;

};

class SkeletonFactory
{
	struct SkeletonNode
	{
		const aiNode* node;
		int parent;
		std::string name;
		bool used;
	};

	static void findAnimatedNodes( const aiScene* scene, std::set<std::string>& result );
	static void flattenHierarchy( const aiNode* node, int parent, const std::set<std::string>& usedBones, std::vector<SkeletonNode>& result );
	static void markParents( std::vector<SkeletonNode>& hierarchy );

	static void filterHierarchy( std::vector<SkeletonNode>& hierarchy, Skeleton& result );

public:
	static Skeleton* extractSkeleton( const aiScene* scene );
};

