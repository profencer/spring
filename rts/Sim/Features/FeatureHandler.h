#ifndef __FEATURE_HANDLER_H__
#define __FEATURE_HANDLER_H__

#include "Object.h"
#include <string>
#include <list>
#include <vector>
#include <boost/noncopyable.hpp>
#include "creg/creg.h"
#include "FeatureDef.h"
#include "FeatureSet.h"

struct S3DOModel;
class CFileHandler;
class CLoadSaveInterface;
class CVertexArray;
class LuaTable;

#define DRAW_QUAD_SIZE 32

class CFeatureHandler : public CObject, public boost::noncopyable
{
	CR_DECLARE(CFeatureHandler);
	CR_DECLARE_SUB(DrawQuad);

public:
	CFeatureHandler();
	~CFeatureHandler();

	CFeature* CreateWreckage(const float3& pos, const std::string& name,
	                         float rot, int facing, int iter, int team, int allyteam,
	                         bool emitSmoke, std::string fromUnit);

	void Update();

	int AddFeature(CFeature* feature);
	void DeleteFeature(CFeature* feature);
	void UpdateDrawQuad(CFeature* feature, const float3& newPos);

	void LoadFeaturesFromMap(bool onlyCreateDefs);
	const FeatureDef* GetFeatureDef(const std::string name);
	const FeatureDef* GetFeatureDefByID(int id);

	void SetFeatureUpdateable(CFeature* feature);
	void TerrainChanged(int x1, int y1, int x2, int y2);

	void Draw();
	void DrawShadowPass();
	void DrawRaw(int extraSize, std::vector<CFeature*>* farFeatures); //the part of draw that both draw and drawshadowpass can use

	const std::map<std::string, const FeatureDef*>& GetFeatureDefs() const { return featureDefs; }
	const CFeatureSet& GetActiveFeatures() const { return activeFeatures; }

private:
	void AddFeatureDef(const std::string& name, FeatureDef* feature);
	const FeatureDef* CreateFeatureDef(const LuaTable& luaTable, const std::string& name);

private:
	std::map<std::string, const FeatureDef*> featureDefs;
	std::vector<const FeatureDef*> featureDefsVector;

	int nextFreeID;
	std::list<int> freeIDs;
	std::list<int> toBeFreedIDs;
	CFeatureSet activeFeatures;

	std::list<int> toBeRemoved;
	CFeatureSet updateFeatures;

	struct DrawQuad {
		CR_DECLARE_STRUCT(DrawQuad);
		CFeatureSet features;
	};

	std::vector<DrawQuad> drawQuads;

	int drawQuadsX;
	int drawQuadsY;

	float farDist;

	void DrawFar(CFeature* feature, CVertexArray* va);

	void Serialize(creg::ISerializer *s);
	void PostLoad();

	friend class CFeatureDrawer;
};

extern CFeatureHandler* featureHandler;

#endif // __FEATURE_HANDLER_H__
