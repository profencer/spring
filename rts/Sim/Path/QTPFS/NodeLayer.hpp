/* This file is part of the Spring engine (GPL v2 or later), see LICENSE.html */

#ifndef QTPFS_NODELAYER_HDR
#define QTPFS_NODELAYER_HDR

#include <vector>
#include <list> // for QTPFS_STAGGERED_LAYER_UPDATES
#include <boost/cstdint.hpp>

#include "System/Rectangle.h"
#include "PathDefines.hpp"

struct SRectangle;
struct MoveDef;
class CMoveMath;

namespace QTPFS {
	struct INode;
	struct NodeLayer {
	public:
		NodeLayer()
			: numLeafNodes(0)
			, layerNumber(0)
			, xsize(0)
			, zsize(0)
			{}

		void Init(unsigned int layerNum);
		void Clear();

		#ifdef QTPFS_STAGGERED_LAYER_UPDATES
		void QueueUpdate(const SRectangle& r, const MoveDef* md, const CMoveMath* mm);
		void PopQueuedUpdate() { layerUpdates.pop_front(); }
		bool ExecQueuedUpdate();
		bool HaveQueuedUpdate() const { return (!layerUpdates.empty()); }
		const SRectangle& GetQueuedUpdateRectangle() const { return ((layerUpdates.front()).rectangle); }
		#endif

		bool Update(
			const SRectangle& r,
			const MoveDef* md,
			const CMoveMath* mm,
			const std::vector<float>* luSpeedMods = NULL,
			const std::vector<int>* luBlockBits = NULL
		);

		float GetNodeRatio() const { return (numLeafNodes / float(xsize * zsize)); }
		const INode* GetNode(unsigned int x, unsigned int z) const { return nodeGrid[z * xsize + x]; }
		      INode* GetNode(unsigned int x, unsigned int z)       { return nodeGrid[z * xsize + x]; }
		const INode* GetNode(unsigned int i) const { return nodeGrid[i]; }
		      INode* GetNode(unsigned int i)       { return nodeGrid[i]; }

		const std::vector<int>& GetOldSpeedBins() const { return oldSpeedBins; }
		const std::vector<int>& GetCurSpeedBins() const { return curSpeedBins; }
		const std::vector<float>& GetOldSpeedMods() const { return oldSpeedMods; }
		const std::vector<float>& GetCurSpeedMods() const { return curSpeedMods; }

		std::vector<INode*>& GetNodes() { return nodeGrid; }
		void RegisterNode(INode* n);

		void SetNumLeafNodes(unsigned int n) { numLeafNodes = n; }
		unsigned int GetNumLeafNodes() const { return numLeafNodes; }

		unsigned int GetLayerNumber() const { return layerNumber; }

		boost::uint64_t GetMemFootPrint() const {
			boost::uint64_t memFootPrint = sizeof(NodeLayer);
			memFootPrint += (curSpeedMods.size() * sizeof(float));
			memFootPrint += (oldSpeedMods.size() * sizeof(float));
			memFootPrint += (curSpeedBins.size() * sizeof(int));
			memFootPrint += (oldSpeedBins.size() * sizeof(int));
			memFootPrint += (nodeGrid.size() * sizeof(INode*));
			return memFootPrint;
		}

	private:
		#ifdef QTPFS_STAGGERED_LAYER_UPDATES
		struct LayerUpdate {
			SRectangle rectangle;

			std::vector<float> speedMods;
			std::vector<int  > blockBits;
		};
		#endif

		std::vector<INode*> nodeGrid;

		std::vector<float> curSpeedMods;
		std::vector<float> oldSpeedMods;
		std::vector<int  > curSpeedBins;
		std::vector<int  > oldSpeedBins;

		#ifdef QTPFS_STAGGERED_LAYER_UPDATES
		std::list<LayerUpdate> layerUpdates;
		#endif

		unsigned int numLeafNodes;
		unsigned int layerNumber;

		unsigned int xsize;
		unsigned int zsize;
	};
};

#endif

