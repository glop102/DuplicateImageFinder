#include "imageinfo.h"

#define numPartsDiff_allowed 3
#define numDiffPerPartAllowed 15
bool ImageInfo::collidesWith(ImageInfo& other){
	int numPartsDiff = 0; // number of average boxes that can be different and still not collide
	for(int x=0; x<9*9; x++){
		if(abs(other.red[x] - this->red[x]) > numDiffPerPartAllowed){
			numPartsDiff++;
			if(numPartsDiff > numPartsDiff_allowed)
				return false;
		}else if(abs(other.green[x] - this->green[x]) > numDiffPerPartAllowed){
			numPartsDiff++;
			if(numPartsDiff > numPartsDiff_allowed)
				return false;
		}else if(abs(other.blue[x] - this->blue[x]) > numDiffPerPartAllowed){
			numPartsDiff++;
			if(numPartsDiff > numPartsDiff_allowed)
				return false;
		}
	}

	return true; // they do collide
}

bool ImageInfo::operator==(const ImageInfo& rhs)const{
	if(fileName != rhs.fileName) return false;
	if(filePath != rhs.filePath) return false;
	if(knownCollisions != rhs.knownCollisions) return false;
	return true;
}
bool ImageInfo::operator !=(const ImageInfo& rhs)const{
	return !((*this)==rhs);
}
