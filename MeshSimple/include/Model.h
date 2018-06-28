#pragma once

#include <base.h>
#include <vector>
#include <map>
#include <set>
#include <queue>
#include <algorithm>

struct EdgePair
{
	int a, b;
	double err;
	glm::vec4 point;

	bool operator < (const EdgePair &rhs) const
	{
		return err > rhs.err;
	}
};

struct Face
{
	int a, b, c;
	Face(int _a, int _b, int _c)
	{
		std::vector<int> v{ _a,_b,_c };
		std::sort(v.begin(), v.end());
		a = v[0]; b = v[1]; c = v[2];
	}
	bool operator < (const Face &rhs) const
	{
		if (a != rhs.a) return a < rhs.a;
		if (b != rhs.b) return b < rhs.b;
		return c < rhs.c;
	}
};


class Model
{
public:
	Model(const char* filename) {
		load_from_obj(filename);
	}
	void load_from_obj(const char* filename);
	void calulateQ(int index);
	void simplify(float ratio);
	std::vector<GLfloat> vertices;
	std::vector<unsigned int> indices;
	void save(const char* filename);

private:
	using pii = std::pair<int, int>;
	using sp = std::set<pii>;
	std::vector<sp> G;
	std::vector<std::set<int>> E;
	std::map<int, glm::mat4> Q;
	std::vector<GLfloat> ori_vertices;
	std::vector<unsigned int> ori_indicies;
	std::priority_queue<EdgePair, std::vector<EdgePair>, std::less<EdgePair>> heap;
	std::set<int> Exist;
	void makeG(std::vector<unsigned int> indices);
	void makeE(std::vector<unsigned int> indices);
	void updateHeap(int index);

	void getPoint(int index, glm::vec3 &point)
	{
		point = glm::vec3(vertices[index*3], vertices[index*3+1], vertices[index*3+2]);
	}
};