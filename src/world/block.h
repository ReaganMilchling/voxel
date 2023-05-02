#pragma once

class Block {
public:
	std::vector<std::vector<float>> getVisibleFaces();
private:
	std::vector<float> getFront();
	std::vector<float> getBack();
	std::vector<float> getLeft();
	std::vector<float> getRight();
	std::vector<float> getTop();
	std::vector<float> getBottom();
};