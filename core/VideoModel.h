
#ifndef VIDEOMODEL_H_
#define VIDEOMODEL_H_

#include "KVFModel.h"
#include <vector>
#include <string>

/****************************************************************************/
class VideoKeyFrame : public KVFModel
{
public:

	VideoKeyFrame(MeshModel* m) : KVFModel(m), duration(1000) {}
	int duration;
};

/****************************************************************************/
class VideoModel : public MeshModel
{
public:
	VideoModel(std::string filename);
	virtual ~VideoModel();

	/* creates a new frame as a clone of existing frame*/
	VideoKeyFrame* forkFrame(VideoKeyFrame* reference);

	/* deletes an key frame, will ignore case where frame exists*/
	void deleteFrame(VideoKeyFrame* frame);

	/* gets an frame at specified index. Index may change over time */
	VideoKeyFrame* keyframe(int index);

	/* Gets either an keyframe or interpolated immediate frame (using BDMORPH) for given time in msecs */
	MeshModel* getFrame(int msecs);

	int getKeyFrameCount();
	int getKeyFrameIndex(VideoKeyFrame* frame);

private:
	std::vector<VideoKeyFrame*> keyframes;
};

#endif /* VIDEOMODEL_H_ */